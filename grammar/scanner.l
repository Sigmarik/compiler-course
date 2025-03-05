%{
#include "parser.y.hpp"
%}

%%
[0-9]+      { yylval = atoi(yytext); return NUMBER; }
[ \t]       ;
\n          { return EOL; }
"+"         { return PLUS; }
"-"         { return MINUS; }
.           { return yytext[0]; }

%%

// Error handling
int yywrap() {
    return 1; // Indicate end of input
}