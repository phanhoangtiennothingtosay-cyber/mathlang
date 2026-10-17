#include "mathlang/semantic.hpp"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <string>
#include <utility>

namespace mathlang {
namespace {

std::string lower(std::string value) {
    for (char& c : value) {
        c = static_cast<char>(
            std::tolower(static_cast<unsigned char>(c)));
    }

    return value;
}

bool is_zero_argument_function(const std::string& name) {
    return name == "pi" ||
           name == "e" ||
           name == "tau" ||
           name == "goldenratio" ||
           name == "golden_ratio";
}

bool is_function_known(const std::string& name) {
    static const char* const names[] = {
        // Mathematical constants.
        "pi",
        "e",
        "tau",
        "goldenratio",
        "golden_ratio",

        // Basic math.
        "abs",
        "sqrt",
        "cbrt",
        "root",
        "exp",

        // Logarithms.
        "ln",
        "log",
        "log10",
        "log2",

        // Trigonometry in radians.
        "sin",
        "cos",
        "tan",
        "asin",
        "acos",
        "atan",
        "atan2",

        // Trigonometry in degrees.
        "sind",
        "sindeg",
        "sin_deg",

        "cosd",
        "cosdeg",
        "cos_deg",

        "tand",
        "tandeg",
        "tan_deg",

        "asind",
        "asindeg",
        "asin_deg",

        "acosd",
        "acosdeg",
        "acos_deg",

        "atand",
        "atandeg",
        "atan_deg",

        // Hyperbolic functions.
        "sinh",
        "cosh",
        "tanh",
        "asinh",
        "acosh",
        "atanh",

        // Rounding and sign.
        "floor",
        "ceil",
        "ceiling",
        "round",
        "trunc",
        "sign",

        // Multiple-argument functions.
        "min",
        "max",
        "pow",
        "hypot",
        "clamp",

        // Angle conversion.
        "deg",
        "rad",

        // Factorial.
        "fact",
        "factorial"
    };

    return std::any_of(
        std::begin(names),
        std::end(names),
        [&](const char* item) {
            return name == item;
        });
}

bool valid_arity(const std::string& name,
                 std::size_t argument_count) {
    if (is_zero_argument_function(name)) {
        return argument_count == 0;
    }

    if (name == "root" ||
        name == "min" ||
        name == "max" ||
        name == "pow" ||
        name == "hypot" ||
        name == "atan2") {
        return argument_count == 2;
    }

    if (name == "clamp") {
        return argument_count == 3;
    }

    if (name == "log") {
        return argument_count == 1 ||
               argument_count == 2;
    }

    return argument_count == 1;
}

} // namespace

SemanticResult SemanticAnalyzer::analyze(
    const Expression& expression) const {
    return analyze_expression(expression);
}

SemanticResult SemanticAnalyzer::analyze_expression(
    const Expression& expression) const {

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
    const NumberExpression&) const {
    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_identifier(
    const IdentifierExpression&) const {
    // Constants and variables are resolved by the runtime.
    return valid(ValueType::Unknown);
}

SemanticResult SemanticAnalyzer::analyze_unary(
    const UnaryExpression& expression) const {
    const auto operand =
        analyze_expression(*expression.operand);

    if (!operand.valid) {
        return operand;
    }

    if (operand.type != ValueType::Number &&
        operand.type != ValueType::Unknown) {
        return error(
            "Unary operator requires a numeric expression.");
    }

    if (expression.op != "+" &&
        expression.op != "-") {
        return error(
            "Unsupported unary operator: " + expression.op);
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_binary(
    const BinaryExpression& expression) const {
    const auto left =
        analyze_expression(*expression.left);

    if (!left.valid) {
        return left;
    }

    const auto right =
        analyze_expression(*expression.right);

    if (!right.valid) {
        return right;
    }

    const std::string& op = expression.op;

    const bool comparison =
        op == "=" ||
        op == "==" ||
        op == "!=" ||
        op == "≠" ||
        op == "<" ||
        op == ">" ||
        op == "<=" ||
        op == ">=" ||
        op == "≤" ||
        op == "≥";

    if (comparison) {
        const bool left_numeric =
            left.type == ValueType::Number ||
            left.type == ValueType::Unknown;

        const bool right_numeric =
            right.type == ValueType::Number ||
            right.type == ValueType::Unknown;

        if (!left_numeric || !right_numeric) {
            return error(
                "Comparison operators require numeric operands.");
        }

        return valid(ValueType::Boolean);
    }

    const bool arithmetic =
        op == "+" ||
        op == "-" ||
        op == "*" ||
        op == "×" ||
        op == "/" ||
        op == "÷" ||
        op == "%" ||
        op == "^";

    if (!arithmetic) {
        return error(
            "Unsupported binary operator: " + op);
    }

    const bool left_numeric =
        left.type == ValueType::Number ||
        left.type == ValueType::Unknown;

    const bool right_numeric =
        right.type == ValueType::Number ||
        right.type == ValueType::Unknown;

    if (!left_numeric) {
        return error("Left operand must be numeric.");
    }

    if (!right_numeric) {
        return error("Right operand must be numeric.");
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_call(
    const CallExpression& expression) const {
    const std::string name =
        lower(expression.callee);

    if (name.empty()) {
        return error("Function name cannot be empty.");
    }

    if (!is_function_known(name)) {
        return error(
            "Unknown math function: " +
            expression.callee + ".");
    }

    if (!valid_arity(name, expression.arguments.size())) {
        return error(
            "Function '" + name +
            "' received the wrong number of arguments.");
    }

    for (const auto& argument : expression.arguments) {
        const auto result =
            analyze_expression(*argument);

        if (!result.valid) {
            return result;
        }

        if (result.type == ValueType::Boolean) {
            return error(
                "Math functions require numeric arguments.");
        }
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::analyze_factorial(
    const FactorialExpression& expression) const {
    const auto operand =
        analyze_expression(*expression.operand);

    if (!operand.valid) {
        return operand;
    }

    if (operand.type != ValueType::Number &&
        operand.type != ValueType::Unknown) {
        return error(
            "Factorial requires a numeric expression.");
    }

    return valid(ValueType::Number);
}

SemanticResult SemanticAnalyzer::error(
    std::string message) const {
    SemanticResult result;
    result.valid = false;
    result.type = ValueType::Unknown;
    result.error = std::move(message);
    return result;
}

SemanticResult SemanticAnalyzer::valid(
    ValueType type) const {
    SemanticResult result;
    result.valid = true;
    result.type = type;
    return result;
}

} // namespace mathlang