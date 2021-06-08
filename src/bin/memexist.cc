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

#define PROGRAM_NAME        "memexist"
#define PROGRAM_DESCRIPTION "Check for the existence of a key within a memcached cluster."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"
#include "common/checks.hpp"

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION, "key [key ...]"};

  for (const auto &def : opt.defaults) {
    opt.add(def);
  }

  char **argp = nullptr;
  if (!opt.parse(argc, argv, &argp)) {
    exit(EXIT_FAILURE);
  }

  memcached_st memc;
  if (!check_memcached(opt, memc)) {
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  if (!check_argp(opt, argp, "No key(s) provided.")) {
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  auto exit_code = EXIT_SUCCESS;
  for (auto arg = argp; *arg; ++arg) {
    auto key = *arg;
    if (*key) {
      if (check_return(opt, memc, key, memcached_exist(&memc, *arg, strlen(*arg)))) {
        if (opt.isset("verbose")) {
          std::cerr << "Found key '" << *arg << "'.\n";
        }
      } else {
        exit_code = EXIT_FAILURE;
      }
    }
  }

  memcached_free(&memc);
  exit(exit_code);
}
