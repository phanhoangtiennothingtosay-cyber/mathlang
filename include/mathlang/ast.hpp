#pragma once

#include "mathlang/lexer.hpp"

#include <memory>
#include <string>
#include <vector>

namespace mathlang {

struct Expression {
    virtual ~Expression() = default;
};

using ExpressionPtr = std::unique_ptr<Expression>;

struct NumberExpression final : Expression {
    std::string value;
};

struct IdentifierExpression final : Expression {
    std::string name;
};

struct UnaryExpression final : Expression {
    TokenType op;
    ExpressionPtr operand;
};

struct BinaryExpression final : Expression {
    TokenType op;
    ExpressionPtr left;
    ExpressionPtr right;
};

struct CallExpression final : Expression {
    std::string function;
    std::vector<ExpressionPtr> arguments;
};

struct FactorialExpression final : Expression {
    ExpressionPtr operand;
};

} // namespace mathlang
