#pragma once

#include <vector>

using namespace std;

class Poll {
private:
  short events;
  int timeout, max;
  float growth;

public:
  explicit Poll(short events_ = 0, int timeout_ = 20, int max_ = 1000, float growth_ = 1.1)
  : events{events_}
  , timeout{timeout_}
  , max{max_}
  , growth{growth_}
  {}

  bool operator () (const vector<int> &fds);
};
