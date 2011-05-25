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

%error-verbose
%debug
%defines
%expect 0
%output "libmemcached/options/parser.cc"
%defines "libmemcached/options/parser.h"
%lex-param { yyscan_t *scanner }
%name-prefix="config_"
%parse-param { Context *context }
%parse-param { yyscan_t *scanner }
%pure-parser
%require "2.2"
%start begin
%verbose

%{

#include <config.h>

#include <stdint.h>

#include <libmemcached/common.h>
#include <libmemcached/options/context.h>
#include <libmemcached/options/symbol.h>
#include <libmemcached/options/scanner.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"

int conf_lex(YYSTYPE* lvalp, void* scanner);

#define parser_abort(A, B) do { (A)->abort((B)); YYABORT; } while (0) 

inline void config_error(Context *context, yyscan_t *scanner, const char *error)
{
  if (not context->end())
    context->abort(error);
}

%}

%token COMMENT
%token END
%token ERROR
%token RESET
%token PARSER_DEBUG
%token INCLUDE
%token CONFIGURE_FILE
%token EMPTY_LINE
%token SERVER
%token SERVERS
%token SERVERS_OPTION
%token UNKNOWN_OPTION
%token UNKNOWN

/* All behavior options */
%token BINARY_PROTOCOL
%token BUFFER_REQUESTS
%token CONNECT_TIMEOUT
%token DISTRIBUTION
%token HASH
%token HASH_WITH_NAMESPACE
%token IO_BYTES_WATERMARK
%token IO_KEY_PREFETCH
%token IO_MSG_WATERMARK
%token KETAMA_HASH
%token KETAMA_WEIGHTED
%token NOREPLY
%token NUMBER_OF_REPLICAS
%token POLL_TIMEOUT
%token RANDOMIZE_REPLICA_READ
%token RCV_TIMEOUT
%token REMOVE_FAILED_SERVERS
%token RETRY_TIMEOUT
%token SND_TIMEOUT
%token SOCKET_RECV_SIZE
%token SOCKET_SEND_SIZE
%token SORT_HOSTS
%token SUPPORT_CAS
%token USER_DATA
%token USE_UDP
%token VERIFY_KEY
%token _TCP_KEEPALIVE
%token _TCP_KEEPIDLE
%token _TCP_NODELAY

/* Callbacks */
%token NAMESPACE

/* Pool */
%token POOL_MIN
%token POOL_MAX

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

/* Boolean values */
%token <boolean> TRUE
%token <boolean> FALSE

%nonassoc ','
%nonassoc '='

%token <number> FLOAT
%token <number> NUMBER
%token <number> PORT
%token <number> WEIGHT_START
%token <server> IPADDRESS
%token <server> HOSTNAME
%token <string> STRING
%token <string> QUOTED_STRING
%token <string> FILE_PATH

%type <behavior> behavior_boolean
%type <behavior> behavior_number
%type <distribution> distribution
%type <hash> hash
%type <number> optional_port
%type <number> optional_weight
%type <string> string

%%

begin:
          statement
        | begin ' ' statement
        ;

statement:
         expression
          { }
        | COMMENT
          { }
        | EMPTY_LINE
          { }
        | END
          {
            context->set_end();
            YYACCEPT;
          }
        | ERROR
          {
            context->rc= MEMCACHED_PARSE_USER_ERROR;
            parser_abort(context, NULL);
          }
        | RESET
          {
            memcached_reset(context->memc);
          }
        | PARSER_DEBUG
          {
            yydebug= 1;
          }
        | INCLUDE ' ' string
          {
            if ((context->rc= memcached_parse_configure_file(context->memc, $3.c_str, $3.size)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);
            }
          }
        ;


expression:
          SERVER HOSTNAME optional_port optional_weight
          {
            if (memcached_failed(context->rc= memcached_server_add_with_weight(context->memc, $2.c_str, $3, $4)))
            {
              parser_abort(context, NULL);
            }
            context->unset_server();
          }
        | SERVER IPADDRESS optional_port optional_weight
          {
            if (memcached_failed(context->rc= memcached_server_add_with_weight(context->memc, $2.c_str, $3, $4)))
            {
              parser_abort(context, NULL);
            }
            context->unset_server();
          }
        | CONFIGURE_FILE string
          {
            memcached_set_configuration_file(context->memc, $2.c_str, $2.size);
          }
        | POOL_MIN NUMBER
          {
            context->memc->configure.initial_pool_size= $2;
          }
        | POOL_MAX NUMBER
          {
            context->memc->configure.max_pool_size= $2;
          }
        | behaviors
        ;

behaviors:
          NAMESPACE string
          {
            if ((context->rc= memcached_set_prefix_key(context->memc, $2.c_str, $2.size)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);;
            }
          }
        | DISTRIBUTION distribution
          {
            if ((context->rc= memcached_behavior_set(context->memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, $2)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);;
            }
          }
        | DISTRIBUTION distribution ',' hash
          {
            if ((context->rc= memcached_behavior_set(context->memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, $2)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);;
            }
            if ((context->rc= memcached_behavior_set_distribution_hash(context->memc, $4)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);;
            }
          }
        | HASH hash
          {
            if ((context->rc= memcached_behavior_set(context->memc, MEMCACHED_BEHAVIOR_HASH, $2)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);; 
            }
          }
        | behavior_number NUMBER
          {
            if ((context->rc= memcached_behavior_set(context->memc, $1, $2)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);;
            }
          }
        | behavior_boolean
          {
            if ((context->rc= memcached_behavior_set(context->memc, $1, true)) != MEMCACHED_SUCCESS)
            {
              parser_abort(context, NULL);;
            }
          }
        |  USER_DATA
          {
          }
        ;

behavior_number:
          REMOVE_FAILED_SERVERS
          {
            $$= MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS;
          }
        | CONNECT_TIMEOUT
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
          BINARY_PROTOCOL
          {
            $$= MEMCACHED_BEHAVIOR_BINARY_PROTOCOL;
          }
        | BUFFER_REQUESTS
          {
            $$= MEMCACHED_BEHAVIOR_BUFFER_REQUESTS;
          }
        | HASH_WITH_NAMESPACE
          {
            $$= MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY;
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


optional_port:
          { $$= MEMCACHED_DEFAULT_PORT;}
        | PORT
          { };
        ;

optional_weight:
          { $$= 1; }
        | WEIGHT_START
          { }
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
            $$.c_str= $1.c_str +1; // +1 to move use passed the initial quote
            $$.size= $1.size -2; // -2 removes the begin and end quote
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

%% 

void Context::start() 
{
  config_parse(this, (void **)scanner);
}

