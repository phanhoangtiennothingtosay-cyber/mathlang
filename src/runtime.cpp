#include "mathlang/runtime.hpp"

#include "mathlang/math.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace mathlang {

RuntimeResult Runtime::evaluate(
    const Expression& expression
) const {
    return evaluate_expression(expression);
}

RuntimeResult Runtime::evaluate_expression(
    const Expression& expression
) const {
    if (const auto* node =
        dynamic_cast<const NumberExpression*>(&expression)) {
        return evaluate_number(*node);
    }

    if (const auto* node =
        dynamic_cast<const UnaryExpression*>(&expression)) {
        return evaluate_unary(*node);
    }

    if (const auto* node =
        dynamic_cast<const BinaryExpression*>(&expression)) {
        return evaluate_binary(*node);
    }

    if (const auto* node =
        dynamic_cast<const FactorialExpression*>(&expression)) {
        return evaluate_factorial(*node);
    }

    if (const auto* node =
        dynamic_cast<const CallExpression*>(&expression)) {
        return evaluate_call(*node);
    }

    return error("Expression cannot be evaluated.");
}

RuntimeResult Runtime::evaluate_number(
    const NumberExpression& expression
) const {
    try {
        return number(std::stod(expression.value));
    } catch (...) {
        return error("Invalid numeric value.");
    }
}

RuntimeResult Runtime::evaluate_unary(
    const UnaryExpression& expression
) const {
    const auto operand =
        evaluate_expression(*expression.operand);

    if (!operand.valid) {
        return operand;
    }

    if (operand.value.type != RuntimeValue::Type::Number) {
        return error(
            "Unary operator requires a number."
        );
    }

    switch (expression.op) {
    case TokenType::Plus:
        return number(operand.value.number);

    case TokenType::Minus:
        return number(-operand.value.number);

    default:
        return error("Unsupported unary operator.");
    }
}

RuntimeResult Runtime::evaluate_binary(
    const BinaryExpression& expression
) const {
    const auto left =
        evaluate_expression(*expression.left);

    if (!left.valid) {
        return left;
    }

    const auto right =
        evaluate_expression(*expression.right);

    if (!right.valid) {
        return right;
    }

    if (
        left.value.type != RuntimeValue::Type::Number ||
        right.value.type != RuntimeValue::Type::Number
    ) {
        return error(
            "Binary arithmetic requires numeric operands."
        );
    }

    const double a = left.value.number;
    const double b = right.value.number;

    math::Result result;

    switch (expression.op) {
    case TokenType::Plus:
        result = math::add(a, b);
        break;

    case TokenType::Minus:
        result = math::subtract(a, b);
        break;

    case TokenType::Multiply:
        result = math::multiply(a, b);
        break;

    case TokenType::Divide:
        result = math::divide(a, b);
        break;

    case TokenType::Modulo:
        result = math::modulo(a, b);
        break;

    case TokenType::Equal:
        return boolean(a == b);

    case TokenType::NotEqual:
        return boolean(a != b);

    case TokenType::Less:
        return boolean(a < b);

    case TokenType::Greater:
        return boolean(a > b);

    case TokenType::LessEqual:
        return boolean(a <= b);

    case TokenType::GreaterEqual:
        return boolean(a >= b);

    default:
        return error("Unsupported binary operator.");
    }

    if (!result.valid) {
        return error(result.error);
    }

    return number(result.value);
}

RuntimeResult Runtime::evaluate_factorial(
    const FactorialExpression& expression
) const {
    const auto operand =
        evaluate_expression(*expression.operand);

    if (!operand.valid) {
        return operand;
    }

    if (operand.value.type != RuntimeValue::Type::Number) {
        return error(
            "Factorial requires a number."
        );
    }

    const auto result =
        math::factorial(operand.value.number);

    if (!result.valid) {
        return error(result.error);
    }

    return number(result.value);
}

RuntimeResult Runtime::evaluate_call(
    const CallExpression& expression
) const {
    return error(
        "Function '" +
        expression.function +
        "' is not implemented yet."
    );
}

RuntimeResult Runtime::error(
    std::string message
) const {
    RuntimeResult result;
    result.valid = false;
    result.value.type = RuntimeValue::Type::Undefined;
    result.error = std::move(message);

    return result;
}

RuntimeResult Runtime::number(
    double value
) const {
    RuntimeResult result;
    result.valid = true;
    result.value.type = RuntimeValue::Type::Number;
    result.value.number = value;

    return result;
}

RuntimeResult Runtime::boolean(
    bool value
) const {
    RuntimeResult result;
    result.valid = true;
    result.value.type = RuntimeValue::Type::Boolean;
    result.value.boolean = value;

    return result;
}

} // namespace mathlang
