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

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <getopt.h>
#include <unistd.h>

#include "libmemcached-1.0/memcached.h"
#include "libmemcachedutil-1.0/util.h"
#include "client_options.h"
#include "utilities.h"

#include <iostream>

static bool opt_binary = false;
static int opt_verbose = 0;
static time_t opt_expire = 0;
static char *opt_servers = NULL;
static char *opt_username;
static char *opt_passwd;

#define PROGRAM_NAME        "memping"
#define PROGRAM_DESCRIPTION "Ping a server to see if it is alive"

/* Prototypes */
void options_parse(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  options_parse(argc, argv);

  if (opt_servers == NULL) {
    char *temp;

    if ((temp = getenv("MEMCACHED_SERVERS"))) {
      opt_servers = strdup(temp);
    }

    if (opt_servers == NULL) {
      std::cerr << "No Servers provided" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  int exit_code = EXIT_SUCCESS;
  memcached_server_st *servers = memcached_servers_parse(opt_servers);
  if (servers == NULL or memcached_server_list_count(servers) == 0) {
    std::cerr << "Invalid server list provided:" << opt_servers << std::endl;
    exit_code = EXIT_FAILURE;
  } else {
    for (uint32_t x = 0; x < memcached_server_list_count(servers); x++) {
      memcached_return_t instance_rc;
      const char *hostname = servers[x].hostname;
      in_port_t port = servers[x].port;

      if (opt_verbose) {
        std::cout << "Trying to ping " << hostname << ":" << port << std::endl;
      }

      if (libmemcached_util_ping2(hostname, port, opt_username, opt_passwd, &instance_rc) == false)
      {
        std::cerr << "Failed to ping " << hostname << ":" << port << " "
                  << memcached_strerror(NULL, instance_rc) << std::endl;
        exit_code = EXIT_FAILURE;
      }
    }
  }
  memcached_server_list_free(servers);

  free(opt_servers);

  return exit_code;
}

void options_parse(int argc, char *argv[]) {
  memcached_programs_help_st help_options[] = {
      {0},
  };

  static struct option long_options[] = {
      {(OPTIONSTRING) "version", no_argument, NULL, OPT_VERSION},
      {(OPTIONSTRING) "help", no_argument, NULL, OPT_HELP},
      {(OPTIONSTRING) "quiet", no_argument, NULL, OPT_QUIET},
      {(OPTIONSTRING) "verbose", no_argument, &opt_verbose, OPT_VERBOSE},
      {(OPTIONSTRING) "debug", no_argument, &opt_verbose, OPT_DEBUG},
      {(OPTIONSTRING) "servers", required_argument, NULL, OPT_SERVERS},
      {(OPTIONSTRING) "expire", required_argument, NULL, OPT_EXPIRE},
      {(OPTIONSTRING) "binary", no_argument, NULL, OPT_BINARY},
      {(OPTIONSTRING) "username", required_argument, NULL, OPT_USERNAME},
      {(OPTIONSTRING) "password", required_argument, NULL, OPT_PASSWD},
      {0, 0, 0, 0},
  };

  bool opt_version = false;
  bool opt_help = false;
  int option_index = 0;
  while (1) {
    int option_rv = getopt_long(argc, argv, "Vhvds:", long_options, &option_index);

    if (option_rv == -1)
      break;

    switch (option_rv) {
    case 0:
      break;

    case OPT_BINARY:
      opt_binary = true;
      break;

    case OPT_VERBOSE: /* --verbose or -v */
      opt_verbose = OPT_VERBOSE;
      break;

    case OPT_DEBUG: /* --debug or -d */
      opt_verbose = OPT_DEBUG;
      break;

    case OPT_VERSION: /* --version or -V */
      version_command(PROGRAM_NAME);
      break;

    case OPT_HELP: /* --help or -h */
      help_command(PROGRAM_NAME, PROGRAM_DESCRIPTION, long_options, help_options);
      break;

    case OPT_SERVERS: /* --servers or -s */
      opt_servers = strdup(optarg);
      break;

    case OPT_EXPIRE: /* --expire */
      errno = 0;
      opt_expire = time_t(strtoll(optarg, (char **) NULL, 10));
      if (errno) {
        std::cerr << "Incorrect value passed to --expire: `" << optarg << "`" << std::endl;
        exit(EXIT_FAILURE);
      }
      break;

    case OPT_USERNAME:
      opt_username = optarg;
      opt_binary = true;
      break;

    case OPT_PASSWD:
      opt_passwd = optarg;
      break;

    case OPT_QUIET:
      close_stdio();
      break;

    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }

  if (opt_version) {
    version_command(PROGRAM_NAME);
    exit(EXIT_SUCCESS);
  }

  if (opt_help) {
    help_command(PROGRAM_NAME, PROGRAM_DESCRIPTION, long_options, help_options);
    exit(EXIT_SUCCESS);
  }
}
