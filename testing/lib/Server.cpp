#include "Server.hpp"
#include "WaitForExec.hpp"
#include "WaitForConn.hpp"

#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <tuple>

using namespace std;

[[nodiscard]]
auto Server::createArgv()  {
  auto i = 0, port = -1, socket = -1;
  auto arr = new char *[str_args.size() + dyn_args.size()*2 + 2] {
      strdup(binary.c_str())
  };

  for (auto &arg : str_args) {
    arr[++i] = strdup(arg.c_str());
    if (arg == "-p") {
      port = i + 1;
    } else if (arg == "-s") {
      socket = i + 1;
    }
  }
  for (auto &arg : dyn_args) {
    arr[++i] = strdup(arg.first.c_str());
    arr[++i] = strdup(arg.second(arg.first).c_str());
    if (arg.first == "-p") {
      port = i;
    } else if (arg.first == "-s") {
      socket = i;
    }

  }
  arr[i+1] = nullptr;

  if (socket > -1) {
    socket_or_port = arr[socket];
  } else if (port > -1) {
    socket_or_port = stoi(arr[port]);
  } else {
    socket_or_port = 11211;
  }

  return arr;
}

[[nodiscard]]
optional<WaitForConn::conn_t> Server::createSocket() {
  sockaddr_storage addr;
  unsigned size = 0;
  int sock;

  if (holds_alternative<string>(socket_or_port)) {
    const auto path = get<string>(socket_or_port);
    const auto safe = path.c_str();
    const auto zlen = path.length() + 1;
    const auto ulen = sizeof(sockaddr_un) - sizeof(sa_family_t);

    if (zlen >= ulen) {
      cerr << "Server::isListening socket(): path too long '" << path << "'\n";
      return {};
    }

    if (0 > (sock = socket(AF_UNIX, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0))) {
      perror("Server::isListening socket()");
      return {};
    }

    auto sa = reinterpret_cast<sockaddr_un *>(&addr);
    sa->sun_family = AF_UNIX;
    strncpy(sa->sun_path, safe, zlen);

    size = sizeof(*sa);
  } else {
    if (0 > (sock = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0))) {
      perror("Server::isListening socket()");
      return {};
    }

    const auto port = get<int>(socket_or_port);
    auto sa = reinterpret_cast<struct sockaddr_in *>(&addr);
    sa->sin_family = AF_INET;
    sa->sin_port = htons(static_cast<unsigned short>(port)),
    sa->sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    size = sizeof(*sa);
  }

  return optional<WaitForConn::conn_t>{make_tuple(sock, addr, size)};
}

optional<pid_t> Server::start() {
  if (pid) {
    return pid;
  }

  WaitForExec wait_for_exec;

  switch (pid = fork()) {
    case 0:
      execvp(binary.c_str(), createArgv());
      [[fallthrough]];
    case -1:
      perror("Server::start fork() & exec()");
      return {};

    default:
      if (!wait_for_exec()) {
        cerr << "Server::start exec(): incomplete\n";
      }
      return pid;
  }
}

bool Server::isListening(int max_timeout) {
  auto conn = createSocket();

  if (!conn) {
    return false;
  }

  WaitForConn wait_for_conn{
    {conn.value()},
    Poll{POLLOUT, 2, max_timeout}
  };
  return wait_for_conn();
}

bool Server::stop() {
  if (!pid) {
    return true;
  }
  if (signalled[SIGTERM]) {
    return signal(SIGKILL);
  } else {
    return signal(SIGTERM);
  }
}

bool Server::signal(int signo) {
  if (!pid) {
    return false;
  }
  signalled[signo] += 1;
  return 0 <= kill(pid, signo);
}

bool Server::check() {
  return signal(0);
}

bool Server::wait(int flags) {
  if (pid && pid == waitpid(pid, &status, flags)) {
    pid = 0;
    return true;
  }
  return false;
}

bool Server::tryWait() {
  return wait(WNOHANG);
}
