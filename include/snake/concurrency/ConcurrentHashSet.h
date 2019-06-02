#ifndef SNAKE_CONCURRENCY_CONCURRENT_HASH_SET_H
#define SNAKE_CONCURRENCY_CONCURRENT_HASH_SET_H

#include<type_traits>
#include<snake/concurrency/ConcurrentHashTableT.h>

namespace __hash_map_impl__
{
	template<typename T, typename V>
	struct HashSetExtractor
	{
		static_assert(std::is_same<T, V>::value, "HashSetExtractor should have same type in key and value");
		using KeyType = const T;
		using MappedValueType = const V;
		using ValueType = const T;

		inline const KeyType& key(const ValueType& v) const
		{
			return v;
		}

		inline KeyType&& key(ValueType&& v) const
		{
			return std::move(v);
		}

		inline const MappedValueType& value(const ValueType& v) const
		{
			return v;
		}

		inline MappedValueType&& value(ValueType&& v) const
		{
			return std::move(v);
		}

		template<typename ...Args>
		inline void set_value(ValueType& v, Args... args) const
		{
		}

		template<typename K, typename... Args>
		inline ValueType extract_ctor_params(K&& key, Args... args)
		{
			return ValueType{std::forward<K>(key)};
		}
	};
}

namespace snake
{
	template<typename Key,
		typename HashT = std::hash<Key>,
		typename EqualT = std::equal_to<Key>,
		typename Alloc = std::allocator<const Key>,
		typename TableMutexT = SharedMutex,
		typename BucketMutexT = LockFreeMutex>
	class ConcurrentHashSet
	{
	public:
		using MapImpl = concurrency::ConcurrentHashTableT<Key, Key, 
						HashT, EqualT, TableMutexT, BucketMutexT, 
						__hash_map_impl__::HashSetExtractor, Alloc>;
		using key_type = typename MapImpl::key_type;
		using hasher = typename MapImpl::hasher;
		using key_equal = typename MapImpl::key_equal;
		using size_type = typename MapImpl::size_type;
		using difference_type = typename MapImpl::difference_type;
		using pointer = typename MapImpl::pointer;
		using const_pointer = typename MapImpl::const_pointer;
		using reference = typename MapImpl::reference;
		using const_reference = typename MapImpl::reference;
		
		using extractor_type = typename MapImpl::extractor_type;
		using node_allocator = typename MapImpl::node_allocator;

		ConcurrentHashSet(
			size_type buckets = 100,
			const hasher& h = hasher(),
			const key_equal& k = key_equal(),
			const extractor_type& e = extractor_type(),
			const node_allocator& a = node_allocator())
		: m_impl_{buckets, h, k, e, a}
		{
		}

		~ConcurrentHashSet() = default;
		ConcurrentHashSet(const ConcurrentHashSet&) = delete;
		ConcurrentHashSet& operator=(const ConcurrentHashSet&) = delete;

		inline bool has(const key_type& key) const
		{
			return m_impl_.has(key);
		}

		template<typename K>
		inline bool insert(K&& key)
		{
			return m_impl_.insert(std::forward<K>(key)).valid();
		}

		template<typename K>
		inline bool remove(K&& key)
		{
			return m_impl_.remove(std::forward<K>(key));
		}

		inline size_type clear()
		{
			return m_impl_.clear();
		}

		inline size_type size() const
		{
			return m_impl_.size();
		}

		inline size_type bucket_size() const
		{
			return m_impl_.size();
		}

		template<typename Func>
		inline void for_each(Func&& f) const
		{
			m_impl_.for_each(std::forward<Func>(f));
		}

	private:
		MapImpl m_impl_;
	};
};

#endif
