#include <iostream>

#include "parser.y.hpp"

int main() {
    printf("Enter expressions (Ctrl+D to exit):\n");
    return yyparse();
}
