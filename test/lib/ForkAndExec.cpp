#include "ForkAndExec.hpp"
#include "p9y/poll.hpp"

#include <cstdio>
#include <fcntl.h>
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <spawn.h>

#if !HAVE_PIPE2
static inline int setfl(int fd, int newflags) {
  auto oldflags = fcntl(fd, F_GETFL);
  return fcntl(fd, F_SETFL, oldflags | newflags);
}
static inline int pipe2(int pipefd[2], int flags) {
  int rc;

  rc = pipe(pipefd);
  if (0 > rc) {
    return rc;
  }
  rc = setfl(pipefd[0], flags);
  if (0 > rc) {
    return rc;
  }
  return setfl(pipefd[1], flags);
}
#endif

ForkAndExec::ForkAndExec(const char *binary_, char **argv_)
: ready{-1, -1}
, pipes{-1, -1}
, binary{binary_}
, argv{argv_}
{
}

ForkAndExec::~ForkAndExec() {
  for (auto &pipe : {ready, pipes}) {
    for (auto m : {READ, WRITE}) {
      closePipe(pipe[m]);
    }
  }
}

int ForkAndExec::createPipe() {
  if (pipes[mode::READ] == -1) {
    if(pipe2(pipes, O_NONBLOCK)) {
      perror("ForkAndExec pipe()");
      return -1;
    }
  }

  return pipes[mode::READ];
}

pid_t ForkAndExec::operator()()  {
  if (!prepareExecReadyPipe()) {
    return 0;
  }

  switch (pid_t pid = fork()) {
    case 0:
      closePipe(pipes[mode::READ]);
      prepareOutputPipe();
      execvp(binary, argv);
      perror("ForkAndExec exec()");
      _exit(EXIT_FAILURE);

    case -1:
      perror("ForkAndExec fork()");
      return 0;

    default:
      pipes[mode::READ] = -1;
      closePipe(pipes[mode::WRITE]);
      pollExecReadyPipe();
      return pid;
  }
}

bool ForkAndExec::prepareExecReadyPipe() {
  if (ready[mode::READ] == -1) {
    if (pipe2(ready, O_CLOEXEC | O_NONBLOCK)) {
      perror("ForkAndExec pipe()");
      return false;
    }
    closePipe(ready[mode::WRITE]);
  }

  return true;
}

void ForkAndExec::prepareOutputPipe() {
  if (pipes[mode::WRITE] != -1) {
    if (0 > dup2(pipes[mode::WRITE], STDERR_FILENO) ||
        0 > dup2(pipes[mode::WRITE], STDOUT_FILENO)) {
      perror("ForkAndExec dup()");
    }
  }
}

void ForkAndExec::closePipe(int &fd) {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
}

void ForkAndExec::pollExecReadyPipe() {
#if __APPLE__
  char c, n = 50;
  do {
    if (0 == read(ready[mode::READ], &c, 1)) {
      return;
    }
    this_thread::sleep_for(100ms);
  } while (errno == EAGAIN && n--);
#else
  pollfd fd{ready[mode::READ], 0, 0};
  if (1 > poll(&fd, 1, 5000)) {
    cerr << "exec() timed out" << endl;
  }
#endif
}
