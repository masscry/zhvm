%option reentrant noyywrap yylineno 8bit
%option bison-bridge
%option bison-locations

%{
 
/**
 * @file zhlex.cpp
 * @author timur
 *
 * ZHVM assembler lexer
 */


#include <zhvm.h>
#include <iostream>
#include <zhlex.h>

#define YY_USER_ACTION yylloc->line = yylineno; yylloc->col = yycolumn;

%}

%x EXPECT_REGISTER
%x EXPECT_ID

DIGIT    [0-9]
ID       [a-zA-Z][a-zA-Z0-9]*
DOLLAR   [$]
SREG     [abczABCZ]
EREG     [012345678sdpSDP]
COMMA    [,]
SPACE    [ ]+
SIGN     [+-]
MACRO    [!]

%%

<INITIAL>{

{ID}            %{
                  if (strlen(yytext)>=ZHVM_MAX_TOKEN_ID){
                    std::cerr << "LEN(ID) >= " << ZHVM_MAX_TOKEN_ID << std::endl;
                    return zhvm::TOK_ERROR;
                  }
                  strcpy(yylval->id.val, yytext);
                  yylval->id.tok = zhvm::TOK_ID;
                  return zhvm::TOK_ID;
                %}

{DOLLAR}        BEGIN(EXPECT_REGISTER);

{MACRO}         BEGIN(EXPECT_ID);

{SIGN}*{DIGIT}+ %{
                  
                  yylval->num.val = strtol(yytext, 0, 10);
                  yylval->num.tok = zhvm::TOK_NUMBER;
                  return zhvm::TOK_NUMBER;
                  
                %}

{SPACE}         %{
                  // DO NOTHING
                %}

{COMMA}         %{
                  yylval->tok = zhvm::TOK_COMMA;
                  return zhvm::TOK_COMMA;
                %}

\n              %{
                  yylval->tok = zhvm::TOK_EOL;
                  return zhvm::TOK_EOL;
                %}

}

<EXPECT_ID>{

{ID}            %{
                  BEGIN(INITIAL);
                                    
                  if (strlen(yytext)>=ZHVM_MAX_TOKEN_ID){
                    std::cerr << "LEN(ID) >= " << ZHVM_MAX_TOKEN_ID << std::endl;
                    return zhvm::TOK_ERROR;
                  }
                  std::cout << "MACRO ID" << std::endl;
                  strcpy (yylval->id.val, yytext);
                  yylval->id.tok = zhvm::TOK_MACRO;
                  return zhvm::TOK_MACRO;

                %}

.               %{
                  std::cerr << "ID word expected" << std::endl;
                  return zhvm::TOK_ERROR;
                %}

}

<EXPECT_REGISTER>{

{SREG}          %{                  
                  BEGIN(INITIAL);

                  switch (yytext[0]){
                  case 'z':
                  case 'Z':
                    yylval->reg.val = zhvm::RZ;
                    yylval->reg.tok = zhvm::TOK_SREG;
                    break;
                  case 'a':
                  case 'A':
                    yylval->reg.val = zhvm::RA;
                    yylval->reg.tok = zhvm::TOK_SREG;
                    break;
                  case 'b':
                  case 'B':
                    yylval->reg.val = zhvm::RB;
                    yylval->reg.tok = zhvm::TOK_SREG;
                    break;
                  case 'c':
                  case 'C':
                    yylval->reg.val = zhvm::RC;
                    yylval->reg.tok = zhvm::TOK_SREG;
                    break;
                  default:
                    std::cerr << "UNHANDLED SIMPLE REGISTER: "<< yytext << std::endl;
                    return zhvm::TOK_ERROR;
                  }
                  return zhvm::TOK_SREG;

                %}

{EREG}          %{
                  BEGIN(INITIAL);

                  switch (yytext[0]){
                  case '0':
                  case '1':
                  case '2':
                  case '3':
                  case '4':
                  case '5':
                  case '6':
                  case '7':
                  case '8':
                    yylval->reg.val = (uint32_t) ( (yytext[1] - '0') + zhvm::R0);
                    yylval->reg.tok = zhvm::TOK_EREG;
                    break;
                  case 's':
                  case 'S':
                    yylval->reg.val = zhvm::RS;
                    yylval->reg.tok = zhvm::TOK_EREG;
                    break;
                  case 'd':
                  case 'D':
                    yylval->reg.val = zhvm::RD;
                    yylval->reg.tok = zhvm::TOK_EREG;
                    break;
                  case 'p':
                  case 'P':
                    yylval->reg.val = zhvm::RP;
                    yylval->reg.tok = zhvm::TOK_EREG;
                    break;
                  default:
                    std::cerr << "UNHANDLED EXTENDED REGISTER: " << yytext << std::endl;
                    return zhvm::TOK_ERROR;
                  }
                  return zhvm::TOK_EREG;
                %}

}

<INITIAL,EXPECT_REGISTER>{

.               %{
                  std::cout << "UNKNOWN TOKEN: " << yylineno << ": " << yytext << std::endl;
                  return zhvm::TOK_ERROR;
                %}

}

%%
