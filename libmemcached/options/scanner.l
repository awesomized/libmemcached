
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

[=] { return EQ; }
[,] { return COMMA; }

[0-9]+ { yylval->number = atoi(yytext); return (NUMBER); }

([0-9]*.[0-9]+) { yylval->double_number = atof(yytext); return (FLOAT); }

[ \t\r\n] ; /* skip whitespace */

"--SERVER"                 { return SERVER; }
"--SERVERS"                 { return SERVERS; }
"--TCP_NODELAY"                 { return TCPNODELAY; }
"--TCP-NODELAY"                 { return TCPNODELAY; }
"--VERIFY_KEY"                 { return VERIFY_KEY; }
"--VERIFY-KEY"                 { return VERIFY_KEY; }

[A-Za-z][A-Za-z0-9_]*[:]    { 
                              yylval->string.c_str = yytext;
                              yylval->string.length = yyleng;
                              return SERVER_WITH_PORT;
                            }

[A-Za-z][A-Za-z0-9_]*   { 
                          yylval->string.c_str = yytext;
                          yylval->string.length = yyleng;
                          return IDENTIFIER;
                        }
[-] ;

.                     {
                        std::cerr << "Near " <<  yytext << std::endl;
                        return UNKNOWN;
                      }

%%
