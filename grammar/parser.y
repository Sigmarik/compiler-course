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
    Sequence* root_sequence = NULL;
}

%defines
%union {
    int integer;
    const char* string;

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

%token DO "do"
%token THEN "then"
%token END "end"

%token LOCAL "local"
%token FUNCTION "function"
%token MAIN "main"

%nterm <sequence> input
%nterm <sequence> sequence
%nterm <action> action
%nterm <expression> expr
%nterm <expression> expr_logics
%nterm <expression> expr_numeric
%nterm <variable> variable

%%

input:
    main opt_do sequence END {
        $$ = $3;
        root_sequence = $$;
        }
    ;

main: LOCAL FUNCTION MAIN BR_ROUND_OP BR_ROUND_CL;

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
    ;

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
    ;

%%

// Error handling function
void yyerror(char const *msg) {
    fprintf(stderr, "Error in %s:%u, column %u: %s\n", s_source_file.c_str(), yylloc.first_line, yylloc.first_column, msg);
    // fprintf(stderr, "Error: %s\n", msg);
}
