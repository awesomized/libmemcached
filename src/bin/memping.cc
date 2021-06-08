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

#define PROGRAM_NAME        "memping"
#define PROGRAM_DESCRIPTION "Ping a server or a set of servers."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"
#include "common/checks.hpp"

#include "libmemcached/util.h"

static memcached_return_t ping(const memcached_st *memc, const memcached_instance_st *s, void *ctx) {
  auto opt = static_cast<client_options *>(ctx);
  memcached_return_t rc;
  bool ok;

  if (opt->isset("debug")) {
    std::cerr << "Trying to ping" << s->_hostname << ":" << s->port() << ".\n";
  }

  if (auto username = opt->argof("username")) {
    auto password = opt->argof("password");
    ok = libmemcached_util_ping2(s->_hostname, s->port(), username, password, &rc);
  } else {
    ok = libmemcached_util_ping(s->_hostname, s->port(), &rc);
  }
  if (!ok && !opt->isset("quiet")) {
    std::cerr << "Failed to ping '" << s->_hostname << ":" << s->port()
              << ":" << memcached_strerror(memc, rc) << ".\n";
  }

  return rc;
}

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION};

  for (const auto &def : opt.defaults) {
    switch (def.opt.val) {
    case 'h': // --help
    case 'V': // --version
    case 'v': // --verbose
    case 'd': // --debug
    case 'q': // --quiet
    case 's': // --servers
    case 'u': // --username
    case 'p': // --password
      opt.add(def);
      break;
    default:
      break;
    }
  }

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

  auto exit_code = EXIT_SUCCESS;
  memcached_server_fn cb[1] = {&ping};
  if (!memcached_success(memcached_server_cursor(&memc, cb, &opt, 1))) {
    exit_code = EXIT_FAILURE;
  }

  memcached_free(&memc);
  exit(exit_code);
}
