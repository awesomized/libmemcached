#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <memcached.h>
#include "client_options.h"

static int opt_verbose;
static char *opt_servers;

struct memcached_st *parse_opt_servers (struct memcached_st *m,
					char *opt_servers)
{
  char *s, *hostname;
  unsigned int portnum;
  while (s = strsep(&opt_servers, ",")) {
    hostname = strsep(&s, ":");
    portnum = atoi(s);
    memcached_server_add(m, hostname, portnum);
  }
  return m;
}

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
    option_rv = getopt_long(argc, argv, "", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv) {
    case 0:
      break;
    case OPT_VERSION: /* --version */
      printf("memcache tools, memrm, v1.0\n");
      exit(0);
      break;
    case OPT_HELP: /* --help */
      printf("useful help messages go here\n");
      exit(0);
      break;
    case OPT_SERVERS: /* --servers */
      opt_servers = strdup(optarg);
      break;
    case OPT_EXPIRE: /* --expire */
      expires = (time_t)atoi(optarg);
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }

  memc= memcached_init(NULL);
  memc= parse_opt_servers(memc, opt_servers);
  
  while (optind <= argc) {

    rc = memcached_delete(memc, argv[optind], strlen(argv[optind]), expires);

    optind++;
  }

  memcached_deinit(memc);

  return 0;
};
