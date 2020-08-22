#include "WaitForExec.hpp"

#include <cerrno>
#include <cstdio>

#include <fcntl.h>
#include <unistd.h>

#include <system_error>

using namespace std;

WaitForExec::WaitForExec(Poll &&poll_)
: poll{poll_}
{
  if (pipe2(pipes, O_CLOEXEC|O_NONBLOCK)) {
    int error = errno;
    perror("Server::start pipe2()");
    throw system_error(error, system_category());
  }
}

WaitForExec::~WaitForExec() {
  if (pipes[0] != -1) {
    close(pipes[0]);
  }
  if (pipes[1] != -1) {
    close(pipes[1]);
  }
}

bool WaitForExec::operator()()  {
  if (pipes[0] == -1) {
    return false;
  }
  if (pipes[1] != -1) {
    close(pipes[1]);
    pipes[1] = -1;
  }

  return poll(initializer_list<int>{pipes[0]});
}
