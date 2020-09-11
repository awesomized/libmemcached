#pragma once

#include "testing/lib/common.hpp"

class Shell {
public:
  explicit Shell(bool redirect_stderr = true);
  explicit Shell(const string &prefix, bool redirect_stderr = true);
  bool run(const string &command, string &output);
  bool run(const string &command);
private:
  string prefix;
  bool redirect;

  string prepareCommand(const string &command_);
};
