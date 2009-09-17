/* -*- Mode: C; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: nil -*- */
#ifndef MEMCACHED_PROTOCOL_INTERNAL_H
#define MEMCACHED_PROTOCOL_INTERNAL_H

#include "config.h"
#include <stdbool.h>
#include <assert.h>
#include <netinet/in.h>

#ifdef linux
/* /usr/include/netinet/in.h defines macros from ntohs() to _bswap_nn to
 * optimize the conversion functions, but the prototypes generate warnings
 * from gcc. The conversion methods isn't the bottleneck for my app, so
 * just remove the warnings by undef'ing the optimization ..
 */
#undef ntohs
#undef ntohl
#undef htons
#undef htonl

#endif


/* Define this here, which will turn on the visibilty controls while we're
 * building libmemcached.
 */
#define BUILDING_LIBMEMCACHED 1

#include <libmemcached/protocol_handler.h>
#include <libmemcached/protocol/cache.h>

struct memcached_binary_protocol_st {
  struct memcached_binary_protocol_callback_st *callback;
  memcached_binary_protocol_recv_func recv;
  memcached_binary_protocol_send_func send;
  char *input_buffer;
  size_t input_buffer_size;
  bool pedantic;
  /* @todo use multiple sized buffers */
  cache_t *buffer_cache;
};


struct chunk_st {
  /* Pointer to the data */
  char *data;
  /* The offset to the first byte into the buffer that is used */
  size_t offset;
  /* The offset into the buffer for the first free byte */
  size_t nbytes;
  /* The number of bytes in the buffer */
  size_t size;
  /* Pointer to the next buffer in the chain */
  struct chunk_st *next;
};

#define CHUNK_BUFFERSIZE 2048

struct memcached_binary_protocol_client_st {
  struct memcached_binary_protocol_st *root;
  int sock;
  int error;

  /* Linked list of data to send */
  struct chunk_st *output;
  struct chunk_st *output_tail;



  char *input_buffer;
  size_t input_buffer_size;
  size_t input_buffer_offset;
  char *curr_input;


  struct chunk_st *input;
  /* Pointer to the last chunk to avoid the need to traverse the complete list */
  struct chunk_st *input_tail;
  size_t bytes_available;

  protocol_binary_request_header *current_command;
  bool quiet;
};

LIBMEMCACHED_LOCAL
bool memcached_binary_protocol_pedantic_check_request(protocol_binary_request_header *request);

LIBMEMCACHED_LOCAL
bool memcached_binary_protocol_pedantic_check_response(protocol_binary_request_header *request,
                                                       protocol_binary_response_header *response);


#endif
