#ifndef SNAKE_CONCURRENCY_CONCURRENT_HASH_MAP_H
#define SNAKE_CONCURRENCY_CONCURRENT_HASH_MAP_H

#include<snake/concurrency/ConcurrentHashTableT.h>

namespace __hash_map_impl__
{
	template<typename T, typename U>
	struct HashMapExtractor
	{
		using KeyType = const T;
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

		template<typename ...Args>
		inline void set_value(ValueType& v, Args&&... args) const
		{
			v.second = MappedValueType{std::forward<Args>(args)...};
		}

		template<typename K, typename... Args>
		inline ValueType extract_ctor_params(K&& key, Args&&... args)
		{
			return ValueType(KeyType{std::forward<K>(key)}, MappedValueType{std::forward<Args>(args)...});
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
