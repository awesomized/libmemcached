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

static void host_reset(memcached_server_st *host, char *hostname, unsigned int port,
                       memcached_connection type)
{
  memset(host,  0, sizeof(memcached_server_st));
  strncpy(host->hostname, hostname, MEMCACHED_MAX_HOST_LENGTH - 1);
  host->port= port;
  host->fd= -1;
  host->type= type;
  host->read_ptr= host->read_buffer;
  host->write_ptr= host->write_buffer;
  host->sockaddr_inited= MEMCACHED_NOT_ALLOCATED;
}

memcached_return memcached_server_push(memcached_st *ptr, memcached_server_st *list)
{
  unsigned int x;
  uint16_t count;
  memcached_server_st *new_host_list;

  if (!list)
    return MEMCACHED_SUCCESS;

  count= list[0].count;

  new_host_list= 
    (memcached_server_st *)realloc(ptr->hosts, 
                                   sizeof(memcached_server_st) * (count + ptr->number_of_hosts));

  if (!new_host_list)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  ptr->hosts= new_host_list;
                                   
  for (x= 0; x < count; x++)
  {
    WATCHPOINT_ASSERT(list[x].hostname[0] != 0);
    host_reset(&ptr->hosts[ptr->number_of_hosts], list[x].hostname, 
               list[x].port, list[x].type);
    ptr->number_of_hosts++;
  }
  ptr->hosts[0].count= ptr->number_of_hosts;

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


  new_host_list= (memcached_server_st *)realloc(ptr->hosts, 
                                                sizeof(memcached_server_st) * (ptr->number_of_hosts+1));
  if (!new_host_list)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  ptr->hosts= new_host_list;

  host_reset(&ptr->hosts[ptr->number_of_hosts], hostname, port, type);
  ptr->number_of_hosts++;
  ptr->hosts[0].count++;

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

  host_reset(&new_host_list[count-1], hostname, port, MEMCACHED_CONNECTION_TCP);
  new_host_list[0].count++;


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
  unsigned int x;

  if (ptr == NULL)
    return;

  for (x= 0; x < ptr->count; x++)
    if (ptr[x].address_info)
      freeaddrinfo(ptr[x].address_info);

  free(ptr);
}
