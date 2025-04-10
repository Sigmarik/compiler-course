%require "3.5"

%code requires {
    #include <stdio.h>
    #include <stdlib.h>

    #include <string>

    #include "ast/ast.hpp"
    #include "ast/symbol_table.h"

    static SymbolTable s_variables;

    void yyerror(const char *s);
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

%token<integer> NUMBER
%left PLUS MINUS EQUAL ASSIGN

%token<string> NAME;
%token IF ELSE
%token BR_ROUND_OP BR_ROUND_CL BR_CURLY_OP BR_CURLY_CL
%token PRINT

%token DO THEN END

%token LOCAL FUNCTION MAIN

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
        if (s_variables.find($2) != s_variables.end()) {
            std::string message = 
                std::string("Could not declare a new variable: name ") +
                $2
                + " already exists.";
            yyerror(message.c_str());
        }
        s_variables[$2] = std::make_unique<ExprValueType>(0);
        $$ = new Action{Assignment{
            new Variable($2, s_variables[$2].get()),
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
        if (s_variables.find($1) == s_variables.end()) {
            std::string message = 
                std::string("Variable \"") +
                $1
                + "\" does not exist.";
            yyerror(message.c_str());

            for (auto& [key, value] : s_variables) {
                printf("Variable - \"%s\"\n", key.c_str());
            }
        }
        $$ = new Variable{
            $1,
            s_variables[$1].get()
        }; }
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
void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
