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

#pragma once

#define MAX_UDP_DATAGRAM_LENGTH           1400
#define UDP_DATAGRAM_HEADER_LENGTH        8
#define UDP_REQUEST_ID_MSG_SIG_DIGITS     10
#define UDP_REQUEST_ID_THREAD_MASK        0xFFFF << UDP_REQUEST_ID_MSG_SIG_DIGITS
#define get_udp_datagram_request_id(A)    ntohs((A)->request_id)
#define get_udp_datagram_seq_num(A)       ntohs((A)->sequence_number)
#define get_udp_datagram_num_datagrams(A) ntohs((A)->num_datagrams)
#define get_msg_num_from_request_id(A)    ((A) & (~(UDP_REQUEST_ID_THREAD_MASK)))
#define get_thread_id_from_request_id(A) \
  ((A) & (UDP_REQUEST_ID_THREAD_MASK)) >> UDP_REQUEST_ID_MSG_SIG_DIGITS
#define generate_udp_request_thread_id(A) (A) << UDP_REQUEST_ID_MSG_SIG_DIGITS
#define UDP_REQUEST_ID_MAX_THREAD_ID      get_thread_id_from_request_id(0xFFFF)

struct udp_datagram_header_st {
  uint16_t request_id;
  uint16_t sequence_number;
  uint16_t num_datagrams;
  uint16_t reserved;
};

bool memcached_io_init_udp_header(memcached_instance_st *, const uint16_t thread_id);
void increment_udp_message_id(memcached_instance_st *);
