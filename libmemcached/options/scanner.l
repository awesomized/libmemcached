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

%top{

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-fpermissive"

#include <iostream>

#include <libmemcached/options/context.h>
#include <libmemcached/options/parser.h>
#include <libmemcached/options/string.h>
#include <libmemcached/options/symbol.h>

#define YY_EXTRA_TYPE Context*
#define YY_USER_ACTION yylloc->first_line = yylineno;

}


%{
#include <cstdlib>
#include <cstring>

#define PARAM yyget_extra(yyscanner)

static void get_lex_chars(char* buffer, int& result, int max_size, Context *context)
{
  if (context->pos >= context->length)
  {
    result= YY_NULL;
  }
  else
  {
    result= context->length - context->pos;
    result > (int)max_size ? result = max_size : 0;
    memcpy(buffer, context->buf + context->pos, result);
    context->pos += result;
  }
}


#define YY_INPUT(buffer, result, max_size) get_lex_chars(buffer, result, max_size, PARAM)

%}

%option bison-bridge
%option bison-locations
%option case-insensitive
%option debug
%option nounput
%option noyywrap
%option yylineno
%option outfile="libmemcached/options/scanner.cc" header-file="libmemcached/options/scanner.h"
%option perf-report
%option prefix="libmemcached_"
%option reentrant

%%


=|,|[ ]       { return yytext[0];}


[[:digit:]]+ { yylval->number = atoi(yytext); return (NUMBER); }

[\t\r\n] ; /* skip whitespace */

^#.*$ {
      return COMMENT;
    }

"--SERVER"                          { yyextra->begin= yytext; return SERVER; }
"--SERVERS"                        { yyextra->begin= yytext; return SERVERS_OPTION; }

"--VERIFY_KEY"                      { yyextra->begin= yytext; return VERIFY_KEY; }
"--VERIFY-KEY"                      { yyextra->begin= yytext; return VERIFY_KEY; }
"--AUTO_EJECT_HOSTS"	        { yyextra->begin= yytext; return AUTO_EJECT_HOSTS; }
"--AUTO-EJECT_HOSTS"	        { yyextra->begin= yytext; return AUTO_EJECT_HOSTS; }
"--BINARY_PROTOCOL"			{ yyextra->begin= yytext; return BINARY_PROTOCOL; }
"--BINARY-PROTOCOL"			{ yyextra->begin= yytext; return BINARY_PROTOCOL; }
"--BUFFER_REQUESTS"			{ yyextra->begin= yytext; return BUFFER_REQUESTS; }
"--BUFFER-REQUESTS"			{ yyextra->begin= yytext; return BUFFER_REQUESTS; }
"--CACHE_LOOKUPS"			{ yyextra->begin= yytext; return CACHE_LOOKUPS; }
"--CACHE-LOOKUPS"			{ yyextra->begin= yytext; return CACHE_LOOKUPS; }
"--CONFIGURE_FILE"			{ yyextra->begin= yytext; return CONFIGURE_FILE; }
"--CONFIGURE-FILE"			{ yyextra->begin= yytext; return CONFIGURE_FILE; }
"--CONNECT_TIMEOUT"			{ yyextra->begin= yytext; return CONNECT_TIMEOUT; }
"--CONNECT-TIMEOUT"			{ yyextra->begin= yytext; return CONNECT_TIMEOUT; }
"--CORK"			        { yyextra->begin= yytext; return _CORK; }
"--DISTRIBUTION"			{ yyextra->begin= yytext; return DISTRIBUTION; }
"--HASH"			        { yyextra->begin= yytext; return HASH; }
"--HASH_WITH_PREFIX_KEY"	        { yyextra->begin= yytext; return HASH_WITH_PREFIX_KEY; }
"--HASH-WITH-PREFIX_KEY"	        { yyextra->begin= yytext; return HASH_WITH_PREFIX_KEY; }
"--IO_BYTES_WATERMARK"	        { yyextra->begin= yytext; return IO_BYTES_WATERMARK; }
"--IO-BYTES-WATERMARK"	        { yyextra->begin= yytext; return IO_BYTES_WATERMARK; }
"--IO_KEY_PREFETCH"			{ yyextra->begin= yytext; return IO_KEY_PREFETCH; }
"--IO-KEY-PREFETCH"			{ yyextra->begin= yytext; return IO_KEY_PREFETCH; }
"--IO_MSG_WATERMARK"	        { yyextra->begin= yytext; return IO_MSG_WATERMARK; }
"--IO-MSG-WATERMARK"	        { yyextra->begin= yytext; return IO_MSG_WATERMARK; }
"--KETAMA"			        { yyextra->begin= yytext; return KETAMA; }
"--KETAMA_HASH"			{ yyextra->begin= yytext; return KETAMA_HASH; }
"--KETAMA-HASH"			{ yyextra->begin= yytext; return KETAMA_HASH; }
"--KETAMA_WEIGHTED"			{ yyextra->begin= yytext; return KETAMA_WEIGHTED; }
"--KETAMA-WEIGHTED"			{ yyextra->begin= yytext; return KETAMA_WEIGHTED; }
"--NOREPLY"                         { yyextra->begin= yytext; return NOREPLY; }
"--NUMBER_OF_REPLICAS"	        { yyextra->begin= yytext; return NUMBER_OF_REPLICAS; }
"--NUMBER-OF-REPLICAS"	        { yyextra->begin= yytext; return NUMBER_OF_REPLICAS; }
"--POLL_TIMEOUT"			{ yyextra->begin= yytext; return POLL_TIMEOUT; }
"--POLL-TIMEOUT"			{ yyextra->begin= yytext; return POLL_TIMEOUT; }
"--RANDOMIZE_REPLICA_READ"	        { yyextra->begin= yytext; return RANDOMIZE_REPLICA_READ; }
"--RANDOMIZE-REPLICA-READ"	        { yyextra->begin= yytext; return RANDOMIZE_REPLICA_READ; }
"--RCV_TIMEOUT"			{ yyextra->begin= yytext; return RCV_TIMEOUT; }
"--RCV-TIMEOUT"			{ yyextra->begin= yytext; return RCV_TIMEOUT; }
"--RETRY_TIMEOUT"			{ yyextra->begin= yytext; return RETRY_TIMEOUT; }
"--RETRY-TIMEOUT"			{ yyextra->begin= yytext; return RETRY_TIMEOUT; }
"--SERVER_FAILURE_LIMIT"	        { yyextra->begin= yytext; return SERVER_FAILURE_LIMIT; }
"--SERVER-FAILURE-LIMIT"	        { yyextra->begin= yytext; return SERVER_FAILURE_LIMIT; }
"--SND_TIMEOUT"			{ yyextra->begin= yytext; return SND_TIMEOUT; }
"--SND-TIMEOUT"			{ yyextra->begin= yytext; return SND_TIMEOUT; }
"--SOCKET_RECV_SIZE"	        { yyextra->begin= yytext; return SOCKET_RECV_SIZE; }
"--SOCKET-RECV-SIZE"	        { yyextra->begin= yytext; return SOCKET_RECV_SIZE; }
"--SOCKET_SEND_SIZE"	        { yyextra->begin= yytext; return SOCKET_SEND_SIZE; }
"--SOCKET-SEND-SIZE"	        { yyextra->begin= yytext; return SOCKET_SEND_SIZE; }
"--SORT_HOSTS"			{ yyextra->begin= yytext; return SORT_HOSTS; }
"--SORT-HOSTS"			{ yyextra->begin= yytext; return SORT_HOSTS; }
"--SUPPORT_CAS"			{ yyextra->begin= yytext; return SUPPORT_CAS; }
"--SUPPORT-CAS"			{ yyextra->begin= yytext; return SUPPORT_CAS; }
"--TCP_NODELAY"			{ yyextra->begin= yytext; return _TCP_NODELAY; }
"--TCP-NODELAY"			{ yyextra->begin= yytext; return _TCP_NODELAY; }
"--TCP_KEEPALIVE"			{ yyextra->begin= yytext; return _TCP_KEEPALIVE; }
"--TCP-KEEPALIVE"			{ yyextra->begin= yytext; return _TCP_KEEPALIVE; }
"--TCP_KEEPIDLE"			{ yyextra->begin= yytext; return _TCP_KEEPIDLE; }
"--TCP-KEEPIDLE"			{ yyextra->begin= yytext; return _TCP_KEEPIDLE; }
"--USER_DATA"			{ yyextra->begin= yytext; return USER_DATA; }
"--USER-DATA"			{ yyextra->begin= yytext; return USER_DATA; }
"--USE_UDP"			        { yyextra->begin= yytext; return USE_UDP; }
"--USE-UDP"	       		        { yyextra->begin= yytext; return USE_UDP; }

"--PREFIX-KEY"	       		        { yyextra->begin= yytext; return PREFIX_KEY; }
"--PREFIX_KEY"	       		        { yyextra->begin= yytext; return PREFIX_KEY; }

INCLUDE           { yyextra->begin= yytext; return INCLUDE; }
RESET           { yyextra->begin= yytext; return RESET; }
DEBUG           { yyextra->begin= yytext; return PARSER_DEBUG; }
SERVERS           { yyextra->begin= yytext; return SERVERS; }
END           { yyextra->begin= yytext; return END; }
ERROR           { yyextra->begin= yytext; return ERROR; }

TRUE           { return TRUE; }
FALSE           { return FALSE; }


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

[[:alnum:]][[:alnum:].]*[[:alpha:]]: { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return HOSTNAME_WITH_PORT;
    }

[[:alnum:]]+"."[[:alpha:].]+[[:alnum:]] { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return HOSTNAME;
    }

(([[:digit:]]{1,3}"."){3}([[:digit:]]{1,3})): {
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return IPADDRESS_WITH_PORT;
    }

(([[:digit:]]{1,3}"."){3}([[:digit:]]{1,3})) {
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return IPADDRESS;
    }

[[:alnum:]]+ { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return STRING;
    }

(\".*\") {
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
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

