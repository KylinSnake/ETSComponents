#ifndef SNAKE_CORE_THREAD_H
#define SNAKE_CORE_THREAD_H

#include <pthread.h>
#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

using namespace std;

namespace snake
{
  namespace core
  {
    class Thread: boost::noncopyable
    {
      public:
        typedef boost::function<void ()> ThreadFunc;
        Thread(const ThreadFunc& f, const string& name=string());
        ~Thread();

        void start();
        int join();

        bool started() const { return is_started_;}
        const string& name() const { return name_;}

        pid_t tid() const {return tid_;}
      private:
        static void* run(void* t);
        pid_t tid_;
        string name_;
        bool is_started_;
        bool is_joined_;
        ThreadFunc func_;
        pthread_t pthread_id_;
    };
  }
}

#endif
