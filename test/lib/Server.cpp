#include "Server.hpp"
#include "Retry.hpp"
#include "ForkAndExec.hpp"

#include <sys/wait.h>
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

Server::Server(string binary_, Server::argv_t args_)
    : binary{move(binary_)}
    , args{move(args_)}
{}

Server::~Server() {
  stop();
  wait();
  if (pipe != -1) {
    close(pipe);
  }
  if (holds_alternative<string>(socket_or_port)) {
    unlink(get<string>(socket_or_port).c_str());
  }
}

static inline string extractArg(const Server::arg_t &arg_cont, const string &func_arg) {
  if (holds_alternative<string>(arg_cont)) {
    return get<string>(arg_cont);
  } else {
    return get<Server::arg_func_t>(arg_cont)(func_arg);
  }
}

static inline void pushArg(vector<char *> &arr, const string &arg) {
  auto len = arg.size();
  auto str = arg.data(), end = str + len + 1;
  auto ptr = new char[len + 1];
  copy(str, end, ptr);
  arr.push_back(ptr);
}

optional<string> Server::handleArg(vector<char *> &arr, const string &arg, const arg_func_t &next_arg) {
  pushArg(arr, arg);
  if (arg == "-U" || arg == "--udp-port") {
    auto port = next_arg(arg);
    pushArg(arr, port);
    pushArg(arr, "-p");
    pushArg(arr, port);
    socket_or_port = stoi(port);
    return port;
  } else if (arg == "-p" || arg == "--port") {
    auto port = next_arg(arg);
    pushArg(arr, port);
    socket_or_port = stoi(port);
    return port;
  } else if (arg == "-s" || arg == "--unix-socket") {
    auto sock = next_arg(arg);
    pushArg(arr, sock);
    socket_or_port = sock;
    return sock;
  } else if (arg == "-S" || arg == "--enable-sasl") {
    sasl = true;
  }
  return {};
}

[[nodiscard]]
vector<char *> Server::createArgv()  {
  vector<char *> arr;

  pushArg(arr, binary);
  pushArg(arr, "-u");
  pushArg(arr, "nobody");

  for (auto it = args.cbegin(); it != args.cend(); ++it) {
    if (holds_alternative<arg_t>(*it)) {
      // a single argument
      auto arg = extractArg(get<arg_t>(*it), binary);
      handleArg(arr, arg, [&it](const string &arg_) {
        return extractArg(get<arg_t>(*++it), arg_);
      });
    } else {
      // an argument pair
      auto &[one, two] = get<arg_pair_t>(*it);
      auto arg_one = extractArg(one, binary);
      auto arg_two = extractArg(two, arg_one);

      auto next = handleArg(arr, arg_one, [&arg_two](const string &) {
        return arg_two;
      });

      if (!next.has_value()) {
        pushArg(arr, arg_two);
      }
    }
  }

  arr.push_back(nullptr);

  return arr;
}

optional<Server::ChildProc> Server::start() {
  if (!pid) {
    auto argv = createArgv();
    ForkAndExec fork_and_exec{binary.c_str(), argv.data()};

    pipe = fork_and_exec.createPipe();
    pid = fork_and_exec();

    for (auto argp : argv) {
      delete [] argp;
    }

    if (!pid) {
      return {};
    }
  }
  return ChildProc{pid, pipe};
}

bool Server::isListening() const {
  MemcachedPtr memc;

  if (holds_alternative<string>(socket_or_port)) {
    if (memcached_server_add_unix_socket(*memc, get<string>(socket_or_port).c_str())) {
      return false;
    }
  } else {
    if (memcached_server_add(*memc, "localhost", get<int>(socket_or_port))) {
      return false;
    }
  }

  if (sasl) {
    memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);
    memcached_set_sasl_auth_data(*memc, "memcached", "memcached");
  }

  Malloced stat(memcached_stat(*memc, nullptr, nullptr));
  if (!*stat || !stat->pid || stat->pid == -1) {
    return false;
  }
  if (stat->pid != pid) {
    cerr << "Another server is listening on " << socket_or_port
         << " (expected pid " << pid << " found pid " << stat->pid << ")\n";
    return false;
  }

  return true;
}

bool Server::ensureListening() {
  if (!start()) {
    return false;
  }
  return Retry{[this] {
    again:
    start();
    if (!isListening()) {
      auto old = pid;
      if (tryWait()) {
        cerr << "Collected zombie " << *this << "(old pid=" << old << ")\n";
        goto again;
      }
    }
    return isListening();
  }}();
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
    if (drain().length() &&
        output.rfind("Signal handled: Terminated", 0) != 0) {
      cerr << "Output of " << *this << ":\n";

      istringstream iss{output};
      string line;

      while (getline(iss, line)) {
        cerr << "  " << line << "\n";
      }

      if (output.back() != '\n') {
        cerr << endl;
      }
      output.clear();
    }
    pid = 0;
    if (pipe != -1) {
      close(pipe);
      pipe = -1;
    }
    return true;
  }
  return false;
}

bool Server::tryWait() {
  return wait(WNOHANG);
}

Server::Server(const Server &s) {
  binary = s.binary;
  args = s.args;
  socket_or_port = s.socket_or_port;
}

Server &Server::operator=(const Server &s) {
  binary = s.binary;
  args = s.args;
  socket_or_port = s.socket_or_port;
  return *this;
}

pid_t Server::getPid() const {
  return pid;
}

const string &Server::getBinary() const {
  return binary;
}

const Server::argv_t &Server::getArgs() const {
  return args;
}

const socket_or_port_t &Server::getSocketOrPort() const {
  return socket_or_port;
}

int Server::getPipe() const {
  return pipe;
}

string &Server::drain() {
  if (pipe != -1) {
    again:
    char read_buf[1<<12];
    auto read_len = read(pipe, read_buf, sizeof(read_buf));

    if (read_len > 0) {
      output.append(read_buf, read_len);
      goto again;
    }
    if (read_len == -1) {
      switch (errno) {
      case EINTR:
        goto again;
      default:
        perror("Server::drain read()");
        [[fallthrough]];
      case EAGAIN:
#if EWOULDBLOCK != EAGAIN
      case EWOULDBLOCK:
#endif
        break;
      }
    }
  }
  return output;
}
