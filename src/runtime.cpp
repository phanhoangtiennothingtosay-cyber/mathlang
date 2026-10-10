#include "mathlang/runtime.hpp"

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <cctype>
#include <vector>
#include <string>
#include <utility>

namespace mathlang {
namespace {
std::string lower(std::string value) {
    for (char& c : value) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}
} // namespace

RuntimeResult Runtime::evaluate(const Expression& expression) const {
    return evaluate_expression(expression);
}

RuntimeResult Runtime::evaluate_expression(const Expression& expression) const {
    if (const auto* node = dynamic_cast<const NumberExpression*>(&expression)) return evaluate_number(*node);
    if (const auto* node = dynamic_cast<const IdentifierExpression*>(&expression)) return evaluate_identifier(*node);
    if (const auto* node = dynamic_cast<const UnaryExpression*>(&expression)) return evaluate_unary(*node);
    if (const auto* node = dynamic_cast<const BinaryExpression*>(&expression)) return evaluate_binary(*node);
    if (const auto* node = dynamic_cast<const FactorialExpression*>(&expression)) return evaluate_factorial(*node);
    if (const auto* node = dynamic_cast<const CallExpression*>(&expression)) return evaluate_call(*node);
    return error("Expression cannot be evaluated.");
}

RuntimeResult Runtime::evaluate_number(const NumberExpression& expression) const {
    if (!boost::multiprecision::isfinite(expression.value)) return error("Numeric literal must be finite.");
    return number(expression.value);
}

RuntimeResult Runtime::evaluate_identifier(const IdentifierExpression& expression) const {
    const std::string name = lower(expression.name);
    using math::Number;
    if (name == "pi" || expression.name == "π") return number(Number(math::PI_DECIMAL));
    if (name == "tau" || name == "τ") return number(Number(math::PI_DECIMAL) * 2);
    if (name == "e") return number(Number("2.718281828459045235360287471352662497757247093699959574966967"));
    if (name == "phi" || expression.name == "φ" || name == "golden_ratio")
        return number((Number(1) + boost::multiprecision::sqrt(Number(5))) / 2);
    return error("Unknown constant or variable: '" + expression.name + "'.");
}

RuntimeResult Runtime::evaluate_unary(const UnaryExpression& expression) const {
    const auto operand = evaluate_expression(*expression.operand);
    if (!operand.valid) return operand;
    if (operand.value.type != RuntimeValue::Type::Number) return error("Unary operator requires a number.");
    if (expression.op == "+") return number(operand.value.number);
    if (expression.op == "-") return number(-operand.value.number);
    return error("Unsupported unary operator: " + expression.op);
}

RuntimeResult Runtime::evaluate_binary(const BinaryExpression& expression) const {
    const auto left = evaluate_expression(*expression.left);
    if (!left.valid) return left;
    const auto right = evaluate_expression(*expression.right);
    if (!right.valid) return right;
    if (left.value.type != RuntimeValue::Type::Number || right.value.type != RuntimeValue::Type::Number)
        return error("Binary operators require numeric operands.");

    const math::Number& a = left.value.number;
    const math::Number& b = right.value.number;
    const std::string& op = expression.op;
    if (op == "=" || op == "==") return boolean(a == b);
    if (op == "!=" || op == "≠") return boolean(a != b);
    if (op == "<") return boolean(a < b);
    if (op == ">") return boolean(a > b);
    if (op == "<=" || op == "≤") return boolean(a <= b);
    if (op == ">=" || op == "≥") return boolean(a >= b);

    math::Result result;
    if (op == "+") result = math::add(a, b);
    else if (op == "-") result = math::subtract(a, b);
    else if (op == "*" || op == "×") result = math::multiply(a, b);
    else if (op == "/" || op == "÷") result = math::divide(a, b);
    else if (op == "%") result = math::modulo(a, b);
    else if (op == "^") result = math::power(a, b);
    else return error("Unsupported binary operator: " + op);
    if (!result.valid) return error(result.error);
    return number(result.value);
}

RuntimeResult Runtime::evaluate_factorial(const FactorialExpression& expression) const {
    const auto operand = evaluate_expression(*expression.operand);
    if (!operand.valid) return operand;
    if (operand.value.type != RuntimeValue::Type::Number) return error("Factorial requires a number.");
    const auto result = math::factorial(operand.value.number);
    if (!result.valid) return error(result.error);
    return number(result.value);
}

RuntimeResult Runtime::evaluate_call(const CallExpression& expression) const {
    std::vector<math::Number> arguments;
    arguments.reserve(expression.arguments.size());
    for (const auto& argument : expression.arguments) {
        const auto result = evaluate_expression(*argument);
        if (!result.valid) return result;
        if (result.value.type != RuntimeValue::Type::Number)
            return error("Math functions require numeric arguments.");
        arguments.push_back(result.value.number);
    }
    const auto result = math::call_function(expression.callee, arguments);
    if (!result.valid) return error(result.error);
    return number(result.value);
}

RuntimeResult Runtime::error(std::string message) const {
    RuntimeResult result;
    result.valid = false;
    result.value.type = RuntimeValue::Type::Undefined;
    result.error = std::move(message);
    return result;
}

RuntimeResult Runtime::number(math::Number value) const {
    RuntimeResult result;
    result.valid = true;
    result.value.type = RuntimeValue::Type::Number;
    result.value.number = std::move(value);
    return result;
}

RuntimeResult Runtime::boolean(bool value) const {
    RuntimeResult result;
    result.valid = true;
    result.value.type = RuntimeValue::Type::Boolean;
    result.value.boolean = value;
    return result;
}

} // namespace mathlang
