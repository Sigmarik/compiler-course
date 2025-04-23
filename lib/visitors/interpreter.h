#pragma once

#include <map>

#include "ast/ast.hpp"
#include "symbol_resolver.h"

class Interpreter : public ASTVisitor {
   public:
    virtual void visit(Module& node) override {}
    virtual void visit(Function& node) override {}
    virtual void visit(Sequence& node) override;

    virtual void visit(Assignment& node) override;
    virtual void visit(Branch& node) override;
    virtual void visit(Print& node) override;
    virtual void visit(Return& node) override {}

    virtual void visit(FunctionCall& node) override {}
    virtual void visit(Constant& node) override;
    virtual void visit(Variable& node) override;
    virtual void visit(BinaryOperator& node) override;

   private:
    int m_return{};
    SymbolTable<int> m_memory{};
    // TODO: Implement a local memory buffer (or use one provided by LLVM)
};
