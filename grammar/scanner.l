%{
#include <stdio.h>
#include <string.h>

#include <string>

#include "ast/ast.hpp"
#include "ast/global.h"

#define YY_DECL int yylex()

#include "parser.y.hpp"

#ifdef FLEX_DEBUG_TOKENS
#define RETURN(something) do { printf("Token " #something "\n"); return something; } while (0)
#else
#define RETURN(something) return something
#endif

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    for(int i = 0; yytext[i] != '\0'; i++) { \
        if(yytext[i] == '\n') { \
            yylloc.last_line++; \
            yylloc.last_column = 0; \
        } \
        else { \
            yylloc.last_column++; \
        } \
    }
%}

%%
[0-9]+      { yylval.integer = atoi(yytext); RETURN(NUMBER); }

[; \t\n]     ;
--[^\n]*\n   ;

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
"do"                    { RETURN(DO); }
"then"                  { RETURN(THEN); }
"end"                   { RETURN(END); }
"print"                 { RETURN(PRINT); }
"function"              { RETURN(FUNCTION); }
"main"                  { RETURN(MAIN); }
"local"                 { RETURN(LOCAL); }

[a-zA-Z_][a-zA-Z_0-9]* { yylval.string = strdup(yytext); RETURN(NAME); }

.           { return yytext[0]; }

%%

int yywrap() {
    return 1;
}

void set_input_file(const char* filename) {
	yyin = fopen(filename, "r");
    s_source_file = std::string(filename);
    /* printf("Opening file %s\n", sFileName.c_str()); */
}
