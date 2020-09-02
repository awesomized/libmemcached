#pragma once

#include "common.hpp"

class ForkAndExec {
public:
  ForkAndExec(const char *binary, char **argv);
  ~ForkAndExec();

  ForkAndExec(const ForkAndExec &) = delete;
  ForkAndExec &operator = (const ForkAndExec &) = delete;
  ForkAndExec(ForkAndExec &&) = default;
  ForkAndExec &operator = (ForkAndExec &&) = default;

  optional<pid_t> operator () ();

private:
  int pipes[2];
  const char *binary;
  char **argv;
};
