#include "ast.hpp"

const std::map<BinaryOperator::Type, std::string> BinaryOperator::kTypeNames = {
    {BinaryOperator::Type::Add, "Add"},
    {BinaryOperator::Type::Equal, "Equal"},
    {BinaryOperator::Type::Subtract, "Subtract"},
    {BinaryOperator::Type::And, "And"},
    {BinaryOperator::Type::Or, "Or"},
    {BinaryOperator::Type::None, "None"},
};

Sequence::~Sequence() {
    for (Action* action : actions) delete action;
}
