#pragma once

#include "common.hpp"

class ForkAndExec {
public:
  enum mode { READ, WRITE };

  ForkAndExec(const char *binary, char **argv);
  ~ForkAndExec();

  ForkAndExec(const ForkAndExec &) = delete;
  ForkAndExec &operator = (const ForkAndExec &) = delete;
  ForkAndExec(ForkAndExec &&) = default;
  ForkAndExec &operator = (ForkAndExec &&) = default;

  [[nodiscard]]
  int createPipe();
  pid_t operator () ();

private:
  int ready[2], pipes[2];
  const char *binary;
  char **argv;

  bool prepareExecReadyPipe();
  void prepareOutputPipe();
  void closePipe(int &fd);
  void pollExecReadyPipe();
};
