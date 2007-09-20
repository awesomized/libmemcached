#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <memcached.h>
#include "client_options.h"

/* Prototypes */
void options_parse(int argc, char *argv[]);

static int opt_verbose;
static char *opt_servers;
static int opt_replace;
uint16_t opt_flags= 0;
time_t opt_expires= 0;

int main(int argc, char *argv[])
{
  memcached_st *memc;
  char *string;
  size_t string_length;
  memcached_return rc;

  options_parse(argc, argv);

  memc= memcached_init(NULL);
  parse_opt_servers(memc, opt_servers);

  while (optind <= argc) 
  {
    char *mptr;
    struct stat sbuf;
    int fd;
    char *ptr;

    fd= open(argv[optind], O_RDONLY);

    if (fd == -1)
    {
      fprintf(stderr, "Failed opening %s\n", argv[optind]);
      continue;
    }

    (void)fstat(fd, &sbuf);
    mptr= mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    ptr= rindex(argv[optind], '/');
    if (ptr)
      ptr++;
    else
      ptr= argv[optind];

    if (opt_verbose) {
      static char *opstr[] = { "set", "add", "replace" };
      printf("op: %s\nsource file: %s\nlength: %d\n"
	     "key: %s\nflags: %d\n expires: %ld\n",
	     opstr[opt_replace], argv[optind], sbuf.st_size,
	     ptr, opt_flags, opt_expires);
    }

    if (opt_replace == 0)
      rc= memcached_set(memc, ptr, strlen(ptr),
			mptr, sbuf.st_size,
			opt_expires, opt_flags);
    else if (opt_replace == 1)
      rc= memcached_add(memc, ptr, strlen(ptr),
			mptr, sbuf.st_size,
			opt_expires, opt_flags);
    else if (opt_replace == 2)
      rc= memcached_replace(memc, ptr, strlen(ptr),
			    mptr, sbuf.st_size,
			    opt_expires, opt_flags);
    else
      abort();

    munmap(mptr, sbuf.st_size);
    close(fd);
    optind++;
  }

  memcached_deinit(memc);

  return 0;
};

void options_parse(int argc, char *argv[])
{
  int option_index= 0;
  int option_rv;

  static struct option long_options[] =
    {
      {"version", no_argument, NULL, OPT_VERSION},
      {"help", no_argument, NULL, OPT_HELP},
      {"verbose", no_argument, &opt_verbose, OPT_VERBOSE},
      {"debug", no_argument, &opt_verbose, OPT_DEBUG},
      {"servers", required_argument, NULL, OPT_SERVERS},
      {"flag", required_argument, NULL, OPT_FLAG},
      {"expire", required_argument, NULL, OPT_EXPIRE},
      {"set",  no_argument, &opt_replace, OPT_SET},
      {"add",  no_argument, &opt_replace, OPT_ADD},
      {"replace",  no_argument, &opt_replace, OPT_REPLACE},
      {0, 0, 0, 0},
    };

  while (1) 
  {
    option_rv= getopt_long(argc, argv, "Vhvds:", long_options, &option_index);

    if (option_rv == -1) break;

    switch (option_rv) {
    case 0:
      break;
    case OPT_VERSION: /* --version or -V */
      printf("memcache tools, memcp, v1.0\n");
      exit(0);
    case OPT_HELP: /* --help or -h */
      printf("useful help messages go here\n");
      exit(0);
    case OPT_SERVERS: /* --servers or -s */
      opt_servers= optarg;
      break;
    case OPT_FLAG: /* --flag */
      opt_flags= (uint16_t)strtol(optarg, (char **)NULL, 10);
      break;
    case OPT_EXPIRE: /* --expire */
      opt_expires= (time_t)strtol(optarg, (char **)NULL, 10);
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }
}
