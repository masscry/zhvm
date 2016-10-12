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

%left ZPRINT
%right '='
%left '&' '|'
%left '>' '<' ZGRE ZLSE
%left '+' '-'
%left '*' '/'
%left '!' UPLUS UMINUS

%union { zlg::node* expr; }
%type <expr> stmt

%destructor { free($$); $$ = 0; } <str> 
%destructor { delete $$; $$ = 0; } <expr>

%start input

%%

input:
     %empty
     | input ZINLINE '\n' { root.AddItem(std::make_shared<zlg::zinline>($2)); free($2); $2 = 0; }
     | input stmt '\n' { root.AddItem( zlg::node_p($2) ); $2 = 0; }
;
     
stmt:
    stmt '=' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::SET, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '&' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::AND, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '|' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::OR, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '+' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::ADD, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '-' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::SUB, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '*' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::MUL, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '/' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::DIV, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '>' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::GR, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt '<' stmt {
        $$ = new zlg::zbinop (zlg::zbinop::LS, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt ZGRE stmt {
        $$ = new zlg::zbinop (zlg::zbinop::GRE, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | stmt ZLSE stmt {
        $$ = new zlg::zbinop (zlg::zbinop::LSE, zlg::node_p($1), zlg::node_p($3));
        $1 = 0;
        $3 = 0;
    }
    | ZPRINT stmt {
        $$ = new zlg::zprint (zlg::node_p($2));
        $2 = 0;
    }
    | ZPREV {
        $$ = new zlg::zprev();
    }
    | ZNUMBER {
        $$ = new zlg::zconst ($1);
    }
    | ZSTRING {
        $$ = new zlg::zvar($1);
        free ($1);
        $1 = 0;
    }
    | '+' stmt %prec UPLUS {
        $$ = $2;        
    }
    | '-' stmt %prec UMINUS {
        $$ = new zlg::zunop(zlg::zunop::MINUS, zlg::node_p($2));
    }
    | '!' stmt {
        $$ = new zlg::zunop(zlg::zunop::NOT, zlg::node_p($2));
    }
    | '(' stmt ')' {
        $$ = $2;
    }
    | error {
        $$ = new zlg::zerror();
        yyerrok; 
    }

;

%%

