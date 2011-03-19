/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Libmemcached Scanner and Parser
 *
 *  Copyright (C) 2011 DataDifferental, http://datadifferential.com
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

%{

#include <stdint.h>
#include <iostream>

#include <libmemcached/options/type.h>
#include <libmemcached/options/string.h>
#include <libmemcached/options/symbol.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <libmemcached/options/scanner.h>

inline int libmemcached_error(YYLTYPE *locp, type_st *parser, yyscan_t *scanner, const char *str)
{
#if 0
  std::cerr << str << std::endl;
#endif
  return 0;
}


%}

%error-verbose
%debug
%defines
%expect 0
%output "libmemcached/options/parser.cc"
%defines "libmemcached/options/parser.h"
%lex-param { yyscan_t *scanner }
%name-prefix="libmemcached_"
%parse-param { type_st *parser }
%parse-param { yyscan_t *scanner }
%locations
%pure-parser
%require "2.2"
%start statement
%verbose

%token SERVER
%token SERVERS
%token UNKNOWN

%token DASH_OPTION

/* All behavior options */
%token AUTO_EJECT_HOSTS
%token BINARY_PROTOCOL
%token BUFFER_REQUESTS
%token CACHE_LOOKUPS
%token CONNECT_TIMEOUT
%token _CORK
%token DISTRIBUTION
%token HASH
%token HASH_WITH_PREFIX_KEY
%token IO_BYTES_WATERMARK
%token IO_KEY_PREFETCH
%token IO_MSG_WATERMARK
%token KETAMA
%token KETAMA_HASH
%token KETAMA_WEIGHTED
%token NOREPLY
%token NUMBER_OF_REPLICAS
%token POLL_TIMEOUT
%token RANDOMIZE_REPLICA_READ
%token RCV_TIMEOUT
%token RETRY_TIMEOUT
%token SERVER_FAILURE_LIMIT
%token SND_TIMEOUT
%token SOCKET_RECV_SIZE
%token SOCKET_SEND_SIZE
%token SORT_HOSTS
%token SUPPORT_CAS
%token _TCP_NODELAY
%token _TCP_KEEPALIVE
%token _TCP_KEEPIDLE
%token USER_DATA
%token USE_UDP
%token VERIFY_KEY

/* Hash types */
%token MD5
%token CRC
%token FNV1_64
%token FNV1A_64
%token FNV1_32
%token FNV1A_32
%token HSIEH
%token MURMUR
%token JENKINS

/* Distributions */
%token CONSISTENT
%token MODULA
%token RANDOM

%nonassoc ','
%nonassoc '='

%token <number> NUMBER
%token <number> FLOAT
%token <string> IDENTIFIER
%token <string> SERVER_WITH_PORT
%token <string> IPADDRESS
%token <string> IPADDRESS_WITH_PORT

%type <server> server
%type <distribution> distribution
%type <hash> hash

%%

statement:
         DASH_OPTION expression
          { }
        | statement ' ' DASH_OPTION expression
          { }
        ;


expression:
          SERVER '=' server
          { 
            (void) memcached_server_add_parsed(parser->memc, $3.c_str, $3.length, $3.port, 0);
          }
        | SERVERS '=' server_list
          {
          }
        | behaviors
        ;

behaviors:
          AUTO_EJECT_HOSTS
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, 1);
          }
        | BINARY_PROTOCOL
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);
          }
        | BUFFER_REQUESTS
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, 1);
          }
        | CACHE_LOOKUPS
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_CACHE_LOOKUPS, 1);
          }
        | CONNECT_TIMEOUT '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, $3);
          }
        | _CORK
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_CORK, 1);
          }
        | DISTRIBUTION '=' distribution
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, $3);
          }
        | HASH '=' hash
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, $3);
          }
        | HASH_WITH_PREFIX_KEY
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY, 1);
          }
        | IO_BYTES_WATERMARK '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK, $3);
          }
        | IO_KEY_PREFETCH '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH, $3);
          }
        | IO_MSG_WATERMARK '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK, $3);
          }
        | KETAMA
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_KETAMA, true);
          }
        | KETAMA_HASH
          {
          }
        | KETAMA_WEIGHTED
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED, true);
          }
        | NOREPLY
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_NOREPLY, 1);
          }
        | NUMBER_OF_REPLICAS '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, $3);
          }
        | POLL_TIMEOUT '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, $3);
          }
        |  RANDOMIZE_REPLICA_READ
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ, true);
          }
        |  RCV_TIMEOUT '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, $3);
          }
        |  RETRY_TIMEOUT '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, $3);
          }
        |  SERVER_FAILURE_LIMIT '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, $3);
          }
        |  SND_TIMEOUT '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_SND_TIMEOUT, $3);
          }
        |  SOCKET_RECV_SIZE '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE, $3);
          }
        |  SOCKET_SEND_SIZE '=' NUMBER
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE, $3);
          }
        |  SORT_HOSTS
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_SORT_HOSTS, true);
          }
        |  SUPPORT_CAS
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_SUPPORT_CAS, true);
          }
        |  _TCP_NODELAY
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, true);
          }
        |  _TCP_KEEPALIVE
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_TCP_KEEPALIVE, true);
          }
        |  _TCP_KEEPIDLE
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_TCP_KEEPIDLE, true);
          }
        |  USER_DATA
          {
          }
        |  USE_UDP
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_USE_UDP, true);
          }
        |  VERIFY_KEY
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_VERIFY_KEY, true);
          }
        ;

server_list:
           server
          {
            (void) memcached_server_add_parsed(parser->memc, $1.c_str, $1.length, $1.port, 0);
          }
        | server_list ',' server
          {
            (void) memcached_server_add_parsed(parser->memc, $3.c_str, $3.length, $3.port, 0);
          }
        ;

server:
          SERVER_WITH_PORT NUMBER
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length -1;
            $$.port= $2;
          }
        | IDENTIFIER
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length;
            $$.port= MEMCACHED_DEFAULT_PORT;
          }
        | IPADDRESS_WITH_PORT NUMBER
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length -1;
            $$.port= $2;
          }
        | IPADDRESS
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length;
            $$.port= MEMCACHED_DEFAULT_PORT;
          }
        ;

hash:
          MD5
          {
            $$= MEMCACHED_HASH_MD5;
          }
        | CRC
          {
            $$= MEMCACHED_HASH_CRC;
          }
        | FNV1_64
          {
            $$= MEMCACHED_HASH_FNV1_64;
          }
        | FNV1A_64
          {
            $$= MEMCACHED_HASH_FNV1A_64;
          }
        | FNV1_32
          {
            $$= MEMCACHED_HASH_FNV1_32;
          }
        | FNV1A_32
          {
            $$= MEMCACHED_HASH_FNV1A_32;
          }
        | HSIEH
          {
            $$= MEMCACHED_HASH_HSIEH;
          }
        | MURMUR
          {
            $$= MEMCACHED_HASH_MURMUR;
          }
        | JENKINS
          {
            $$= MEMCACHED_HASH_JENKINS;
          }
        ;

distribution:
          CONSISTENT
          {
            $$= MEMCACHED_DISTRIBUTION_CONSISTENT;
          }
        | MODULA
          {
            $$= MEMCACHED_DISTRIBUTION_MODULA;
          }
        | RANDOM
          {
            $$= MEMCACHED_DISTRIBUTION_RANDOM;
          }
        ;
