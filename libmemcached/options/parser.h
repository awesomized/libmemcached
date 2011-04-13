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
     BINARY_PROTOCOL = 271,
     BUFFER_REQUESTS = 272,
     CONNECT_TIMEOUT = 273,
     DISTRIBUTION = 274,
     HASH = 275,
     HASH_WITH_NAMESPACE = 276,
     IO_BYTES_WATERMARK = 277,
     IO_KEY_PREFETCH = 278,
     IO_MSG_WATERMARK = 279,
     KETAMA_HASH = 280,
     KETAMA_WEIGHTED = 281,
     NOREPLY = 282,
     NUMBER_OF_REPLICAS = 283,
     POLL_TIMEOUT = 284,
     RANDOMIZE_REPLICA_READ = 285,
     RCV_TIMEOUT = 286,
     REMOVE_FAILED_SERVERS = 287,
     RETRY_TIMEOUT = 288,
     SND_TIMEOUT = 289,
     SOCKET_RECV_SIZE = 290,
     SOCKET_SEND_SIZE = 291,
     SORT_HOSTS = 292,
     SUPPORT_CAS = 293,
     USER_DATA = 294,
     USE_UDP = 295,
     VERIFY_KEY = 296,
     _TCP_KEEPALIVE = 297,
     _TCP_KEEPIDLE = 298,
     _TCP_NODELAY = 299,
     NAMESPACE = 300,
     POOL_MIN = 301,
     POOL_MAX = 302,
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
     FLOAT = 317,
     NUMBER = 318,
     PORT = 319,
     WEIGHT_START = 320,
     IPADDRESS = 321,
     HOSTNAME = 322,
     STRING = 323,
     QUOTED_STRING = 324,
     FILE_PATH = 325
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




