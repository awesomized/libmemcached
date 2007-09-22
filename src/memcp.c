#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include <memcached.h>
#include "client_options.h"

/* Prototypes */
void options_parse(int argc, char *argv[]);

static int opt_verbose= 0;
static char *opt_servers= NULL;
static int opt_method= 0;
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
  if (opt_servers)
    parse_opt_servers(memc, opt_servers);
  else
    parse_opt_servers(memc, argv[--argc]);

  while (optind < argc) 
  {
    struct stat sbuf;
    int fd;
    char *ptr;
    ssize_t read_length;
    char *file_buffer_ptr;

    fd= open(argv[optind], O_RDONLY);
    if (fd < 0) 
    {
      fprintf(stderr, "memcp: %s: %s\n", argv[optind], strerror(errno));
      optind++;
      continue;
    }

    (void)fstat(fd, &sbuf);

    ptr= rindex(argv[optind], '/');
    if (ptr)
      ptr++;
    else
      ptr= argv[optind];

    if (opt_verbose) 
    {
      static char *opstr[] = { "set", "add", "replace" };
      printf("op: %s\nsource file: %s\nlength: %zu\n"
	     "key: %s\nflags: %u\n expires: %llu\n",
	     opstr[opt_method], argv[optind], sbuf.st_size,
	     ptr, opt_flags, opt_expires);
    }

    if ((file_buffer_ptr= (char *)malloc(sizeof(char) * sbuf.st_size)) == NULL)
    {
      fprintf(stderr, "malloc: %s\n", strerror(errno)); 
      exit(1);
    }

    if ((read_length= read(fd, file_buffer_ptr, sbuf.st_size)) == -1)
    {
      fprintf(stderr, "read: %s\n", strerror(errno)); 
      exit(1);
    }
    assert(read_length == sbuf.st_size);

    if (opt_method == OPT_ADD)
      rc= memcached_add(memc, ptr, strlen(ptr),
                        file_buffer_ptr, sbuf.st_size,
			opt_expires, opt_flags);
    else if (opt_method == OPT_REPLACE)
      rc= memcached_replace(memc, ptr, strlen(ptr),
			    file_buffer_ptr, sbuf.st_size,
			    opt_expires, opt_flags);
    else
      rc= memcached_set(memc, ptr, strlen(ptr),
                        file_buffer_ptr, sbuf.st_size,
                        opt_expires, opt_flags);

    if (rc != MEMCACHED_SUCCESS)
      fprintf(stderr, "memcp: %s: memcache error %s\n", 
	      ptr, memcached_strerror(memc, rc));

    WATCHPOINT;
    free(file_buffer_ptr);
    close(fd);
    optind++;
  }

  memcached_deinit(memc);

  cleanup();

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
      {"set",  no_argument, NULL, OPT_SET},
      {"add",  no_argument, NULL, OPT_ADD},
      {"replace",  no_argument, NULL, OPT_REPLACE},
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
      opt_servers= strdup_cleanup(optarg);
      break;
    case OPT_FLAG: /* --flag */
      opt_flags= (uint16_t)strtol(optarg, (char **)NULL, 10);
      break;
    case OPT_EXPIRE: /* --expire */
      opt_expires= (time_t)strtol(optarg, (char **)NULL, 10);
      break;
    case OPT_SET:
      opt_method= OPT_SET;
      break;
    case OPT_REPLACE:
      opt_method= OPT_REPLACE;
      break;
    case OPT_ADD:
      opt_method= OPT_ADD;
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }
}
