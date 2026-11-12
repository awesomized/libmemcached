#include "test/lib/common.hpp"
#include "test/lib/ReturnMatcher.hpp"

#include <algorithm>
#include <atomic>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

/*
 * A memcached proxy (memcached -o proxy_config=...) answers an ASCII multi-get one key at a time.
 * When one key's backend fails, the proxy writes that key's error into the middle of the reply
 * and still sends the remaining keys and the final END:
 *
 *   VALUE a 0 1\r\na\r\n
 *   SERVER_ERROR out of memory writing get response\r\n
 *   VALUE c 0 1\r\nc\r\n
 *   END\r\n
 *
 * memcached_fetch_result() stops at the error. Whatever the proxy sends after it must not be read
 * as the reply to the next command on the same connection.
 *
 * FakeProxy replies that way: keys starting with "oom" get the error, any other key is a hit whose
 * value is the key itself.
 */
class FakeProxy {
public:
  FakeProxy() {
    listener = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE(listener >= 0);
    int on = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    REQUIRE(0 == bind(listener, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)));
    REQUIRE(0 == listen(listener, 8));

    socklen_t len = sizeof(addr);
    REQUIRE(0 == getsockname(listener, reinterpret_cast<sockaddr *>(&addr), &len));
    bound_port = ntohs(addr.sin_port);

    thread = std::thread([this] { serve(); });
  }

  ~FakeProxy() {
    stop = true;
    thread.join();
    for (auto &c : clients) {
      close(c.fd);
    }
    close(listener);
  }

  in_port_t port() const {
    return bound_port;
  }

private:
  struct Client {
    int fd;
    string buffer;
  };

  int listener = -1;
  in_port_t bound_port = 0;
  std::atomic<bool> stop{false};
  vector<Client> clients;
  std::thread thread;

  static string reply_for(const string &line) {
    std::istringstream words{line};
    string command, key, reply;

    words >> command;
    if (command == "quit") {
      return {};
    }
    if (command != "get" && command != "gets") {
      return "ERROR\r\n";
    }
    while (words >> key) {
      if (key.rfind("oom", 0) == 0) {
        reply += "SERVER_ERROR out of memory writing get response\r\n";
      } else {
        reply += "VALUE " + key + " 0 " + std::to_string(key.size()) + "\r\n" + key + "\r\n";
      }
    }
    return reply + "END\r\n";
  }

  void serve() {
    while (!stop) {
      vector<pollfd> fds{{listener, POLLIN, 0}};
      for (auto &c : clients) {
        fds.push_back({c.fd, POLLIN, 0});
      }
      if (poll(fds.data(), fds.size(), 10) <= 0) {
        continue;
      }
      if (fds[0].revents & POLLIN) {
        int fd = accept(listener, nullptr, nullptr);
        if (fd >= 0) {
          clients.push_back({fd, {}});
        }
      }
      for (size_t i = 1; i < fds.size(); ++i) {
        if (!(fds[i].revents & (POLLIN | POLLHUP | POLLERR))) {
          continue;
        }
        auto &c = clients[i - 1];
        char chunk[4096];
        auto n = read(c.fd, chunk, sizeof(chunk));
        if (n <= 0) {
          close(c.fd);
          c.fd = -1;
          continue;
        }
        c.buffer.append(chunk, n);
        for (auto eol = c.buffer.find("\r\n"); eol != string::npos; eol = c.buffer.find("\r\n")) {
          auto reply = reply_for(c.buffer.substr(0, eol));
          c.buffer.erase(0, eol + 2);
          if (!reply.empty() && write(c.fd, reply.data(), reply.size()) < 0) {
            break;
          }
        }
      }
      clients.erase(std::remove_if(clients.begin(), clients.end(), [](const Client &c) { return c.fd < 0; }),
                    clients.end());
    }
  }
};

TEST_CASE("memcached_regression_proxy_multiget_server_error") {
  FakeProxy proxy;
  MemcachedPtr memc_ptr;
  auto memc = *memc_ptr;
  LoneReturnMatcher test{memc};
  auto nonblock = GENERATE(0, 1);

  INFO("nonblock=" << nonblock);
  REQUIRE_SUCCESS(memcached_server_add(memc, "127.0.0.1", proxy.port()));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, nonblock));

  const vector<vector<const char *>> multigets{
      {"a", "oom1", "c"},          // error between two hits
      {"a", "c", "oom1"},          // error after the last hit: only END is left over
      {"a", "oom1", "c", "oom2", "d"}, // two errors: two replies left over
      {"oom1", "b", "c"},          // error before any hit: fetch reports NOTFOUND
  };

  for (const auto &keys : multigets) {
    string label;
    for (auto key : keys) {
      label += string(key) + " ";
    }
    INFO("multi-get: " << label);

    vector<size_t> lengths;
    for (auto key : keys) {
      lengths.push_back(strlen(key));
    }
    REQUIRE_SUCCESS(memcached_mget(memc, keys.data(), lengths.data(), keys.size()));

    memcached_return_t rc;
    memcached_result_st *result;
    while ((result = memcached_fetch_result(memc, nullptr, &rc))) {
      memcached_result_free(result);
    }
    REQUIRE(rc != MEMCACHED_END);

    // the next commands on this connection must get their own replies
    for (auto next : {"next1", "next2", "next3"}) {
      size_t len;
      Malloced value(memcached_get(memc, S(next), &len, nullptr, &rc));
      REQUIRE_SUCCESS(rc);
      REQUIRE(*value);
      REQUIRE(string(*value, len) == string(next));
    }
  }

}
