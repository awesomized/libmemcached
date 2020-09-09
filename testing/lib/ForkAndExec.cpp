#include "ForkAndExec.hpp"

#include <cerrno>
#include <cstdio>

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

ForkAndExec::ForkAndExec(const char *binary_, char **argv_)
: binary{binary_}
, argv{argv_}
{
  if (pipe2(ready, O_CLOEXEC | O_NONBLOCK)) {
    int error = errno;
    perror("Server::start pipe2()");
    throw system_error(error, system_category());
  }
}

ForkAndExec::~ForkAndExec() {
  if (ready[0] != -1) {
    close(ready[0]);
  }
  if (ready[1] != -1) {
    close(ready[1]);
  }
}

optional<pid_t> ForkAndExec::operator()()  {
  if (ready[0] == -1) {
    return {};
  }
  if (ready[1] != -1) {
    close(ready[1]);
    ready[1] = -1;
  }

  switch (pid_t pid = fork()) {
    case 0:
      execvp(binary, argv);
      [[fallthrough]];
    case -1:
      perror("fork() && exec()");
      return {};

    default:
      pollfd fd{ready[0], 0, 0};
      if (1 > poll(&fd, 1, 5000)) {
        cerr << "exec() timed out" << endl;
      }
      return pid;
  }
}
