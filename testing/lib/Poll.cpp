#include "Poll.hpp"

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <iostream>

#include <poll.h>

using namespace std;

bool Poll::operator() (const vector<int> &fds)  {
  vector<pollfd> pfds;

  pfds.reserve(fds.size());
  for (auto fd : fds) {
    pfds.emplace_back(pollfd{fd, events, 0});
  }

  while (!pfds.empty() && timeout <= max) {
    auto nfds = poll(pfds.data(), pfds.size(), timeout);

    if (nfds == -1) {
      perror("Poll::() poll()");
      return false;
    }

    /* timeout */
    if (!nfds) {
      timeout = ceil(static_cast<float>(timeout) * growth);
      continue;
    }

    auto pred = [](const pollfd &pfd){
      return pfd.revents & POLLHUP
          || pfd.revents & POLLERR
          || pfd.revents & pfd.events;
    };
    auto iter = remove_if(pfds.begin(), pfds.end(), pred);
    pfds.erase(iter, pfds.end());
  }

  return pfds.empty();
}
