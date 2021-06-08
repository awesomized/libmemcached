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

#include "mem_config.h"

#define PROGRAM_NAME        "memerror"
#define PROGRAM_DESCRIPTION "Translate a memcached error code into a string."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"
#include "common/checks.hpp"

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION, "code [code ...]"};

  for (const auto &def : opt.defaults) {
    switch (def.opt.val) {
    case 'h': // --help
    case 'V': // --version
    case 'v': // --verbose
    case 'd': // --debug
    case 'q': // --quiet
      opt.add(def);
      break;
    default:
      break;
    }
  }

  char **argp = nullptr;
  if (!opt.parse(argc, argv, &argp)) {
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(nullptr)) {
    exit(EXIT_FAILURE);
  }

  if (!check_argp(opt, argp, "No error code(s) provided.")) {
    exit(EXIT_FAILURE);
  }

  auto exit_code = EXIT_SUCCESS;
  for (auto arg = argp; *arg; ++arg) {
    auto code = std::stoul(*arg);
    auto rc = static_cast<memcached_return_t>(code);

    if (opt.isset("verbose")) {
      std::cout << "code: " << code << "\n";
      std::cout << "name: ";
    }
    if (rc >= MEMCACHED_MAXIMUM_RETURN) {
      exit_code = EXIT_FAILURE;
      std::cerr << memcached_strerror(nullptr, rc) << std::endl;
    } else {
      std::cout << memcached_strerror(nullptr, rc) << std::endl;
    }
  }

  exit(exit_code);
}
