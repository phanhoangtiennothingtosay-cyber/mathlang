#include "mathlang/semantic.hpp"

#include <string>
#include <utility>

namespace mathlang {

SemanticResult SemanticAnalyzer::analyze(
    const Expression& expression
) const {
    return analyze_expression(expression);
}

SemanticResult SemanticAnalyzer::analyze_expression(
    const Expression& expression
) const {
    if (const auto* node =
        dynamic_cast<const NumberExpression*>(&expression)) {
        return analyze_number(*node);
    }

    if (const auto* node =
        dynamic_cast<const IdentifierExpression*>(&expression)) {
        return analyze_identifier(*node);
    }

    if (const auto* node =
        dynamic_cast<const UnaryExpression*>(&expression)) {
        return analyze_unary(*node);
    }

    if (const auto* node =
        dynamic_cast<const BinaryExpression*>(&expression)) {
        return analyze_binary(*node);
    }

    if (const auto* node =
        dynamic_cast<const CallExpression*>(&expression)) {
        return analyze_call(*node);
    }

    if (const auto* node =
        dynamic_cast<const FactorialExpression*>(&expression)) {
        return analyze_factorial(*node);
    }

    return error("Unknown expression.");
}

SemanticResult SemanticAnalyzer::analyze_number(
    const NumberExpression&
) const {
    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_identifier(
    const IdentifierExpression&
) const {
    return valid(ValueType::Unknown);
}

SemanticResult SemanticAnalyzer::analyze_unary(
    const UnaryExpression& expression
) const {
    const auto operand =
        analyze_expression(*expression.operand);

    if (!operand.valid) {
        return operand;
    }

    if (operand.type != ValueType::Number &&
        operand.type != ValueType::Unknown) {
        return error(
            "Unary operator requires a numeric expression."
        );
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_binary(
    const BinaryExpression& expression
) const {
    const auto left = analyze_expression(*expression.left);
    if (!left.valid) return left;

    const auto right = analyze_expression(*expression.right);
    if (!right.valid) return right;

    const std::string& op = expression.op;
    const bool comparison = op == "=" || op == "==" || op == "!=" ||
        op == "≠" || op == "<" || op == ">" || op == "<=" ||
        op == ">=" || op == "≤" || op == "≥";

    if (comparison) {
        if ((left.type != ValueType::Number && left.type != ValueType::Unknown) ||
            (right.type != ValueType::Number && right.type != ValueType::Unknown)) {
            return error("Comparison operators require numeric operands.");
        }
        return valid(ValueType::Boolean);
    }

    const bool arithmetic = op == "+" || op == "-" || op == "*" ||
        op == "×" || op == "/" || op == "÷" || op == "%";
    if (!arithmetic) return error("Unsupported binary operator: " + op);

    if (left.type != ValueType::Number && left.type != ValueType::Unknown)
        return error("Left operand must be numeric.");
    if (right.type != ValueType::Number && right.type != ValueType::Unknown)
        return error("Right operand must be numeric.");
    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_call(
    const CallExpression& expression
) const {
    if (expression.callee.empty()) {
        return error("Function name cannot be empty.");
    }

    for (const auto& argument : expression.arguments) {
        const auto result =
            analyze_expression(*argument);

        if (!result.valid) {
            return result;
        }
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_factorial(
    const FactorialExpression& expression
) const {
    const auto operand =
        analyze_expression(*expression.operand);

    if (!operand.valid) {
        return operand;
    }

    if (operand.type != ValueType::Number &&
        operand.type != ValueType::Unknown) {
        return error(
            "Factorial requires a numeric expression."
        );
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::error(
    std::string message
) const {
    SemanticResult result;
    result.valid = false;
    result.type = ValueType::Unknown;
    result.error = std::move(message);

    return result;
}

SemanticResult SemanticAnalyzer::valid(
    ValueType type
) const {
    SemanticResult result;
    result.valid = true;
    result.type = type;

    return result;
}

} // namespace mathlang
