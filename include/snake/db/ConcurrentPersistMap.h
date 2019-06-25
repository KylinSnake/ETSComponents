#ifndef SNAKE_DB_CONCURRENT_PERSIST_MAP_H
#define SNAKE_DB_CONCURRENT_PERSIST_MAP_H

#include <snake/concurrency/ConcurrentHashMap.h>
#include <snake/db/KeyValueLocalDb.h>

namespace snake
{
	template<typename Key, typename T, typename HashT = std::hash<Key>,
		typename EqualT = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, T>>>
	class ConcurrentPersistMap
	{
	public:
		using MapImpl = ConcurrentHashMap<Key, T, HashT, EqualT, Alloc>;
        using key_type = typename MapImpl::key_type;
		using hasher = typename MapImpl::hasher;
		using key_equal = typename MapImpl::key_equal;
		using size_type = typename MapImpl::size_type;
		using difference_type = typename MapImpl::difference_type;
		using pointer = typename MapImpl::pointer;
		using const_pointer = typename MapImpl::const_pointer;
		using reference = typename MapImpl::reference;
		using const_reference = typename MapImpl::reference;
		using value_type = typename MapImpl::value_type;
		using mapped_type = typename MapImpl::mapped_type;

		ConcurrentPersistMap() = default;
		~ConcurrentPersistMap() = default;

		bool inited() const
		{
			return inited_;
		}

		bool init(const std::string& persist_path, const DbOption& option) noexcept(false)
		{
			db_.open(persist_path, option);
			if(db_.is_open())
			{
				for(auto& p : db_.read_all<key_type, mapped_type>())
				{
					if(!map_.put(std::move(p.first), std::move(p.second), true))
					{
						map_.clear();
						db_.close();
						return false;
					}
				}
				inited_ = true;
			}
			return inited_;
		}

		bool get_copy(const key_type& k, mapped_type& v) const
		{
			auto p = map_.get(k);
			if(p.valid())
			{
				v = p->second;
				return true;
			}
			return false;
		}

		bool has(const key_type& key) const
		{
			return map_.has(key);
		}

		size_t size() const
		{
			return map_.size();
		}

		bool put(const key_type& k, const mapped_type& v, bool is_persist = false) noexcept(false)
		{
			auto ret = map_.put(k, v, true);
			if(ret && is_persist)
			{
				persist(k);
			}
			return ret;
		}

		bool put(const key_type& k, mapped_type&& v, bool is_persist = false) noexcept(false)
		{
			auto ret = map_.put(k, std::move(v), true);
			if(ret && is_persist)
			{
				persist(k);
			}
			return ret;
		}

		void persist(const key_type& key) noexcept(false)
		{
			auto p = map_.get(key);
			if(p.valid())
			{
				db_.put(p->first, p->second);
			}
		}

		void remove(const key_type& key) noexcept(false)
		{
			auto p = map_.get(key);
			if(p.valid())
			{
				db_.remove(key);
				p.erase();
			}
		}

		void clear() noexcept(false)
		{
			try
			{
				map_.clear([this]()
				{
					db_.unsafe_clear();
				});
			}
			catch(const Exception&)
			{
				db_.close();
				inited_ = false;
				throw;
			}
		}

		template<typename Func>
		void for_each(Func&& f)
		{
			map_.for_each(std::forward<Func>(f));
		}

		class TransactionAccessor
		{
		public:
			TransactionAccessor() = default;
			~TransactionAccessor() = default;
			TransactionAccessor(const TransactionAccessor&) = delete;
			TransactionAccessor& operator=(const TransactionAccessor&) = delete;
			TransactionAccessor(TransactionAccessor&&) = default;
			TransactionAccessor& operator=(TransactionAccessor&&) = default;
			using value_type = ConcurrentPersistMap::mapped_type;

			bool valid() const
			{
				return db_ptr_ != nullptr && impl_.valid();
			}

			value_type& value()
			{
				assert(valid());
				return impl_->second;
			}

			void release()
			{
				impl_.release();
				db_ptr_ = nullptr;
			}

			void remove_and_commit() noexcept(false)
			{
				assert(valid());
				db_ptr_->remove(impl_->first);
				impl_.erase();
				db_ptr_ = nullptr;
			}

			void commit_and_release() noexcept(false)
			{
				assert(valid());
				db_ptr_->put(impl_->first, impl_->second);
				release();
			}

			void rollback_and_release() noexcept(false)
			{
				assert(valid());
				if(db_ptr_->has(impl_->first))
				{
					value_type out{};
					if(db_ptr_->get(impl_->first, out))
					{
						impl_->second = std::move(out);
						release();
						return;
					}
				}
				impl_.erase();
				db_ptr_ = nullptr;
			}
				
		private:
			TransactionAccessor(typename MapImpl::UniqueAccessor&& p, KeyValueLocalDb* ptr) 
			: impl_{std::move(p)}
			, db_ptr_{ptr}
			{}

			typename MapImpl::UniqueAccessor impl_{};
			KeyValueLocalDb* db_ptr_{nullptr};
			friend class ConcurrentPersistMap;
		};

		TransactionAccessor get_or_create(const key_type& key)
		{
			return TransactionAccessor(map_.get_or_create(key), &db_);
		}

		TransactionAccessor get(const key_type& key)
		{
			return TransactionAccessor(map_.get(key), &db_);
		}
	
	private:
		MapImpl map_{};
		KeyValueLocalDb db_{};
		bool inited_ {false};
	};
}

#endif
