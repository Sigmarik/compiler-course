#include "ast.hpp"

const std::map<BinaryOperator::Type, std::string> BinaryOperator::kTypeNames = {
    {BinaryOperator::Type::Add, "Add"},
    {BinaryOperator::Type::Equal, "Equal"},
    {BinaryOperator::Type::Subtract, "Subtract"},
    {BinaryOperator::Type::None, "None"},
};

Function FunctionTemplate::implement() const {
    Function func;

    func.name = name;
    func.body = new Sequence(body->copy());
    func.pos = pos;

    for (const std::string& param : paramNames) {
        Variable* param_var = new Variable();
        param_var->pos = pos;
        param_var->name = param;
        func.parameters.push_back(param_var);
    }

    return func;
}

Sequence::~Sequence() {
    for (Action* action : actions) delete action;
}

Sequence Sequence::copy() const {
    Sequence node;

    node.pos = pos;

    for (Action* action : actions) {
        node.actions.push_back(new Action(copyVariant(*action)));
    }

    return node;
}

Return Return::copy() const {
    Return node;

    node.pos = pos;
    node.expression = new Expression(copyVariant(*expression));

    return node;
}

FunctionCall FunctionCall::copy() const {
    FunctionCall call;

    call.pos = pos;
    call.name = name;

    for (Expression* expr : arguments) {
        call.arguments.push_back(new Expression(copyVariant(*expr)));
    }

    return call;
}

Variable Variable::copy() const {
    Variable var;

    var.pos = pos;
    var.name = name;

    return var;
}

BinaryOperator BinaryOperator::copy() const {
    BinaryOperator op(type, new Expression(copyVariant(*left)),
                      new Expression(copyVariant(*right)));

    op.pos = pos;

    return op;
}

Constant Constant::copy() const {
    Constant cnst;

    cnst.pos = pos;
    cnst.value = value;

    return cnst;
}

Assignment Assignment::copy() const {
    Assignment ass;  // Yes, this is intentional. Go on. Call me unprofessional.

    ass.pos = pos;
    ass.var = new Variable(var->copy());
    ass.expr = new Expression(copyVariant(*expr));

    return ass;
}

Branch Branch::copy() const {
    Branch branch;

    branch.pos = pos;
    branch.condition = new Expression(copyVariant(*condition));
    if (true_branch) branch.true_branch = new Sequence(true_branch->copy());
    if (false_branch) branch.false_branch = new Sequence(false_branch->copy());

    return branch;
}

Print Print::copy() const {
    Print print;

    print.pos = pos;
    print.expr = new Expression(copyVariant(*expr));

    return print;
}
