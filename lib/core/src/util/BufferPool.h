#ifndef SNAKE_CORE_BUFFERPOOL
#define SNAKE_CORE_BUFFERPOOL

#include <list>
#include <algorithm>
#include <type_traits>

namespace snake
{
	namespace core
	{
		template<class T>
		class BufferPool
		{
		public:
			using value_type = typename std::remove_cv<typename std::remove_reference<typename std::remove_pointer<T>::type>::type>::type;
			using pointer_type = value_type*;
			BufferPool(size_t buffer_length, size_t capacity = 16)
			: length_{buffer_length}
			{
				fill(capacity);
			}

			~BufferPool()
			{
				std::for_each(available_.begin(), available_.end(), [](pointer_type p){ delete[] p; });
			}

			BufferPool(const BufferPool&) = delete;
			BufferPool& operator=(const BufferPool&) = delete;
			pointer_type malloc()
			{
				if (available_.size() == 0)
				{
					fill(allocated_.size() * 2);
				}
				pointer_type ret = available_.front();
				available_.pop_front();
				allocated_.push_back(ret);
				return ret;
			}
			void free(pointer_type ptr)
			{
				auto it = std::find(allocated_.begin(), allocated_.end(), ptr);
				if (it != allocated_.end())
				{
					allocated_.erase(it);
					available_.push_back(ptr);
				}
				else
				{
					delete[] ptr;
				}
			}
			
		protected:
			void fill(size_t capacity)
			{
				if(available_.size() == 0)
				{
					std::list<pointer_type> ret{capacity};
					std::generate(ret.begin(), ret.end(), 
						[length=length_](){return new T[length];});
					available_.swap(ret);
				}
			}
		private:
			size_t length_;
			std::list<pointer_type> available_{};
			std::list<pointer_type> allocated_{};
		};
	}
}

#endif
