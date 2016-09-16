%{

#include <cstdio>
#include <cstdlib>
#include <zlg.h>
#include <zlgy.gen.hpp>  /* Generated by bison. */

%}

%option noyywrap reentrant
%option bison-bridge bison-locations

%%

[[:digit:]]+  { yylval->value = atoi(yytext);   return NUMBER;}
[[:alnum:]]+  { yylval->text.assign(strdup(yytext)); return STRING;}
"="|";"       { return yytext[0];}
.  {}

%%

void yyerror(YYLTYPE* loc, void* scanner, const char * err){
    fprintf(stderr, "%d: %s: %s", loc->first_line, "ERROR", err);
    exit(1);
}