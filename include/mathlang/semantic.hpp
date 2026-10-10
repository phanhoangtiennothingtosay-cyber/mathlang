#pragma once

#include "mathlang/ast.hpp"

#include <string>

namespace mathlang {

enum class ValueType {
    Unknown,
    Number,
    Boolean
};

struct SemanticResult {
    ValueType type = ValueType::Unknown;
    bool valid = true;
    std::string error;
};

class SemanticAnalyzer {
public:
    SemanticResult analyze(const Expression& expression) const;

private:
    SemanticResult analyze_expression(
        const Expression& expression
    ) const;

    SemanticResult analyze_number(
        const NumberExpression& expression
    ) const;

    SemanticResult analyze_identifier(
        const IdentifierExpression& expression
    ) const;

    SemanticResult analyze_unary(
        const UnaryExpression& expression
    ) const;

    SemanticResult analyze_binary(
        const BinaryExpression& expression
    ) const;

    SemanticResult analyze_call(
        const CallExpression& expression
    ) const;

    SemanticResult analyze_factorial(
        const FactorialExpression& expression
    ) const;

    SemanticResult error(std::string message) const;

    SemanticResult valid(ValueType type) const;
};

} // namespace mathlang