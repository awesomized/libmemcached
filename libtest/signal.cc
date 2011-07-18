/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  uTest, libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <libtest/common.h>

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include <libtest/signal.h>

using namespace libtest;

struct context_st {
  sigset_t set;
  sem_t lock;

  context_st()
  {
    sigemptyset(&set);
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGUSR2);

    sem_init(&lock, 0, 0);
  }
  
  void test()
  {
    assert(sigismember(&set, SIGABRT));
    assert(sigismember(&set, SIGINT));
    assert(sigismember(&set, SIGUSR2));
  }

  int wait(int& sig)
  {
    return sigwait(&set, &sig);
  }

  ~context_st()
  {
    sem_destroy(&lock);
  }
};

static volatile shutdown_t __shutdown;
static pthread_mutex_t shutdown_mutex;
static pthread_t thread;

bool is_shutdown()
{
  bool ret;
  pthread_mutex_lock(&shutdown_mutex);
  ret= bool(__shutdown != SHUTDOWN_RUNNING);
  pthread_mutex_unlock(&shutdown_mutex);

  return ret;
}

void set_shutdown(shutdown_t arg)
{
  pthread_mutex_lock(&shutdown_mutex);
  __shutdown= arg;
  pthread_mutex_unlock(&shutdown_mutex);

  if (arg == SHUTDOWN_GRACEFUL)
  {
    pthread_kill(thread, SIGUSR2);

    void *retval;
    pthread_join(thread, &retval);
  }
}

shutdown_t get_shutdown()
{
  shutdown_t local;
  pthread_mutex_lock(&shutdown_mutex);
  local= __shutdown;
  pthread_mutex_unlock(&shutdown_mutex);

  return local;
}

extern "C" {

static void *sig_thread(void *arg)
{   
  context_st *context= (context_st*)arg;
  assert(context);

  context->test();
  sem_post(&context->lock);

  while (get_shutdown() == SHUTDOWN_RUNNING)
  {
    int sig;

    if (context->wait(sig) == -1)
    {
      Error << "sigwait() returned errno:" << strerror(errno);
      continue;
    }

    switch (sig)
    {
    case SIGABRT:
    case SIGINT:
      Error << "Signal handling thread got signal " <<  strsignal(sig);
      set_shutdown(SHUTDOWN_FORCED);
      break;

      // Signal thread is being told that a graceful shutdown is occuring
    case SIGUSR2:
      break;

    default:
      Error << "Signal handling thread got unexpected signal " <<  strsignal(sig);
      break;
    }
  }

  delete context;

  return NULL;
}

}

void setup_signals()
{
  pthread_mutex_init(&shutdown_mutex, NULL);
  set_shutdown(SHUTDOWN_RUNNING);

  context_st *context= new context_st;

  assert(context);

  int error;
  if ((error= pthread_sigmask(SIG_BLOCK, &context->set, NULL)) != 0)
  {
    Error << "pthread_sigmask() died during pthread_sigmask(" << strerror(error) << ")";
    exit(EXIT_FAILURE);
  }

  if ((error= pthread_create(&thread, NULL, &sig_thread, (void *) &context->set)) != 0)
  {
    Error << "pthread_create() died during pthread_create(" << strerror(error) << ")";
    exit(EXIT_FAILURE);
  }

  sem_wait(&context->lock);
}
