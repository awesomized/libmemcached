/*
*/

#include "common.h"

static char *memcached_stat_keys[] = {
  "pid",
  "uptime",
  "time",
  "version",
  "pointer_size",
  "rusage_user",
  "rusage_system",
  "rusage_user_seconds",
  "rusage_user_microseconds",
  "rusage_system_seconds",
  "rusage_system_microseconds",
  "curr_items",
  "total_items",
  "bytes",
  "curr_connections",
  "total_connections",
  "connection_structures",
  "cmd_get",
  "cmd_set",
  "get_hits",
  "get_misses",
  "evictions",
  "bytes_read",
  "bytes_written",
  "limit_maxbytes",
  "threads",
  NULL
};


static void set_data(memcached_stat_st *stat, char *key, char *value)
{
  if (!memcmp("pid", key, strlen("pid")))
  {
    stat->pid= strtol(value, (char **)NULL, 10);
  }
  else if (!memcmp("uptime", key, strlen("uptime")))
  {
    stat->uptime= strtol(value, (char **)NULL, 10);
  }
  else if (!memcmp("time", key, strlen("time")))
  {
    stat->time= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("version", key, strlen("version")))
  {
    memcpy(stat->version, value, 8);
  }
  else if (!memcmp("pointer_size", key, strlen("pointer_size")))
  {
    stat->pointer_size= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("rusage_user", key, strlen("rusage_user")))
  {
    stat->rusage_user= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("rusage_system", key, strlen("rusage_system")))
  {
    stat->rusage_system= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("rusage_user_seconds", key, strlen("rusage_user_seconds")))
  {
    stat->rusage_user_seconds= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("rusage_user_microseconds", key, strlen("rusage_user_microseconds")))
  {
    stat->rusage_user_microseconds= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("rusage_system_seconds", key, strlen("rusage_system_seconds")))
  {
    stat->rusage_system_seconds= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("rusage_system_microseconds", key, strlen("rusage_system_microseconds")))
  {
    stat->rusage_system_microseconds= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("curr_items", key, strlen("curr_items")))
  {
    stat->curr_items= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("total_items", key, strlen("total_items")))
  {
    stat->total_items= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("bytes", key, strlen("bytes")))
  {
    stat->bytes= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("curr_connections", key, strlen("curr_connections")))
  {
    stat->curr_connections= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("total_connections", key, strlen("total_connections")))
  {
    stat->total_connections= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("connection_structures", key, strlen("connection_structures")))
  {
    stat->connection_structures= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("cmd_get", key, strlen("cmd_get")))
  {
    stat->cmd_get= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("cmd_set", key, strlen("cmd_set")))
  {
    stat->cmd_set= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("get_hits", key, strlen("get_hits")))
  {
    stat->get_hits= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("get_misses", key, strlen("get_misses")))
  {
    stat->get_misses= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("evictions", key, strlen("evictions")))
  {
    stat->evictions= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("bytes_read", key, strlen("bytes_read")))
  {
    stat->bytes_read= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("bytes_written", key, strlen("bytes_written")))
  {
    stat->bytes_written= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("limit_maxbytes", key, strlen("limit_maxbytes")))
  {
    stat->limit_maxbytes= strtoll(value, (char **)NULL, 10);
  }
  else if (!memcmp("threads", key, strlen("threads")))
  {
    stat->threads= strtol(key, (char **)NULL, 10);
  }
  else
  {
    fprintf(stderr, "Unknown key %s\n", key);
  }
}

char *memcached_stat_get_value(memcached_st *ptr, memcached_stat_st *stat, 
                               char *key, memcached_return *error)
{
  char buffer[SMALL_STRING_LEN];
  *error= MEMCACHED_SUCCESS;


  memset(buffer, 0, SMALL_STRING_LEN);

  if (!memcmp("pid", key, strlen("pid")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->pid);
  else if (!memcmp("uptime", key, strlen("uptime")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->uptime);
  else if (!memcmp("time", key, strlen("time")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", (unsigned long long)stat->time);
  else if (!memcmp("version", key, strlen("version")))
    snprintf(buffer, SMALL_STRING_LEN,"%s", stat->version);
  else if (!memcmp("pointer_size", key, strlen("pointer_size")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->pointer_size);
  else if (!memcmp("rusage_user", key, strlen("rusage_user")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->rusage_user);
  else if (!memcmp("rusage_system", key, strlen("rusage_system")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->rusage_system);
  else if (!memcmp("rusage_user_seconds", key, strlen("rusage_user_seconds")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->rusage_user_seconds);
  else if (!memcmp("rusage_user_microseconds", key, strlen("rusage_user_microseconds")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->rusage_user_microseconds);
  else if (!memcmp("rusage_system_seconds", key, strlen("rusage_system_seconds")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->rusage_system_seconds);
  else if (!memcmp("rusage_system_microseconds", key, strlen("rusage_system_microseconds")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->rusage_system_microseconds);
  else if (!memcmp("curr_items", key, strlen("curr_items")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->curr_items);
  else if (!memcmp("total_items", key, strlen("total_items")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->total_items);
  else if (!memcmp("bytes", key, strlen("bytes")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->bytes);
  else if (!memcmp("curr_connections", key, strlen("curr_connections")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->curr_connections);
  else if (!memcmp("total_connections", key, strlen("total_connections")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->total_connections);
  else if (!memcmp("connection_structures", key, strlen("connection_structures")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->connection_structures);
  else if (!memcmp("cmd_get", key, strlen("cmd_get")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->cmd_get);
  else if (!memcmp("cmd_set", key, strlen("cmd_set")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->cmd_set);
  else if (!memcmp("get_hits", key, strlen("get_hits")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->get_hits);
  else if (!memcmp("get_misses", key, strlen("get_misses")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->get_misses);
  else if (!memcmp("evictions", key, strlen("evictions")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->evictions);
  else if (!memcmp("bytes_read", key, strlen("bytes_read")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->bytes_read);
  else if (!memcmp("bytes_written", key, strlen("bytes_written")))
    snprintf(buffer, SMALL_STRING_LEN,"%llu", stat->bytes_written);
  else if (!memcmp("limit_maxbytes", key, strlen("limit_maxbytes")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->limit_maxbytes);
  else if (!memcmp("threads", key, strlen("threads")))
    snprintf(buffer, SMALL_STRING_LEN,"%u", stat->threads);
  else
  {
    *error= MEMCACHED_NOTFOUND;
    return NULL;
  }

  return strdup(buffer);
}

static memcached_return memcached_stats_fetch(memcached_st *ptr,
                                              memcached_stat_st *stat,
                                              char *args,
                                              unsigned int server_key)
{
  memcached_return rc;
  char buffer[HUGE_STRING_LEN];
  size_t send_length, sent_length;

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  if (args)
    send_length= snprintf(buffer, HUGE_STRING_LEN, 
                          "stats %s\r\n", args);
  else
    send_length= snprintf(buffer, HUGE_STRING_LEN, 
                          "stats\r\n");

  if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
    return MEMCACHED_WRITE_FAILURE;

  sent_length= send(ptr->hosts[server_key].fd, buffer, send_length, 0);

  if (sent_length == -1 || sent_length != send_length)
    return MEMCACHED_WRITE_FAILURE;

  rc= memcached_response(ptr, buffer, HUGE_STRING_LEN, 0);

  if (rc == MEMCACHED_SUCCESS)
  {
    char *string_ptr, *end_ptr;
    char *key, *value;

    string_ptr= buffer;
    while (1)
    {
      if (memcmp(string_ptr, "STAT ", 5))
        break;
      string_ptr+= 5;
      for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);
      key= string_ptr;
      key[(size_t)(end_ptr-string_ptr)]= 0;

      string_ptr= end_ptr + 1;
      for (end_ptr= string_ptr; *end_ptr != '\r'; end_ptr++);
      value= string_ptr;
      value[(size_t)(end_ptr-string_ptr)]= 0;
      string_ptr= end_ptr + 2;
      set_data(stat, key, value);
    }
  }

  return rc;
}

memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return *error)
{
  unsigned int x;
  memcached_return rc;
  memcached_stat_st *stats;

  rc= memcached_connect(ptr);
  if (rc != MEMCACHED_SUCCESS)
  {
    *error= rc;
    return NULL;
  }

  stats= (memcached_stat_st *)malloc(sizeof(memcached_st)*(ptr->number_of_hosts+1));
  if (!stats)
  {
    *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    free(stats);
    return NULL;
  }
  memset(stats, 0, sizeof(memcached_st)*(ptr->number_of_hosts+1));

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    rc= memcached_stats_fetch(ptr, stats+x, args, x);
    if (rc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;
  }

  *error= rc;
  return stats;
}

memcached_return memcached_stat_servername(memcached_stat_st *stat, char *args, 
                                           char *hostname, unsigned int port)
{
  memcached_return rc;
  memcached_st memc;

  memcached_init(&memc);

  memcached_server_add(&memc, hostname, port);

  rc= memcached_connect(&memc);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  rc= memcached_stats_fetch(&memc, stat, args, 0);

  memcached_deinit(&memc);

  return rc;
}

/* 
  We make a copy of the keys since at some point in the not so distant future
  we will add support for "found" keys.
*/
char ** memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *stat, 
                                memcached_return *error)
{
  char **list= (char **)malloc(sizeof(memcached_stat_keys));

  memset(list, 0, sizeof(memcached_stat_keys));

  if (!list)
  {
    *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    return NULL;
  }

  memcpy(list, memcached_stat_keys, sizeof(memcached_stat_keys));

  *error= MEMCACHED_SUCCESS;

  return list;
}
