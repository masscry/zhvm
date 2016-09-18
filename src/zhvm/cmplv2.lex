%option reentrant noyywrap yylineno 8bit stack
%option bison-bridge bison-locations
%option prefix="cmplv2"

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
#include <zhvm/cmplv2.h>

#define YY_USER_ACTION *yylloc = yylineno;
#define YY_NO_UNPUT

#define ZHVM_MAX_INCLUDE (16)
YY_BUFFER_STATE include_stack[ZHVM_MAX_INCLUDE];
int include_stack_top = 0;

%}

%x REGISTER
%x COMMENT_STATE
%x MACRO_STATE
%x INCLUDE_FILE

DIGIT          [0-9]
NUMBER         {DIGIT}+
SIGN           [+-]
DOLLAR         [$]
COMMA          [,]
AT             [@]
OPEN           \[
CLOSE          \]
SET            [=]
REG            [abczABCZ012345678sdpSDP]
WORD           [a-zA-Z][a-zA-Z0-9]*
SPACE          [ \t]+
COMMENT        [#]
EOL            \n
MACRO          [!]

%%

<INITIAL>{

{COMMENT}       %{
                  // IGNORE EVERYTHING FROM # TO EOL
                  BEGIN(COMMENT_STATE);
                %}

{MACRO}         %{
                  BEGIN(MACRO_STATE);
                  yylval->type = zhvm::TT2_MACRO;
                  return zhvm::TT2_MACRO;
                %}

{WORD}          %{
                  yylval->type = zhvm::TT2_WORD;
                  yylval->opr.assign(yytext);
                  return zhvm::TT2_WORD;
                %}

{AT}            %{
                  yylval->type = zhvm::TT2_AT;
                  return zhvm::TT2_AT;
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

(0x)*{NUMBER}[slqSLQ]*  %{
                  {
                    char* end = yytext;
                    yylval->num = strtol(yytext, &end, 0);

                    switch(*end){
                    case 0:
                      yylval->type = zhvm::TT2_NUMBER_BYTE;
                      return zhvm::TT2_NUMBER_BYTE;
                    case 's':
                    case 'S':
                      yylval->type = zhvm::TT2_NUMBER_SHORT;
                      return zhvm::TT2_NUMBER_SHORT;
                    case 'l':
                    case 'L':
                      yylval->type = zhvm::TT2_NUMBER_LONG;
                      return zhvm::TT2_NUMBER_LONG;
                    case 'q':
                    case 'Q':
                      yylval->type = zhvm::TT2_NUMBER_QUAD;
                      return zhvm::TT2_NUMBER_QUAD;
                    }
                    yylval->type = zhvm::TT2_ERROR;
                    yylval->num = *end;
                    ERROR_MSG("%s: %s", "UNEXPECTED NUMBER TYPE", yytext);
                    return zhvm::TT2_ERROR; 
                  }
                %}

{SPACE}         %{
                  // DO NOTHING
                %}

{DOLLAR}        yy_push_state(REGISTER,  yyscanner);

{COMMA}         %{
                  yylval->type = zhvm::TT2_COMMA;
                  return zhvm::TT2_COMMA;
                %}

{EOL}           %{

                %}

<<EOF>>         %{
                   if ( -- include_stack_top < 0){
                     yyterminate();
                   } else {
                     yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);
                     yy_switch_to_buffer(
                       include_stack[include_stack_top] , yyscanner
                     );
                     BEGIN(MACRO_STATE);
                   }
                %}

.               %{
                  yylval->type = zhvm::TT2_ERROR;
                  yylval->num = yytext[0];
                  ERROR_MSG("%s: %s", "UNEXPECTED CHARACTER", yytext);
                  return zhvm::TT2_ERROR;
                %}

}

<REGISTER>{

{REG}         %{
                  yy_pop_state(yyscanner);
                  switch (yytext[0]){
                  case 'z':
                  case 'Z':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RZ;
                    break;
                  case 'a':
                  case 'A':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RA;
                    break;
                  case 'b':
                  case 'B':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RB;
                    break;
                  case 'c':
                  case 'C':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RC;
                    break;
                  case '0':
                  case '1':
                  case '2':
                  case '3':
                  case '4':
                  case '5':
                  case '6':
                  case '7':
                  case '8':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = (uint32_t) ( (yytext[0] - '0') + zhvm::R0);
                    break;
                  case 's':
                  case 'S':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RS;
                    break;
                  case 'd':
                  case 'D':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RD;
                    break;
                  case 'p':
                  case 'P':
                    yylval->type = zhvm::TT2_REG;
                    yylval->reg = zhvm::RP;
                    break;
                  default:
                    ERROR_MSG("%s: %s", "UNHANDLED REGISTER", yytext);
                    return zhvm::TT2_ERROR;
                  }
                  return zhvm::TT2_REG;
                %}
                
.               %{
                  yy_pop_state(yyscanner);
                  yylval->type = zhvm::TT2_ERROR;
                  yylval->num = yytext[0];
                  ERROR_MSG("%s: %s", "UNEXPECTED CHARACTER", yytext);
                  return zhvm::TT2_ERROR;
                %}

}

<INCLUDE_FILE>{

[ \t]*          // Eat spaces 

[^ \t\n]+       %{  // Got file name

                  if (include_stack_top >= ZHVM_MAX_INCLUDE){
                     ERROR_MSG("%s: %s", "Too many includes", yytext);
                     return zhvm::TT2_ERROR;
                  }

                  include_stack[include_stack_top++] = YY_CURRENT_BUFFER;
                  yyin = fopen(yytext, "r");

                  if (yyin == 0){
                     ERROR_MSG("%s: %s [%s]", "Failed to open include file", yytext, strerror(errno));  
                     return zhvm::TT2_ERROR;
                  }

                  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE, yyscanner), yyscanner);
                  BEGIN(INITIAL);

                %}

}

<MACRO_STATE>{

include         BEGIN(INCLUDE_FILE);

{DOLLAR}        yy_push_state(REGISTER,  yyscanner);

{WORD}          %{
                  yylval->type = zhvm::TT2_WORD;
                  yylval->opr.assign(yytext);
                  return zhvm::TT2_WORD;
                %}

{SPACE}         %{
                  // DO NOTHING
                %}

(0x)*{NUMBER}[slqSLQ]*  %{
                  {
                    char* end = yytext;
                    yylval->num = strtol(yytext, &end, 0);

                    switch(*end){
                    case 0:
                      yylval->type = zhvm::TT2_NUMBER_BYTE;
                      return zhvm::TT2_NUMBER_BYTE;
                    case 's':
                    case 'S':
                      yylval->type = zhvm::TT2_NUMBER_SHORT;
                      return zhvm::TT2_NUMBER_SHORT;
                    case 'l':
                    case 'L':
                      yylval->type = zhvm::TT2_NUMBER_LONG;
                      return zhvm::TT2_NUMBER_LONG;
                    case 'q':
                    case 'Q':
                      yylval->type = zhvm::TT2_NUMBER_QUAD;
                      return zhvm::TT2_NUMBER_QUAD;
                    }
                    yylval->type = zhvm::TT2_ERROR;
                    yylval->num = *end;
                    ERROR_MSG("%s: %s", "UNEXPECTED NUMBER TYPE", yytext);
                    return zhvm::TT2_ERROR; 
                  }
                %}

{EOL}          BEGIN(INITIAL);

{COMMENT}       %{
                  // IGNORE EVERYTHING FROM # TO EOL
                  BEGIN(COMMENT_STATE);
                %}
               
}

<COMMENT_STATE>{

{EOL}           BEGIN(INITIAL);

.*              %{
                  // IGNORE ALL
                %}

}




%%
