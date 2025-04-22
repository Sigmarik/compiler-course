#include "symbol_resolver.h"

#include <assert.h>

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
    SymbolId id = findOrCreate(node.var->name);

    DataType type = DataType::Int;
    if (m_types.find(id) != m_types.end()) {
        type = m_types[id];
    } else {
        type = m_current_type;
        m_types[id] = type;
    }

    if (type != m_current_type) {
        fail();
        error(node.var->pos.line, node.var->pos.column,
              "Cannot assign a value of different type to a variable");
    }

    // This is going to be soo much more painful when I add type deduction for
    // functions...
    node.var->accept(*this);
}

void SymbolResolveVisitor::visit(Branch& node) {
    static uint64_t s_branch_id = 0;
    uint64_t branch_id = ++s_branch_id;

    visitVariant(*node.condition);

    enterScope((std::stringstream{} << "brn_true" << branch_id).str());
    if (node.true_branch) node.true_branch->accept(*this);
    exitScope();

    enterScope((std::stringstream{} << "brn_false" << branch_id).str());
    if (node.false_branch) node.false_branch->accept(*this);
    exitScope();
}

void SymbolResolveVisitor::visit(Print& node) { visitVariant(*node.expr); }

void SymbolResolveVisitor::visit(Constant& node) {
    m_current_type = DataType::Int;
}

void SymbolResolveVisitor::visit(Variable& node) {
    node.entry = findOrCreate(node.name);
    if (m_types.find(node.entry) == m_types.end()) {
        fail();
        error(
            node.pos.line, node.pos.column,
            (std::stringstream{} << "Unknown symbol " << std::quoted(node.name))
                .str());
        return;
    }

    m_current_type = m_types[node.entry];
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

SymbolId SymbolResolveVisitor::findOrCreate(const std::string& name) {
    for (auto it = m_symbol_table.rbegin(); it != m_symbol_table.rend(); ++it) {
        if (it->count(name)) {
            return (*it)[name];
        }
    }

    SymbolId newId = ++m_highest_id;
    m_symbol_table.back().emplace(name, newId);
    return newId;
}

void SymbolResolveVisitor::enterScope(const std::string& scopeName) {
    m_scope_layers.push_back(scopeName);
    m_symbol_table.emplace_back();
}

void SymbolResolveVisitor::exitScope() {
    assert(!m_symbol_table.empty());
    m_symbol_table.pop_back();
    m_scope_layers.pop_back();
}

std::string SymbolResolveVisitor::getFullScopeName() const {
    std::string fullScope;
    for (const auto& layer : m_scope_layers) {
        if (!fullScope.empty()) {
            fullScope += "::";
        }
        fullScope += layer;
    }
    return fullScope;
}

void error(unsigned line, unsigned column, std::string msg) {
    std::cout << "Error at line " << line << ", column " << column << ": "
              << msg << std::endl;
}
