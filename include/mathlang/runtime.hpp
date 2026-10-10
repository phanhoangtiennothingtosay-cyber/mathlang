#pragma once

#include "mathlang/ast.hpp"
#include "mathlang/math.hpp"

namespace mathlang {

struct RuntimeResult {
    bool valid = false;
    RuntimeValue value;
    std::string error;
};

class Runtime {
public:
    RuntimeResult evaluate(const Expression& expression) const;

private:
    RuntimeResult evaluate_expression(const Expression& expression) const;
    RuntimeResult evaluate_number(const NumberExpression& expression) const;
    RuntimeResult evaluate_unary(const UnaryExpression& expression) const;
    RuntimeResult evaluate_binary(const BinaryExpression& expression) const;
    RuntimeResult evaluate_factorial(const FactorialExpression& expression) const;
    RuntimeResult evaluate_call(const CallExpression& expression) const;
};

} // namespace mathlang