#pragma once

#include "ast/ast.hpp"
#include "ast/symbol_table.h"

class SymbolResolveVisitor : public ASTVisitor {
   public:
    virtual void visit(Sequence& node) override;
    virtual void visit(Assignment& node) override;
    virtual void visit(Branch& node) override;
    virtual void visit(Print& node) override;
    virtual void visit(Constant& node) override;
    virtual void visit(Variable& node) override;
    virtual void visit(BinaryOperator& node) override;

    using TableT = SymbolTable<DataType>;
    TableT getSymbols() const { return m_table; }

    bool successful() const { return !m_failed; }

   private:
    void fail() { m_failed = true; }

   protected:
    TableT m_table{};

   private:
    bool m_failed = false;
    STContext m_context{};
    DataType m_current_type = DataType::Int;
};
