/*
*/

#include <memcached.h>

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

memcached_stat_st **memcached_stat(memcached_st *ptr, memcached_return *error)
{
  return NULL;
}

memcached_return memcached_stat_hostname(memcached_stat_st *stat, char *args, 
                                         char *hostname, unsigned int port)
{
  size_t send_length;
  memcached_return rc;
  char buffer[HUGE_STRING_LEN];
  memcached_st memc;

  memcached_init(&memc);

  rc= memcached_connect(&memc);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  if (args)
    send_length= snprintf(buffer, HUGE_STRING_LEN, 
                          "stats %s\r\n", args);
  else
    send_length= snprintf(buffer, HUGE_STRING_LEN, 
                          "stats\r\n");

  if ((send(memc.fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed on stats\n");

    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  rc= memcached_response(&memc, buffer, HUGE_STRING_LEN);

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

error:
  memcached_deinit(&memc);

  return rc;
}
