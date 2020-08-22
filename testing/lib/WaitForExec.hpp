#pragma once

#include "Poll.hpp"

class WaitForExec {
private:
  Poll poll;
  int pipes[2];

public:
  explicit WaitForExec(Poll &&poll = Poll{0});
  ~WaitForExec();

  WaitForExec(const WaitForExec &) = delete;
  WaitForExec(WaitForExec &&) = default;

  bool operator () ();
};
