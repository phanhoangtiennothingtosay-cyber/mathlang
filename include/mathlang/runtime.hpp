#pragma once

#include "mathlang/ast.hpp"
#include "mathlang/math.hpp"

#include <string>

namespace mathlang {

struct RuntimeValue {
    enum class Type {
        Number,
        Boolean,
        Undefined
    };

    Type type = Type::Undefined;
    double number = 0.0;
    bool boolean = false;
};

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

    RuntimeResult error(std::string message) const;
    RuntimeResult number(double value) const;
    RuntimeResult boolean(bool value) const;
};

} // namespace mathlang
