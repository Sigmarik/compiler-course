%require "3.5"

%code requires {
    #include <stdio.h>
    #include <stdlib.h>

    #include <string>

    #include "ast/ast.hpp"

    static NameMap s_variables;

    void yyerror(const char *s);
    int yylex();

#define MOVE_TO_UNIQUE(type, what)\
    std::unique_ptr<type>(new type(std::move(what)))
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

%token<integer> NUMBER
%left PLUS MINUS EQUAL ASSIGN
%token EOL

%token<string> NAME;
%token IF ELSE
%token BR_ROUND_OP BR_ROUND_CL BR_CURLY_OP BR_CURLY_CL
%token PRINT
%token DECL SEP

%token MAIN

%nterm <sequence> input
%nterm <sequence> sequence
%nterm <action> action
%nterm <expression> expr
%nterm <variable> variable

%%

input:
    MAIN BR_CURLY_OP sequence BR_CURLY_CL {
        $$ = $3;
        root_sequence = $$;
        }
    ;

sequence: {
        $$ = new Sequence{};
    }
    | sequence action {
        $$ = $1;
        $$->actions.push_back($2);
    }
    ;

action:
    DECL NAME SEP expr EOL {
        if (s_variables.find($2) != s_variables.end()) {
            std::string message = 
                std::string("Could not declare a new variable: name ") +
                $2
                + " already exists.";
            yyerror(message.c_str());
        }
        s_variables[$2] = std::unique_ptr<ExprValueType>(new ExprValueType{0});
        $$ = new Action{Assignment{
            .var = new Variable { .name = $2, .value = s_variables[$2].get() },
            .expr = $4
        }};
        }
    | variable ASSIGN expr EOL {
        $$ = new Action{Assignment {
            .var = $1,
            .expr = $3
        }};
        }
    | IF BR_ROUND_OP expr BR_ROUND_CL
        BR_CURLY_OP sequence BR_CURLY_CL {
            $$ = new Action{Branch {
                .condition = $3,
                .true_branch = $6,
                .false_branch = new Sequence{}
            }};
        }
    | IF BR_ROUND_OP expr BR_ROUND_CL 
        BR_CURLY_OP sequence BR_CURLY_CL ELSE
        BR_CURLY_OP sequence BR_CURLY_CL {
            $$ = new Action{Branch {
                .condition = $3,
                .true_branch = $6,
                .false_branch = $10
            }};
        }
    | PRINT BR_ROUND_OP expr BR_ROUND_CL EOL {
            $$ = new Action{Print { $3 }};
        }
    ;

variable:
    NAME { 
        if (s_variables.find($1) == s_variables.end()) {
            std::string message = 
                std::string("Variable ") +
                $1
                + " does not exist.";
            yyerror(message.c_str());
        }
        $$ = new Variable{
        .name = $1,
        .value = s_variables[$1].get()
        }; }
    ;

expr:
    NUMBER { $$ = new Expression{Constant{ .value = $1 }}; }
    | variable { $$ = new Expression{*$1}; }
    | BR_ROUND_OP expr BR_ROUND_CL { $$ = $2; }
    | expr PLUS expr { $$ = new Expression{Add{
            .left = $1,
            .right = $3}}; 
        }
    | expr MINUS expr { $$ = new Expression{Subtract{
            .left = $1,
            .right = $3}
            };
        }
    | expr EQUAL expr { $$ = new Expression{Equal{
            .left = $1,
            .right = $3}
            };
        }
    ;

%%

// Error handling function
void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
