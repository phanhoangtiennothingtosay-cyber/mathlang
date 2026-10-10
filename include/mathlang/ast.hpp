#pragma once

#include "mathlang/math.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace mathlang {

struct Expression {
    virtual ~Expression() = default;
};

struct NumberExpression final : Expression {
    math::Number value;
    explicit NumberExpression(math::Number value) : value(std::move(value)) {}
};

struct IdentifierExpression final : Expression {
    std::string name;
    explicit IdentifierExpression(std::string name) : name(std::move(name)) {}
};

struct UnaryExpression final : Expression {
    std::string op;
    std::unique_ptr<Expression> operand;
    UnaryExpression(std::string op, std::unique_ptr<Expression> operand)
        : op(std::move(op)), operand(std::move(operand)) {}
};

struct BinaryExpression final : Expression {
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinaryExpression(std::string op, std::unique_ptr<Expression> left,
                     std::unique_ptr<Expression> right)
        : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
};

struct CallExpression final : Expression {
    std::string callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    CallExpression(std::string callee, std::vector<std::unique_ptr<Expression>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {}
};

struct FactorialExpression final : Expression {
    std::unique_ptr<Expression> operand;
    explicit FactorialExpression(std::unique_ptr<Expression> operand)
        : operand(std::move(operand)) {}
};

struct Statement { virtual ~Statement() = default; };

struct AssignmentStatement final : Statement {
    std::string target;
    std::unique_ptr<Expression> value;
    AssignmentStatement(std::string target, std::unique_ptr<Expression> value)
        : target(std::move(target)), value(std::move(value)) {}
};

struct PrintStatement final : Statement {
    std::unique_ptr<Expression> value;
    explicit PrintStatement(std::unique_ptr<Expression> value) : value(std::move(value)) {}
};

struct ExpressionStatement final : Statement {
    std::unique_ptr<Expression> expression;
    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
};

struct Program { std::vector<std::unique_ptr<Statement>> statements; };

std::unique_ptr<Expression> clone_expression(const Expression& expression);

} // namespace mathlang
