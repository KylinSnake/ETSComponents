#include <sys/syscall.h>
#include <cstdio>
#include "Thread.h"



namespace snake
{
  namespace core
  {
    inline pid_t get_tid()
    {
      return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    Thread::Thread(const Thread::ThreadFunc& f, const string& name)
    : tid_(0)
    , name_(name)
    , is_started_(false)
    , is_joined_(false)
    , func_(f)
    , pthread_id_()
    {
    }

    Thread::~Thread()
    {
      if(is_started_ && !is_joined_)
      {
        pthread_detach(pthread_id_);
      }
    }

    void Thread::start()
    {
      assert(!is_started_);
      is_started_ = (pthread_create(&pthread_id_, NULL, Thread::run, (void*)this) == 0);
    }

    void* Thread::run(void* t)
    {
      assert(t!= NULL);
      Thread* thread = (Thread*)t;
      thread->tid_ = get_tid();
      if (thread->name_.empty())
      {
        char buf[32];
        sprintf(buf,"%ld", static_cast<long>(thread->tid_));
        thread->name_ = buf;
      }

      //TODO: set TLS data
      try
      {
        thread->func_();
      }
      catch(std::exception&)
      {
      }
      return NULL;
    }
  }
}

