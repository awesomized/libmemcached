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

inline void libmemcached_error(YYLTYPE *locp, type_st *parser, yyscan_t *scanner, const char *str)
{
  memcached_string_t local_string;
  local_string.size= strlen(str);
  local_string.c_str= str;
  memcached_set_error(parser->memc, MEMCACHED_FAILURE, &local_string);
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
%token COMMENT
%token EMPTY_LINE

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

/* Callbacks */
%token PREFIX_KEY

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
%token <string> HOSTNAME
%token <string> HOSTNAME_WITH_PORT
%token <string> IPADDRESS
%token <string> IPADDRESS_WITH_PORT
%token <string> STRING
%token <string> QUOTED_STRING

%type <server> server
%type <string> string
%type <distribution> distribution
%type <hash> hash
%type <behavior> behavior_boolean
%type <behavior> behavior_number

%%

statement:
         DASH_OPTION expression
          { }
        | statement ' ' DASH_OPTION expression
          { }
        | COMMENT
          { }
        | EMPTY_LINE
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
          PREFIX_KEY '=' string
          {
            memcached_callback_set(parser->memc, MEMCACHED_CALLBACK_PREFIX_KEY, std::string($3.c_str, $3.length).c_str());
          }
        | DISTRIBUTION '=' distribution
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, $3);
          }
        | HASH '=' hash
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, $3);
          }
        | KETAMA_HASH '=' hash
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_KETAMA_HASH, $3);
          }
        | behavior_number '=' NUMBER
          {
            memcached_behavior_set(parser->memc, $1, $3);
          }
        | behavior_boolean
          {
            memcached_behavior_set(parser->memc, $1, true);
          }
        |  USER_DATA
          {
          }
        ;

behavior_number:
          CONNECT_TIMEOUT
          {
            $$= MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT;
          }
        | IO_MSG_WATERMARK
          {
            $$= MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK;
          }
        | IO_BYTES_WATERMARK
          {
            $$= MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK;
          }
        | IO_KEY_PREFETCH
          {
            $$= MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH;
          }
        | NUMBER_OF_REPLICAS
          {
            $$= MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS;
          }
        | POLL_TIMEOUT
          {
            $$= MEMCACHED_BEHAVIOR_POLL_TIMEOUT;
          }
        |  RCV_TIMEOUT
          {
            $$= MEMCACHED_BEHAVIOR_RCV_TIMEOUT;
          }
        |  RETRY_TIMEOUT
          {
            $$= MEMCACHED_BEHAVIOR_RETRY_TIMEOUT;
          }
        |  SERVER_FAILURE_LIMIT
          {
            $$= MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT;
          }
        |  SND_TIMEOUT
          {
            $$= MEMCACHED_BEHAVIOR_SND_TIMEOUT;
          }
        |  SOCKET_RECV_SIZE
          {
            $$= MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE;
          }
        |  SOCKET_SEND_SIZE
          {
            $$= MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE;
          }
        ;

behavior_boolean: 
          AUTO_EJECT_HOSTS
          {
            $$= MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS;
          }
        | BINARY_PROTOCOL
          {
            $$= MEMCACHED_BEHAVIOR_BINARY_PROTOCOL;
          }
        | BUFFER_REQUESTS
          {
            $$= MEMCACHED_BEHAVIOR_BUFFER_REQUESTS;
          }
        | CACHE_LOOKUPS
          {
            $$= MEMCACHED_BEHAVIOR_CACHE_LOOKUPS;
          }
        | _CORK
          {
            $$= MEMCACHED_BEHAVIOR_CORK;
          }
        | HASH_WITH_PREFIX_KEY
          {
            $$= MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY;
          }
        | KETAMA
          {
            $$= MEMCACHED_BEHAVIOR_KETAMA;
          }
        | KETAMA_WEIGHTED
          {
            $$= MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED;
          }
        | NOREPLY
          {
            $$= MEMCACHED_BEHAVIOR_NOREPLY;
          }
        |  RANDOMIZE_REPLICA_READ
          {
            $$= MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ;
          }
        |  SORT_HOSTS
          {
            $$= MEMCACHED_BEHAVIOR_SORT_HOSTS;
          }
        |  SUPPORT_CAS
          {
            $$= MEMCACHED_BEHAVIOR_SUPPORT_CAS;
          }
        |  _TCP_NODELAY
          {
            $$= MEMCACHED_BEHAVIOR_TCP_NODELAY;
          }
        |  _TCP_KEEPALIVE
          {
            $$= MEMCACHED_BEHAVIOR_TCP_KEEPALIVE;
          }
        |  _TCP_KEEPIDLE
          {
            $$= MEMCACHED_BEHAVIOR_TCP_KEEPIDLE;
          }
        |  USE_UDP
          {
            $$= MEMCACHED_BEHAVIOR_USE_UDP;
          }
        |  VERIFY_KEY
          {
            $$= MEMCACHED_BEHAVIOR_VERIFY_KEY;
          }


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
          HOSTNAME_WITH_PORT NUMBER
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length -1;
            $$.port= $2;
          }
        | HOSTNAME
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length;
            $$.port= MEMCACHED_DEFAULT_PORT;
          }
        | STRING /* a match can be against "localhost" which is just a string */
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

string:
          STRING
          {
            $$= $1;
          }
        | QUOTED_STRING
          {
            $$.c_str= $1.c_str +1;
            $$.length= $1.length -2;
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
