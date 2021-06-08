/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "libmemcached/common.h"

/*
 * The udp request id consists of two seperate sections
 *   1) The thread id
 *   2) The message number
 * The thread id should only be set when the memcached_st struct is created
 * and should not be changed.
 *
 * The message num is incremented for each new message we send, this function
 * extracts the message number from message_id, increments it and then
 * writes the new value back into the header
 */
void increment_udp_message_id(memcached_instance_st *ptr) {
  struct udp_datagram_header_st *header = (struct udp_datagram_header_st *) ptr->write_buffer;
  uint16_t cur_req = get_udp_datagram_request_id(header);
  int msg_num = get_msg_num_from_request_id(cur_req);
  int thread_id = get_thread_id_from_request_id(cur_req);

  if (((++msg_num) & UDP_REQUEST_ID_THREAD_MASK))
    msg_num = 0;

  header->request_id = htons((uint16_t)(thread_id | msg_num));
}

bool memcached_io_init_udp_header(memcached_instance_st *ptr, const uint16_t thread_id) {
  if (thread_id > UDP_REQUEST_ID_MAX_THREAD_ID) {
    return MEMCACHED_FAILURE;
  }

  struct udp_datagram_header_st *header = (struct udp_datagram_header_st *) ptr->write_buffer;
  header->request_id = htons(uint16_t((generate_udp_request_thread_id(thread_id))));
  header->num_datagrams = htons(1);
  header->sequence_number = htons(0);

  return MEMCACHED_SUCCESS;
}
