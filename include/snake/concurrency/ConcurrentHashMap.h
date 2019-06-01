#ifndef SNAKE_CONCURRENCY_CONCURRENT_HASH_MAP_H
#define SNAKE_CONCURRENCY_CONCURRENT_HASH_MAP_H

#include<snake/concurrency/ConcurrentHashTableT.h>

namespace __hash_map_impl__
{
	template<typename T, typename U>
	struct HashMapExtractor
	{
		using KeyType = T;
		using MappedValueType = U;
		using ValueType = std::pair<KeyType,MappedValueType>;

		inline const KeyType& key(const ValueType& v) const
		{
			return v.first;
		}

		inline KeyType&& key(ValueType&& v) const
		{
			return std::move(v.first);
		}

		inline const MappedValueType& value(const ValueType& v) const
		{
			return v.second;
		}

		inline MappedValueType&& value(ValueType&& v) const
		{
			return std::move(v.second);
		}

		inline void set_key(const KeyType& key, ValueType& v) const
		{
			v.first = key;
		}

		inline void set_key(KeyType&& key, ValueType& v) const
		{
			v.first = std::move(key);
		}

		template<typename... Args>
		inline ValueType extract_ctor_params(const KeyType& key, Args... args)
		{
			return ValueType(KeyType{key}, MappedValueType{std::forward<Args>(args)...});
		}
	};
}

namespace snake
{
	template<typename Key, typename T, 
		typename HashT = std::hash<Key>,
		typename EqualT = std::equal_to<Key>,
		typename Alloc = std::allocator< std::pair<const Key, T> >,
		typename TableMutexT = SharedMutex,
		typename BucketMutexT = LockFreeMutex>
	using ConcurrentHashMap = concurrency::ConcurrentHashTableT<Key, T, 
			HashT, EqualT, TableMutexT, BucketMutexT, 
			__hash_map_impl__::HashMapExtractor, Alloc>;
};

#endif
