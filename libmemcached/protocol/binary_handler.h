/* -*- Mode: C; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: nil -*- */
#ifndef LIBMEMCACHED_PROTOCOL_BINARY_HANDLER_H
#define LIBMEMCACHED_PROTOCOL_BINARY_HANDLER_H

LIBMEMCACHED_LOCAL
bool memcached_binary_protocol_pedantic_check_request(protocol_binary_request_header *request);

LIBMEMCACHED_LOCAL
bool memcached_binary_protocol_pedantic_check_response(protocol_binary_request_header *request,
                                                       protocol_binary_response_header *response);

LIBMEMCACHED_LOCAL
enum MEMCACHED_PROTOCOL_EVENT memcached_binary_protocol_process_data(struct memcached_protocol_client_st *client, ssize_t *length, void **endptr);

#endif
