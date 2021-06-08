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

#define PROGRAM_NAME        "memcp"
#define PROGRAM_DESCRIPTION "Copy a set of files to a memcached cluster."
#define PROGRAM_VERSION     "1.1"

#include "common/options.hpp"
#include "common/checks.hpp"
#include "p9y/libgen.hpp"
#include "p9y/realpath.hpp"

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <sstream>

#ifndef PATH_MAX
#  ifdef MAX_PATH
#    define PATH_MAX MAX_PATH
#  else
#    define PATH_MAX 256
#  endif
#endif

struct memcp_file {
  enum class type {
    basename,
    relative,
    absolute
  } key;
  enum class mode {
    SET,
    ADD,
    REPLACE
  } op;
  char *path;
  uint32_t flags;
  time_t expire;
};

static inline std::string stream2string(const std::istream &istream) {
  return dynamic_cast<std::ostringstream &&>(std::ostringstream{} << istream.rdbuf()).str();
}

static memcached_return_t memcp(const client_options &opt, memcached_st &memc, const char *key,
                  const memcp_file &file) {
  std::ifstream fstream{};
  std::istream *istream = check_istream(opt, file.path, fstream);

  if (!istream){
    return MEMCACHED_ERROR;
  }

  const char *mode;
  memcached_return_t rc;
  auto data = stream2string(*istream);
  if (file.op == memcp_file::mode::REPLACE) {
    mode = "replace";
    rc = memcached_replace(&memc, key, strlen(key), data.c_str(), data.length(),
        file.expire, file.flags);
  } else if (file.op == memcp_file::mode::ADD) {
    mode = "add";
    rc = memcached_add(&memc, key, strlen(key), data.c_str(), data.length(),
        file.expire, file.flags);
  } else {
    mode = "set";
    rc = memcached_set(&memc, key, strlen(key), data.c_str(), data.length(),
        file.expire, file.flags);
  }

  if (!memcached_success(rc)) {
    auto error = memcached_last_error(&memc)
                 ? memcached_last_error_message(&memc)
                 : memcached_strerror(&memc, rc);
    std::cerr << "Error occurred during memcached_" << mode <<"('" << key << "'): " << error << "\n";
  }
  return rc;
}

static void add_file(std::vector<memcp_file> &files, const client_options &opt, char *file) {
  memcp_file::type type = memcp_file::type::basename;
  memcp_file::mode mode = memcp_file::mode::SET;
  uint32_t flags = 0;
  time_t expire = 0;

  if (opt.isset("absolute")) {
    type = memcp_file::type::absolute;
  } else if (opt.isset("relative")) {
    type = memcp_file::type::relative;
  }

  if (opt.isset("replace")) {
    mode = memcp_file::mode::REPLACE;
  } else if (opt.isset("add")) {
    mode = memcp_file::mode::ADD;
  }

  if (auto flags_str = opt.argof("flags")) {
    flags = std::stoul(flags_str);
  }
  if (auto expire_str = opt.argof("expire")) {
    expire = std::stoul(expire_str);
  }

  if (opt.isset("debug")) {
    auto mode_str = mode == memcp_file::mode::REPLACE ? "REPLACE" : mode == memcp_file::mode::ADD ? "ADD" : "SET";
    std::cerr << "Scheduling " << mode_str << " '" << file << "' (expire=" << expire << ", flags=" << flags << ").\n";
  }

  files.emplace_back(memcp_file{type, mode, file, flags, expire});
}

static bool path2key(const client_options &opt, memcp_file &file, char **path) {
  static char rpath[PATH_MAX + 1];
  if (file.key == memcp_file::type::absolute) {
    *path = realpath(file.path, rpath);
    if (!*path) {
      if (!opt.isset("quiet")) {
        perror(file.path);
      }
      return false;
    }
  } else if (file.key == memcp_file::type::relative) {
    *path = file.path;
  } else {
    *path = basename(file.path);
  }
  return true;
}

int main(int argc, char *argv[]) {
  std::vector<memcp_file> files{};
  client_options opt{PROGRAM_NAME, PROGRAM_VERSION, PROGRAM_DESCRIPTION,
                     "file [file ...]"
                     "\n\t\t\t# NOTE: order of flags and positional"
                     "\n\t\t\t#       arguments matters on GNU systems"};

  opt.add(nullptr, '-', no_argument, "GNU argv extension")
      .parse = [&files](client_options &opt_, client_options::extended_option &ext) {
    add_file(files, opt_, ext.arg);
    return true;
  };

  for (const auto &def : opt.defaults) {
    opt.add(def);
  }

  opt.add("set", 'S', no_argument, "Perform SET operations.")
      .parse = [](client_options &opt_, client_options::extended_option &) {
    opt_.unset("add");
    opt_.unset("replace");
    return true;
  };
  opt.add("add", 'A', no_argument, "Perform ADD operations.")
      .parse = [](client_options &opt_, client_options::extended_option &) {
    opt_.unset("set");
    opt_.unset("replace");
    return true;
  };
  opt.add("replace", 'R', no_argument, "Perform REPLACE operations.")
      .parse = [](client_options &opt_, client_options::extended_option &) {
    opt_.unset("set");
    opt_.unset("add");
    return true;
  };

  opt.add("udp", 'U', no_argument, "Use UDP.")
      .apply = [](const client_options &opt_, const client_options::extended_option &ext, memcached_st *memc) {
    if (MEMCACHED_SUCCESS != memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_USE_UDP, ext.set)) {
      if (!opt_.isset("quiet")) {
        std::cerr << memcached_last_error_message(memc) << "\n";
      }
      return false;
    }
    return true;
  };
  opt.add("flags", 'F', required_argument, "Set key flags, too.");
  opt.add("expire", 'e', required_argument, "Set expire time, too.");

  opt.add("basename", '.', no_argument, "Use basename of path as key (default).");
  opt.add("relative", '+', no_argument, "Use relative path (as passed), instead of basename only.");
  opt.add("absolute", '/', no_argument, "Use absolute path (real path), instead of basename only.");

  // defaults
  opt.set("set");
  opt.set("basename");

  char **argp = nullptr;
  if (!opt.parse(argc, argv, &argp)) {
    exit(EXIT_FAILURE);
  }

  memcached_st memc;
  if (!check_memcached(opt, memc)) {
    exit(EXIT_FAILURE);
  }

  if (!opt.apply(&memc)) {
    exit(EXIT_FAILURE);
  }

  if (files.empty()) {
    if (!check_argp(opt, argp, "No file(s) provided.")) {
      memcached_free(&memc);
      exit(EXIT_FAILURE);
    }
    for (auto arg = argp; *arg; ++arg) {
      add_file(files, opt, *arg);
    }
  }

  auto exit_code = EXIT_SUCCESS;
  for (auto &file : files) {
    char *path = nullptr;
    if (!path2key(opt, file, &path)) {
      exit_code = EXIT_FAILURE;
      continue;
    }

    auto rc = memcp(opt, memc, path, file);
    if (memcached_success(rc)) {
      if (opt.isset("verbose")) {
        std::cout << path << "\n";
      }
    } else {
      exit_code = EXIT_FAILURE;
    }
  }

  if (!check_buffering(opt, memc)) {
    exit_code = EXIT_FAILURE;
  }

  memcached_free(&memc);
  exit(exit_code);
}
