#include "common.h"
#include <math.h>

/* Protoypes (static) */
static memcached_return server_add(memcached_st *ptr, char *hostname, 
                                   unsigned int port,
                                   memcached_connection type);
memcached_return update_continuum(memcached_st *ptr);

static int compare_servers(const void *p1, const void *p2)
{
  int return_value;
  memcached_server_st *a= (memcached_server_st *)p1;
  memcached_server_st *b= (memcached_server_st *)p2;

  return_value= strcmp(a->hostname, b->hostname);

  if (return_value == 0)
  {
    return_value= (int) (a->port - b->port);
  }

  return return_value;
}

void sort_hosts(memcached_st *ptr)
{
  if (ptr->number_of_hosts)
  {
    qsort(ptr->hosts, ptr->number_of_hosts, sizeof(memcached_server_st), compare_servers);
    ptr->hosts[0].count= ptr->number_of_hosts;
  }
}


memcached_return run_distribution(memcached_st *ptr)
{
  switch (ptr->distribution) 
  {
  case MEMCACHED_DISTRIBUTION_CONSISTENT:
  case MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA:
    return update_continuum(ptr);
  case MEMCACHED_DISTRIBUTION_MODULA:
    if (ptr->flags & MEM_USE_SORT_HOSTS)
      sort_hosts(ptr);
    break;
  default:
    WATCHPOINT_ASSERT(0); /* We have added a distribution without extending the logic */
  }

  return MEMCACHED_SUCCESS;
}

static void host_reset(memcached_st *ptr, memcached_server_st *host, 
                       char *hostname, unsigned int port,
                       memcached_connection type)
{
  memset(host,  0, sizeof(memcached_server_st));
  strncpy(host->hostname, hostname, MEMCACHED_MAX_HOST_LENGTH - 1);
  host->root= ptr ? ptr : NULL;
  host->port= port;
  host->fd= -1;
  host->type= type;
  host->read_ptr= host->read_buffer;
  if (ptr)
    host->next_retry= ptr->retry_timeout;
  host->sockaddr_inited= MEMCACHED_NOT_ALLOCATED;
}

void server_list_free(memcached_st *ptr, memcached_server_st *servers)
{
  unsigned int x;

  if (servers == NULL)
    return;

  for (x= 0; x < servers->count; x++)
    if (servers[x].address_info)
    {
      freeaddrinfo(servers[x].address_info);
      servers[x].address_info= NULL;
    }

  if (ptr && ptr->call_free)
    ptr->call_free(ptr, servers);
  else
    free(servers);
}

static int continuum_item_cmp(const void *t1, const void *t2)
{
  memcached_continuum_item_st *ct1= (memcached_continuum_item_st *)t1;
  memcached_continuum_item_st *ct2= (memcached_continuum_item_st *)t2;

  /* Why 153? Hmmm... */
  WATCHPOINT_ASSERT(ct1->value != 153);
  if (ct1->value == ct2->value)
    return 0;
  else if (ct1->value > ct2->value)
    return 1;
  else
    return -1;
}

memcached_return update_continuum(memcached_st *ptr)
{
  uint32_t index;
  uint32_t host_index;
  uint32_t continuum_index= 0;
  uint32_t value;
  memcached_server_st *list;
  uint32_t pointer_counter= 0;
  uint32_t pointer_per_server= MEMCACHED_POINTS_PER_SERVER;
  memcached_return rc;
  uint64_t total_mem_bytes= 0;
  memcached_stat_st *stat_p= NULL;
  uint32_t is_ketama_weighted= 0;

  if (ptr->number_of_hosts > ptr->continuum_count)
  {
    memcached_continuum_item_st *new_ptr;

    if (ptr->call_realloc)
      new_ptr= (memcached_continuum_item_st *)ptr->call_realloc(ptr, ptr->continuum, sizeof(memcached_continuum_item_st) * (ptr->number_of_hosts + MEMCACHED_CONTINUUM_ADDITION) * MEMCACHED_POINTS_PER_SERVER);
    else
      new_ptr= (memcached_continuum_item_st *)realloc(ptr->continuum, sizeof(memcached_continuum_item_st) * (ptr->number_of_hosts + MEMCACHED_CONTINUUM_ADDITION) * MEMCACHED_POINTS_PER_SERVER);

    if (new_ptr == 0)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

    ptr->continuum= new_ptr;
    ptr->continuum_count= ptr->number_of_hosts + MEMCACHED_CONTINUUM_ADDITION;
  }

  list = ptr->hosts;

  is_ketama_weighted= memcached_behavior_get(ptr, MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED);
  if(is_ketama_weighted) 
  {
    stat_p = memcached_stat(ptr, NULL, &rc);
    for (host_index = 0; host_index < ptr->number_of_hosts; ++host_index) 
    {
      list[host_index].limit_maxbytes= (stat_p + host_index)->limit_maxbytes;
      total_mem_bytes += (stat_p + host_index)->limit_maxbytes;
    }
  }

  for (host_index = 0; host_index < ptr->number_of_hosts; ++host_index) 
  {
    if(is_ketama_weighted) 
    {
        float pct = (float)list[host_index].limit_maxbytes/ (float)total_mem_bytes;
        pointer_per_server= floorf( pct * MEMCACHED_POINTS_PER_SERVER * (float)(ptr->number_of_hosts));
#ifdef HAVE_DEBUG
        printf("ketama_weighted:%s|%d|%llu|%u\n", list[host_index].hostname, list[host_index].port,  list[host_index].limit_maxbytes, pointer_per_server);
#endif
    }
    for(index= 1; index <= pointer_per_server; ++index) 
    {
      char sort_host[MEMCACHED_MAX_HOST_SORT_LENGTH]= "";
      size_t sort_host_length;

      sort_host_length= snprintf(sort_host, MEMCACHED_MAX_HOST_SORT_LENGTH, "%s:%d-%d", 
                                 list[host_index].hostname, list[host_index].port, index);
      WATCHPOINT_ASSERT(sort_host_length);
      value= generate_hash_value(sort_host, sort_host_length, ptr->hash_continuum);
      ptr->continuum[continuum_index].index= host_index;
      ptr->continuum[continuum_index++].value= value;
    }
    pointer_counter+= pointer_per_server;
  }

  WATCHPOINT_ASSERT(ptr);
  WATCHPOINT_ASSERT(ptr->continuum);
  WATCHPOINT_ASSERT(ptr->number_of_hosts);
  WATCHPOINT_ASSERT(ptr->number_of_hosts * MEMCACHED_POINTS_PER_SERVER <= MEMCACHED_CONTINUUM_SIZE);
  ptr->continuum_points_counter= pointer_counter;
  qsort(ptr->continuum, ptr->continuum_points_counter, sizeof(memcached_continuum_item_st), continuum_item_cmp);

  if (stat_p)
    memcached_stat_free(NULL, stat_p);

#ifdef HAVE_DEBUG
  for (index= 0; index < ((ptr->number_of_hosts * MEMCACHED_POINTS_PER_SERVER) - 1); index++) 
  {
    WATCHPOINT_ASSERT(ptr->continuum[index].value <= ptr->continuum[index + 1].value);
  }
#endif

  return MEMCACHED_SUCCESS;
}


memcached_return memcached_server_push(memcached_st *ptr, memcached_server_st *list)
{
  unsigned int x;
  uint16_t count;
  memcached_server_st *new_host_list;

  if (!list)
    return MEMCACHED_SUCCESS;

  count= list[0].count;

  if (ptr->call_realloc)
    new_host_list= 
      (memcached_server_st *)ptr->call_realloc(ptr, ptr->hosts, 
                                               sizeof(memcached_server_st) * (count + ptr->number_of_hosts));
  else
    new_host_list= 
      (memcached_server_st *)realloc(ptr->hosts, 
                                     sizeof(memcached_server_st) * (count + ptr->number_of_hosts));

  if (!new_host_list)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  ptr->hosts= new_host_list;
                                   
  for (x= 0; x < count; x++)
  {
    WATCHPOINT_ASSERT(list[x].hostname[0] != 0);
    host_reset(ptr, &ptr->hosts[ptr->number_of_hosts], list[x].hostname, 
               list[x].port, list[x].type);
    ptr->number_of_hosts++;
  }
  ptr->hosts[0].count= ptr->number_of_hosts;

  return run_distribution(ptr);
}

memcached_return memcached_server_add_unix_socket(memcached_st *ptr, char *filename)
{
  if (!filename)
    return MEMCACHED_FAILURE;

  return server_add(ptr, filename, 0, MEMCACHED_CONNECTION_UNIX_SOCKET);
}

memcached_return memcached_server_add_udp(memcached_st *ptr, 
                                          char *hostname,
                                          unsigned int port)
{
  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  if (!hostname)
    hostname= "localhost"; 

  return server_add(ptr, hostname, port, MEMCACHED_CONNECTION_UDP);
}

memcached_return memcached_server_add(memcached_st *ptr, 
                                      char *hostname, 
                                      unsigned int port)
{
  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  if (!hostname)
    hostname= "localhost"; 

  return server_add(ptr, hostname, port, MEMCACHED_CONNECTION_TCP);
}

static memcached_return server_add(memcached_st *ptr, char *hostname, 
                                   unsigned int port,
                                   memcached_connection type)
{
  memcached_server_st *new_host_list;

  if (ptr->call_realloc)
    new_host_list= (memcached_server_st *)ptr->call_realloc(ptr, ptr->hosts, 
                                                            sizeof(memcached_server_st) * (ptr->number_of_hosts+1));
  else
    new_host_list= (memcached_server_st *)realloc(ptr->hosts, 
                                                  sizeof(memcached_server_st) * (ptr->number_of_hosts+1));
  if (new_host_list == NULL)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  ptr->hosts= new_host_list;

  host_reset(ptr, &ptr->hosts[ptr->number_of_hosts], hostname, port, type);
  ptr->number_of_hosts++;
  ptr->hosts[0].count= ptr->number_of_hosts;

  return run_distribution(ptr);
}

memcached_return memcached_server_remove(memcached_server_st *st_ptr)
{
  uint32_t x, index;
  memcached_st *ptr= st_ptr->root;
  memcached_server_st *list= ptr->hosts;

  for (x= 0, index= 0; x < ptr->number_of_hosts; x++) 
  {
    if (strncmp(list[x].hostname, st_ptr->hostname, MEMCACHED_MAX_HOST_LENGTH)!=0 || list[x].port != st_ptr->port) 
    {
      memcpy(list+index, list+x, sizeof(memcached_server_st));
      index++;
    } 
  }
  ptr->number_of_hosts= index;

  if (st_ptr->address_info) 
  {
    freeaddrinfo(st_ptr->address_info);
    st_ptr->address_info= NULL;
  }
  run_distribution(ptr);

  return MEMCACHED_SUCCESS;
}

memcached_server_st *memcached_server_list_append(memcached_server_st *ptr, 
                                                  char *hostname, unsigned int port, 
                                                  memcached_return *error)
{
  unsigned int count;
  memcached_server_st *new_host_list;

  if (hostname == NULL || error == NULL)
    return NULL;

  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  /* Increment count for hosts */
  count= 1;
  if (ptr != NULL)
  {
    count+= ptr[0].count;
  } 

  new_host_list= (memcached_server_st *)realloc(ptr, sizeof(memcached_server_st) * count);
  if (!new_host_list)
  {
    *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    return NULL;
  }

  host_reset(NULL, &new_host_list[count-1], hostname, port, MEMCACHED_CONNECTION_TCP);

  /* Backwards compatibility hack */
  new_host_list[0].count= count;

  *error= MEMCACHED_SUCCESS;
  return new_host_list;
}

unsigned int memcached_server_list_count(memcached_server_st *ptr)
{
  if (ptr == NULL)
    return 0;

  return ptr[0].count;
}

void memcached_server_list_free(memcached_server_st *ptr)
{
  server_list_free(NULL, ptr);
}
