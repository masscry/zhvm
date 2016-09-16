%{

#include <zlg.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

void yyerror(YYLTYPE* loc, void* scanner, const char * err);

%}

%locations
%pure-parser
%define api.value.type {struct zlg::token}

%lex-param {void* scanner}
%parse-param {void* scanner}

%token <text.c_str()> STRING
%token <value> NUMBER

%start input

%%

input:

     | input assignment
;

assignment:
    STRING '=' NUMBER ';' {
        printf( "(setf %s %ld)", $1, $3 );
    }
;

%%

