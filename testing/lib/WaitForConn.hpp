#pragma once

#include "Poll.hpp"

#include <netinet/in.h>
#include <sys/poll.h>

#include <tuple>
#include <vector>

using namespace std;

class WaitForConn {
public:
  using conn_t = tuple<int, sockaddr_storage, unsigned int>;

private:
  vector<conn_t> conns;
  Poll poll;

public:
  explicit
  WaitForConn(vector<conn_t> &&conns_, Poll &&poll_ = Poll{POLLOUT})
  : conns{conns_}
  , poll{poll_}
  { }

  static bool connect(const conn_t &conn);

  bool operator () ();
};
