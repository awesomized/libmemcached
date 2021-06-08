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

#include <fstream>
#include "mem_config.h"

#define PROGRAM_NAME        "memdump"
#define PROGRAM_DESCRIPTION "Dump all values from one or many servers."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"
#include "common/checks.hpp"

static memcached_return_t print(const memcached_st *, const char *k, size_t l, void *ctx) {
  auto out = static_cast<std::ostream *>(ctx);
  out->write(k, l);
  out->put('\n');
  return MEMCACHED_SUCCESS;
}

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION};

  for (const auto &def : opt.defaults) {
    switch (def.opt.val) {
    case 'H': // no need for --hash
    case 'b': // binary proto not available
      break;
    default:
      opt.add(def);
    }
  }

  opt.add("file", 'f', required_argument, "Output to file instead of standard output.");

  if (!opt.parse(argc, argv)) {
    exit(EXIT_FAILURE);
  }

  if (opt.isset("quiet") && !opt.isset("file")) {
    std::cerr << "--quiet operation was requested, but --file was not set.\n";
    exit(EXIT_FAILURE);
  }

  memcached_st memc;
  if (!check_memcached(opt, memc)) {
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    exit(EXIT_FAILURE);
  }

  memcached_dump_fn cb[1] = {&print};
  std::ofstream outfile{};
  std::ostream *outstream = check_ostream(opt, opt.argof("file"), outfile);

  auto rc = memcached_dump(&memc, cb, outstream, 1);

  if (outfile) {
    if (opt.isset("debug")) {
      std::cerr << "Flushing " << opt.argof("file") << ".\n";
    }
    outfile.flush();
  }

  if (MEMCACHED_SUCCESS != rc) {
    if (!opt.isset("quiet")) {
      std::cerr << "Failed to dump keys:" << memcached_last_error_message(&memc) << "\n";
    }
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  memcached_free(&memc);
  exit(EXIT_SUCCESS);
}
