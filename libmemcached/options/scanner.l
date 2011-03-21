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

#include <libmemcached/options/parser.h>
#include <libmemcached/options/string.h>
#include <libmemcached/options/symbol.h>
#include <libmemcached/options/type.h>

}


%{
#include <cstdlib>
#include <cstring>

#define PARAM yyget_extra(yyscanner)

static void get_lex_chars(char* buffer, int& result, int max_size, struct type_st *parser)
{
  if (parser->pos >= parser->length)
  {
    result = YY_NULL;
  }
  else
  {
    result = parser->length - parser->pos;
    result > (int)max_size ? result = max_size : 0;
    memcpy(buffer, parser->buf + parser->pos, result);
    parser->pos += result;
  }
}


#define YY_INPUT(buffer, result, max_size) get_lex_chars(buffer, result, max_size, PARAM)

%}

%option bison-locations
%option bison-bridge
%option case-insensitive
%option debug
%option nounput
%option noyywrap
%option outfile="libmemcached/options/scanner.cc" header-file="libmemcached/options/scanner.h"
%option perf-report
%option prefix="libmemcached_"
%option reentrant

%%


=|,|[ ]       { return yytext[0];}


[[:digit:]]+ { yylval->number = atoi(yytext); return (NUMBER); }

([[:digit:]]*.[:digit:]+) { yylval->double_number = atof(yytext); return (FLOAT); }

[\t\r\n] ; /* skip whitespace */

"--" { return DASH_OPTION; }

SERVER                          { return SERVER; }
SERVERS                         { return SERVERS; }

VERIFY_KEY                      { return VERIFY_KEY; }
VERIFY-KEY                      { return VERIFY_KEY; }
AUTO_EJECT_HOSTS	        { return AUTO_EJECT_HOSTS; }
AUTO-EJECT_HOSTS	        { return AUTO_EJECT_HOSTS; }
BINARY_PROTOCOL			{ return BINARY_PROTOCOL; }
BINARY-PROTOCOL			{ return BINARY_PROTOCOL; }
BUFFER_REQUESTS			{ return BUFFER_REQUESTS; }
BUFFER-REQUESTS			{ return BUFFER_REQUESTS; }
CACHE_LOOKUPS			{ return CACHE_LOOKUPS; }
CACHE-LOOKUPS			{ return CACHE_LOOKUPS; }
CONNECT_TIMEOUT			{ return CONNECT_TIMEOUT; }
CONNECT-TIMEOUT			{ return CONNECT_TIMEOUT; }
CORK			        { return _CORK; }
DISTRIBUTION			{ return DISTRIBUTION; }
HASH			        { return HASH; }
HASH_WITH_PREFIX_KEY	        { return HASH_WITH_PREFIX_KEY; }
HASH-WITH-PREFIX_KEY	        { return HASH_WITH_PREFIX_KEY; }
IO_BYTES_WATERMARK	        { return IO_BYTES_WATERMARK; }
IO-BYTES-WATERMARK	        { return IO_BYTES_WATERMARK; }
IO_KEY_PREFETCH			{ return IO_KEY_PREFETCH; }
IO-KEY-PREFETCH			{ return IO_KEY_PREFETCH; }
IO_MSG_WATERMARK	        { return IO_MSG_WATERMARK; }
IO-MSG-WATERMARK	        { return IO_MSG_WATERMARK; }
KETAMA			        { return KETAMA; }
KETAMA_HASH			{ return KETAMA_HASH; }
KETAMA-HASH			{ return KETAMA_HASH; }
KETAMA_WEIGHTED			{ return KETAMA_WEIGHTED; }
KETAMA-WEIGHTED			{ return KETAMA_WEIGHTED; }
NOREPLY                         { return NOREPLY; }
NUMBER_OF_REPLICAS	        { return NUMBER_OF_REPLICAS; }
NUMBER-OF-REPLICAS	        { return NUMBER_OF_REPLICAS; }
POLL_TIMEOUT			{ return POLL_TIMEOUT; }
POLL-TIMEOUT			{ return POLL_TIMEOUT; }
RANDOMIZE_REPLICA_READ	        { return RANDOMIZE_REPLICA_READ; }
RANDOMIZE-REPLICA-READ	        { return RANDOMIZE_REPLICA_READ; }
RCV_TIMEOUT			{ return RCV_TIMEOUT; }
RCV-TIMEOUT			{ return RCV_TIMEOUT; }
RETRY_TIMEOUT			{ return RETRY_TIMEOUT; }
RETRY-TIMEOUT			{ return RETRY_TIMEOUT; }
SERVER_FAILURE_LIMIT	        { return SERVER_FAILURE_LIMIT; }
SERVER-FAILURE-LIMIT	        { return SERVER_FAILURE_LIMIT; }
SND_TIMEOUT			{ return SND_TIMEOUT; }
SND-TIMEOUT			{ return SND_TIMEOUT; }
SOCKET_RECV_SIZE	        { return SOCKET_RECV_SIZE; }
SOCKET-RECV-SIZE	        { return SOCKET_RECV_SIZE; }
SOCKET_SEND_SIZE	        { return SOCKET_SEND_SIZE; }
SOCKET-SEND-SIZE	        { return SOCKET_SEND_SIZE; }
SORT_HOSTS			{ return SORT_HOSTS; }
SORT-HOSTS			{ return SORT_HOSTS; }
SUPPORT_CAS			{ return SUPPORT_CAS; }
SUPPORT-CAS			{ return SUPPORT_CAS; }
TCP_NODELAY			{ return _TCP_NODELAY; }
TCP-NODELAY			{ return _TCP_NODELAY; }
TCP_KEEPALIVE			{ return _TCP_KEEPALIVE; }
TCP-KEEPALIVE			{ return _TCP_KEEPALIVE; }
TCP_KEEPIDLE			{ return _TCP_KEEPIDLE; }
TCP-KEEPIDLE			{ return _TCP_KEEPIDLE; }
USER_DATA			{ return USER_DATA; }
USER-DATA			{ return USER_DATA; }
USE_UDP			        { return USE_UDP; }
USE-UDP	       		        { return USE_UDP; }

PREFIX-KEY	       		        { return PREFIX_KEY; }
PREFIX_KEY	       		        { return PREFIX_KEY; }

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

[[:alnum:]]+"."[[:alpha:].]+ { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return HOSTNAME;
    }

[[:digit:]]{1,3}"."[[:digit:]]{1,3}"."[[:digit:]]{1,3}"."[[:digit:]]{1,3}: { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return IPADDRESS_WITH_PORT;
    }

[[:digit:]]{1,3}"."[[:digit:]]{1,3}"."[[:digit:]]{1,3}"."[[:digit:]]{1,3}  { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return IPADDRESS;
    }

[[:alnum:]]+ { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return STRING;
    }

\"[[:alnum:]]*\" { 
      yylval->string.c_str = yytext;
      yylval->string.length = yyleng;
      return QUOTED_STRING;
    }

.   {
      return UNKNOWN;
    }

%%
