#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>

#include <libmemcached/memcached.h>

#include "client_options.h"
#include "utilities.h"

#define PROGRAM_NAME "memstat"
#define PROGRAM_DESCRIPTION "Output the state of a memcached cluster."

/* Prototypes */
static void options_parse(int argc, char *argv[]);
static void print_server_listing(memcached_st *memc, memcached_stat_st *stat,
                                 memcached_server_st *server_list);
static void print_analysis_report(memcached_st *memc,
                                  memcached_analysis_st *report,
                                  memcached_server_st *server_list);

static int opt_verbose= 0;
static int opt_displayflag= 0;
static int opt_analyze= 0;
static char *opt_servers= NULL;

static struct option long_options[]=
{
  {"version", no_argument, NULL, OPT_VERSION},
  {"help", no_argument, NULL, OPT_HELP},
  {"verbose", no_argument, &opt_verbose, OPT_VERBOSE},
  {"debug", no_argument, &opt_verbose, OPT_DEBUG},
  {"servers", required_argument, NULL, OPT_SERVERS},
  {"flag", no_argument, &opt_displayflag, OPT_FLAG},
  {"analyze", no_argument, NULL, OPT_ANALYZE},
  {0, 0, 0, 0},
};

int main(int argc, char *argv[])
{
  memcached_return rc;
  memcached_st *memc;
  memcached_stat_st *stat;
  memcached_server_st *servers;
  memcached_server_st *server_list;
  memcached_analysis_st *report;

  options_parse(argc, argv);

  if (!opt_servers)
  {
    char *temp;

    if ((temp= getenv("MEMCACHED_SERVERS")))
      opt_servers= strdup(temp);
    else
    {
      fprintf(stderr, "No Servers provided\n\n");
      help_command(PROGRAM_NAME, PROGRAM_DESCRIPTION, long_options, 0);
      exit(1);
    }
  }

  memc= memcached_create(NULL);

  servers= memcached_servers_parse(opt_servers);
  memcached_server_push(memc, servers);
  memcached_server_list_free(servers);

  stat= memcached_stat(memc, NULL, &rc);

  if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_SOME_ERRORS)
  {
    printf("Failure to communicate with servers (%s)\n",
           memcached_strerror(memc, rc));
    exit(1);
  }

  server_list= memcached_server_list(memc);

  if (opt_analyze)
  {
    report= memcached_analyze(memc, stat, &rc);
    if (rc != MEMCACHED_SUCCESS || report == NULL)
    {
      printf("Failure to analyze servers (%s)\n",
             memcached_strerror(memc, rc));
      exit(1);
    }
    print_analysis_report(memc, report, server_list);
    free(report);
  }
  else
    print_server_listing(memc, stat, server_list);

  free(stat);
  free(opt_servers);

  memcached_free(memc);

  return 0;
}

static void print_server_listing(memcached_st *memc, memcached_stat_st *stat,
                                 memcached_server_st *server_list)
{
  unsigned int x;
  memcached_return rc;

  printf("Listing %u Server\n\n", memcached_server_count(memc));
  for (x= 0; x < memcached_server_count(memc); x++)
  {
    char **list;
    char **ptr;

    list= memcached_stat_get_keys(memc, &stat[x], &rc);

    printf("Server: %s (%u)\n", memcached_server_name(memc, server_list[x]),
           memcached_server_port(memc, server_list[x]));
    for (ptr= list; *ptr; ptr++)
    {
      memcached_return rc;
      char *value= memcached_stat_get_value(memc, &stat[x], *ptr, &rc);

      printf("\t %s: %s\n", *ptr, value);
      free(value);
    }

    free(list);
    printf("\n");
  }
}

static void print_analysis_report(memcached_st *memc,
                                  memcached_analysis_st *report,
                                  memcached_server_st *server_list)
{
  uint32_t server_count= memcached_server_count(memc);

  printf("Memcached Cluster Analysis Report\n\n");

  printf("\tNumber of Servers Analyzed         : %d\n", server_count);
  printf("\tAverage Item Size (incl/overhead)  : %u bytes\n",
         report->average_item_size);

  if (server_count == 1)
  {
    printf("\nFor a detailed report, you must supply multiple servers.\n");
    return;
  }

  printf("\n");
  printf("\tNode with most memory consumption  : %s:%u (%u bytes)\n",
         memcached_server_name(memc, server_list[report->most_consumed_server]),
         memcached_server_port(memc, server_list[report->most_consumed_server]),
         report->most_used_bytes);
  printf("\tNode with least free space         : %s:%u (%u bytes remaining)\n",
         memcached_server_name(memc, server_list[report->least_free_server]),
         memcached_server_port(memc, server_list[report->least_free_server]),
         report->least_remaining_bytes);
  printf("\tNode with longest uptime           : %s:%u (%us)\n",
         memcached_server_name(memc, server_list[report->oldest_server]),
         memcached_server_port(memc, server_list[report->oldest_server]),
         report->longest_uptime);
  printf("\tPool-wide Hit Ratio                : %1.f%%\n", report->pool_hit_ratio);
  printf("\n");
}

static void options_parse(int argc, char *argv[])
{
  memcached_programs_help_st help_options[]=
  {
    {0},
  };

  int option_index= 0;
  int option_rv;

  while (1) 
  {
    option_rv= getopt_long(argc, argv, "Vhvds:a", long_options, &option_index);
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
      version_command(PROGRAM_NAME);
      break;
    case OPT_HELP: /* --help or -h */
      help_command(PROGRAM_NAME, PROGRAM_DESCRIPTION, long_options, help_options);
      break;
    case OPT_SERVERS: /* --servers or -s */
      opt_servers= strdup(optarg);
      break;
    case OPT_ANALYZE: /* --analyze or -a */
      opt_analyze= OPT_ANALYZE;
      break;
    case '?':
      /* getopt_long already printed an error message. */
      exit(1);
    default:
      abort();
    }
  }
}
