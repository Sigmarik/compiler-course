%require "3.5"

%locations
%define parse.error detailed
// %define api.pure full

%code requires {
    #include <stdio.h>
    #include <stdlib.h>

    #include <string>
    #include <iostream>

    #include "ast/ast.hpp"
    #include "ast/global.h"

    void yyerror(char const *msg);
    int yylex();

#define MOVE_TO_UNIQUE(type, what)  \
    std::unique_ptr<type>(new type(std::move(what)))

#define BINARY_OP(tp, lft, rht) \
new Expression{BinaryOperator{  \
    BinaryOperator::Type::tp,   \
    lft,                        \
    rht                         \
}}
}
%code{
    Module* root_module = NULL;
}

%defines
%union {
    int integer;
    const char* string;

    Module* module;
    FunctionTemplate* function;
    FunctionCall* call;
    Sequence* sequence;
    Action* action;
    Expression* expression;
    Variable* variable;
}

%token<integer> NUMBER "number"
%left PLUS "+" MINUS "-" EQUAL "==" ASSIGN "="

%token<string> NAME "name"
%token IF "if"
%token ELSE "else"
%token BR_ROUND_OP "("
%token BR_ROUND_CL ")"
%token BR_CURLY_OP "{"
%token BR_CURLY_CL "}"
%token PRINT "print"

%token COMMA ","

%token DO "do"
%token THEN "then"
%token END "end"

%token LOCAL "local"
%token FUNCTION "function"
%token MAIN "main"

%token RETURN "return"

%type <module> module
%type <function> func_decl
%type <function> func_decl_left
%type <function> pure_func_decl

%type <call> function_call_left
%type <call> call

%type <sequence> input
%type <sequence> sequence
%type <action> action
%type <expression> expr
%type <expression> expr_logics
%type <expression> expr_numeric
%type <variable> variable

%%

input: module { root_module = $1; }

module: {
        $$ = new Module();
    }
    | module func_decl {
        $$ = $1;
        $$->templates.push_back($2);
    }

func_decl: func_decl_left BR_ROUND_CL opt_do sequence END {
        $$ = $1;
        $$->body = $4;
    }
    | pure_func_decl opt_do sequence END {
        $$ = $1;
        $$->body = $3;
    }

pure_func_decl: optional_local FUNCTION NAME BR_ROUND_OP BR_ROUND_CL {
        $$ = new FunctionTemplate();
        $$->name = $3;
    }

func_decl_left: optional_local FUNCTION NAME BR_ROUND_OP NAME {
        $$ = new FunctionTemplate();
        $$->name = $3;
        $$->paramNames.push_back($5);
    }
    | func_decl_left COMMA NAME {
        $$ = $1;
        $$->paramNames.push_back($3);
    }

optional_local:
    | LOCAL

opt_do:
    | DO

sequence: {
        $$ = new Sequence{};
    }
    | sequence action {
        $$ = $1;
        $$->actions.push_back($2);
    }
    ;

action:
    LOCAL NAME ASSIGN expr {
        $$ = new Action{Assignment{
            new Variable($2),
            $4
        }};
        }
    | variable ASSIGN expr {
        $$ = new Action{Assignment {
            $1,
            $3
        }};
        }
    | IF expr
        THEN sequence END {
            $$ = new Action{Branch {
                $2,
                $4,
                new Sequence{}
            }};
        }
    | IF expr 
        THEN sequence ELSE
        sequence END {
            $$ = new Action{Branch {
                $2,
                $4,
                $6
            }};
        }
    | PRINT BR_ROUND_OP expr BR_ROUND_CL {
            $$ = new Action{Print { $3 }};
        }
    | RETURN expr {
            $$ = new Action{Return{$2}};
        }
    | call {
            $$ = new Action{*$1};
        }
    ;

call: NAME BR_ROUND_OP BR_ROUND_CL {
            $$ = new FunctionCall();
            $$->name = $1;
        }
    | function_call_left BR_ROUND_CL {
            $$ = $1;
        }

function_call_left: NAME BR_ROUND_OP expr {
            $$ = new FunctionCall{};
            $$->name = $1;
            $$->arguments.push_back($3);
        }
    | function_call_left COMMA expr {
            $$ = $1;
            $$->arguments.push_back($3);
        }

variable:
    NAME {
        $$ = new Variable{$1};
        $$->pos.line = yylloc.first_line;
        $$->pos.column = yylloc.first_column;
        }
    ;

expr:
    expr_logics { $$ = $1; }

expr_logics:
    expr_numeric { $$ = $1; }
    | expr_numeric EQUAL expr_numeric { $$ = BINARY_OP(Equal, $1, $3); }

expr_numeric:
    NUMBER { $$ = new Expression{Constant{ $1 }}; }
    | variable { $$ = new Expression{*$1}; }
    | BR_ROUND_OP expr BR_ROUND_CL { $$ = $2; }
    | expr_numeric PLUS expr_numeric { $$ = BINARY_OP(Add, $1, $3); }
    | expr_numeric MINUS expr_numeric { $$ = BINARY_OP(Subtract, $1, $3); }
    | call { $$ = new Expression(*$1); }
    ;

%%

// Error handling function
void yyerror(char const *msg) {
    fprintf(stderr, "Error in %s:%u, column %u: %s\n", s_source_file.c_str(), yylloc.first_line, yylloc.first_column, msg);
    // fprintf(stderr, "Error: %s\n", msg);
}
