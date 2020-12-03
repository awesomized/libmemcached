#include "gettimeofday.hpp"

#include <chrono>

#if !defined HAVE_GETTIMEOFDAY
int gettimeofday(struct timeval* tp, struct timezone*) {
  using clock = std::chrono::system_clock;
  auto as_sec = [] (auto d) {
      return std::chrono::duration_cast<std::chrono::seconds>(d);
  };
  auto as_usec = [] (auto d) {
      return std::chrono::duration_cast<std::chrono::microseconds>(d);
  };

  auto now = clock::now().time_since_epoch();
  auto sec = as_sec(now);
  auto usec = as_usec(now - sec);

  tp->tv_sec = sec.count();
  tp->tv_usec = usec.count();
  return 0;
}
#endif
