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

#define PROGRAM_NAME        "memflush"
#define PROGRAM_DESCRIPTION "Erase all data in a cluster of memcached servers."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"
#include "common/checks.hpp"

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION};

  for (const auto &def : opt.defaults) {
    switch (def.opt.val) {
    case 'H': // no need for --hash
      break;
    default:
      opt.add(def);
      break;
    }
  }

  opt.add("expire", 'e', required_argument, "Flush based on expire time.");

  if (!opt.parse(argc, argv, nullptr)) {
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

  time_t expire = 0;
  if (auto exp_str = opt.argof("expire")) {
    expire = std::stoul(exp_str);
  }

  auto exit_code = EXIT_SUCCESS;
  memcached_return_t rc = memcached_flush(&memc, expire);
  if (!memcached_success(rc)) {
    exit_code = EXIT_FAILURE;
    if (!opt.isset("quiet")) {
      std::cerr << "Failed to flush server: " << memcached_last_error_message(&memc) << "\n";
    }
  }

  if (!check_buffering(opt, memc)) {
    exit_code = EXIT_FAILURE;
  }

  memcached_free(&memc);
  exit(exit_code);
}
