#pragma once

#include "common.hpp"

class Retry {
public:

  using predicate = function<bool()>;

  explicit Retry(predicate &&pred_, unsigned max_ = 10, chrono::milliseconds sleep_for_ = 20ms);

  bool operator () ();

private:
  unsigned max;
  chrono::milliseconds sleep_for;
  predicate pred;
};

