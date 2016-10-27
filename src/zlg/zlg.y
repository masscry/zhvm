%{

#include <zlg.h>
#include <zlgy.gen.hpp>
#include <zlg.gen.h>

void yyerror(YYLTYPE* loc, void* scanner, zlg::ast& root, const char * err);

%}

%locations
%pure-parser
%define parse.error verbose
%define parse.lac full

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

%token ZIF
%token ZELSE
%token ZWHILE

%union { zlg::node* expr; }
%type <expr> stmt
%type <expr> primary_stmt
%type <expr> unary_stmt
%type <expr> mul_stmt
%type <expr> add_stmt
%type <expr> cmp_stmt
%type <expr> and_stmt
%type <expr> or_stmt
%type <expr> set_stmt
%type <expr> print_stmt


%union { zlg::zblock* blk; }
%type <blk> block
%type <blk> stmt_list


%destructor { free($$); $$ = 0; } <str> 
%destructor { delete $$; $$ = 0; } <expr>
%destructor { delete $$; $$ = 0; } <blk>

%start input

%%

input:
     %empty
     | input ZINLINE '\n'                    { root.AddItem(std::make_shared<zlg::zinline>($2)); free($2); $2 = 0; }
     | input stmt '\n'                       { root.AddItem( zlg::node_p($2) ); $2 = 0; }
     | input block '\n'                      { root.AddItem( zlg::node_p($2) ); $2 = 0; }
     | input ZIF stmt block '\n'             { root.AddItem( std::make_shared<zlg::zif>( zlg::node_p($3), zlg::node_p($4)) ); $3 = 0; $4 = 0; }
     | input ZIF stmt block ZELSE block '\n' { root.AddItem( std::make_shared<zlg::zifelse>( zlg::node_p($3), zlg::node_p($4), zlg::node_p($6) )); $3 = 0; $4 = 0; $6 = 0; }
     | input ZWHILE stmt block '\n'          { root.AddItem( std::make_shared<zlg::zwhile>( zlg::node_p($3), zlg::node_p($4))); $3 = 0; $4 = 0; }
;

block:
       '[' ']'            { $$ = new zlg::zblock(); }
     | '[' stmt_list ']'  { $$ = $2; $2 = 0;  }
;

stmt_list:
       stmt                 { $$ = new zlg::zblock(); $$->add_item(zlg::node_p($1)); $1 = 0; }
     | stmt_list '\n' stmt  { $$ = $1;                $$->add_item(zlg::node_p($3)); $3 = 0; $1 = 0; }
;

primary_stmt:
      ZNUMBER      { $$ = new zlg::zconst($1);   }
    | ZSTRING      { $$ = new zlg::zvar($1);     }
    | ZPREV        { $$ = new zlg::zprev();      }
    | '(' stmt ')' { $$ = $2; $2 = 0;            }
    | error        { $$ = new zlg::zerror(); yyerrok; }
;

unary_stmt:
      primary_stmt   { $$ = $1;  $1 = 0; }
    | '+' unary_stmt { $$ = $2;  $2 = 0; }
    | '-' unary_stmt { $$ = new zlg::zunop(zlg::zunop::MINUS, zlg::node_p($2)); $2 = 0; }
    | '!' unary_stmt { $$ = new zlg::zunop(zlg::zunop::NOT,   zlg::node_p($2)); $2 = 0; }
;    
    
mul_stmt:
      unary_stmt              { $$ = $1;  $1 = 0; }
    | mul_stmt '*' unary_stmt { $$ = new zlg::zbinop (zlg::zbinop::MUL, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }
    | mul_stmt '/' unary_stmt { $$ = new zlg::zbinop (zlg::zbinop::DIV, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }    
;    

add_stmt:
      mul_stmt                { $$ = $1; $1 = 0; }
    | add_stmt '+' mul_stmt   { $$ = new zlg::zbinop (zlg::zbinop::ADD, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }
    | add_stmt '-' mul_stmt   { $$ = new zlg::zbinop (zlg::zbinop::SUB, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }    
;
    
cmp_stmt:
      add_stmt                { $$ = $1;  $1 = 0; }
    | cmp_stmt '>' add_stmt   { $$ = new zlg::zbinop (zlg::zbinop::GR, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }      
    | cmp_stmt '<' add_stmt   { $$ = new zlg::zbinop (zlg::zbinop::LS, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }      
    | cmp_stmt ZGRE add_stmt  { $$ = new zlg::zbinop (zlg::zbinop::GRE, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }      
    | cmp_stmt ZLSE add_stmt  { $$ = new zlg::zbinop (zlg::zbinop::LSE, zlg::node_p($1), zlg::node_p($3));
                                $1 = 0; $3 = 0; }      
;
    
and_stmt:
      cmp_stmt { $$ = $1; $1 = 0; }
    | and_stmt '&' cmp_stmt { $$ = new zlg::zbinop (zlg::zbinop::AND, zlg::node_p($1), zlg::node_p($3));
                              $1 = 0; $3 = 0; }
;

or_stmt:
      and_stmt             { $$ = $1; $1 = 0; }
    | or_stmt '|' and_stmt { $$ = new zlg::zbinop (zlg::zbinop::OR, zlg::node_p($1), zlg::node_p($3));
                             $1 = 0; $3 = 0; }  
;

set_stmt:
      or_stmt                { $$ = $1; $1 = 0; }
    | unary_stmt '=' or_stmt { $$ = new zlg::zbinop (zlg::zbinop::SET, zlg::node_p($1), zlg::node_p($3));
                               $1 = 0; $3 = 0; }      
;

print_stmt:
    set_stmt          { $$ = $1; $1 = 0; }
    | ZPRINT stmt     { $$ = new zlg::zprint (zlg::node_p($2)); $2 = 0; }

stmt:
      print_stmt      { $$ = $1 ; $1 = 0; }
     

;

%%

