#pragma once

#include "ast/ast.hpp"

class Printer : public ASTVisitor {
   public:
    virtual void visit(Sequence& node) override;
    virtual void visit(Assignment& node) override;
    virtual void visit(Branch& node) override;
    virtual void visit(Print& node) override;
    virtual void visit(Constant& node) override;
    virtual void visit(Variable& node) override;
    virtual void visit(BinaryOperator& node) override;

   private:
    unsigned m_indentation = 0;
};
