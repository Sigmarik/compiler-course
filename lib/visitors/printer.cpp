#include "printer.h"

#include <assert.h>

#include <iostream>

static void indent(unsigned count) {
    for (unsigned id = 0; id < count; ++id) {
        std::cout << '|';
    }
}

void Printer::visit(Module& node) {
    indent(m_indentation);
    std::cout << "Module functions:\n";
    ++m_indentation;
    for (Function* func : node.functions) {
        func->accept(*this);
    }
    --m_indentation;
    indent(m_indentation);
    std::cout << "Module templates:\n";
    ++m_indentation;
    for (FunctionTemplate* templ : node.templates) {
        visit(*templ);
    }
}

void Printer::visit(Function& node) {
    indent(m_indentation);
    std::cout << "Function " << node.name << ":\n";
    indent(m_indentation);
    std::cout << "Params:\n";
    ++m_indentation;
    for (Variable* var : node.parameters) {
        var->accept(*this);
    }
    --m_indentation;

    node.body->accept(*this);
}

void Printer::visit(FunctionTemplate& node) {
    indent(m_indentation);
    std::cout << "Function Template " << node.name << ":\n";
    indent(m_indentation);
    std::cout << "Params:\n";
    ++m_indentation;
    for (const std::string& par : node.paramNames) {
        indent(m_indentation);
        std::cout << par << "\n";
    }
    --m_indentation;

    node.body->accept(*this);
}

void Printer::visit(Sequence& node) {
    indent(m_indentation);
    std::cout << "Sequence:\n";
    ++m_indentation;
    for (Action* action : node.actions) {
        assert(action);
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
    assert(node.expr);
    visitVariant(*node.expr);
    --m_indentation;
}

void Printer::visit(Branch& node) {
    indent(m_indentation);
    std::cout << "If\n";
    ++m_indentation;
    assert(node.condition);
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
    assert(node.expr);
    visitVariant(*node.expr);
    --m_indentation;
}

void Printer::visit(Return& node) {
    indent(m_indentation);
    std::cout << "Return\n";
    visitVariant(*node.expression);
}

void Printer::visit(FunctionCall& node) {
    indent(m_indentation);
    std::cout << "Call " << node.name << "\n";
    indent(m_indentation);
    std::cout << "Arguments:\n";
    ++m_indentation;
    for (Expression* arg : node.arguments) {
        visitVariant(*arg);
    }
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
