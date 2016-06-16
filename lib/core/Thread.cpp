#include <sys/syscall.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <cstdio>
#include "Thread.h"

namespace global
{
 __thread pid_t local_tid = 0;
 __thread char tid_string[32] ={0};
 __thread char thread_name[32] = {0}; 

 pid_t current_thread_id()
 {
   return local_tid;
 }

 const char* current_thread_id_str()
 {
   return tid_string;
 }

 const char * current_thread_name()
 {
   return thread_name;
 }

 bool is_main_thread()
 {
   return local_tid == ::getpid();
 }

 void sleep_usec(long usec)
 {
   static long usec_per_sec = 1000 * 1000;
   struct timespec ts = {0, 0};
   ts.tv_sec = static_cast<time_t>(usec / usec_per_sec);
   ts.tv_nsec = static_cast<long>(usec % usec_per_sec);
   ::nanosleep(&ts, NULL);
 }
}

inline pid_t get_tid()
{
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

bool main_thread_init()
{
  global::local_tid = get_tid();
  sprintf(global::tid_string,"%ld", static_cast<long>(global::local_tid));
  strcpy(global::thread_name, "Main");
  return true;
}

bool is_main_initialized = main_thread_init();

namespace snake
{
  namespace core
  {

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
      global::local_tid = get_tid();
      sprintf(global::tid_string,"%ld", static_cast<long>(global::local_tid));

      Thread* thread = (Thread*)t;
      thread->tid_ = global::local_tid;
      if (thread->name_.empty())
      {
        thread->name_ = global::tid_string;
      }
      strncpy(global::thread_name, thread->name_.c_str(), sizeof(global::thread_name));
      ::prctl(PR_SET_NAME, global::current_thread_name());

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

