%{
#include <stdio.h>
#include <string.h>

#include <string>

#include "ast/ast.hpp"

#define YY_DECL int yylex()

#include "parser.y.hpp"

#ifdef FLEX_DEBUG_TOKENS
#define RETURN(something) do { printf("Token " #something "\n"); return something; } while (0)
#else
#define RETURN(something) return something
#endif
%}

%%
[0-9]+      { yylval.integer = atoi(yytext); RETURN(NUMBER); }

[ \t\n]     ;

";"                     { RETURN(EOL); }
"+"                     { RETURN(PLUS); }
"-"                     { RETURN(MINUS); }
"=="                    { RETURN(EQUAL); }
"="                     { RETURN(ASSIGN); }
"if"                    { RETURN(IF); }
"else"                  { RETURN(ELSE); }
"("                     { RETURN(BR_ROUND_OP); }
")"                     { RETURN(BR_ROUND_CL); }
"{"                     { RETURN(BR_CURLY_OP); }
"}"                     { RETURN(BR_CURLY_CL); }
"print"                 { RETURN(PRINT); }
"local"                 { RETURN(DECL); }
":"                     { RETURN(SEP); }
"local function main"   { RETURN(MAIN); }

[a-zA-Z_][a-zA-Z_0-9]* { yylval.string = strdup(yytext); RETURN(NAME); }

.           { return yytext[0]; }

%%

int yywrap() {
    return 1;
}

void set_input_file(char* filename) {
	yyin = fopen(filename, "r");
}
