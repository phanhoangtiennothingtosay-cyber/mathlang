#include "mathlang/parser.hpp"

#include <stdexcept>
#include <utility>

namespace mathlang {

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens) {}

const Token& Parser::current() const {
    return tokens_[position_];
}

const Token& Parser::peek(std::size_t offset) const {
    const std::size_t index = position_ + offset;

    if (index >= tokens_.size()) {
        return tokens_.back();
    }

    return tokens_[index];
}

bool Parser::check(TokenType type) const {
    return current().type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }

    ++position_;
    return true;
}

const Token& Parser::consume(TokenType type) {
    if (!check(type)) {
        throw std::runtime_error(
            "Syntax Error at position " +
            std::to_string(current().position)
        );
    }

    const Token& token = current();
    ++position_;
    return token;
}

ExpressionPtr Parser::parse() {
    return parse_expression();
}

ExpressionPtr Parser::parse_expression() {
    return parse_comparison();
}

ExpressionPtr Parser::parse_comparison() {
    auto expression = parse_additive();

    while (
        check(TokenType::Equal) ||
        check(TokenType::NotEqual) ||
        check(TokenType::Less) ||
        check(TokenType::Greater) ||
        check(TokenType::LessEqual) ||
        check(TokenType::GreaterEqual)
    ) {
        const TokenType op = current().type;
        ++position_;

        auto right = parse_additive();

        auto node = std::make_unique<BinaryExpression>();
        node->op = op;
        node->left = std::move(expression);
        node->right = std::move(right);

        expression = std::move(node);
    }

    return expression;
}

ExpressionPtr Parser::parse_additive() {
    auto expression = parse_multiplicative();

    while (
        check(TokenType::Plus) ||
        check(TokenType::Minus)
    ) {
        const TokenType op = current().type;
        ++position_;

        auto right = parse_multiplicative();

        auto node = std::make_unique<BinaryExpression>();
        node->op = op;
        node->left = std::move(expression);
        node->right = std::move(right);

        expression = std::move(node);
    }

    return expression;
}

ExpressionPtr Parser::parse_multiplicative() {
    auto expression = parse_unary();

    while (true) {
        if (
            check(TokenType::Multiply) ||
            check(TokenType::Divide) ||
            check(TokenType::Modulo)
        ) {
            const TokenType op = current().type;
            ++position_;

            auto right = parse_unary();

            auto node = std::make_unique<BinaryExpression>();
            node->op = op;
            node->left = std::move(expression);
            node->right = std::move(right);

            expression = std::move(node);
            continue;
        }

        if (starts_implicit_multiplication()) {
            auto right = parse_unary();

            auto node = std::make_unique<BinaryExpression>();
            node->op = TokenType::Multiply;
            node->left = std::move(expression);
            node->right = std::move(right);

            expression = std::move(node);
            continue;
        }

        break;
    }

    return expression;
}

ExpressionPtr Parser::parse_unary() {
    if (
        check(TokenType::Plus) ||
        check(TokenType::Minus)
    ) {
        const TokenType op = current().type;
        ++position_;

        auto node = std::make_unique<UnaryExpression>();
        node->op = op;
        node->operand = parse_unary();

        return node;
    }

    return parse_postfix();
}

ExpressionPtr Parser::parse_postfix() {
    auto expression = parse_primary();

    while (match(TokenType::Factorial)) {
        auto node = std::make_unique<FactorialExpression>();
        node->operand = std::move(expression);
        expression = std::move(node);
    }

    return expression;
}

ExpressionPtr Parser::parse_primary() {
    if (check(TokenType::Number)) {
        const Token token = current();
        ++position_;
        return make_number(token);
    }

    if (check(TokenType::Identifier)) {
        const Token token = current();
        ++position_;

        if (match(TokenType::LeftParen)) {
            auto call = std::make_unique<CallExpression>();
            call->function = token.text;

            if (!check(TokenType::RightParen)) {
                do {
                    call->arguments.push_back(parse_expression());
                } while (match(TokenType::Comma));
            }

            consume(TokenType::RightParen);
            return call;
        }

        return make_identifier(token);
    }

    if (match(TokenType::LeftParen)) {
        auto expression = parse_expression();
        consume(TokenType::RightParen);
        return expression;
    }

    throw std::runtime_error(
        "Syntax Error at position " +
        std::to_string(current().position)
    );
}

bool Parser::starts_implicit_multiplication() const {
    return
        check(TokenType::Number) ||
        check(TokenType::Identifier) ||
        check(TokenType::LeftParen);
}

ExpressionPtr Parser::make_number(const Token& token) {
    auto node = std::make_unique<NumberExpression>();
    node->value = token.text;
    return node;
}

ExpressionPtr Parser::make_identifier(const Token& token) {
    auto node = std::make_unique<IdentifierExpression>();
    node->name = token.text;
    return node;
}

} // namespace mathlang
