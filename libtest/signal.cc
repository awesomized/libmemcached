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

#include <csignal>

#include <libtest/signal.h>

using namespace libtest;

#define MAGIC_MEMORY 123569

bool SignalThread::is_shutdown()
{
  bool ret;
  pthread_mutex_lock(&shutdown_mutex);
  ret= bool(__shutdown != SHUTDOWN_RUNNING);
  pthread_mutex_unlock(&shutdown_mutex);

  return ret;
}

void SignalThread::set_shutdown(shutdown_t arg)
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

shutdown_t SignalThread::get_shutdown()
{
  shutdown_t local;
  pthread_mutex_lock(&shutdown_mutex);
  local= __shutdown;
  pthread_mutex_unlock(&shutdown_mutex);

  return local;
}

void SignalThread::post()
{
  sem_post(&lock);
}

void SignalThread::test()
{
  assert(magic_memory == MAGIC_MEMORY);
  if (not getenv("LIBTEST_IN_GDB"))
  {
    assert(sigismember(&set, SIGABRT));
    assert(sigismember(&set, SIGQUIT));
    assert(sigismember(&set, SIGINT));
  }
  assert(sigismember(&set, SIGUSR2));
}

extern "C" {

static void *sig_thread(void *arg)
{   
  SignalThread *context= (SignalThread*)arg;

  context->test();
  context->post();

  while (context->get_shutdown() == SHUTDOWN_RUNNING)
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
    case SIGUSR2:
    case SIGINT:
    case SIGQUIT:
      if (context->is_shutdown() == false)
      {
        Error << "Signal handling thread got signal " <<  strsignal(sig);
        context->set_shutdown(SHUTDOWN_FORCED);
      }
      break;

    default:
      Error << "Signal handling thread got unexpected signal " <<  strsignal(sig);
      break;
    }
  }

  return NULL;
}

}

SignalThread::SignalThread() :
  magic_memory(MAGIC_MEMORY)
{
  pthread_mutex_init(&shutdown_mutex, NULL);
  sigemptyset(&set);
  if (not getenv("LIBTEST_IN_GDB"))
  {
    sigaddset(&set, SIGABRT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGINT);
  }

  sigaddset(&set, SIGUSR2);

  sem_init(&lock, 0, 0);
}


bool SignalThread::setup()
{
  set_shutdown(SHUTDOWN_RUNNING);

  sigset_t old_set;
  sigemptyset(&old_set);
  pthread_sigmask(SIG_BLOCK, NULL, &old_set);

  if (sigismember(&old_set, SIGQUIT))
  {
    Error << strsignal(SIGQUIT) << " has been previously set.";
  }
  if (sigismember(&old_set, SIGINT))
  {
    Error << strsignal(SIGINT) << " has been previously set.";
  }
  if (sigismember(&old_set, SIGUSR2))
  {
    Error << strsignal(SIGUSR2) << " has been previously set.";
  }

  int error;
  if ((error= pthread_sigmask(SIG_BLOCK, &set, NULL)) != 0)
  {
    Error << "pthread_sigmask() died during pthread_sigmask(" << strerror(error) << ")";
    return false;
  }

  if ((error= pthread_create(&thread, NULL, &sig_thread, this)) != 0)
  {
    Error << "pthread_create() died during pthread_create(" << strerror(error) << ")";
    return false;
  }

  sem_wait(&lock);

  return true;
}
