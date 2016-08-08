%option reentrant noyywrap yylineno 8bit
%option bison-bridge
%option bison-locations
%option nounistd

%top{
/**
 * @file zyaml.gen.cpp
 * @author timur and flex
 */
}

%{
// Include parser header
#include <zyaml.h>

#define YY_USER_ACTION *yylloc = yylineno;

#define YY_NO_INPUT
#define YY_NO_UNPUT

#define isatty(a) 0

%}

%x MULTIWORD

DIGIT  [[:digit:]]
SIGN   [+-]
DOT    [.]
LETTER [[:alpha:]]
CHAR   [[:alnum:]-]
COMMA  [,]
COLON  [:]
OLIST  [[]
CLIST  ]
ODICT  [{]
CDICT  [}]
HASH   [#]
SPACE  [ \t]
EOL    [\n]
STR    ["]

NUMBER  {SIGN}*{DIGIT}+{DOT}{DIGIT}*
INTEGER {SIGN}*{DIGIT}+
WORD    {LETTER}+{CHAR}*

%%

<INITIAL>{

{HASH}.*$ %{

          %}

{STR}     %{
            BEGIN(MULTIWORD);
            yylval->type = ZTT_WORD;
            yylval->text.clear();
          %}

{OLIST}   %{
            yylval->type = ZTT_LIST_START;
            yylval->text = yytext;
            return ZTT_LIST_START;
          %}

{CLIST}   %{
            yylval->type = ZTT_LIST_END;
            yylval->text = yytext;
            return ZTT_LIST_END;
          %}

{ODICT}   %{
            yylval->type = ZTT_DICT_START;
            yylval->text = yytext;
            return ZTT_DICT_START;
          %}

{CDICT}   %{
            yylval->type = ZTT_DICT_END;
            yylval->text = yytext;
            return ZTT_DICT_END;
          %}

{COMMA}   %{
            yylval->type = ZTT_COMMA;
            yylval->text = yytext;
            return ZTT_COMMA;
          %}

{COLON}   %{
            yylval->type = ZTT_COLON;
            yylval->text = yytext;
            return ZTT_COLON;
          %}

{INTEGER} %{
            yylval->type = ZTT_INTEGER;
            yylval->text = yytext;
            return ZTT_INTEGER;
          %}

{NUMBER} %{
            yylval->type = ZTT_NUMBER;
            yylval->text = yytext;
            return ZTT_NUMBER;
          %}


{WORD}    %{
            yylval->type = ZTT_WORD;
            yylval->text = yytext;
            return ZTT_WORD;
          %}

{SPACE}   %{

          %}

{EOL}     %{
            
          %}

}

<MULTIWORD>{

{STR}     %{
            BEGIN(INITIAL);
            return ZTT_WORD;
          %}

.         %{
            yylval->text.append(yytext);
          %}

}

%%
