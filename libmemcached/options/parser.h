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
     DISTRIBUTION = 276,
     HASH = 277,
     HASH_WITH_PREFIX_KEY = 278,
     IO_BYTES_WATERMARK = 279,
     IO_KEY_PREFETCH = 280,
     IO_MSG_WATERMARK = 281,
     KETAMA_HASH = 282,
     KETAMA_WEIGHTED = 283,
     NOREPLY = 284,
     NUMBER_OF_REPLICAS = 285,
     POLL_TIMEOUT = 286,
     RANDOMIZE_REPLICA_READ = 287,
     RCV_TIMEOUT = 288,
     RETRY_TIMEOUT = 289,
     SERVER_FAILURE_LIMIT = 290,
     SND_TIMEOUT = 291,
     SOCKET_RECV_SIZE = 292,
     SOCKET_SEND_SIZE = 293,
     SORT_HOSTS = 294,
     SUPPORT_CAS = 295,
     _TCP_NODELAY = 296,
     _TCP_KEEPALIVE = 297,
     _TCP_KEEPIDLE = 298,
     USER_DATA = 299,
     USE_UDP = 300,
     VERIFY_KEY = 301,
     PREFIX_KEY = 302,
     MD5 = 303,
     CRC = 304,
     FNV1_64 = 305,
     FNV1A_64 = 306,
     FNV1_32 = 307,
     FNV1A_32 = 308,
     HSIEH = 309,
     MURMUR = 310,
     JENKINS = 311,
     CONSISTENT = 312,
     MODULA = 313,
     RANDOM = 314,
     TRUE = 315,
     FALSE = 316,
     NUMBER = 317,
     FLOAT = 318,
     HOSTNAME = 319,
     HOSTNAME_WITH_PORT = 320,
     IPADDRESS = 321,
     IPADDRESS_WITH_PORT = 322,
     STRING = 323,
     QUOTED_STRING = 324,
     FILE_PATH = 325
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




