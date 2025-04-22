#pragma once

#include <deque>

#include "ast/ast.hpp"
#include "ast/symbol_table.h"

//* Symbol resolve visitor is also a type resolver, since functions can
//* "duplicate" depending on their argument's type.

class SymbolResolveVisitor : public ASTVisitor {
   public:
    virtual void visit(Sequence& node) override;
    virtual void visit(Assignment& node) override;
    virtual void visit(Branch& node) override;
    virtual void visit(Print& node) override;
    virtual void visit(Constant& node) override;
    virtual void visit(Variable& node) override;
    virtual void visit(BinaryOperator& node) override;

    bool successful() const { return !m_failed; }

    const SymbolTable<DataType>& getTypes() const { return m_types; }

   private:
    void fail() { m_failed = true; }

   private:
    SymbolId findOrCreate(const std::string& name);

    void enterScope(const std::string& scopeName);
    void exitScope();

    std::string getFullScopeName() const;

    bool m_failed = false;

    SymbolId m_highest_id = 0;
    DataType m_current_type = DataType::Int;
    std::deque<std::unordered_map<std::string, SymbolId>> m_symbol_table{{}};
    SymbolTable<DataType> m_types{};
    std::deque<std::string> m_scope_layers{};
};
