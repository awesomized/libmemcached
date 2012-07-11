/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  LibMemcached
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

void initialize_binary_request(org::libmemcached::Instance* server, protocol_binary_request_header&);

bool memcached_io_write(org::libmemcached::Instance* ptr);

ssize_t memcached_io_write(org::libmemcached::Instance* ptr,
                           const void *buffer, size_t length, bool with_flush);

bool memcached_io_writev(org::libmemcached::Instance* ptr,
                         libmemcached_io_vector_st vector[],
                         const size_t number_of, const bool with_flush);

memcached_return_t memcached_io_wait_for_write(org::libmemcached::Instance* ptr);

void memcached_io_reset(org::libmemcached::Instance* ptr);

memcached_return_t memcached_io_read(org::libmemcached::Instance* ptr,
                                     void *buffer, size_t length, ssize_t& nread);

/* Read a line (terminated by '\n') into the buffer */
memcached_return_t memcached_io_readline(org::libmemcached::Instance* ptr,
                                         char *buffer_ptr,
                                         size_t size,
                                         size_t& total);

void memcached_io_close(org::libmemcached::Instance* ptr);

/* Read n bytes of data from the server and store them in dta */
memcached_return_t memcached_safe_read(org::libmemcached::Instance* ptr,
                                       void *dta,
                                       const size_t size);

org::libmemcached::Instance* memcached_io_get_readable_server(memcached_st *memc);

memcached_return_t memcached_io_slurp(org::libmemcached::Instance* ptr);
