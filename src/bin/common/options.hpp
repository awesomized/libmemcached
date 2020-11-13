/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include <cstdint>
#include <climits>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

struct client_options {
  static const int first_flag = '/';
  static const int last_flag = 'z';
  static const int flag_count = last_flag - first_flag + 1;

  enum class flag : int {
    _success = -1,
    _failure = '?',

    absolute = '/',
    flush = '0',
    add_cmd = 'A',
    buffer = 'B',
    concurrency = 'C',
    flags = 'F',
    hash = 'H',
    load = 'L',
    tcp_nodelay = 'N',
    query = 'Q',
    replace_cmd = 'R',
    set_cmd = 'S',
    udp = 'U',
    version = 'V',
    analyze = 'a',
    binary = 'b',
    debug = 'd',
    expire = 'e',
    file = 'f',
    help = 'h',
    non_block = 'n',
    password = 'p',
    quiet = 'q',
    repeat = 'r',
    servers = 's',
    test = 't',
    username = 'u',
    verbose = 'v',
    zero = 'z',
  };

  struct flags {
    int absolute;
    int flush;
    int add_cmd;
    int buffer;
    int flags;
    int tcp_nodelay;
    int replace_cmd;
    int set_cmd;
    int udp;
    int version;
    int analyze;
    int binary;
    int debug;
    int help;
    int non_block;
    int quiet;
    int verbose;
    int zero;
  } flags;

  struct args {
    unsigned long concurrency;
    const char *hash;
    unsigned long load;
    const char *query;
    const char *file;
    unsigned long expire;
    const char *password;
    unsigned long repeat;
    const char *servers;
    const char *test;
    const char *username;
    const char * const *positional;
  } args;

  option avail[flag_count];
  const char *help[flag_count];

  std::string short_opts;
  std::vector<struct option> long_opts;

  const char *progname;
  const char *progvers;
  const char *progdesc;

  client_options(const char *prg, const char *ver, const char *dsc, const std::vector<client_options::flag> &f)
  : flags{}
  , args{}
  , avail{}
  , help{}
  , short_opts{}
  , long_opts{}
  , progname{prg}
  , progvers{ver}
  , progdesc{dsc}
  {

# define opt_flag(s, a, h) opt_flag_ex(#s, s, a, h)
# define opt_flag_ex(n, v, a, h) \
  [static_cast<int>(flag::v)-first_flag] = {n, a, &flags.v, static_cast<int>(flag::v)}; \
    help[static_cast<int>(flag::v)-first_flag] = h

# define opt_data(s, a, h) opt_data_ex(#s, s, a, h)
# define opt_data_ex(n, v, a, h) \
  [static_cast<int>(flag::v)-first_flag] = {n, a, nullptr, static_cast<int>(flag::v)}; \
    help[static_cast<int>(flag::v)-first_flag] = h

    avail opt_flag(                 absolute,   no_argument,      "Use absolute path names.");
    avail opt_flag(                 flush,      no_argument,      "Flush the server(s).");
    avail opt_flag_ex("add",        add_cmd,    no_argument,      "Perform ADD operations.");
    avail opt_flag(                 buffer,     no_argument,      "Buffer requests.");
    avail opt_data(                 concurrency,required_argument,"Level of concurrency.");
    avail opt_flag(                 flags,      no_argument,      "Print or set associated flags for key(s).");
    avail opt_data(                 hash,       required_argument,"Select key hash.");
    avail opt_data(                 load,       required_argument,"Initial load.");
    avail opt_flag_ex("tcp-nodelay",tcp_nodelay,no_argument,      "Disable Nagle's algorithm.");
    avail opt_data(                 query,      no_argument,      "Query arguments.");
    avail opt_flag_ex("replace",    replace_cmd,no_argument,      "Perform REPLACE operations.");
    avail opt_flag_ex("set",        set_cmd,    no_argument,      "Perform SET operations.");
    avail opt_flag(                 udp,        no_argument,      "Use UDP.");
    avail opt_flag(                 version,    no_argument,      "Print program version.");
    avail opt_flag(                 analyze,    no_argument,      "Analyze the server's statistics.");
    avail opt_flag(                 binary,     no_argument,      "Use the binary memcached protocol.");
    avail opt_flag(                 debug,      no_argument,      "Print output useful only for debugging.");
    avail opt_data(                 expire,     required_argument,"Set associated expiry time for key(s).");
    avail opt_data(                 file,       required_argument,"File to save to or read from.");
    avail opt_flag(                 help,       no_argument,      "Print this help.");
    avail opt_flag_ex("non-block",  non_block,  no_argument,      "Use non blocking connections.");
    avail opt_data(                 password,   required_argument,"SASL password.");
    avail opt_flag(                 quiet,      no_argument,      "Print no output, not even errors.");
    avail opt_data(                 repeat,     required_argument,"Repeat operation n times.");
    avail opt_data(                 servers,    required_argument,"List of servers to connect to.");
    avail opt_data(                 test,       required_argument,"Test to perform.");
    avail opt_data(                 username,   required_argument,"SASL username.");
    avail opt_flag(                 verbose,    no_argument,      "Print more informational output.");
    avail opt_flag(                 zero,       no_argument,      "Zero.");

    long_opts.reserve(f.size() + 1);
    short_opts.reserve(f.size() * 3);

    for (auto o : f) {
      auto &opt = avail[static_cast<int>(o)-first_flag];

      long_opts.push_back(opt);
      short_opts.push_back(opt.val);

      for (int i = 0; i < opt.has_arg; ++i) {
        short_opts.push_back(':');
      }
    }

    long_opts.push_back(option{});
  }

  void printVersion() const {
    std::cout << progname << " v" << progvers
              << " (libmemcached v" << LIBMEMCACHED_VERSION_STRING << ")"
              << std::endl;
  }

  void printHelp(const char *positional_args, const char *extra_doc = nullptr) const {
    printVersion();
    std::cout << "\n\t" << progdesc << "\n\n";
    std::cout << "Usage:\n\t" << progname << " -[";
    for (auto &opt : long_opts) {
      if (!opt.has_arg) {
        std::cout << (char) opt.val;
      }
    }
    std::cout << "] [--";
    for (auto &opt : long_opts) {
      if (opt.has_arg) {
        std::cout << (char) opt.val;
        if ((&opt)+1 != &*long_opts.rbegin())
        std::cout << '|';
      }
    }
    std::cout << " <arg>] ";

    std::cout << positional_args << "\n\n";
    std::cout << "Options:\n";
    for (auto &opt : long_opts) {
      if (opt.name) {
        std::cout << "\t-" << (char) opt.val << "|--" << opt.name;
        if (opt.has_arg) {
          if (opt.has_arg == optional_argument) {
            std::cout << "[";
          }
          std::cout << "=arg";
          if (opt.has_arg == optional_argument) {
            std::cout << "]";
          }
        }
        std::cout << "\n\t\t" << help[opt.val - first_flag];
        std::cout << std::endl;
      }
    }

    if (extra_doc) {
      std::cout << extra_doc << std::endl;
    }
  }

  void printLastError(memcached_st *memc) const {
    if (!flags.quiet) {
      std::cerr << memcached_last_error_message(memc);
    }
  }

  memcached_hash_t getHash() const {
    memcached_hash_t hash = MEMCACHED_HASH_DEFAULT;
    if (args.hash && *args.hash) {
      if (flags.verbose) {
        std::cerr << "Checking for hash '" << args.hash << "'.\n";
      }
      for (int h = hash; h < MEMCACHED_HASH_MAX; ++h) {
        auto hash_type = static_cast<memcached_hash_t>(h);
        std::string hash_string{libmemcached_string_hash(hash_type)};
        std::string hash_wanted{args.hash};

        if (hash_wanted.length() == hash_string.length()) {
          auto ci = std::equal(hash_string.begin(), hash_string.end(), hash_wanted.begin(), [](int a, int b) {
            return std::tolower(a) == std::tolower(b);
          });
          if (ci) {
            hash = hash_type;
            break;
          }
        }
      }
      if (hash == MEMCACHED_HASH_DEFAULT) {
        if (!flags.quiet) {
          std::cerr << "Could not find hash '" << args.hash << "'.\n";
        }
      }
    }
    return hash;
  }

  memcached_server_st *getServers() {
    if (!args.servers) {
      if (flags.verbose) {
        std::cerr << "Checking environment for a server list in MEMCACHED_SERVERS.\n";
      }
      args.servers = getenv("MEMCACHED_SERVERS");
      if (!args.servers || !*args.servers) {
        if (!flags.quiet) {
          std::cerr << "No servers provided.\n";
        }
        return nullptr;
      }
    }

    auto servers = memcached_servers_parse(args.servers);
    if (!servers || !memcached_server_list_count(servers)) {
      if (!flags.quiet) {
        std::cerr << "Invalid server list provided: '" << args.servers << "'\n";
      }
      if (servers) {
        memcached_server_list_free(servers);
      }
      return nullptr;
    }

    return servers;
  }

  bool setupServers(memcached_st *memc) {
    auto servers = getServers();
    if (!servers) {
      return false;
    }
    if (MEMCACHED_SUCCESS != memcached_server_push(memc, servers)) {
      printLastError(memc);
      memcached_server_list_free(servers);
      return false;
    }
    memcached_server_list_free(servers);
    return true;
  }

  bool setupBehavior(memcached_st *memc) const {
    if (MEMCACHED_SUCCESS != memcached_behavior_set_key_hash(memc, getHash())) {
      goto failure;
    }
    if (MEMCACHED_SUCCESS != memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, flags.binary)) {
      goto failure;
    }
    if (MEMCACHED_SUCCESS != memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, flags.buffer)) {
      goto failure;
    }
    if (MEMCACHED_SUCCESS != memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, flags.tcp_nodelay)) {
      goto failure;
    }
    if (MEMCACHED_SUCCESS != memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, flags.non_block)) {
      goto failure;
    }
    if (MEMCACHED_SUCCESS != memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_USE_UDP, flags.udp)) {
      goto failure;
    }
    return true;

  failure:
    printLastError(memc);
    return false;
  }

  bool setupSASL(memcached_st *memc) const {
    if (args.username) {
      if (!LIBMEMCACHED_WITH_SASL_SUPPORT) {
        if (!flags.quiet) {
          std::cerr << "SASL username was supplied, but binary was not built with SASL support.\n";
          return false;
        }
      }
      if (MEMCACHED_SUCCESS != memcached_set_sasl_auth_data(memc, args.username, args.password)) {
        printLastError(memc);
        return false;
      }
    }
    return true;
  }

  bool apply(memcached_st *memc) {
    if (!setupBehavior(memc)) {
      return false;
    }
    if (!setupServers(memc)) {
      return false;
    }
    if (!setupSASL(memc)) {
      return false;
    }
#if defined(_WIN32)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData)) {
      std::cerr << "Socket Initialization Error.\n";
      return false;
    }
#endif // #if defined(_WIN32)

    return true;
  }

  bool parse(int argc, char *argv[]) {
    optind = 1;

    while (true) {
      auto opt = getopt_long(argc, argv, short_opts.c_str(), long_opts.data(), nullptr);

      if (flags.debug && opt != -1 && opt != '?') {
        std::cerr << "Processing option '" << (char) opt << "'\n";
      }

      switch (static_cast<flag>(opt)) {
      case flag::_success:
        args.positional = &argv[optind];
        return true;
      case flag::_failure:
        return false;
      case flag::absolute:
        flags.absolute = true;
        break;
      case flag::flush:
        flags.flush = true;
        break;
      case flag::add_cmd:
        flags.add_cmd = true;
        flags.replace_cmd = false;
        flags.set_cmd = false;
        break;
      case flag::buffer:
        flags.buffer = true;
        break;
      case flag::concurrency:
        args.concurrency = std::stoul(optarg);
        break;
      case flag::flags:
        flags.flags = true;
        break;
      case flag::hash:
        args.hash = optarg;
        break;
      case flag::load:
        args.load = std::stoul(optarg);
        break;
      case flag::tcp_nodelay:
        flags.tcp_nodelay = true;
        break;
      case flag::query:
        args.query = optarg;
        break;
      case flag::replace_cmd:
        flags.add_cmd = false;
        flags.replace_cmd = true;
        flags.set_cmd = false;
        break;
      case flag::set_cmd:
        flags.add_cmd = false;
        flags.replace_cmd = false;
        flags.set_cmd = true;
        break;
      case flag::udp:
        flags.udp = true;
        break;
      case flag::version:
        flags.version = true;
        break;
      case flag::analyze:
        flags.analyze = true;
        break;
      case flag::binary:
        flags.binary = true;
        break;
      case flag::debug:
        flags.debug = true;
        flags.quiet = false;
        flags.verbose = true;
        break;
      case flag::expire:
        args.expire = std::stoul(optarg);
        break;
      case flag::file:
        args.file = optarg;
        break;
      case flag::help:
        flags.help = true;
        break;
      case flag::non_block:
        flags.non_block = true;
        break;
      case flag::password:
        args.password = optarg;
        break;
      case flag::quiet:
        flags.debug = false;
        flags.quiet = true;
        flags.verbose = false;
        break;
      case flag::repeat:
        args.repeat = std::stoul(optarg);
        break;
      case flag::servers:
        args.servers = optarg;
        break;
      case flag::test:
        args.test = optarg;
        break;
      case flag::username:
        args.username = optarg;
        break;
      case flag::verbose:
        flags.quiet = false;
        flags.verbose = true;
        break;
      case flag::zero:
        flags.zero = true;
        break;
      }
    }
  }
};
