%option reentrant noyywrap yylineno 8bit
%option bison-bridge bison-locations


%top{

/**
 * @file bfc.gen.cpp
 * @author timur
 *
 * Brainfuck ZHVM compiler
 * 
 * ++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++
 * .>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.
 * ------.--------.>+.>.
 *
 */

}

%{

#include <bfc.h>

#define YY_USER_ACTION *yylloc = yylineno;
#define YY_NO_UNPUT

%}

MOVL  [>]
MOVR  [<]
PLUS  [+]
MINUS [-]
PUTC  [.]
GETC  [,]
BEGL  \[
ENDL  \]
EOL   \n

%%

{MOVL}+  %{
            yylval->type = bfc::BT_MOVL_N;
            yylval->count = strlen(yytext);
            return bfc::BT_MOVL_N;
         %}

{MOVR}+  %{
            yylval->type = bfc::BT_MOVR_N;
            yylval->count = strlen(yytext);
            return bfc::BT_MOVR_N;
         %}

{PLUS}+  %{
            yylval->type = bfc::BT_ADD_N;
            yylval->count = strlen(yytext);
            return bfc::BT_ADD_N;
         %}

{MINUS}+ %{
            yylval->type = bfc::BT_SUB_N;
            yylval->count = strlen(yytext);
            return bfc::BT_SUB_N;
         %}

{PUTC}   %{
            yylval->type = bfc::BT_PUT;
            yylval->count = strlen(yytext);
            return bfc::BT_PUT;
         %}

{GETC}   %{
            yylval->type = bfc::BT_GET;
            yylval->count = strlen(yytext);
            return bfc::BT_GET;
         %}

{BEGL}   %{
            yylval->type = bfc::BT_LOOP;
            yylval->count = strlen(yytext);
            return bfc::BT_LOOP;
         %}

{ENDL}   %{
            yylval->type = bfc::BT_END;
            yylval->count = strlen(yytext);
            return bfc::BT_END;
         %}

{EOL}    %{

         %}

.        %{
           // IGNORE ALL
         %}

%%