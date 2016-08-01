%option reentrant stack noyywrap yylineno 8bit
%option extra-type="void*"

%{
 
/**
 * @file zhlex.cpp
 * @author timur
 *
 * ZHVM assembler lexer
 */

#include <zhvm.h>
#include <iostream>

union


#define YY_DECL int cmpl();

%}

%x EXPECT_REGISTER

DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*
DOLLAR   [$]
SREG     [abczABCZ]
EREG     [012345678sdpSDP]
COMMA    [,]
SPACE    [ ]+
SIGN     [+-]

%%

<INITIAL>{

{ID}            %{
                  std::cout << "ID: " << yytext << std::endl;
                %}

{DOLLAR}        BEGIN(EXPECT_REGISTER);

{SIGN}*{DIGIT}+ %{
                  std::cout << "NUM: " << yytext << std::endl;
                %}

{SPACE}         %{
                  std::cout << "SPACE" << std::endl;
                %}

{COMMA}         %{
                  std::cout << "COMMA" << std::endl;
                %}

\n              %{
                  
                %}

}

<EXPECT_REGISTER>{

{SREG}          %{
                  std::cout << "SREG: " << yytext << std::endl;
                  BEGIN(INITIAL);
                %}

{EREG}          %{
                  std::cout << "EREG: " << yytext << std::endl;
                  BEGIN(INITIAL);
                %}

}

.               %{
                  std::cout << "UNKNOWN: " << yylineno << ": " << yytext << std::endl;
                %}

%%

int test(){

  yyscan_t scanner;
  void* extra = 0;

  yylex_init_extra(extra, &scanner);

  yylex(scanner);

  yylex_destroy(scanner);
  return 0;
}
