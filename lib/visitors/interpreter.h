#pragma once

#include <map>

#include "ast/ast.hpp"

class Interpreter : public ASTVisitor {
   public:
    virtual void visit(Sequence& node) override;
    virtual void visit(Assignment& node) override;
    virtual void visit(Branch& node) override;
    virtual void visit(Print& node) override;
    virtual void visit(Constant& node) override;
    virtual void visit(Variable& node) override;
    virtual void visit(BinaryOperator& node) override;

   private:
    ExprValueType m_return{};
    // TODO: Implement a local memory buffer (or use a one provided by LLVM)
};
