#ifndef SNAKE_CORE_THREAD_EXIT_STRATEGY_T_H
#define SNAKE_CORE_THREAD_EXIT_STRATEGY_T_H

namespace snake
{
	namespace core
	{
		template<typename ElementContainerT>
		struct exit_immediately
		{
			bool available_stop(const ElementContainerT& c) const
			{
				return !stopped;
			}
			bool pop_stopped( const ElementContainerT& c ) const
			{
				return stopped;
			}
			bool push_stopped( const ElementContainerT& c ) const
			{
				return stopped;
			}
			void stop()
			{
				stopped = true;
			}
		private:
			bool stopped = false;	
		};

		template<typename ElementContainerT>
		struct exit_after_handle_all : public exit_immediately<ElementContainerT>
		{
			bool pop_stopped( const ElementContainerT& c ) const
			{
				return c.empty() && exit_immediately<ElementContainerT>::pop_stopped(c);
			}
		};
	}
}

#endif