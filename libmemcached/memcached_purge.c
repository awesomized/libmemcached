#include <assert.h>

#include "common.h"
#include "memcached_io.h"

memcached_return memcached_purge(memcached_server_st *ptr) 
{
  memcached_return rc;
  int32_t timeout;
  char buffer[2048];
  memcached_result_st result;
  memcached_result_st *result_ptr;

  if (ptr->root->purging || /* already purging */
      (memcached_server_response_count(ptr) < ptr->root->io_msg_watermark && 
       ptr->io_bytes_sent < ptr->root->io_bytes_watermark) ||
      (ptr->io_bytes_sent > ptr->root->io_bytes_watermark && 
       memcached_server_response_count(ptr) < 10)) 
  {
    return MEMCACHED_SUCCESS;
  }

  /* memcached_io_write and memcached_response may call memcached_purge
     so we need to be able stop any recursion.. */
  ptr->root->purging= 1;

  WATCHPOINT_ASSERT(ptr->fd != -1);
  /* Force a flush of the buffer to ensure that we don't have the n-1 pending
     requests buffered up.. */
  if (memcached_io_write(ptr, NULL, 0, 1) == -1)
    return MEMCACHED_FAILURE;
  WATCHPOINT_ASSERT(ptr->fd != -1);

  /* we have already incremented the response counter, and memcached_response
     will read out all messages.. To avoid memcached_response to wait forever
     for a response to a command I have in my buffer, let's decrement the 
     response counter :) */
  memcached_server_response_decrement(ptr);
  
  /* memcached_response may call memcached_io_read, but let's use a short
     timeout if there is no data yet */
  timeout= ptr->root->poll_timeout;
  ptr->root->poll_timeout= 1;
  result_ptr= memcached_result_create(ptr->root, &result);

  if (result_ptr == NULL)
    return MEMCACHED_FAILURE;

  WATCHPOINT_ASSERT(ptr->fd != -1);
  rc= memcached_response(ptr, buffer, sizeof(buffer), &result);
  WATCHPOINT_ERROR(rc);
  WATCHPOINT_ASSERT(ptr->fd != -1);
  ptr->root->poll_timeout= timeout;
  memcached_server_response_increment(ptr);
  ptr->root->purging = 0;

  memcached_result_free(&result);

  return rc;
}
