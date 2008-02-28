#include <memcached.h>
#include "common.h"

/* Protoypes (static) */
static memcached_return server_add(memcached_st *ptr, char *hostname, 
                                   unsigned int port,
                                   memcached_connection type);

#define MEMCACHED_WHEEL_SIZE 1024
#define MEMCACHED_STRIDE 4
static void rebalance_wheel(memcached_st *ptr)
{
  unsigned int x;
  unsigned int y;
  unsigned int latch;

  /* Seed the Wheel */
  memset(ptr->wheel, 0, sizeof(unsigned int) * MEMCACHED_WHEEL_SIZE);

  for (latch= y= x= 0; x < MEMCACHED_WHEEL_SIZE; x++, latch++)
  {
    if (latch == MEMCACHED_STRIDE)
    {
      y++;
      if (y == ptr->number_of_hosts)
        y= 0;
      latch= 0;
    }

    ptr->wheel[x]= y;
  }
}

static int compare_servers(const void *p1, const void *p2)
{
  int return_value;
  memcached_server_st *a= (memcached_server_st *)p1;
  memcached_server_st *b= (memcached_server_st *)p2;

  return_value= strcmp(a->hostname, b->hostname);

  if (return_value == 0)
  {
    if (a->port > b->port)
      return_value++;
    else
      return_value--;
  }

  return return_value;
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
  host->sockaddr_inited= MEMCACHED_NOT_ALLOCATED;
}

void server_list_free(memcached_st *ptr, memcached_server_st *servers)
{
  unsigned int x;

  if (servers == NULL)
    return;

  for (x= 0; x < servers->count; x++)
    if (servers[x].address_info)
      freeaddrinfo(servers[x].address_info);

  if (ptr && ptr->call_free)
    ptr->call_free(ptr, servers);
  else
    free(servers);
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

  if (ptr->number_of_hosts > 1)
    qsort(ptr->hosts, ptr->number_of_hosts, sizeof(memcached_server_st), compare_servers);

  rebalance_wheel(ptr);

  return MEMCACHED_SUCCESS;
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
  LIBMEMCACHED_MEMCACHED_SERVER_ADD_START();


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
  ptr->hosts[0].count++;

  if (ptr->number_of_hosts > 1)
    qsort(ptr->hosts, ptr->number_of_hosts, sizeof(memcached_server_st), compare_servers);

  rebalance_wheel(ptr);

  LIBMEMCACHED_MEMCACHED_SERVER_ADD_END();

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
  new_host_list[0].count++;

  count= new_host_list[0].count;

  if (new_host_list[0].count > 1)
    qsort(new_host_list, count, sizeof(memcached_server_st), compare_servers);

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
