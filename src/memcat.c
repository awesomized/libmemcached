#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <memcached.h>

#include "client_options.h"
#include "utilities.h"


/* Prototypes */
void options_parse(int argc, char *argv[]);

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
  unsigned int x;

  memc= memcached_init(NULL);

  options_parse(argc, argv);

  parse_opt_servers(memc, opt_servers);

  for (x= 1; x < argc; x++)
  {
    string= memcached_get(memc, argv[x], strlen(argv[x]),
                          &string_length, &flags, &rc);
    if (rc == MEMCACHED_SUCCESS) {
      if (opt_displayflag) {
	printf("%d\n", flags);
      } else {
	if (string) 
	{
	  printf("%.*s\n", string_length, string);
	  free(string);
	}
      }
    }
  }

  memcached_deinit(memc);

  return 0;
};


void options_parse(int argc, char *argv[])
{
  int option_index = 0;
  int option_rv;

  static struct option long_options[] =
    {
      {"version", no_argument, NULL, OPT_VERSION},
      {"help", no_argument, NULL, OPT_HELP},
      {"verbose", no_argument, &opt_verbose, OPT_VERBOSE},
      {"debug", no_argument, &opt_verbose, OPT_DEBUG},
      {"servers", required_argument, NULL, OPT_SERVERS},
      {"flag", no_argument, &opt_displayflag, OPT_FLAG},
      {0, 0, 0, 0},
    };

  while (1) 
  {
    option_rv = getopt_long(argc, argv, "", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv) {
    case 0:
      break;
    case OPT_VERSION: /* --version */
      printf("memcache tools, memcat, v1.0\n");
      exit(0);
      break;
    case OPT_HELP: /* --help */
      printf("useful help messages go here\n");
      exit(0);
      break;
    case OPT_SERVERS: /* --servers */
      opt_servers= strdup(optarg);
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }
}
