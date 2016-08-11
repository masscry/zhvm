%option reentrant noyywrap yylineno 8bit
%option bison-bridge bison-locations

%top{

/**
 * @file cmplv2.gen.cpp
 * @author timur
 *
 * ZHVM assembler lexer ver. 2
 * 
 * $A = OPER[ ($B,)* ( ($C)* [+-] 100)*]
 *
 */

}

%{

#include <zhvm.h>
#include <cmplv2.h>

#define YY_USER_ACTION *yylloc = yylineno;
#define YY_NO_UNPUT

%}

%x REGISTER
%x COMMENT_STATE

DIGIT          [0-9]
NUMBER         {DIGIT}+
SIGN           [+-]
DOLLAR         [$]
COMMA          [,]
OPEN           \[
CLOSE          \]
SET            [=]
LOREG          [abczABCZ]
HIREG          [012345678sdpSDP]
OPER           [a-zA-Z][a-zA-Z0-9]*
SPACE          [ \t]+
COMMENT        [#]
EOL            \n

%%

<INITIAL>{

{COMMENT}       %{
                  // IGNORE EVERYTHING FROM # TO EOL
                  BEGIN(COMMENT_STATE);
                %}

{OPER}          %{
                  if (strlen(yytext)>=ZHVM_MAX_CMPL_ID){
                    ERROR_MSG("%s: %s", "OPERATOR TOO LONG (>=ZHVM_MAX_CMPL_ID)", yytext);
                    return zhvm::TT2_ERROR;
                  }
                  yylval->type = zhvm::TT2_OPERATOR;
                  strcpy(yylval->opr.val, yytext);
                  return zhvm::TT2_OPERATOR;
                %}

{SET}           %{
                  yylval->type= zhvm::TT2_SET;
                  return zhvm::TT2_SET;
                %}

{OPEN}          %{
                  yylval->type = zhvm::TT2_OPEN;
                  return zhvm::TT2_OPEN;
                %}

{CLOSE}         %{
                  yylval->type = zhvm::TT2_CLOSE;
                  return zhvm::TT2_CLOSE;
                %}

{SIGN}          %{
                  switch (yytext[0]){
                    case '+':
                      yylval->type = zhvm::TT2_SIGN_PLUS;
                      return zhvm::TT2_SIGN_PLUS;
                    case '-':
                      yylval->type = zhvm::TT2_SIGN_MINUS;
                      return zhvm::TT2_SIGN_MINUS;
                  }
                %}

{NUMBER}        %{
                  yylval->type = zhvm::TT2_NUMBER;         
                  yylval->num.val = strtol(yytext, 0, 10);
                  return zhvm::TT2_NUMBER;
                %}


{SPACE}         %{
                  // DO NOTHING
                %}

{DOLLAR}        BEGIN(REGISTER);

{COMMA}         %{
                  yylval->type = zhvm::TT2_COMMA;
                  return zhvm::TT2_COMMA;
                %}

{EOL}           %{

                %}

.               %{
                  yylval->type = zhvm::TT2_ERROR;
                  yylval->num.val = yytext[0];
                  ERROR_MSG("%s: %s", "UNEXPECTED CHARACTER", yytext);
                  return zhvm::TT2_ERROR;
                %}

}

<REGISTER>{

{LOREG}         %{
                  BEGIN(INITIAL);
                  switch (yytext[0]){
                  case 'z':
                  case 'Z':
                    yylval->type = zhvm::TT2_LOREG;
                    yylval->reg.val = zhvm::RZ;
                    break;
                  case 'a':
                  case 'A':
                    yylval->type = zhvm::TT2_LOREG;
                    yylval->reg.val = zhvm::RA;
                    break;
                  case 'b':
                  case 'B':
                    yylval->type = zhvm::TT2_LOREG;
                    yylval->reg.val = zhvm::RB;
                    break;
                  case 'c':
                  case 'C':
                    yylval->type = zhvm::TT2_LOREG;
                    yylval->reg.val = zhvm::RC;
                    break;
                  default:
                    ERROR_MSG("%s: %s", "UNHANDLED LOW REGISTER", yytext);
                    return zhvm::TT2_ERROR;
                  }
                  return zhvm::TT2_LOREG;
                %}

{HIREG}         %{
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
                    yylval->type = zhvm::TT2_HIREG;
                    yylval->reg.val = (uint32_t) ( (yytext[1] - '0') + zhvm::R0);
                    break;
                  case 's':
                  case 'S':
                    yylval->type = zhvm::TT2_HIREG;
                    yylval->reg.val = zhvm::RS;
                    break;
                  case 'd':
                  case 'D':
                    yylval->type = zhvm::TT2_HIREG;
                    yylval->reg.val = zhvm::RD;
                    break;
                  case 'p':
                  case 'P':
                    yylval->type = zhvm::TT2_HIREG;
                    yylval->reg.val = zhvm::RP;
                    break;
                  default:
                    ERROR_MSG("%s: %s", "UNHANDLED HIGH REGISTER", yytext);
                    return zhvm::TT2_ERROR;
                  }
                  return zhvm::TT2_HIREG;
                %}
                
.               %{
                  BEGIN(INITIAL);
                  yylval->type = zhvm::TT2_ERROR;
                  yylval->num.val = yytext[0];
                  ERROR_MSG("%s: %s", "UNEXPECTED CHARACTER", yytext);
                  return zhvm::TT2_ERROR;
                %}

}

<COMMENT_STATE>{

{EOL}           BEGIN(INITIAL);

.*              %{
                  // IGNORE ALL
                %}

}




%%
