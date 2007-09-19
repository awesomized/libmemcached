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

static int opt_verbose;
static char *opt_servers;
static int opt_replace;

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
  unsigned int x;
  size_t string_length;
  uint16_t flags = 0;
  time_t expires = 0;
  memcached_return rc;

  static struct option long_options[] =
    {
      {"version", no_argument, NULL, OPT_VERSION},
      {"help", no_argument, NULL, OPT_HELP},
      {"verbose", no_argument, &opt_verbose, 1},
      {"debug", no_argument, &opt_verbose, 2},
      {"servers", required_argument, NULL, OPT_SERVERS},
      {"flag", required_argument, NULL, OPT_FLAG},
      {"expire", required_argument, NULL, OPT_EXPIRE},
      {"set",  no_argument, &opt_replace, 0},
      {"add",  no_argument, &opt_replace, 1},
      {"replace",  no_argument, &opt_replace, 2},
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
      if (long_options[option_index].name)
      break;
    case OPT_VERSION: /* --version */
      printf("memcache tools, memcp, v1.0\n");
      exit(0);
      break;
    case OPT_HELP: /* --help */
      printf("useful help messages go here\n");
      exit(0);
      break;
    case OPT_SERVERS: /* --servers */
      opt_servers = strdup(optarg);
      break;
    case OPT_FLAG: /* --flag */
      flags = (uint16_t) atoi(optarg);
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
    {
      ptr++;
    }
    else
    {
      ptr= argv[optind];
    }

    if (opt_replace == 0) {
      rc= memcached_set(memc, ptr, strlen(ptr),
			mptr, sbuf.st_size,
			expires, flags);
    } else if (opt_replace == 1) {
      rc= memcached_add(memc, ptr, strlen(ptr),
			mptr, sbuf.st_size,
			expires, flags);
    } else if (opt_replace == 2) {
      rc= memcached_replace(memc, ptr, strlen(ptr),
			    mptr, sbuf.st_size,
			    expires, flags);
    } else {
      abort();
    }

    munmap(mptr, sbuf.st_size);
    close(fd);
    optind++;
  }

  memcached_deinit(memc);

  return 0;
};
