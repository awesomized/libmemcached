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

#include "mem_config.h"

#define PROGRAM_NAME        "memexist"
#define PROGRAM_DESCRIPTION "Check for the existence of a key within a memcached cluster."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"

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
  if (!memcached_create(&memc)) {
    if (!opt.isset("quiet")) {
      std::cerr << "Failed to initialize memcached client.\n";
    }
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  if (!*argp) {
    if (!opt.isset("quiet")) {
      std::cerr << "No key(s) provided.\n";
    }
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  auto exit_code = EXIT_SUCCESS;
  for (auto arg = argp; *arg; ++arg) {
    auto rc = memcached_exist(&memc, *arg, strlen(*arg));

    if (MEMCACHED_SUCCESS == rc) {
      if (opt.isset("verbose")) {
        std::cerr << "Found key '" << *arg << "'.\n";
      }
    } else {
      exit_code = EXIT_FAILURE;
      if (opt.isset("verbose")) {
        if (rc == MEMCACHED_NOTFOUND) {
          std::cerr << "Could not find key '" << *arg << "'.\n";
        } else {
          std::cerr << "Fatal error for key '" << *arg << "': "
                    << memcached_last_error_message(&memc) << "\n";
        }
      }
    }
  }

  memcached_free(&memc);
  exit(exit_code);
}
