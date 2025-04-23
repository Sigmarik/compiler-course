#include <iostream>

#include "ast/ast.hpp"
#include "ast/global.h"
#include "parser.y.hpp"
#include "visitors/interpreter.h"
#include "visitors/ir_builder.h"
#include "visitors/printer.h"
#include "visitors/symbol_resolver.h"

int main(int argc, char** argv) {
    // TODO: Replace with actual argument parsing
    if (argc > 1) {
        set_input_file(argv[1]);
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    int status = yyparse();
    if (status) {
        std::cout << "Failed to parse the file." << std::endl;
    }

    if (root_sequence == nullptr) {
        std::cout << "`root_sequence` is NULL" << std::endl;
    } else {
        SymbolResolveVisitor symbolResolver;
        root_sequence->accept(symbolResolver);

        // {
        //     Printer printer;
        //     root_sequence->accept(printer);
        // }

        {
            Printer printer;
            root_sequence->accept(printer);
        }

        // {
        //     LLVMIRBuilder visitor(symbolResolver.getTypes());
        //     root_sequence->accept(visitor);
        //     visitor.getModule().dump();
        // }

        // {
        //     Interpreter visitor;
        //     root_sequence->accept(visitor);
        // }
        // {
        //     Interpreter visitor;
        //     root_sequence->accept(visitor);
        // }
    }
}
