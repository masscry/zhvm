%{

#include <cstdio>
#include <cstdlib>
#include <zlg.h>
#include <zlgy.gen.hpp>  /* Generated by bison. */

#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

#define YY_NO_UNISTD
    
%}


%option noyywrap reentrant
%option bison-bridge bison-locations
%option yylineno 8bit

%x INLINE

%%

<INITIAL>{

\%                        { BEGIN(INLINE); }
[\(\)\[\]\{\}\*\/]        { return yytext[0]; }
[\n]+                     { return '\n'; }
\!\=                      { return ZNEQ; }
\=\=                      { return ZEQ; }
[&\|!]                    { return yytext[0]; }
[+-]                      { return yytext[0]; }
[=]                       { return '='; }
[><]                      { return yytext[0]; }
\>\=                      { return ZGRE; }
\<\=                      { return ZLSE; }
_[[:blank:]]*             { return ZPREV; }
presi[[:blank:]]*         { return ZPRINT; }
se[[:blank:]]*            { return ZIF; }  
alie[[:blank:]]*          { return ZELSE; }  
dum[[:blank:]]*           { return ZWHILE; }  
funk[[:blank:]]*          { return ZFUNC; }
rezulto[[:blank:]]*       { return ZRESULT; }
[[:digit:]]+              { yylval->value = atoi(yytext); return ZNUMBER;}
[_[:alpha:]][_[:alnum:]]* { yylval->str = strdup(yytext); return ZSTRING;}
[[:blank:]]*              { }
.                         { }

}

<INLINE>{

\%                   { BEGIN(INITIAL); }
[^%]*                { yylval->str = strdup(yytext); return ZINLINE; }

}


%%

void yyerror(YYLTYPE* loc, void* scanner, zlg::ast& root, const char * err){
    fprintf(stderr, "%d: %d: %s: %s\n", loc->first_line, loc->first_column, "ERROR", err);
    exit(1);
}