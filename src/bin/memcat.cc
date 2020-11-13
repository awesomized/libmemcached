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

#define PROGRAM_NAME        "memcat"
#define PROGRAM_DESCRIPTION "Cat a set of key values to stdout."
#define PROGRAM_VERSION     "1.1"

#include "libmemcached/common.h"
#include "common/options.hpp"

#include <iostream>
#include <fstream>

memcached_return_t memcat(const client_options &opt, memcached_st *memc, const char *key, std::ostream &ref) {
  memcached_return_t rc;
  uint32_t flags;
  size_t len;
  auto val = memcached_get(memc, key, strlen(key), &len, &flags, &rc);

  if (MEMCACHED_SUCCESS == rc) {
    if (opt.flags.verbose) {
      ref << "key: " << key << "\n";
    }
    if (opt.flags.flags) {
      ref << "flags: " << flags << "\n";
    }
    if (opt.flags.verbose) {
      ref << "value: ";
    }
    ref.write(val, len);
    ref << std::endl;
  }

  if (val) {
    free(val);
  }
  return rc;
}

int main(int argc, char *argv[]) {
  memcached_st memc;
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION, {
      client_options::flag::help,
      client_options::flag::version,
      client_options::flag::verbose,
      client_options::flag::debug,
      client_options::flag::quiet,
      client_options::flag::servers,
      client_options::flag::binary,
      client_options::flag::username,
      client_options::flag::password,
      client_options::flag::hash,
      client_options::flag::flags,
      client_options::flag::file,
  }};

  if (!opt.parse(argc, argv)) {
    exit(EXIT_FAILURE);
  }
  if (opt.flags.help) {
    opt.printHelp("key [ key ... ]");
    exit(EXIT_SUCCESS);
  }
  if (opt.flags.version) {
    opt.printVersion();
    exit(EXIT_SUCCESS);
  }

  if (opt.flags.quiet && !opt.args.file) {
    std::cerr << "--quiet operation was requested, but --file was not set.\n";
    exit(EXIT_FAILURE);
  }

  if (!memcached_create(&memc)) {
    if (!opt.flags.quiet) {
      std::cerr << "Failed to initialize memcached client.\n";
    }
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    memcached_free(&memc);
    exit(EXIT_FAILURE);
  }

  if (!*opt.args.positional) {
    if (!opt.flags.quiet) {
      std::cerr << "No key(s) provided.\n";
      memcached_free(&memc);
      exit(EXIT_FAILURE);
    }
  }

  auto exit_code = EXIT_SUCCESS;
  for (auto arg = opt.args.positional; *arg; ++arg) {
    auto key = *arg;
    if (*key) {
      memcached_return_t rc;
      if (opt.args.file && *opt.args.file) {
        std::ofstream file{opt.args.file, std::ios::binary};
        rc = memcat(opt, &memc, key, file);
      } else {
        rc = memcat(opt, &memc, key, std::cout);
      }
      if (MEMCACHED_SUCCESS != rc) {
        exit_code = EXIT_FAILURE;

        if (MEMCACHED_NOTFOUND == rc) {
          if (opt.flags.verbose) {
            std::cerr << "not found: " << key << "\n";
          }
          // continue;
        } else {
          if (!opt.flags.quiet) {
            std::cerr << memcached_last_error_message(&memc) << "\n";
          }
          break;
        }
      }
    }
  }

  memcached_free(&memc);
  exit(exit_code);
}
