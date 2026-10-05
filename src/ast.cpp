#include "mathlang/ast.hpp"

#include <memory>
#include <stdexcept>

namespace mathlang {

namespace {

ExpressionPtr clone_node(const Expression& expression) {
    if (const auto* node =
        dynamic_cast<const NumberExpression*>(&expression)) {

        auto copy = std::make_unique<NumberExpression>();
        copy->value = node->value;
        return copy;
    }

    if (const auto* node =
        dynamic_cast<const IdentifierExpression*>(&expression)) {

        auto copy = std::make_unique<IdentifierExpression>();
        copy->name = node->name;
        return copy;
    }

    if (const auto* node =
        dynamic_cast<const UnaryExpression*>(&expression)) {

        auto copy = std::make_unique<UnaryExpression>();
        copy->op = node->op;
        copy->operand = clone_node(*node->operand);
        return copy;
    }

    if (const auto* node =
        dynamic_cast<const BinaryExpression*>(&expression)) {

        auto copy = std::make_unique<BinaryExpression>();
        copy->op = node->op;
        copy->left = clone_node(*node->left);
        copy->right = clone_node(*node->right);
        return copy;
    }

    if (const auto* node =
        dynamic_cast<const CallExpression*>(&expression)) {

        auto copy = std::make_unique<CallExpression>();
        copy->function = node->function;

        copy->arguments.reserve(node->arguments.size());

        for (const auto& argument : node->arguments) {
            copy->arguments.push_back(
                clone_node(*argument)
            );
        }

        return copy;
    }

    if (const auto* node =
        dynamic_cast<const FactorialExpression*>(&expression)) {

        auto copy = std::make_unique<FactorialExpression>();
        copy->operand = clone_node(*node->operand);
        return copy;
    }

    throw std::runtime_error(
        "Cannot clone unknown AST node."
    );
}

} // namespace

ExpressionPtr clone_expression(
    const Expression& expression
) {
    return clone_node(expression);
}

} // namespace mathlang
