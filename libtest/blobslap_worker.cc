/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
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
#include <libtest/blobslap_worker.h>
#include <libtest/killpid.h>

using namespace libtest;

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <libgearman/gearman.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

using namespace libtest;

class BlobslapWorker : public Server
{
private:
public:
  BlobslapWorker(in_port_t port_arg) :
    Server("localhost", port_arg)
  { }

  pid_t get_pid(bool error_is_ok)
  {
    if (not pid_file().empty())
    {
      Wait wait(pid_file(), 0);

      if (error_is_ok and not wait.successful())
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;

        return get_pid_from_file(pid_file());
      }
    }

    return -1;
  }

  bool ping()
  {
    if (pid_file().empty())
    {
      Error << "No pid file available";
      return false;
    }

    Wait wait(pid_file(), 0);
    if (not wait.successful())
    {
      Error << "Pidfile was not found:" << pid_file();
      return false;
    }

    pid_t local_pid= get_pid_from_file(pid_file());
    if (local_pid <= 0)
    {
      return false;
    }

    if (::kill(local_pid, 0) == 0)
    {
      return true;
    }

    return false;
  }

  const char *name()
  {
    return "blobslap_worker";
  };

  const char *executable()
  {
    return GEARMAND_BLOBSLAP_WORKER;
  }

  const char *pid_file_option()
  {
    return "--pid-file=";
  }

  const char *daemon_file_option()
  {
    return "--daemon";
  }

  const char *log_file_option()
  {
    return NULL;
  }

  const char *port_option()
  {
    return "--port=";
  }

  bool is_libtool()
  {
    return true;
  }

  bool build(int argc, const char *argv[]);
};


#include <sstream>

bool BlobslapWorker::build(int argc, const char *argv[])
{
  std::stringstream arg_buffer;

  for (int x= 1 ; x < argc ; x++)
  {
    arg_buffer << " " << argv[x] << " ";
  }

  set_extra_args(arg_buffer.str());

  return true;
}

namespace libtest {

Server *build_blobslap_worker(in_port_t try_port)
{
  return new BlobslapWorker(try_port);
}

}
