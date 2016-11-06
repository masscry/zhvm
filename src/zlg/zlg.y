%{

#define YYDEBUG 1

#include <zlg.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

void yyerror(YYLTYPE* loc, void* scanner, zlg::ast& root, const char * err);

%}

%locations
%pure-parser
%define parse.error verbose
%define parse.lac full
%define parse.trace true

%lex-param {void* scanner} 

%parse-param {void* scanner}
%parse-param {zlg::ast& root}

%union { char* str; }
%token <str> ZSTRING
%token <str> ZINLINE

%union { int64_t value; }
%token <value> ZNUMBER
%token ZPRINT
%token ZPREV

%token ZGRE
%token ZLSE
%token ZEQ
%token ZNEQ

%token ZIF
%token ZELSE
%token ZWHILE

%nonassoc ZIFX
%nonassoc ZELSE

%union { zlg::node* expr; }
%type <expr> primary_expr
%type <expr> unary_expr
%type <expr> mult_expr
%type <expr> add_expr
%type <expr> rel_expr
%type <expr> eq_expr
%type <expr> and_expr
%type <expr> or_expr
%type <expr> assign_expr
%type <expr> expr
%type <expr> stat
%type <expr> expr_stat
%type <expr> if_stat
%type <expr> iter_stat
%type <expr> block_stat
%type <expr> inline_stat
%type <expr> block_item

%union { zlg::zblock* blk; }
%type <blk> block_item_list

%union { zlg::zunop::opid unid; }
%type <unid> unary_operator

%destructor { free($$); $$ = 0; } <str> 
%destructor { delete $$; $$ = 0; } <expr>
%destructor { delete $$; $$ = 0; } <blk>

%start input

%%

primary_expr:
      ZSTRING      { $$ = new zlg::zvar($1); }
    | ZNUMBER      { $$ = new zlg::zconst($1); }
    | ZPREV        { $$ = new zlg::zprev(); }
    | '(' expr ')' { $$ = $2; $2 = 0; }
;

unary_expr:
      primary_expr                 { $$ = $1; $1 = 0; }
    | unary_operator primary_expr  { $$ = new zlg::zunop($1, zlg::node_p($2)); $2 = 0; }
;

unary_operator:
      '+'                          { $$ = zlg::zunop::PLUS;  }
    | '-'                          { $$ = zlg::zunop::MINUS; }
    | '!'                          { $$ = zlg::zunop::NOT;   }
;

mult_expr:
      unary_expr                   { $$ = $1; $1 = 0; }
    | mult_expr '*' unary_expr     { $$ = new zlg::zbinop(zlg::zbinop::MUL, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
    | mult_expr '/' unary_expr     { $$ = new zlg::zbinop(zlg::zbinop::DIV, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

add_expr:
      mult_expr                    { $$ = $1; $1 = 0; }
    | add_expr '+' mult_expr       { $$ = new zlg::zbinop(zlg::zbinop::ADD, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
    | add_expr '-' mult_expr       { $$ = new zlg::zbinop(zlg::zbinop::SUB, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

rel_expr:
      add_expr                     { $$ = $1; $1 = 0; }
    | rel_expr '<' add_expr        { $$ = new zlg::zbinop( zlg::zbinop::LS, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
    | rel_expr '>' add_expr        { $$ = new zlg::zbinop( zlg::zbinop::GR, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
    | rel_expr ZGRE add_expr       { $$ = new zlg::zbinop(zlg::zbinop::GRE, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
    | rel_expr ZLSE add_expr       { $$ = new zlg::zbinop(zlg::zbinop::LSE, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

eq_expr:
      rel_expr                     { $$ = $1; $1 = 0; }
    | eq_expr ZEQ rel_expr         { $$ = new zlg::zbinop( zlg::zbinop::EQ, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
    | eq_expr ZNEQ rel_expr        { $$ = new zlg::zbinop(zlg::zbinop::NEQ, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

and_expr:
      eq_expr                      { $$ = $1; $1 = 0; }
    | and_expr '&' eq_expr         { $$ = new zlg::zbinop(zlg::zbinop::AND, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

or_expr:
      and_expr                     { $$ = $1; $1 = 0; }
    | or_expr '|' and_expr         { $$ = new zlg::zbinop( zlg::zbinop::OR, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

assign_expr:
      or_expr                           { $$ = $1; $1 = 0; }
    | primary_expr '=' assign_expr      { $$ = new zlg::zbinop( zlg::zbinop::SET, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

expr:
      assign_expr                       { $$ = $1; $1 = 0; }
    | expr ',' assign_expr              { $$ = new zlg::zbinop( zlg::zbinop::COMMA, zlg::node_p($1), zlg::node_p($3)); $1 = 0; $3 = 0; }
;

stat:
      expr_stat                         { $$ = $1; $1 = 0; }
    | if_stat                           { $$ = $1; $1 = 0; }
    | iter_stat                         { $$ = $1; $1 = 0; }
    | block_stat                        { $$ = $1; $1 = 0; }
    | inline_stat                       { $$ = $1; $1 = 0; }
;

inline_stat:
      ZINLINE                           { $$ = new zlg::zinline($1); free($1); $1 = 0; }

expr_stat:
      '\n'                              { $$ = new zlg::znull(); }                              
    | expr '\n'                         { $$ = $1; $1 = 0; }
    | ZPRINT expr '\n'                  { $$ = new zlg::zprint(zlg::node_p($2)); $2 = 0; }
;

if_stat:
      ZIF '(' expr ')' stat %prec ZIFX  { $$ = new zlg::zif( zlg::node_p($3), zlg::node_p($5)); $3 = 0; $5 = 0; }
    | ZIF '(' expr ')' stat ZELSE stat  { $$ = new zlg::zifelse( zlg::node_p($3), zlg::node_p($5), zlg::node_p($7)); $3 = 0; $5 = 0; $7 = 0;  }
;

iter_stat:
      ZWHILE '(' expr ')' stat          { $$ = new zlg::zwhile(zlg::node_p($3), zlg::node_p($5)); $3 = 0; $5 = 0; }
;

block_stat:
      '[' ']' '\n'                      { $$ = new zlg::zblock(); }
    | '[' block_item_list ']' '\n'      { $$ = $2; $2 = 0; }
;

block_item_list:
      block_item                        { $$ = new zlg::zblock(); $$->add_item(zlg::node_p($1)); $1 = 0; }
    | block_item_list block_item        { $$ = $1;                $$->add_item(zlg::node_p($2)); $1 = 0; $2 = 0; }
;

block_item:
      stat                              { $$ = $1; $1 = 0; }
;

input:
       %empty
    |  stat                              { root.AddItem( zlg::node_p($1)); $1 = 0; }
;

%%

