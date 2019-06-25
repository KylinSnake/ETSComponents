#ifndef SNAKE_DB_KEYVALUE_LOCALDB_H
#define SNAKE_DB_KEYVALUE_LOCALDB_H

#include <cstring>
#include <string>
#include <list>
#include <type_traits>
#include <snake/util/Exception.h>
#include <snake/db/DBException.h>

namespace leveldb
{
	class DB;
}

namespace snake
{
	namespace db
	{
		static const std::size_t MAX_BUFFER_SIZE = 1024 * 4;
		template<typename T, bool is_pointer, bool is_fundamental>
		struct slice_t
		{
			static_assert(!is_pointer, "slice doesn't support non-char array");
		};

		template<>
		struct slice_t<char, true, true>
		{
			static constexpr std::size_t buf_size(std::size_t) { return 0;}
			static std::pair<const char*, std::size_t> serialize(const char* s, const char*, std::size_t)
			{
				return std::pair<const char*, std::size_t>(s, std::strlen(s));
			}
		};

		template<typename T>
		struct slice_t<T, false, true>
		{
			static constexpr std::size_t buf_size(std::size_t) { return sizeof(T); }
			static std::pair<const char*, std::size_t> serialize(T t, char* buf, std::size_t size)
			{
				assert(buf_size(size) <= size);
				*(reinterpret_cast<T*>(buf)) = t;
				return std::pair<const char*, std::size_t>(buf, size);
			}
			static T parse_from(const std::string& s)
			{
				return *(reinterpret_cast<const T*>(s.data()));
			}
		};

		template<typename T>
		struct slice_t<T, false, false>
		{
			static constexpr std::size_t buf_size(std::size_t size) { return size; }
			static std::pair<const char*, std::size_t> serialize(const T& t, char* buf, std::size_t size)
			{
				return std::pair<const char*, std::size_t>(buf, t.serialize(buf, size));
			}
			static T parse_from(const std::string& s)
			{
				T ret;
				ret.deserialize(s.data(), s.length());
				return ret;
			}
		};

		template<>
		struct slice_t<std::string, false, false>
		{
			static constexpr std::size_t buf_size(std::size_t) { return 0; }
			static std::pair<const char*, std::size_t> serialize(const std::string& t, char* buf, std::size_t size)
			{
				return std::pair<const char*, std::size_t>(t.data(), t.length());
			}
			static std::string parse_from(std::string&& s)
			{
				return std::move(s);
			}

			static std::string parse_from(const std::string& s)
			{
				return s;
			}
		};

		template<typename T>
		using slice_type_t=std::remove_cv_t<std::remove_reference_t<T>>;

		template<typename T>
		using Slice = slice_t<std::remove_cv_t<std::remove_pointer_t<slice_type_t<T>>>, 
			std::is_pointer<slice_type_t<T>>::value, 
			std::is_fundamental<std::remove_pointer_t<slice_type_t<T>>>::value>;
	}

	struct DbOption
	{
		bool create_if_not_exist_ {true};
		size_t key_buffer_size_ {db::MAX_BUFFER_SIZE};
		size_t value_buffer_size_ {db::MAX_BUFFER_SIZE};
	};

	// KeyValueLocalDb use leveldb, so reading/writing is thread-safe,
	// but open/close/destroy are not thread-safe
	class KeyValueLocalDb final
	{
	public:
		KeyValueLocalDb() = default;
		~KeyValueLocalDb();
		KeyValueLocalDb(const KeyValueLocalDb&) = delete;
		KeyValueLocalDb& operator=(const KeyValueLocalDb&) = delete;
		KeyValueLocalDb(KeyValueLocalDb&&);
		KeyValueLocalDb& operator=(KeyValueLocalDb&&);

		void open(const std::string& path, const DbOption& option) noexcept(false);
		void open(const std::string& path, const std::string& name, const DbOption& option) noexcept(false);
		void close();
		bool is_open() const;

		void unsafe_clear() noexcept(false);

		const std::string& name() const { return db_name_; }

		template<typename KeyT, typename ValueT>
		void put(KeyT&& key, ValueT&& v) noexcept(false);

		template<typename KeyT, typename ValueT>
		auto read_all() const noexcept(false);

		template<typename KeyT, typename ValueT>
		bool get(KeyT&& key, ValueT& v) noexcept(false);

		template<typename KeyT>
		bool has(KeyT&& key) noexcept(false);

		template<typename KeyT>
		void remove(KeyT&& key) noexcept(false);

		void set_key_buffer_size(std::size_t v)
		{
			db_option_.key_buffer_size_ = v;
		}

		void set_value_buffer_size(std::size_t v)
		{
			db_option_.value_buffer_size_ = v;
		}
	
	private:
		void put_to_db(const char* key_buf, std::size_t key_size, const char* value_buf, std::size_t value_size);
		bool get_from_db(const char* key_buf, std::size_t key_size, std::string& ret);
		void remove_from_db(const char* key_buf, std::size_t key_size);
		std::list<std::pair<std::string, std::string>> read_all_from_db() const;
		leveldb::DB* db_ptr_ {nullptr};
		std::string db_name_{};
		std::string db_path_{};
		DbOption db_option_{};
	};

	template<typename KeyT, typename ValueT>
	inline void KeyValueLocalDb::put(KeyT&& key, ValueT&& value) noexcept(false)
	{
		char key_buf[db::Slice<KeyT>::buf_size(db_option_.key_buffer_size_)]{};
		char value_buf[db::Slice<ValueT>::buf_size(db_option_.value_buffer_size_)]{};
		auto k = db::Slice<KeyT>::serialize(std::forward<KeyT>(key), key_buf, sizeof(key_buf));
		auto v = db::Slice<ValueT>::serialize(std::forward<ValueT>(value), value_buf, sizeof(value_buf));
		put_to_db(k.first, k.second, v.first, v.second);
	}

	template<typename KeyT, typename ValueT>
	inline bool KeyValueLocalDb::get(KeyT&& key, ValueT& v) noexcept(false)
	{
		char key_buf[db::Slice<KeyT>::buf_size(db_option_.key_buffer_size_)]{};
		auto k = db::Slice<KeyT>::serialize(std::forward<KeyT>(key), key_buf, sizeof(key_buf));
		std::string ret{};
		if(get_from_db(k.first, k.second, ret))
		{
			v = db::Slice<ValueT>::parse_from(ret);
			return true;
		}
		return false;
	}

	template<typename KeyT>
	inline bool KeyValueLocalDb::has(KeyT&& key) noexcept(false)
	{
		char key_buf[db::Slice<KeyT>::buf_size(db_option_.key_buffer_size_)]{};
		auto k = db::Slice<KeyT>::serialize(std::forward<KeyT>(key), key_buf, sizeof(key_buf));
		std::string ret {};
		return get_from_db(k.first, k.second, ret);
	}

	template<typename KeyT>
	inline void KeyValueLocalDb::remove(KeyT&& key) noexcept(false)
	{
		char key_buf[db::Slice<KeyT>::buf_size(db_option_.key_buffer_size_)]{};
		auto k = db::Slice<KeyT>::serialize(std::forward<KeyT>(key), key_buf, sizeof(key_buf));
		remove_from_db(k.first, k.second);
	}

	template<typename KeyT, typename ValueT>
	inline auto KeyValueLocalDb::read_all() const noexcept(false)
	{
		using KeyType = std::remove_cv_t<std::remove_pointer_t<db::slice_type_t<KeyT>>>;
		using ValueType = std::remove_cv_t<std::remove_pointer_t<db::slice_type_t<ValueT>>>;
		using ListType = std::list<std::pair<KeyType,ValueType>>;
		ListType ret{};
		for(auto& p : read_all_from_db())
		{
			ret.push_back(std::pair<KeyType,ValueType>(db::Slice<KeyT>::parse_from(p.first),
				db::Slice<ValueT>::parse_from(p.second)));
		}
		return ret;
	}
}


#endif
