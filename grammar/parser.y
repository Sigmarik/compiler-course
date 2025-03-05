%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *s);
int yylex();
%}

%union {
  int value;
  void *expression;
}

%token<value> NUMBER
%token PLUS MINUS
%token EOL

%type <value> expr
%type <value> term

%%

// Grammar rules
input:
    | input line
    ;

line:
    expr EOL { printf("Result: %d\n", $1); }
    ;

expr:
    expr PLUS term { $$ = $1 + $3; }
    | expr MINUS term { $$ = $1 - $3; }
    | term { $$ = $1; }
    ;

term:
    NUMBER { $$ = $1; }
    ;

%%

// Error handling function
void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
