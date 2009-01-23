/* Server IO, Not public! */
#include <memcached.h>

#ifndef __MEMCACHED_IO_H__
#define __MEMCACHED_IO_H__

ssize_t memcached_io_write(memcached_server_st *ptr,
                           const void *buffer, size_t length, char with_flush);
void memcached_io_reset(memcached_server_st *ptr);
ssize_t memcached_io_read(memcached_server_st *ptr,
                          void *buffer, size_t length);
/* Read a line (terminated by '\n') into the buffer */
memcached_return memcached_io_readline(memcached_server_st *ptr,
                                       char *buffer_ptr,
                                       size_t size);
memcached_return memcached_io_close(memcached_server_st *ptr);
/* Read n bytes of data from the server and store them in dta */
memcached_return memcached_safe_read(memcached_server_st *ptr, 
                                     void *dta, 
                                     size_t size);
/* Read a single response from the server */
memcached_return memcached_read_one_response(memcached_server_st *ptr,
                                             char *buffer, size_t buffer_length,
                                             memcached_result_st *result);
#endif /* __MEMCACHED_IO_H__ */
