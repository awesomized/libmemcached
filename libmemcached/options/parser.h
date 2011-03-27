/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     COMMENT = 258,
     END = 259,
     ERROR = 260,
     RESET = 261,
     PARSER_DEBUG = 262,
     INCLUDE = 263,
     CONFIGURE_FILE = 264,
     EMPTY_LINE = 265,
     SERVER = 266,
     SERVERS = 267,
     SERVERS_OPTION = 268,
     UNKNOWN_OPTION = 269,
     UNKNOWN = 270,
     AUTO_EJECT_HOSTS = 271,
     BINARY_PROTOCOL = 272,
     BUFFER_REQUESTS = 273,
     CACHE_LOOKUPS = 274,
     CONNECT_TIMEOUT = 275,
     _CORK = 276,
     DISTRIBUTION = 277,
     HASH = 278,
     HASH_WITH_PREFIX_KEY = 279,
     IO_BYTES_WATERMARK = 280,
     IO_KEY_PREFETCH = 281,
     IO_MSG_WATERMARK = 282,
     KETAMA = 283,
     KETAMA_HASH = 284,
     KETAMA_WEIGHTED = 285,
     NOREPLY = 286,
     NUMBER_OF_REPLICAS = 287,
     POLL_TIMEOUT = 288,
     RANDOMIZE_REPLICA_READ = 289,
     RCV_TIMEOUT = 290,
     RETRY_TIMEOUT = 291,
     SERVER_FAILURE_LIMIT = 292,
     SND_TIMEOUT = 293,
     SOCKET_RECV_SIZE = 294,
     SOCKET_SEND_SIZE = 295,
     SORT_HOSTS = 296,
     SUPPORT_CAS = 297,
     _TCP_NODELAY = 298,
     _TCP_KEEPALIVE = 299,
     _TCP_KEEPIDLE = 300,
     USER_DATA = 301,
     USE_UDP = 302,
     VERIFY_KEY = 303,
     PREFIX_KEY = 304,
     MD5 = 305,
     CRC = 306,
     FNV1_64 = 307,
     FNV1A_64 = 308,
     FNV1_32 = 309,
     FNV1A_32 = 310,
     HSIEH = 311,
     MURMUR = 312,
     JENKINS = 313,
     CONSISTENT = 314,
     MODULA = 315,
     RANDOM = 316,
     TRUE = 317,
     FALSE = 318,
     NUMBER = 319,
     FLOAT = 320,
     HOSTNAME = 321,
     HOSTNAME_WITH_PORT = 322,
     IPADDRESS = 323,
     IPADDRESS_WITH_PORT = 324,
     STRING = 325,
     QUOTED_STRING = 326,
     FILE_PATH = 327
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




