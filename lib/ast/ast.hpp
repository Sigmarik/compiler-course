#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct Sequence;

struct Assignment;
struct Branch;
struct Print;

using Action = std::variant<Assignment, Branch, Print>;

struct Constant;
struct Variable;
struct BinaryOperator;

using Expression = std::variant<BinaryOperator, Constant, Variable>;

class ASTVisitor {
   public:
    virtual void visit(Sequence& node) = 0;

    virtual void visit(Assignment& node) = 0;
    virtual void visit(Branch& node) = 0;
    virtual void visit(Print& node) = 0;

    virtual void visit(Constant& node) = 0;
    virtual void visit(Variable& node) = 0;
    virtual void visit(BinaryOperator& node) = 0;

    template <class T>
    void visitVariant(T& variant) {
        std::visit([this](auto& node) { this->visit(node); }, variant);
    }
};

class Visitable {
    virtual void accept(ASTVisitor& visitor) = 0;
};

#define DEFAULT_VISIT \
    virtual void accept(ASTVisitor& visitor) override { visitor.visit(*this); }

struct Sequence : public Visitable {
    Sequence() = default;
    ~Sequence();

    std::vector<Action*> actions{};

    DEFAULT_VISIT
};

using ExprValueType = int;

struct Variable : public Visitable {
    Variable() = default;
    Variable(std::string name, ExprValueType* val_ptr)
        : name(name), value(val_ptr) {}

    std::string name{};
    ExprValueType* value{};

    DEFAULT_VISIT
};

struct BinaryOperator : public Visitable {
    enum class Type { None, Add, Subtract, Equal };
    static const std::map<Type, std::string> kTypeNames;

    BinaryOperator();
    BinaryOperator(Type type, Expression* left, Expression* right)
        : type(type), left(left), right(right) {}

    Type type = Type::None;

    Expression* left{};
    Expression* right{};

    DEFAULT_VISIT
};

struct Constant : public Visitable {
    Constant() = default;
    Constant(ExprValueType value) : value(value) {}

    ExprValueType value{};

    DEFAULT_VISIT
};

struct Assignment : public Visitable {
    Assignment() = default;
    Assignment(Variable* var, Expression* expr) : var(var), expr(expr) {}

    Variable* var{};
    Expression* expr{};

    DEFAULT_VISIT
};

struct Branch : public Visitable {
    Branch() = default;
    Branch(Expression* condition, Sequence* true_br, Sequence* false_br)
        : condition(condition), true_branch(true_br), false_branch(false_br) {}

    Expression* condition{};

    Sequence* true_branch{};
    Sequence* false_branch{};

    DEFAULT_VISIT
};

struct Print : public Visitable {
    Print() = default;
    Print(Expression* expr) : expr(expr) {}

    Expression* expr{};

    DEFAULT_VISIT
};
