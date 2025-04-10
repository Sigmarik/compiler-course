#include "printer.h"

#include <iostream>

static void indent(unsigned count) {
    for (unsigned id = 0; id < count; ++id) {
        std::cout << '|';
    }
}

void Printer::visit(Sequence& node) {
    indent(m_indentation);
    std::cout << "Sequence:\n";
    ++m_indentation;
    for (Action* action : node.actions) {
        visitVariant(*action);
    }
    --m_indentation;
}

void Printer::visit(Assignment& node) {
    indent(m_indentation);
    std::cout << "Assign\n";
    ++m_indentation;
    node.var->accept(*this);
    indent(m_indentation - 1);
    std::cout << "to\n";
    visitVariant(*node.expr);
    --m_indentation;
}

void Printer::visit(Branch& node) {
    indent(m_indentation);
    std::cout << "If\n";
    ++m_indentation;
    visitVariant(*node.condition);
    indent(m_indentation - 1);
    std::cout << "Then\n";
    if (node.true_branch) node.true_branch->accept(*this);

    indent(m_indentation - 1);
    std::cout << "Else\n";
    if (node.false_branch) node.false_branch->accept(*this);
    --m_indentation;
}

void Printer::visit(Print& node) {
    indent(m_indentation);
    std::cout << "Print\n";
    ++m_indentation;
    visitVariant(*node.expr);
    --m_indentation;
}

void Printer::visit(Constant& node) {
    indent(m_indentation);
    std::cout << "Const " << node.value << '\n';
}

void Printer::visit(Variable& node) {
    indent(m_indentation);
    std::cout << "Variable " << node.name << '\n';
}

void Printer::visit(BinaryOperator& node) {
    indent(m_indentation);
    std::cout << BinaryOperator::kTypeNames.at(node.type) << '\n';
    ++m_indentation;
    visitVariant(*node.left);
    indent(m_indentation - 1);
    std::cout << "and\n";
    visitVariant(*node.right);
    --m_indentation;
}
