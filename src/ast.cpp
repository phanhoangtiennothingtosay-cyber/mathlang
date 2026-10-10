#include "mathlang/ast.hpp"

#include <stdexcept>
#include <utility>

namespace mathlang {
namespace {

std::unique_ptr<Expression> clone_ptr(
    const std::unique_ptr<Expression>& expression
) {
    if (!expression) {
        throw std::invalid_argument("Cannot clone a null expression.");
    }

    return clone_expression(*expression);
}

} // namespace

std::unique_ptr<Expression> clone_expression(const Expression& expression) {
    if (const auto* node = dynamic_cast<const NumberExpression*>(&expression)) {
        return std::make_unique<NumberExpression>(node->value);
    }

    if (const auto* node = dynamic_cast<const IdentifierExpression*>(&expression)) {
        return std::make_unique<IdentifierExpression>(node->name);
    }

    if (const auto* node = dynamic_cast<const UnaryExpression*>(&expression)) {
        return std::make_unique<UnaryExpression>(
            node->op,
            clone_ptr(node->operand)
        );
    }

    if (const auto* node = dynamic_cast<const BinaryExpression*>(&expression)) {
        return std::make_unique<BinaryExpression>(
            node->op,
            clone_ptr(node->left),
            clone_ptr(node->right)
        );
    }

    if (const auto* node = dynamic_cast<const CallExpression*>(&expression)) {
        std::vector<std::unique_ptr<Expression>> arguments;
        arguments.reserve(node->arguments.size());

        for (const auto& argument : node->arguments) {
            arguments.push_back(clone_ptr(argument));
        }

        return std::make_unique<CallExpression>(
            node->callee,
            std::move(arguments)
        );
    }

    if (const auto* node = dynamic_cast<const FactorialExpression*>(&expression)) {
        return std::make_unique<FactorialExpression>(
            clone_ptr(node->operand)
        );
    }

    throw std::invalid_argument(
        "Cannot clone an unsupported expression type."
    );
}

} // namespace mathlang
