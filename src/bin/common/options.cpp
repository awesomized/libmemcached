/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "options.hpp"

void client_options::print_version() const {
  std::cout << prog_name << " v" << prog_vers << " (libmemcached v" << LIBMEMCACHED_VERSION_STRING << ")"
            << std::endl;
}

void client_options::print_help() const {
  print_version();
  std::cout << "\n\t" << prog_desc << "\n\n";
  std::cout << "Usage:\n\t" << prog_name << " -[";
  for (const auto &opt : options) {
    if (!opt.opt.has_arg && opt.opt.val != '-') {
      std::cout << (char) opt.opt.val;
    }
  }
  std::cout << "] [-";
  for (const auto &ext : options) {
    if (ext.opt.has_arg) {
      std::cout << (char) ext.opt.val;
      if ((&ext) != &*options.rbegin())
        std::cout << '|';
    }
  }
  std::cout << " <arg>] ";

  if (prog_argp) {
    std::cout << prog_argp;
  }
  std::cout << "\n\nOptions:\n";
  for (const auto &ext : options) {
    if (ext.opt.val == '-' || !(ext.opt.val || ext.opt.name)) {
      continue;
    }
    std::cout << "\t";
    if (ext.opt.val) {
      std::cout << "-" << (char) ext.opt.val;
      if (ext.opt.name) {
        std::cout << "|";
      }
    } else {
      std::cout << "   ";
    }
    if (ext.opt.name) {
      std::cout << "--" << ext.opt.name << " ";
    } else {
      std::cout << " ";
    }
    if (ext.opt.has_arg) {
      if (ext.opt.has_arg == optional_argument) {
        std::cout << "[";
      } else {
        std::cout << "<";
      }
      std::cout << "arg";
      if (ext.opt.has_arg == optional_argument) {
        std::cout << "]";
      } else {
        std::cout << ">";
      }
    }
    std::cout << "\n\t\t" << ext.help << "\n";
  }

  if (has("servers")) {
    std::cout << "\nEnvironment:\n";
    std::cout << "\tMEMCACHED_SERVERS=\n";
    std::cout << "\t\tList of servers to use if `-s|--servers` was not provided.\n";
  }
  std::cout << std::endl;
}

bool client_options::parse(int argc, char **argv, char ***argp) {
  /* extern */ optind = 1;
  auto debug = has("debug") ? &get("debug") : nullptr;
  std::string short_opts{};
  std::vector<option> long_opts{};

  short_opts.reserve(options.size() * 3);
  long_opts.reserve(options.size() + 1);

  for (const auto &ext : options) {
    if (ext.opt.val) {
      short_opts.push_back(ext.opt.val);
      for (int i = 0; i < ext.opt.has_arg; ++i) {
        short_opts.push_back(':');
      }
    }
    if (ext.opt.name) {
      long_opts.push_back(ext.opt);
    }
  }
  long_opts.push_back({});

  while (true) {
    auto opt = getopt_long(argc, argv, short_opts.c_str(), long_opts.data(), nullptr);

    if (debug->set && opt > 0) {
      std::cerr << "Processing option '" << (char) opt << "' (" << opt << ")\n";
    }
    if (opt == '?') {
      return false;
    }
    if (opt == -1) {
      if (argp) {
        *argp = &argv[optind];
      }
      return true;
    }

    auto &ext_opt = get(opt);

    // grab optional argument
    if (ext_opt.opt.has_arg == optional_argument && !optarg) {
      auto next_arg = argv[optind];

      if (next_arg && *next_arg && *next_arg != '-') {
        optarg = next_arg;
        ++optind;
      }
    }

    ext_opt.set = true;
    ext_opt.arg = optarg;

    if (ext_opt.parse) {
      if (!ext_opt.parse(*this, ext_opt)) {
        return false;
      }
    }
  }
}

bool client_options::apply(memcached_st *memc) {
#ifdef _WIN32
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
    std::cerr << "Socket Initialization Error.\n";
    return false;
  }
#endif // _WIN32

  extended_option *servers = nullptr;
  for (auto &opt : options) {
    if (opt.apply) {
      // servers should be applied last, so they take up any behaviors previously set
      if (opt.opt.val == 's' && opt.opt.name == std::string("servers")) {
        servers = &opt;
        continue;
      }
      if (!opt.apply(*this, opt, memc)) {
        return false;
      }
    }
  }
  if (servers) {
    if (!servers->apply(*this, *servers, memc)) {
      return false;
    }
  }
  return true;
}
