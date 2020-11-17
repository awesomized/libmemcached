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

#include "common/options.hpp"
#include "common/checks.hpp"

#include <iostream>
#include <fstream>

memcached_return_t memcat(const client_options &opt, memcached_st *memc, const char *key, std::ostream *ref) {
  memcached_return_t rc;
  uint32_t flags;
  size_t len;
  auto val = memcached_get(memc, key, strlen(key), &len, &flags, &rc);
  auto verbose = opt.isset("verbose");

  if (MEMCACHED_SUCCESS == rc) {
    if (verbose) {
      *ref << "key: " << key << "\n";
    }
    if (opt.isset("flags")) {
      if (verbose) {
      *ref << "flags: ";
      }
      *ref << flags << "\n";
    }
    if (verbose) {
      *ref << "value: ";
    }
    
    ref->write(val, len);

    if (verbose || !opt.isset("file")) {
      *ref << std::endl;
    }

    ref->flush();
  }

  if (val) {
    free(val);
  }
  return rc;
}

int main(int argc, char *argv[]) {
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION, "key [key ...]"};

  for (const auto &def : opt.defaults) {
    opt.add(def);
  }
  opt.add("flags", 'F', no_argument, "Display key flags, too.");
  opt.add("file", 'f', required_argument, "Output to file instead of standard output.");

  char **argp = nullptr;
  if (!opt.parse(argc, argv, &argp)) {
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
      std::ofstream fstream{};
      std::ostream *ostream = check_ostream(opt, opt.argof("file"), fstream);

      if (!check_return(opt, memc, key, memcat(opt, &memc, key, ostream))) {
        exit_code = EXIT_FAILURE;
      }
    }
  }

  memcached_free(&memc);
  exit(exit_code);
}
