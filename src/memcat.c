#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <memcached.h>

#include "client_options.h"
#include "utilities.h"


/* Prototypes */
void options_parse(int argc, char *argv[]);

static int opt_verbose= 0;
static int opt_displayflag= 0;
static char *opt_servers;

int main(int argc, char *argv[])
{
  memcached_st *memc;
  char *string;
  size_t string_length;
  uint16_t flags;
  memcached_return rc;

  memc= memcached_init(NULL);

  options_parse(argc, argv);

  if (opt_servers)
    parse_opt_servers(memc, opt_servers);

  while (optind <= argc) 
  {
    string= memcached_get(memc, argv[optind], strlen(argv[optind]),
                          &string_length, &flags, &rc);
    if (rc == MEMCACHED_SUCCESS) 
    {
      if (opt_displayflag)
      {
        if (opt_verbose)
          printf("key: %s\nflags: ", argv[optind]);
        printf("%x\n", flags);
      }
      else 
      {
        if (opt_verbose)
          printf("key: %s\nflags: %x\nlength: %zu\nvalue: ",
                 argv[optind], flags, string_length);
        printf("%.*s\n", (int)string_length, string);
        free(string);
      }
    }
    else
    {
      fprintf(stderr, "memcat: %s: memcache error %s\n", 
              argv[optind], memcached_strerror(memc, rc));
    }
    optind++;
  }

  memcached_deinit(memc);

  free(opt_servers);

  return 0;
};


void options_parse(int argc, char *argv[])
{
  int option_index= 0;
  int option_rv;

  static struct option long_options[]=
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
    option_rv= getopt_long(argc, argv, "Vhvds:", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv)
    {
    case 0:
      break;
    case OPT_VERSION: /* --version or -V */
      printf("memcache tools, memcat, v1.0\n");
      exit(0);
      break;
    case OPT_HELP: /* --help or -h */
      printf("useful help messages go here\n");
      exit(0);
      break;
    case OPT_SERVERS: /* --servers or -s */
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
