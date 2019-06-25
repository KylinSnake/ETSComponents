#include <cassert>
#include <leveldb/db.h>
#include <snake/db/KeyValueLocalDb.h>

namespace snake
{
	KeyValueLocalDb::~KeyValueLocalDb()
	{
		close();
	}

	KeyValueLocalDb::KeyValueLocalDb(KeyValueLocalDb&& rhs)
	{
		db_ptr_ = rhs.db_ptr_;
		db_name_ = std::move(rhs.db_name_);
		db_path_ = std::move(rhs.db_path_);
		db_option_ = std::move(rhs.db_option_);
		rhs.db_ptr_ = nullptr;
	}

	KeyValueLocalDb& KeyValueLocalDb::operator=(KeyValueLocalDb&& rhs)
	{
		if(&rhs != this)
		{
			if(db_ptr_ != nullptr)
			{
				close();
			}
			db_ptr_ = rhs.db_ptr_;
			db_name_ = std::move(rhs.db_name_);
			db_path_ = std::move(rhs.db_path_);
			db_option_ = std::move(rhs.db_option_);
			rhs.db_ptr_ = nullptr;
		}
		return *this;
	}

	bool KeyValueLocalDb::is_open() const
	{
		return db_ptr_ != nullptr;
	}

	void KeyValueLocalDb::open(const std::string& path, const DbOption& option) noexcept(false)
	{
		return open(path, path, option);
	}

	void KeyValueLocalDb::open(const std::string& path, const std::string& name, const DbOption& option) noexcept(false)
	{
		if(is_open())
		{
			throw DBException(name, "DB is already open");
		}

		leveldb::DB* db {nullptr};
		leveldb::Options options{};
		options.create_if_missing = option.create_if_not_exist_;
		auto status = leveldb::DB::Open(options, path, &db);
		if(!status.ok())
		{
			throw DBException(name, status.ToString());
		}
		db_ptr_ = db;
		db_name_ = name;
		db_path_ = path;
		db_option_ = option;
	}

	void KeyValueLocalDb::close()
	{
		if(is_open())
		{
			auto db = db_ptr_;
			db_ptr_ = nullptr;
			delete db;
		}
	}

	void KeyValueLocalDb::unsafe_clear() noexcept(false)
	{
		if(is_open())
		{
			close();
		}
		leveldb::Options options{};
		auto status = leveldb::DestroyDB(db_path_, options);
		if(status.ok())
		{
			options.create_if_missing = true;
			status = leveldb::DB::Open(options, db_path_, &db_ptr_);
		}
		if(!status.ok())
		{
			throw DBException(db_name_, "unsafe_clear() : " + status.ToString());
		}
	}

	void KeyValueLocalDb::put_to_db(const char* key_buf, std::size_t key_size, const char* value_buf, std::size_t value_size)
	{
		assert(db_ptr_ != nullptr);
		leveldb::Slice key(key_buf, key_size);
		leveldb::Slice value(value_buf, value_size);
		auto status = db_ptr_->Put(leveldb::WriteOptions{}, key, value);
		if(!status.ok())
		{
			throw DBException(db_name_, status.ToString());
		}
	}

	bool KeyValueLocalDb::get_from_db(const char* key_buf, std::size_t key_size, std::string& ret)
	{
		assert(db_ptr_ != nullptr);
		leveldb::Slice key(key_buf, key_size);
		auto status = db_ptr_->Get(leveldb::ReadOptions{},key, &ret);
		if(status.ok())
		{
			return true;
		}
		else if(status.IsNotFound())
		{
			return false;
		}
		throw DBException(db_name_, status.ToString());
	}

	void KeyValueLocalDb::remove_from_db(const char* key_buf, std::size_t key_size)
	{
		assert(db_ptr_ != nullptr);
		leveldb::Slice key(key_buf, key_size);
		auto status = db_ptr_->Delete(leveldb::WriteOptions{}, key);
		if(!status.ok())
		{
			throw DBException(db_name_, status.ToString());
		}
	}

	std::list<std::pair<std::string, std::string>> KeyValueLocalDb::read_all_from_db() const
	{
		assert(db_ptr_ != nullptr);
		leveldb::Iterator* it = db_ptr_->NewIterator(leveldb::ReadOptions());
		std::list<std::pair<std::string, std::string>> ret;
		for (it->SeekToFirst(); it->Valid(); it->Next())
		{
			ret.push_back(std::pair<std::string, std::string>(it->key().ToString(), it->value().ToString()));
		}
		auto status = it->status();
		delete it;
		if(!status.ok())
		{
			throw DBException(db_name_, status.ToString());
		}
		return ret;
	}
}
