#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <getopt.h>

#include <memcached.h>

#include "client_options.h"
#include "utilities.h"


/* Use this for string generation */
static const char ALPHANUMERICS[]=
  "0123456789ABCDEFGHIJKLMNOPQRSTWXYZabcdefghijklmnopqrstuvwxyz";

#define ALPHANUMERICS_SIZE (sizeof(ALPHANUMERICS)-1)

/* Types */
typedef struct pairs_st pairs_st;

struct pairs_st {
  char *key;
  size_t key_length;
  char *value;
  size_t value_length;
};

/* Prototypes */
void options_parse(int argc, char *argv[]);
static pairs_st *pairs_generate(void);
static void pairs_free(pairs_st *pairs);
static void get_random_string(char *buffer, size_t size);

static int opt_verbose= 0;
static int opt_default_pairs= 100;
static int opt_displayflag= 0;
static char *opt_servers= NULL;

int main(int argc, char *argv[])
{
  unsigned int x;
  memcached_return rc;
  memcached_st *memc;
  pairs_st *pairs;

  srandom(time(NULL));
  memc= memcached_init(NULL);
  options_parse(argc, argv);

  if (!opt_servers)
    exit(0);

  parse_opt_servers(memc, opt_servers);

  pairs= pairs_generate();


  for (x= 0; x < opt_default_pairs; x++)
  {
    printf("Key(%u) %.10s \t%.10s\n", x, pairs[x].key, pairs[x].value);
    rc= memcached_set(memc, pairs[x].key, pairs[x].key_length,
                      pairs[x].value, pairs[x].value_length,
                      0, 0);
    if (rc != MEMCACHED_SUCCESS)
      fprintf(stderr, "Failured on insert of %.*s\n", 
              (unsigned int)pairs[x].key_length, pairs[x].key);
  }

  pairs_free(pairs);

  free(opt_servers);

  memcached_deinit(memc);

  return 0;
}

void options_parse(int argc, char *argv[])
{
  static struct option long_options[]=
    {
      {"version", no_argument, NULL, OPT_VERSION},
      {"help", no_argument, NULL, OPT_HELP},
      {"verbose", no_argument, &opt_verbose, OPT_VERBOSE},
      {"debug", no_argument, &opt_verbose, OPT_DEBUG},
      {"servers", required_argument, NULL, OPT_SERVERS},
      {"flag", no_argument, &opt_displayflag, OPT_FLAG},
      {"default-pairs", required_argument, NULL, OPT_SLAP_DEFAULT_PAIRS},
      {0, 0, 0, 0},
    };

  int option_index= 0;
  int option_rv;

  while (1) 
  {
    option_rv= getopt_long(argc, argv, "Vhvds:", long_options, &option_index);
    if (option_rv == -1) break;
    switch (option_rv)
    {
    case 0:
      break;
    case OPT_VERBOSE: /* --verbose or -v */
      opt_verbose = OPT_VERBOSE;
      break;
    case OPT_DEBUG: /* --debug or -d */
      opt_verbose = OPT_DEBUG;
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
    case OPT_SLAP_DEFAULT_PAIRS:
      opt_default_pairs= strtol(optarg, (char **)NULL, 10);
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }
}

static void pairs_free(pairs_st *pairs)
{
  unsigned int x;

  for (x= 0; x < opt_default_pairs; x++)
  {
    free(pairs[x].key);
    free(pairs[x].value);
  }

  free(pairs);
}

static pairs_st *pairs_generate(void)
{
  unsigned int x;
  pairs_st *pairs;

  pairs= (pairs_st*)malloc(sizeof(pairs_st) * opt_default_pairs);

  if (!pairs)
    goto error;

  for (x= 0; x < opt_default_pairs; x++)
  {
    pairs[x].key= (char *)malloc(sizeof(char) * 100);
    if (!pairs[x].key)
      goto error;
    get_random_string(pairs[x].key, 100);
    pairs[x].key_length= 100;

    pairs[x].value= (char *)malloc(sizeof(char) * 400);
    if (!pairs[x].value)
      goto error;
    get_random_string(pairs[x].value, 400);
    pairs[x].value_length= 400;
  }

  return pairs;
error:
    fprintf(stderr, "Memory Allocation failure in pairs_generate.\n");
    exit(0);
}

static void get_random_string(char *buffer, size_t size)
{
  char *buffer_ptr= buffer;

  while (--size)
    *buffer_ptr++= ALPHANUMERICS[random() % ALPHANUMERICS_SIZE];
  *buffer_ptr++= ALPHANUMERICS[random() % ALPHANUMERICS_SIZE];
}
