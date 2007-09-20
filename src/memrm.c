#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <memcached.h>
#include "client_options.h"

static int opt_verbose;
static char *opt_servers;

int main(int argc, char *argv[])
{
  memcached_st *memc;
  char *string;
  size_t string_length;
  time_t expires = 0;
  memcached_return rc;

  static struct option long_options[] =
    {
      {"version", no_argument, NULL, OPT_VERSION},
      {"help", no_argument, NULL, OPT_HELP},
      {"verbose", no_argument, &opt_verbose, 1},
      {"debug", no_argument, &opt_verbose, 2},
      {"servers", required_argument, NULL, OPT_SERVERS},
      {"expire", required_argument, NULL, OPT_EXPIRE},
      {0, 0, 0, 0},
    };
  int option_index = 0;
  int option_rv;

  while (1) 
  {
    option_rv = getopt_long(argc, argv, "Vhvds:", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv) {
    case 0:
      break;
    case OPT_VERSION: /* --version or -V */
      printf("memcache tools, memrm, v1.0\n");
      exit(0);
      break;
    case OPT_HELP: /* --help or -h */
      printf("useful help messages go here\n");
      exit(0);
      break;
    case OPT_SERVERS: /* --servers or -s */
      opt_servers = optarg;
      break;
    case OPT_EXPIRE: /* --expire */
      expires = (time_t)strtol(optarg, (char **)NULL, 10);
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }

  memc= memcached_init(NULL);
  parse_opt_servers(memc, opt_servers);
  
  while (optind <= argc) 
  {
    rc = memcached_delete(memc, argv[optind], strlen(argv[optind]), expires);
    optind++;
  }

  memcached_deinit(memc);

  return 0;
};
