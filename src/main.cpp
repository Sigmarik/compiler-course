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
        return EXIT_FAILURE;
    } else {
        SymbolResolveVisitor symbolResolver;
        root_sequence->accept(symbolResolver);

        if (!symbolResolver.successful()) {
            return EXIT_FAILURE;
        }

        {
            Printer printer;
            root_sequence->accept(printer);
        }

        // {
        //     LLVMIRBuilder visitor(symbolResolver.getTypes());
        //     root_sequence->accept(visitor);
        //     visitor.finish();
        //     visitor.getModule().print(llvm::outs(), nullptr);
        // }

        // {
        //     Interpreter visitor;
        //     root_sequence->accept(visitor);
        // }
    }
}
