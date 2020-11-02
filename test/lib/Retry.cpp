#include "Retry.hpp"

Retry::Retry(predicate pred_, unsigned int max_, chrono::milliseconds sleep_for_)
: max{max_}
, sleep_for{sleep_for_}
, pred{move(pred_)}
{}

bool Retry::operator()() {
  auto cnt = max;
  auto dur = sleep_for;

  while (cnt--) {
    if (pred()) {
      return true;
    }
    this_thread::sleep_for(dur);
    dur += dur / growth;
  }

  return false;
}
