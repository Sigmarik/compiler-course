#include "interpreter.h"

#include <iostream>

void Interpreter::visit(Sequence& node) {
    for (Action* action : node.actions) {
        visitVariant(*action);
    }
}

void Interpreter::visit(Assignment& node) {
    visitVariant(*node.expr);
    m_memory[node.var->entry] = m_return;
}

void Interpreter::visit(Branch& node) {
    visitVariant(*node.condition);
    if (node.true_branch && m_return) node.true_branch->accept(*this);
    if (node.false_branch && !m_return) node.false_branch->accept(*this);
}

void Interpreter::visit(Print& node) {
    visitVariant(*node.expr);
    std::cout << m_return << std::endl;
}

void Interpreter::visit(Constant& node) {
    std::visit([&](const auto& value) { m_return = value; }, node.value);
}

void Interpreter::visit(Variable& node) { m_return = m_memory[node.entry]; }

void Interpreter::visit(BinaryOperator& node) {
    visitVariant(*node.left);
    auto val_left = m_return;
    visitVariant(*node.right);
    auto val_right = m_return;

    switch (node.type) {
        case BinaryOperator::Type::Add: {
            m_return = val_left + val_right;
        } break;
        case BinaryOperator::Type::Subtract: {
            m_return = val_left - val_right;
        } break;
        case BinaryOperator::Type::Equal: {
            m_return = val_left == val_right;
        } break;
    }
}
