/* Server IO, Not public! */
#include <memcached.h>

ssize_t memcached_io_write(memcached_server_st *ptr,
                           const void *buffer, size_t length, char with_flush);
void memcached_io_reset(memcached_server_st *ptr);
ssize_t memcached_io_read(memcached_server_st *ptr,
                          void *buffer, size_t length);
memcached_return memcached_io_close(memcached_server_st *ptr, uint8_t io_death);
