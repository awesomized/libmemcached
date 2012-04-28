/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Libmemcached library
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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


%top{

#include <libmemcached/csl/common.h>
#include <libmemcached/csl/context.h>
#include <libmemcached/csl/parser.h>
#include <libmemcached/csl/symbol.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#define YY_NO_INPUT

#define YY_EXTRA_TYPE Context*

}


%{
#include <cstdlib>
#include <cstring>

#define PARAM yyget_extra(yyscanner)

#define get_lex_chars(buffer, result, max_size, context) \
{ \
  if (context->pos >= context->length) \
  { \
    result= YY_NULL; \
  } \
  else \
  { \
    result= (int)(context->length - context->pos); \
    (size_t)result > (size_t)max_size ? result= max_size : 0; \
    memcpy(buffer, context->buf + context->pos, result); \
    context->pos += result; \
  } \
}


#define YY_INPUT(buffer, result, max_size) get_lex_chars(buffer, result, max_size, PARAM)

%}

%option 8bit
%option bison-bridge
%option case-insensitive
%option debug
%option nounput
%option noyywrap
%option outfile="libmemcached/csl/scanner.cc" header-file="libmemcached/csl/scanner.h"
%option perf-report
%option prefix="config_"
%option reentrant

%%


=|,|[ ]       { return yytext[0];}

[[:digit:]]+ { yylval->number= atoi(yytext); return (NUMBER); }

:[[:digit:]]{1,5} { yylval->number= atoi(yytext +1); return PORT; }

"/?"[[:digit:]]{1,5} { yylval->number= atoi(yytext +2); return WEIGHT_START; }

[\t\r\n] ; /* skip whitespace */


^#.*$ {
      return COMMENT;
    }

"--SERVER="                          { yyextra->begin= yytext; yyextra->set_server(); return yyextra->previous_token= SERVER; }

"--SOCKET="                          { yyextra->begin= yytext; return yyextra->previous_token= SOCKET; }

"--BINARY-PROTOCOL"			{ yyextra->begin= yytext; return yyextra->previous_token= BINARY_PROTOCOL; }
"--BUFFER-REQUESTS"			{ yyextra->begin= yytext; return yyextra->previous_token= BUFFER_REQUESTS; }
"--CONFIGURE-FILE="			{ yyextra->begin= yytext; return yyextra->previous_token= CONFIGURE_FILE; }
"--CONNECT-TIMEOUT="			{ yyextra->begin= yytext; return yyextra->previous_token= CONNECT_TIMEOUT; }
"--DISTRIBUTION="			{ yyextra->begin= yytext; return yyextra->previous_token= DISTRIBUTION; }
"--HASH-WITH-NAMESPACE"	        { yyextra->begin= yytext; return yyextra->previous_token= HASH_WITH_NAMESPACE; }
"--HASH="			        { yyextra->begin= yytext; return yyextra->previous_token= HASH; }
"--IO-BYTES-WATERMARK="	        { yyextra->begin= yytext; return yyextra->previous_token= IO_BYTES_WATERMARK; }
"--IO-KEY-PREFETCH="			{ yyextra->begin= yytext; return yyextra->previous_token= IO_KEY_PREFETCH; }
"--IO-MSG-WATERMARK="	        { yyextra->begin= yytext; return yyextra->previous_token= IO_MSG_WATERMARK; }
"--NOREPLY"                         { yyextra->begin= yytext; return yyextra->previous_token= NOREPLY; }
"--NUMBER-OF-REPLICAS="	        { yyextra->begin= yytext; return yyextra->previous_token= NUMBER_OF_REPLICAS; }
"--POLL-TIMEOUT="			{ yyextra->begin= yytext; return yyextra->previous_token= POLL_TIMEOUT; }
"--RANDOMIZE-REPLICA-READ"	        { yyextra->begin= yytext; return yyextra->previous_token= RANDOMIZE_REPLICA_READ; }
"--RCV-TIMEOUT="			{ yyextra->begin= yytext; return yyextra->previous_token= RCV_TIMEOUT; }
"--REMOVE-FAILED-SERVERS="			{ yyextra->begin= yytext; return yyextra->previous_token= REMOVE_FAILED_SERVERS; }
"--RETRY-TIMEOUT="			{ yyextra->begin= yytext; return yyextra->previous_token= RETRY_TIMEOUT; }
"--SND-TIMEOUT="			{ yyextra->begin= yytext; return yyextra->previous_token= SND_TIMEOUT; }
"--SOCKET-RECV-SIZE="	        { yyextra->begin= yytext; return yyextra->previous_token= SOCKET_RECV_SIZE; }
"--SOCKET-SEND-SIZE="	        { yyextra->begin= yytext; return yyextra->previous_token= SOCKET_SEND_SIZE; }
"--SORT-HOSTS"			{ yyextra->begin= yytext; return yyextra->previous_token= SORT_HOSTS; }
"--SUPPORT-CAS"			{ yyextra->begin= yytext; return yyextra->previous_token= SUPPORT_CAS; }
"--TCP-KEEPALIVE"			{ yyextra->begin= yytext; return yyextra->previous_token= _TCP_KEEPALIVE; }
"--TCP-KEEPIDLE"			{ yyextra->begin= yytext; return yyextra->previous_token= _TCP_KEEPIDLE; }
"--TCP-NODELAY"			{ yyextra->begin= yytext; return yyextra->previous_token= _TCP_NODELAY; }
"--USE-UDP"	       		        { yyextra->begin= yytext; return yyextra->previous_token= USE_UDP; }
"--USER-DATA"			{ yyextra->begin= yytext; return yyextra->previous_token= USER_DATA; }
"--VERIFY-KEY"                      { yyextra->begin= yytext; return yyextra->previous_token= VERIFY_KEY; }

"--POOL-MIN="	       		        { yyextra->begin= yytext; return yyextra->previous_token= POOL_MIN; }
"--POOL-MAX="	       		        { yyextra->begin= yytext; return yyextra->previous_token= POOL_MAX; }

"--NAMESPACE="	       		        { yyextra->begin= yytext; return yyextra->previous_token= NAMESPACE; }

INCLUDE           { yyextra->begin= yytext; return yyextra->previous_token= INCLUDE; }
RESET           { yyextra->begin= yytext; return yyextra->previous_token= RESET; }
DEBUG           { yyextra->begin= yytext; return yyextra->previous_token= PARSER_DEBUG; }
SERVERS           { yyextra->begin= yytext; return yyextra->previous_token= SERVERS; }
END           { yyextra->begin= yytext; return yyextra->previous_token= END; }
ERROR           { yyextra->begin= yytext; return yyextra->previous_token= ERROR; }

TRUE           { return yyextra->previous_token= TRUE; }
FALSE           { return yyextra->previous_token= FALSE; }


"--"[[:alnum:]]*   {
      yyextra->begin= yytext;
      return UNKNOWN_OPTION;
    }

CONSISTENT      { return CONSISTENT; }
MODULA          { return MODULA; }
RANDOM          { return RANDOM; }

MD5			{ return MD5; }
CRC			{ return CRC; }
FNV1_64			{ return FNV1_64; }
FNV1A_64			{ return FNV1A_64; }
FNV1_32			{ return FNV1_32; }
FNV1A_32			{ return FNV1A_32; }
HSIEH			{ return HSIEH; }
MURMUR			{ return MURMUR; }
JENKINS			{ return JENKINS; }

(([[:digit:]]{1,3}"."){3}([[:digit:]]{1,3})) {
      yylval->server.port= MEMCACHED_DEFAULT_PORT;
      yylval->server.weight= 1;
      yylval->server.c_str= yyextra->set_hostname(yytext, yyleng);
      if (yylval->server.c_str)
        yylval->server.size= yyleng;
      return IPADDRESS;
    }

[[:alnum:]]["."[:alnum:]_-]+[[:alnum:]] {
      if (yyextra->is_server())
      {
        yylval->server.port= MEMCACHED_DEFAULT_PORT;
        yylval->server.weight= 1;
        yylval->server.c_str= yyextra->set_hostname(yytext, yyleng);
        if (yylval->server.c_str)
          yylval->server.size= yyleng;

        return HOSTNAME;
      }

      yylval->string.c_str = yytext;
      yylval->string.size = yyleng;

      return STRING;
    }

L?\"(\\.|[^\\"])*\" {
      yyget_text(yyscanner)[yyleng -1]= 0;
      yylval->string.c_str= yytext +1;
      yylval->string.size= yyleng -2;
      return QUOTED_STRING;
    }

.   {
      yyextra->begin= yytext;
      return UNKNOWN;
    }

%%

void Context::init_scanner()
{
  yylex_init(&scanner);
  yyset_extra(this, scanner);
}

void Context::destroy_scanner()
{
  yylex_destroy(scanner);
}

