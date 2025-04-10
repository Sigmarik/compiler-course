#include "symbol_resolver.h"

#include <iomanip>
#include <iostream>

// TODO: Replace the temporary solution
static void error(unsigned line, unsigned column, std::string msg);

void SymbolResolveVisitor::visit(Sequence& node) {
    for (Action* action : node.actions) {
        visitVariant(*action);
    }
}

void SymbolResolveVisitor::visit(Assignment& node) {
    visitVariant(*node.expr);

    // I treat all variable declarations as auto-s for compatibility with LUA.
    m_table.getOrRegister(m_context, node.var->name, DataType(m_current_type));

    // This is going to be soo much more painful when I add untyped functions...
    node.var->accept(*this);
}

void SymbolResolveVisitor::visit(Branch& node) {
    static uint64_t s_branch_id = 0;
    uint64_t branch_id = ++s_branch_id;

    visitVariant(*node.condition);

    m_context.push_back((std::stringstream{} << "brn_true" << branch_id).str());
    if (node.true_branch) node.true_branch->accept(*this);
    m_context.pop_back();

    m_context.push_back(
        (std::stringstream{} << "brn_false" << branch_id).str());
    if (node.false_branch) node.false_branch->accept(*this);
    m_context.pop_back();
}

void SymbolResolveVisitor::visit(Print& node) { visitVariant(*node.expr); }

void SymbolResolveVisitor::visit(Constant& node) {
    m_current_type = DataType::Int;
}

void SymbolResolveVisitor::visit(Variable& node) {
    node.entry = m_table.findHereOrLower(m_context, node.name);

    if (!node.entry) {
        fail();
        error(
            node.pos.line, node.pos.column,
            (std::stringstream{} << "Unknown symbol " << std::quoted(node.name))
                .str());
    }

    m_current_type = *m_table.get(node.entry);
}

void SymbolResolveVisitor::visit(BinaryOperator& node) {
    visitVariant(*node.left);
    DataType left_type = m_current_type;
    visitVariant(*node.right);
    DataType right_type = m_current_type;

    // Implicit type conversion should happen here

    if (left_type != right_type) {
        fail();
        error(node.pos.line, node.pos.column,
              "Cannot binary combine values of different types.");
    }

    m_current_type = right_type;
}

void error(unsigned line, unsigned column, std::string msg) {
    std::cout << "Error at line " << line << ", column " << column << ": "
              << msg << std::endl;
}
