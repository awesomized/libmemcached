#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <memcached.h>

static int opt_verbose;
static int opt_displayflag;
static char *opt_servers;

int main(int argc, char *argv[])
{
  memcached_st *memc;
  char *string;
  size_t string_length;
  uint16_t  flags;
  memcached_return rc;

  static struct option long_options[] =
    {
      {"version", no_argument, NULL, 257},
      {"help", no_argument, NULL, 258},
      {"verbose", no_argument, &opt_verbose, 1},
      {"debug", no_argument, &opt_verbose, 2},
      {"servers", required_argument, NULL, 259},
      {"flag", no_argument, &opt_displayflag, 1},
      {0, 0, 0, 0},
    };
  int option_index = 0;
  int option_rv;

  while (1) {
    option_rv = getopt_long(argc, argv, "", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv) {
    case 0:
      break;
    case 257: /* --version */
      printf("memcache tools, memcat, v1.0\n");
      exit(0);
      break;
    case 258: /* --help */
      printf("useful help messages go here\n");
      exit(0);
      break;
    case 259: /* --servers */
      opt_servers = strdup(optarg);
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }

  /* todo, turn opt_servers into something to pass to memcached_init */
  memc= memcached_init(NULL);

  while (optind < argc) {
    string= memcached_get(memc, argv[optind], strlen(argv[optind]),
                          &string_length, &flags, &rc);
    if (string) {
      printf("%.*s\n", string_length, string);
      free(string);
    }
    optind++;
  }

  memcached_deinit(memc);

  return 0;
};
