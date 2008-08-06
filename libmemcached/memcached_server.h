/*
 * Summary: String structure used for libmemcached.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Brian Aker
 */

#ifndef __MEMCACHED_SERVER_H__
#define __MEMCACHED_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

struct memcached_server_st {
  char hostname[MEMCACHED_MAX_HOST_LENGTH];
  unsigned int port;
  int fd;
  int cached_errno;
  unsigned int cursor_active;
  char write_buffer[MEMCACHED_MAX_BUFFER];
  size_t write_buffer_offset;
  char read_buffer[MEMCACHED_MAX_BUFFER];
  size_t read_data_length;
  size_t read_buffer_length;
  char *read_ptr;
  memcached_allocated sockaddr_inited;
  struct addrinfo *address_info;
  memcached_connection type;
  uint8_t major_version;
  uint8_t minor_version;
  uint8_t micro_version;
  uint16_t count;
  time_t next_retry;
  memcached_st *root;
  uint64_t limit_maxbytes;
  uint32_t server_failure_counter;
};

#define memcached_server_count(A) (A)->number_of_hosts
#define memcached_server_name(A,B) (B).hostname
#define memcached_server_port(A,B) (B).port
#define memcached_server_list(A) (A)->hosts
#define memcached_server_response_count(A) (A)->cursor_active


#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_SERVER_H__ */
