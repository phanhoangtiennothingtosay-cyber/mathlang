
#pragma once

#include "mathlang/ast.hpp"
#include "mathlang/variables.hpp"

#include <string>
#include <vector>

namespace mathlang {

struct RuntimeValue {
    enum class Type { Number, Boolean, Undefined };

    Type type = Type::Undefined;
    double number = 0.0;
    bool boolean = false;
};

struct RuntimeResult {
    bool valid = false;
    RuntimeValue value;
    std::string error;
};

struct ExecutionResult {
    bool valid = false;
    std::vector<RuntimeValue> outputs;
    std::string error;
};

class Runtime {
public:
    RuntimeResult evaluate(const Expression& expression);
    ExecutionResult execute(const Program& program);
    const Variables& variables() const noexcept;

private:
    RuntimeResult evaluate_expression(const Expression& expression);
    RuntimeResult evaluate_number(const NumberExpression& expression);
    RuntimeResult evaluate_identifier(const IdentifierExpression& expression);
    RuntimeResult evaluate_unary(const UnaryExpression& expression);
    RuntimeResult evaluate_binary(const BinaryExpression& expression);
    RuntimeResult evaluate_factorial(const FactorialExpression& expression);
    RuntimeResult evaluate_call(const CallExpression& expression);

    RuntimeResult error(std::string message) const;
    RuntimeResult number(double value) const;
    RuntimeResult boolean(bool value) const;

    Variables variables_;
};

} // namespace mathlang
