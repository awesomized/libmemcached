#include <memcached.h>
#include "common.h"

static void host_reset(memcached_server_st *host, char *new_hostname, unsigned int port)
{
  host->stack_responses= 0;
  host->cursor_active= 0;
  host->hostname= new_hostname;
  host->port= port;
  host->fd= -1;
}

memcached_return memcached_server_push(memcached_st *ptr, memcached_server_st *list)
{
  unsigned int x;
  unsigned int count;
  memcached_server_st *new_host_list;

  if (!list)
    return MEMCACHED_SUCCESS;

  for (count= 0; list[count].hostname; count++);

  new_host_list= 
    (memcached_server_st *)realloc(ptr->hosts, 
                                   sizeof(memcached_server_st) * (count + ptr->number_of_hosts + 1));

  if (!new_host_list)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  ptr->hosts= new_host_list;
                                   
  for (x= 0; list[x].hostname; x++)
  {
    ptr->hosts[ptr->number_of_hosts].hostname= strdup(list[x].hostname);
    ptr->hosts[ptr->number_of_hosts].port= list[x].port;
    ptr->hosts[ptr->number_of_hosts].fd= list[x].fd;
    ptr->hosts[ptr->number_of_hosts].stack_responses= list[x].stack_responses;
    ptr->hosts[ptr->number_of_hosts].cursor_active= list[x].cursor_active;
    ptr->number_of_hosts++;
  }
  host_reset(&ptr->hosts[ptr->number_of_hosts], NULL, 0);

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_server_add(memcached_st *ptr, char *hostname, unsigned int port)
{
  memcached_server_st *new_host_list;
  char *new_hostname;
  LIBMEMCACHED_MEMCACHED_SERVER_ADD_START();

  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  if (!hostname)
    hostname= "localhost"; 


  if (ptr->number_of_hosts)
  {
    new_host_list= (memcached_server_st *)realloc(ptr->hosts, 
                                                  sizeof(memcached_server_st) * (ptr->number_of_hosts+1));
    if (!new_host_list)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    host_reset(&new_host_list[ptr->number_of_hosts], NULL, 0);
  }
  else
  {
    new_host_list= 
      (memcached_server_st *)malloc(sizeof(memcached_server_st) * 2);
    if (!new_host_list)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    host_reset(&new_host_list[0], NULL, 0);
    host_reset(&new_host_list[1], NULL, 0);
  }

  ptr->hosts= new_host_list;

  new_hostname=
    (char *)malloc(sizeof(char) * (strlen(hostname)+1));

  if (!new_hostname)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  memset(new_hostname, 0, strlen(hostname)+1);
  memcpy(new_hostname, hostname, strlen(hostname));
  host_reset(&ptr->hosts[ptr->number_of_hosts], new_hostname, port);
  ptr->number_of_hosts++;

  LIBMEMCACHED_MEMCACHED_SERVER_ADD_END();

  return MEMCACHED_SUCCESS;
}

memcached_server_st *memcached_server_list_append(memcached_server_st *ptr, 
                                                  char *hostname, unsigned int port, 
                                                  memcached_return *error)
{
  unsigned int count;
  memcached_server_st *new_host_list;
  char *new_hostname;

  if (!hostname)
    return ptr;

  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  /* Always count so that we keep a free host at the end */
  if (ptr)
  {
    for (count= 0; ptr[count].hostname; count++);
    count+= 2;
    new_host_list= (memcached_server_st *)realloc(ptr, sizeof(memcached_server_st) * count);
    if (!new_host_list)
      goto error;
    host_reset(&new_host_list[count-1], NULL, 0);
  }
  else
  {
    count= 2;
    new_host_list= (memcached_server_st *)malloc(sizeof(memcached_server_st) * count);
    if (!new_host_list)
      goto error;
    host_reset(&new_host_list[0], NULL, 0);
    host_reset(&new_host_list[1], NULL, 0);
  }

  new_hostname= strdup(hostname);

  if (!new_hostname)
    goto error;

  host_reset(&new_host_list[count-2], new_hostname, port);

  *error= MEMCACHED_SUCCESS;
  return new_host_list;
error:
  *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  return NULL;
}

unsigned int memcached_server_list_count(memcached_server_st *ptr)
{
  unsigned int x;

  for (x= 0; ptr[x].hostname; x++);
  
  return x;
}

void memcached_server_list_free(memcached_server_st *ptr)
{
  unsigned int x;

  for (x= 0; ptr[x].hostname; x++)
    free(ptr[x].hostname);

  free(ptr);
}
