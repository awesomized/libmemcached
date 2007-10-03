#include <memcached.h>
#include "common.h"

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
    ptr->number_of_hosts++;
  }
  memset(&ptr->hosts[ptr->number_of_hosts], 0, sizeof(memcached_server_st));

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
    memset(&new_host_list[ptr->number_of_hosts], 0, sizeof(memcached_server_st));
  }
  else
  {
    new_host_list= 
      (memcached_server_st *)malloc(sizeof(memcached_server_st) * 2);
    if (!new_host_list)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    memset(new_host_list, 0, sizeof(memcached_server_st) * 2);
  }

  ptr->hosts= new_host_list;

  new_hostname=
    (char *)malloc(sizeof(char) * (strlen(hostname)+1));

  if (!new_hostname)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  memset(new_hostname, 0, strlen(hostname)+1);
  memcpy(new_hostname, hostname, strlen(hostname));
  ptr->hosts[ptr->number_of_hosts].hostname= new_hostname;
  ptr->hosts[ptr->number_of_hosts].port= port;
  ptr->hosts[ptr->number_of_hosts].fd= -1;
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
    memset(&new_host_list[count-1], 0, sizeof(memcached_server_st));
  }
  else
  {
    count= 2;
    new_host_list= (memcached_server_st *)malloc(sizeof(memcached_server_st) * count);
    if (!new_host_list)
      goto error;
    memset(new_host_list, 0, sizeof(memcached_server_st) * 2);
  }

  new_hostname= strdup(hostname);

  if (!new_hostname)
    goto error;

  new_host_list[count-2].hostname= new_hostname;
  new_host_list[count-2].port= port;
  new_host_list[count-2].fd= -1;

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
