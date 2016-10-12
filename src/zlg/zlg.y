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
%token ZPRINT
%token ZPREV

%token ZGRE
%token ZLSE

%left ZPRINT
%right '='
%left '&' '|'
%left '>' '<' ZGRE ZLSE
%left '+' '-'
%left '*' '/'
%left '!' UPLUS UMINUS

%type <expr> stmt

%start input

%%

input:
     %empty
     | input ZINLINE '\n' { root.AddItem(std::make_shared<zlg::zinline>($2)); }
     | input stmt '\n' { root.AddItem($2); }
;
     
stmt:
    stmt '=' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::SET, $1, $3);
    }
    | stmt '&' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::AND, $1, $3);
    }
    | stmt '|' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::OR, $1, $3);
    }
    | stmt '+' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::ADD, $1, $3);
    }
    | stmt '-' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::SUB, $1, $3);
    }
    | stmt '*' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::MUL, $1, $3);
    }
    | stmt '/' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::DIV, $1, $3);
    }
    | stmt '>' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::GR, $1, $3);
    }
    | stmt '<' stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::LS, $1, $3);
    }
    | stmt ZGRE stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::GRE, $1, $3);
    }
    | stmt ZLSE stmt {
        $$ = std::make_shared<zlg::zbinop>(zlg::zbinop::LSE, $1, $3);
    }
    | ZPRINT stmt {
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
    | '+' stmt %prec UPLUS {
        $$ = $2;        
    }
    | '-' stmt %prec UMINUS {
        $$ = std::make_shared<zlg::zunop>(zlg::zunop::MINUS,$2);
    }
    | '!' stmt {
        $$ = std::make_shared<zlg::zunop>(zlg::zunop::NOT,$2);
    }
    | '(' stmt ')' {
        $$ = $2;
    }
    | error {
        $$ = std::make_shared<zlg::zerror>();
        yyerrok; 
    }

;

%%

