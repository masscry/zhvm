%{

#include <zlg.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

void yyerror(YYLTYPE* loc, void* scanner, const char * err);

%}

%locations
%pure-parser

%lex-param {void* scanner}
%parse-param {void* scanner}

%union {
    int num;
    char* str;
}

%token <str> STRING
%token <num> NUMBER

%%

assignment:
    STRING '=' NUMBER ';' {
        printf( "(setf %s %d)", $1, $3 );
    }
;

%%

