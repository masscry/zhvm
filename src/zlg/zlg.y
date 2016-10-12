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


%define api.value.type {struct zlg::token}

%lex-param {void* scanner} 

%parse-param {void* scanner}
%parse-param {zlg::ast& root}

%token <text.c_str()> ZSTRING
%token <text.c_str()> ZINLINE
%token <value> ZNUMBER
%token ZEND
%token ZFUN
%token ZRESULT
%token ZBYTE
%token ZSHORT
%token ZLONG
%token ZQUAD
%token ZREG
%token ZPRINT
%token ZPREV

%right ZPRINT
%left '='
%left '+' '-'
%left '*' '/'
%left UPLUS UMINUS

%type <expr> expr

%start input

%%

input:
     %empty
     | input ZINLINE { root.AddItem(std::make_shared<zlg::zinline>($2)); }
     | input exprline
;

exprline:
    '\n'
    | end '\n'
    | expr '\n' { root.AddItem($1); }
;

end:
    ZEND {
        exit(0);
    }
;

expr:
    expr '=' expr {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::SET, $1, $3);
    }
    | expr '+' expr {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::ADD, $1, $3);
    }
    | expr '-' expr {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::SUB, $1, $3);
    }
    | expr '*' expr {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::MUL, $1, $3);
    }
    | expr '/' expr {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::DIV, $1, $3);
    }
    | ZPRINT expr {
        $$ = std::make_shared<zlg::zprint>($2);
    }
    | ZPREV {
        $$ = std::make_shared<zlg::zprev>();
    }
    | ZNUMBER {
        $$ = std::make_shared<zlg::zconst>($1);
    }
    | ZSTRING {
        $$ = std::make_shared<zlg::zvar>($1);
    }
    | '+' ZNUMBER %prec UPLUS {
        $$ = std::make_shared<zlg::zconst>($2);        
    }
    | '-' ZNUMBER %prec UMINUS {
        $$ = std::make_shared<zlg::zconst>(-$2);
    }
    | '(' expr ')' {
        $$ = $2;
    }
    | error {
        $$ = std::make_shared<zlg::zerror>();
        yyerrok; 
    }

;

%%

