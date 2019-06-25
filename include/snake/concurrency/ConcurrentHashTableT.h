#ifndef SNAKE_CONCURRENCY_CONCURRENT_HASHTABLE_T_H
#define SNAKE_CONCURRENCY_CONCURRENT_HASHTABLE_T_H

#include <algorithm>
#include <vector>
#include <memory>
#include <cassert>
#include <utility>
#include <functional>
#include <type_traits>
#include <snake/concurrency/Atomic.h>
#include <snake/concurrency/Mutex.h>

namespace snake
{
	namespace concurrency
	{
		
		static const std::size_t num_primes = 28;

		static const unsigned long prime_list[num_primes] {
			53ul,         97ul,         193ul,       389ul,       769ul,
			1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
			49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
			1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
			50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
			1610612741ul, 3221225473ul, 4294967291ul
		};

		inline unsigned long next_prime(unsigned long n)
		{
			auto last = prime_list + num_primes;
			auto p = std::lower_bound(prime_list, last, n);
			return p == last ? *(last - 1) : *p;
		}

		template<class Val>
		struct HashTableNode
		{
			HashTableNode() = default;

			template<typename... Args>
			HashTableNode(Args&&... args) : m_value_{std::forward<Args>(args)...}{}
			HashTableNode(const HashTableNode&) = default;
			HashTableNode(HashTableNode&&) = default;
			HashTableNode& operator=(const HashTableNode&) = default;
			HashTableNode& operator=(HashTableNode&&) = default;

			HashTableNode(const Val& v) : m_value_{v}{}
			HashTableNode(Val&& v) : m_value_{std::move(v)}{}

			Val m_value_{};
			HashTableNode* m_next_{nullptr};
		};

		// 1. TableMutex is ReadWrite lock, while the BucketMutex is only lockable
		// 2. Allocator should be thread safe. the std::allocator is stateless to use
		//    new/delete operator, so it may not be efficient. To use memory pool
		//    we can wrapper std::pmr classes (GCC >= 9 or MSVC >= 19.13 to write a synchronized pool allocator
		//    so in this class, we don't handle allocator concurrency
		//
		template<class KeyT, class ValueT, class HashFuncT, class KeyEqualFuncT,
			class TableMutexT, class BucketMutexT, template<typename, typename> class ExtractorT, class AllocT>
		class ConcurrentHashTableT final
		{
		public:
			using extractor_type=ExtractorT<KeyT, ValueT>;
			using key_type = typename extractor_type::KeyType;
			using value_type = typename extractor_type::ValueType;
			using hasher = HashFuncT;
			using key_equal = KeyEqualFuncT;

			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using node_type = HashTableNode<value_type>;
			using node_allocator = typename std::allocator_traits<AllocT>::template rebind_alloc<node_type>;

			using mapped_type = typename extractor_type::MappedValueType;

			static_assert(std::is_const<key_type>::value, "Key type of concurrent hash table should be const");
		
		private:
			hasher m_hasher_;
			key_equal m_equal_;
			extractor_type m_extractor_;
			Atomic<size_type> m_num_elements_;


			mutable TableMutexT m_table_mutex_;
			// for buckets mutex, we have to use raw arary pointer,
			// as we assume the bucket mutex are non-copyable and non-moveable
			mutable BucketMutexT* m_bucket_mutex_array_;
			using node_ptr_allocator = typename std::allocator_traits<AllocT>::template rebind_alloc<node_type*>;
			std::vector<node_type*, node_ptr_allocator> m_buckets_;
			Atomic<size_type> m_buckets_size_;
			node_allocator allocator_;

		private:
			size_type get_bucket_index(const key_type& key, size_type n) const
			{
				return m_hasher_(key) % n;
			}

			template<typename... Args>
			node_type* construct(Args&&... args)
			{
				node_type* n = allocator_.allocate(1);
				std::allocator_traits<node_allocator>::construct(allocator_, n, m_extractor_.extract_ctor_params(std::forward<Args>(args)...));
				return n;
			}

			void destroy(node_type* p)
			{
				std::allocator_traits<node_allocator>::destroy(allocator_, p);
				allocator_.deallocate(p, 1);
			}

			node_type* get_from_bucket_with_no_lock(const key_type& key, size_type n)
			{
				assert(n < m_buckets_size_);
				auto p = m_buckets_[n];
				while(p != nullptr)
				{
					if(m_equal_(key, m_extractor_.key(p->m_value_)))
					{
						break;
					}
					p = p->m_next_;
				}
				return p;
			}

			void add_new_node_with_no_lock(size_type n, node_type* p)
			{
				p->m_next_ = m_buckets_[n];
				m_buckets_[n] = p;
				m_num_elements_++;
			}

			void resize(size_type num_elements)
			{
				size_type old_bucket_num = m_buckets_size_;
				if(num_elements > old_bucket_num)
				{
					const size_type next_bucket_num = next_prime(num_elements);
					if(next_bucket_num > old_bucket_num)
					{
						WriteLock<TableMutexT> tl(m_table_mutex_);
						//read it again in case that some other thread rehash it already.
						old_bucket_num = m_buckets_size_;
						if(next_bucket_num > old_bucket_num)
						{
							std::vector<node_type*, node_ptr_allocator> tmp(next_bucket_num, nullptr, m_buckets_.get_allocator());
							try
							{
								for(size_type index = 0; index < old_bucket_num; ++ index)
								{
									auto p = m_buckets_[index];
									while(p != nullptr)
									{
										size_type new_index = get_bucket_index(m_extractor_.key(p->m_value_), next_bucket_num);
										m_buckets_[index] = p->m_next_;
										p->m_next_ = tmp[new_index];
										tmp[new_index] = p;
										p = m_buckets_[index];
									}
								}
								m_buckets_.swap(tmp);
								m_buckets_size_ = m_buckets_.size();

								auto ptr_tmp = m_bucket_mutex_array_;
								m_bucket_mutex_array_ = new BucketMutexT[next_bucket_num];
								delete[] ptr_tmp;
							}
							catch(...)
							{
								for(std::size_t index = 0; index < tmp.size(); ++ index)
								{
									while(tmp[index] != nullptr)
									{
										auto p = tmp[index]->m_next_;
										destroy(tmp[index]);
										m_num_elements_--;
										tmp[index] = p;
									}
								}
								throw;
							}
						}
					}
				}
			}

			void init_buckets(size_type n_bucket)
			{
				assert(m_bucket_mutex_array_ == nullptr);
				size_type num = next_prime(n_bucket);
				m_buckets_.assign(num, nullptr);
				m_buckets_size_ = m_buckets_.size();
				m_bucket_mutex_array_ = new BucketMutexT[m_buckets_size_];
			}

		public:	
			ConcurrentHashTableT(
				size_type buckets = 100,
				const hasher& h = hasher(),
				const key_equal& k = key_equal(),
				const extractor_type& e = extractor_type(),
				const node_allocator& a = node_allocator())
			: m_hasher_{h}
			, m_equal_{k}
			, m_extractor_{e}
			, m_num_elements_{0}
			, m_table_mutex_{}
			, m_bucket_mutex_array_{nullptr}
			, allocator_{a}
			{
				init_buckets(buckets);
			}

			// as Mutex are assumed to be non-copyable and non-moveable,
			// we disable assignment and copy-constructor
			ConcurrentHashTableT(const ConcurrentHashTableT&) = delete;
			ConcurrentHashTableT& operator=(const ConcurrentHashTableT&) = delete;

			class AccessorBase
			{
			protected:
				node_type* m_node_ {nullptr};
				size_type m_index_{0};
				ConcurrentHashTableT* m_table_{nullptr};
				void clear()
				{
					m_node_ = nullptr;
					m_index_ = 0;
					m_table_ = nullptr;
				}

				void erase_with_no_lock()
				{
					if(m_node_ != nullptr)
					{
						assert(m_table_ != nullptr);
						node_type* p = m_table_->m_buckets_[m_index_];
						if(p == m_node_)
						{
							p = m_node_->m_next_;
							m_table_->m_buckets_[m_index_] = p;
						}
						else
						{
							while(p != nullptr)
							{
								if(p->m_next_ == m_node_)
								{
									p->m_next_ = m_node_->m_next_;
									p = p->m_next_;
									break;
								}
								p = p->m_next_;
							}
						}
						m_table_->destroy(m_node_);
						--(m_table_->m_num_elements_);

						m_node_ = p;
						if(m_node_== nullptr)
						{
							m_table_->m_bucket_mutex_array_[m_index_].unlock();
						}
					}
				}

				friend class ConcurrentHashTableT;
			public:
				AccessorBase() = default;
				AccessorBase(const AccessorBase&) = delete;
				AccessorBase& operator=(const AccessorBase&) = delete;
				AccessorBase(AccessorBase&& rhs)
				{
					m_node_ = rhs.m_node_;
					m_index_ = rhs.m_index_;
					m_table_ = rhs.m_table_;
					rhs.clear();
				}

				AccessorBase& operator=(AccessorBase&& rhs)
				{
					m_node_ = rhs.m_node_;
					m_index_ = rhs.m_index_;
					m_table_ = rhs.m_table_;
					rhs.clear();

					return *this;
				}
				void release()
				{
					if(m_node_ != nullptr)
					{
						m_table_->m_bucket_mutex_array_[m_index_].unlock();
					}

					if(m_table_!= nullptr)
					{
						m_table_->m_table_mutex_.unlock_shared();
					}
					clear();
				}

				~AccessorBase()
				{
					release();
				}

				bool valid() const
				{
					return m_node_ != nullptr && m_table_ != nullptr;
				}

				value_type* operator->()
				{
					return (m_node_ == nullptr) ? nullptr : &(m_node_->m_value_);
				}

				value_type& operator*()
				{
					assert(m_node_ != nullptr);
					return m_node_->m_value_;
				}

				const value_type* operator->() const
				{
					return (m_node_ == nullptr) ? nullptr : &(m_node_->m_value_);
				}

				const value_type& operator*() const
				{
					assert(m_node_ != nullptr);
					return m_node_->m_value_;
				}
				
			};

			class UniqueAccessor final : public AccessorBase
			{
			public:
				UniqueAccessor() = default;
				~UniqueAccessor() = default;

				UniqueAccessor(const UniqueAccessor&) = delete;
				UniqueAccessor& operator=(const UniqueAccessor&) = delete;

				UniqueAccessor(UniqueAccessor&&) = default;
				UniqueAccessor& operator=(UniqueAccessor&&) = default;

				UniqueAccessor(AccessorBase&& p) : AccessorBase{std::move(p)}{}

				void erase()
				{
					this->erase_with_no_lock();
					this->release();
				}

			};

			class UniqueIterator final : public AccessorBase
			{
			private:
				void move_to_next_bucket()
				{
					assert(this->m_node_ == nullptr && this->m_table_ != nullptr);
					while(this->m_index_ < this->m_table_->bucket_size())
					{
						this->m_table_->m_bucket_mutex_array_[this->m_index_].lock();
						if(this->m_table_->m_buckets_[this->m_index_] != nullptr)
						{
							this->m_node_ = this->m_table_->m_buckets_[this->m_index_];
							return;
						}
						this->m_table_->m_bucket_mutex_array_[this->m_index_].unlock();
						++(this->m_index_);
					}
					if(this->m_node_ == nullptr)
					{
						this->m_table_->m_table_mutex_.unlock_shared();
						this->clear();
					}
				}
			public:
				~UniqueIterator() = default;

				UniqueIterator(ConcurrentHashTableT* p) : AccessorBase()
				{
					this->m_table_ = p;
				}

				UniqueIterator(const UniqueIterator&) = delete;
				UniqueIterator& operator=(const UniqueIterator&) = delete;

				UniqueIterator(UniqueIterator&&) = default;
				UniqueIterator& operator=(UniqueIterator&&) = default;

				void next()
				{
					if(this->m_table_ == nullptr)
					{
						return;
					}

					if(this->m_node_ != nullptr)
					{
						this->m_node_ = this->m_node_->m_next_;
						if(this->m_node_ == nullptr)
						{
							this->m_table_->m_bucket_mutex_array_[this->m_index_].unlock();
							++(this->m_index_);
						}
					}
					else
					{
						this->m_index_ = 0;
						this->m_table_->m_table_mutex_.lock_shared();
					}

					if(this->m_node_ == nullptr)
					{
						move_to_next_bucket();
					}
				}

				void erase_and_next()
				{
					if(this->m_table_ == nullptr || this->m_node_ == nullptr)
					{
						return;
					}
					this->erase_with_no_lock();
					if(this->m_node_ == nullptr)
					{
						++(this->m_index_);
						move_to_next_bucket();
					}
				}

			};



		private:
			enum INSERT_TYPE
			{
				ONLY_GET_EXISTING = 0,
				CREATE_IF_ABSENT = 1,
				INSERT_ON_ABSENT = 2
			};

			template<typename K, typename ...Args>
			UniqueAccessor get_or_create_impl(K&& key, INSERT_TYPE type, Args&&... args)
			{
				if(type != ONLY_GET_EXISTING)
				{
					resize(m_num_elements_ + 1);
				}
				m_table_mutex_.lock_shared();
				{
					auto index = get_bucket_index(key, m_buckets_size_);
					{
						m_bucket_mutex_array_[index].lock();
						auto p = get_from_bucket_with_no_lock(key, index);
						if(p == nullptr && type != ONLY_GET_EXISTING)
						{
							p = construct(std::forward<K>(key), std::forward<Args>(args)...);
							add_new_node_with_no_lock(index, p);
						}
						else if (p != nullptr && type == INSERT_ON_ABSENT)
						{
							p = nullptr;
						}

						if(p != nullptr)
						{
							UniqueAccessor ret{};
							ret.m_table_ = this;
							ret.m_node_ = p;
							ret.m_index_ = index;
							return ret;
						}
						m_bucket_mutex_array_[index].unlock();
					}
				}
				m_table_mutex_.unlock_shared();
				return UniqueAccessor{};
			}

		public:
			size_type clear(const std::function<void()>& f)
			{
				WriteLock<TableMutexT> tl(m_table_mutex_);
				size_type left = m_num_elements_;
				for(auto& p : m_buckets_)
				{
					while(p != nullptr)
					{
						auto t = p;
						p = p->m_next_;
						destroy(t);
						--m_num_elements_;
					}
				}
				if(f)
				{
					f();
				}
				return left - m_num_elements_;
			}

			size_type clear()
			{
				static std::function<void()> f;
				return clear(f);
			}

			~ConcurrentHashTableT()
			{
				clear();
				delete[] m_bucket_mutex_array_;
			}
			
			inline size_type size() const
			{
				return m_num_elements_;
			}

			inline size_type bucket_size() const
			{
				return m_buckets_size_;
			}

			template<typename K>
			inline bool remove(K&& key)
			{
				auto p = get(std::forward<K>(key));
				if(p.valid())
				{
					p.erase();
					return true;
				}
				return false;
			}

			inline bool has(const key_type& key) const
			{
				return get(key).valid();
			}

			template<typename K>
			inline UniqueAccessor get(K&& key)
			{
				return get_or_create_impl(std::forward<K>(key), ONLY_GET_EXISTING);
			}

			template<typename K>
			inline UniqueAccessor get(K&& key) const
			{
				return const_cast<ConcurrentHashTableT*>(this)->get(std::forward<K>(key));
			}

			template<typename K>
			inline mapped_type get_copy(K&& key) const
			{
				auto accessor = get(std::forward<K>(key));
				return accessor.valid() ? m_extractor_.value(*accessor) : mapped_type{};
			}

			template<typename K, typename ...Args>
			inline UniqueAccessor get_or_create(K&& key, Args&&... args)
			{
				return get_or_create_impl(std::forward<K>(key), CREATE_IF_ABSENT, std::forward<Args>(args)...);
			}

			template<typename K, typename ...Args>
			inline UniqueAccessor insert(K&& key, Args&&... args)
			{
				return get_or_create_impl(std::forward<K>(key), INSERT_ON_ABSENT, std::forward<Args>(args)...);
			}

			template<typename T>
			inline bool put(T&& value, bool put_if_absent)
			{
				auto key = m_extractor_.key(std::forward<T>(value));
				UniqueAccessor p{};
				if(put_if_absent)
				{
					p = get_or_create(key);
				}
				else
				{
					p = get(key);
				}
				if(p.valid())
				{
					m_extractor_.set_value(*p , m_extractor_.value(std::forward<T>(value)));
					return true;
				}
				return false;
			}

			template<typename K, typename V>
			inline bool put(K&& key, V&& v, bool put_if_absent)
			{
				UniqueAccessor p{};
				if(put_if_absent)
				{
					p = get_or_create(std::forward<K>(key));
				}
				else
				{
					p = get(std::forward<K>(key));
				}
				if(p.valid())
				{
					m_extractor_.set_value(*p , std::forward<V>(v));
					return true;
				}
				return false;
			}

			inline UniqueIterator unique_iterator() const
			{
				return UniqueIterator{const_cast<ConcurrentHashTableT*>(this)};
			}

			template<typename Func>
			inline void for_each(Func&& p) const
			{
				auto it = unique_iterator();
				it.next();
				while(it.valid())
				{
					p(*it);
					it.next();
				}
			}

			template<typename Pred>
			inline UniqueAccessor find_if(Pred&& p) const
			{
				auto it = unique_iterator();
				it.next();
				while(it.valid())
				{
					if(p(*it))
					{
						return std::move(it);
					}
					it.next();
				}
				return UniqueAccessor{};
			}
		};
	}
}


#endif
