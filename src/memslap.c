#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <getopt.h>
#include <pthread.h>

#include <memcached.h>

#include "client_options.h"
#include "utilities.h"
#include "generator.h"

/* Global Thread counter */
unsigned int thread_counter;
pthread_mutex_t counter_mutex;
pthread_cond_t count_threshhold;
unsigned int master_wakeup;
pthread_mutex_t sleeper_mutex;
pthread_cond_t sleep_threshhold;

void *run_task(void *p);

/* Types */
typedef struct conclusions_st conclusions_st;
typedef struct thread_context_st thread_context_st;
typedef enum {
  AC_SET,
  AC_GET,
} run_action;

struct thread_context_st {
  pairs_st *pairs;
  run_action action;
  memcached_server_st *servers;
};

struct conclusions_st {
  long int load_time;
  long int read_time;
  unsigned int rows_loaded;
  unsigned int rows_read;
};

/* Prototypes */
void options_parse(int argc, char *argv[]);
void conclusions_print(conclusions_st *conclusion);
void scheduler(memcached_server_st *servers, conclusions_st *conclusion);

static int opt_verbose= 0;
static unsigned int opt_default_pairs= 100;
static unsigned int opt_concurrency= 10;
static int opt_displayflag= 0;
static char *opt_servers= NULL;

int main(int argc, char *argv[])
{
  conclusions_st conclusion;
  memcached_server_st *servers;

  memset(&conclusion, 0, sizeof(conclusions_st));

  srandom(time(NULL));
  options_parse(argc, argv);

  if (!opt_servers)
    exit(0);

  servers= parse_opt_servers(opt_servers);

  pthread_mutex_init(&counter_mutex, NULL);
  pthread_cond_init(&count_threshhold, NULL);
  pthread_mutex_init(&sleeper_mutex, NULL);
  pthread_cond_init(&sleep_threshhold, NULL);

  scheduler(servers, &conclusion);

  free(opt_servers);

  (void)pthread_mutex_init(&counter_mutex, NULL);
  (void)pthread_cond_init(&count_threshhold, NULL);
  (void)pthread_mutex_init(&sleeper_mutex, NULL);
  (void)pthread_cond_init(&sleep_threshhold, NULL);
  conclusions_print(&conclusion);
  memcached_server_list_free(servers);

  return 0;
}

void scheduler(memcached_server_st *servers, conclusions_st *conclusion)
{
  unsigned int x;
  struct timeval start_time, end_time;
  pthread_t mainthread;            /* Thread descriptor */
  pthread_attr_t attr;          /* Thread attributes */
  pairs_st *pairs;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,
                              PTHREAD_CREATE_DETACHED);

  pairs= pairs_generate(opt_default_pairs);

  pthread_mutex_lock(&counter_mutex);
  thread_counter= 0;

  pthread_mutex_lock(&sleeper_mutex);
  master_wakeup= 1;
  pthread_mutex_unlock(&sleeper_mutex);

  for (x= 0; x < opt_concurrency; x++)
  {
    thread_context_st *context;
    context= (thread_context_st *)malloc(sizeof(thread_context_st));

    context->servers= servers;
    context->pairs= pairs;
    context->action= AC_SET;

    /* now you create the thread */
    if (pthread_create(&mainthread, &attr, run_task,
                       (void *)context) != 0)
    {
      fprintf(stderr,"Could not create thread\n");
      exit(1);
    }
    thread_counter++;
  }

  pthread_mutex_unlock(&counter_mutex);
  pthread_attr_destroy(&attr);

  pthread_mutex_lock(&sleeper_mutex);
  master_wakeup= 0;
  pthread_mutex_unlock(&sleeper_mutex);
  pthread_cond_broadcast(&sleep_threshhold);

  gettimeofday(&start_time, NULL);
  /*
    We loop until we know that all children have cleaned up.
  */
  pthread_mutex_lock(&counter_mutex);
  while (thread_counter)
  {
    struct timespec abstime;

    memset(&abstime, 0, sizeof(struct timespec));
    abstime.tv_sec= 1;

    pthread_cond_timedwait(&count_threshhold, &counter_mutex, &abstime);
  }
  pthread_mutex_unlock(&counter_mutex);

  gettimeofday(&end_time, NULL);

  conclusion->load_time= timedif(end_time, start_time);
  conclusion->read_time= timedif(end_time, start_time);
  pairs_free(pairs);
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

void conclusions_print(conclusions_st *conclusion)
{
  printf("\tLoaded %u rows\n", conclusion->rows_loaded);
  printf("\tRead %u rows\n", conclusion->rows_read);
  printf("\tTook %ld.%03ld seconds to load data\n", conclusion->load_time / 1000, 
         conclusion->load_time % 1000);
  printf("\tTook %ld.%03ld seconds to read data\n", conclusion->read_time / 1000, 
         conclusion->read_time % 1000);
}

void *run_task(void *p)
{
  unsigned int x;
  thread_context_st *context= (thread_context_st *)p;
  memcached_return rc;
  memcached_st *memc;
  pairs_st *pairs= context->pairs;

  memc= memcached_init(NULL);
  
  memcached_server_push(memc, context->servers);

  pthread_mutex_lock(&sleeper_mutex);
  while (master_wakeup)
  {
    pthread_cond_wait(&sleep_threshhold, &sleeper_mutex);
  } 
  pthread_mutex_unlock(&sleeper_mutex);

  /* Do Stuff */
  switch (context->action)
  {
  case AC_SET:
    for (x= 0; x < opt_default_pairs; x++)
    {
      rc= memcached_set(memc, pairs[x].key, pairs[x].key_length,
                        pairs[x].value, pairs[x].value_length,
                        0, 0);
      if (rc != MEMCACHED_SUCCESS)
        fprintf(stderr, "Failured on insert of %.*s\n", 
                (unsigned int)pairs[x].key_length, pairs[x].key);
  }
    break;
  case AC_GET:
    for (x= 0; x < opt_default_pairs; x++)
    {
      char *value;
      size_t value_length;
      uint16_t flags;

      value= memcached_get(memc, pairs[x].key, pairs[x].key_length,
                           &value_length,
                           &flags, &rc);

      if (rc != MEMCACHED_SUCCESS)
        fprintf(stderr, "Failured on read of %.*s\n", 
                (unsigned int)pairs[x].key_length, pairs[x].key);
      free(value);
    }
    break;
  }

  pthread_mutex_lock(&counter_mutex);
  thread_counter--;
  pthread_cond_signal(&count_threshhold);
  pthread_mutex_unlock(&counter_mutex);
  memcached_deinit(memc);

  free(context);

  return NULL;
}
