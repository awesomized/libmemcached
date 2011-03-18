/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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
  std::cerr << str << std::endl;
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

%token EQ
%token SERVER
%token SERVERS
%token TCPNODELAY
%token UNKNOWN
%token VERIFY_KEY
%token COMMA

%token <number> NUMBER
%token <number> FLOAT
%token <string> IDENTIFIER
%token <string> SERVER_WITH_PORT

%type <server> server

%%

statement:
          expression
          { }
        | statement expression
          { }
        ;


expression:
          SERVER EQ server
          { 
            (void) memcached_server_add(parser->memc, $3.c_str, $3.port);
          }
        | SERVERS EQ server_list
          { }
        | TCPNODELAY
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, true);
          }
        | VERIFY_KEY
          {
            memcached_behavior_set(parser->memc, MEMCACHED_BEHAVIOR_VERIFY_KEY, true);
          }
        ;

server_list:
          server
          {
            (void) memcached_server_add(parser->memc, $1.c_str, $1.port);
          }
        | server_list COMMA server
          {
            (void) memcached_server_add(parser->memc, $3.c_str, $3.port);
          }
        ;

server:
          SERVER_WITH_PORT NUMBER
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length;
            $$.port= $2;
          }
        | IDENTIFIER
          {
            $$.c_str= $1.c_str;
            $$.length= $1.length;
            $$.port= 80;
          }
        ;
