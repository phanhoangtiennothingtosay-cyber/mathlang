
#include "mathlang/parser.hpp"

#include <stdexcept>
#include <utility>

namespace mathlang {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

const Token& Parser::peek() const {
    return tokens_[current_ < tokens_.size() ? current_ : tokens_.size() - 1];
}

const Token& Parser::previous() const {
    return tokens_[current_ > 0 ? current_ - 1 : 0];
}

const Token& Parser::advance() {
    if (!at_end()) ++current_;
    return previous();
}

bool Parser::check(TokenType type) const {
    return !at_end() && peek().type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

bool Parser::at_end() const {
    return current_ >= tokens_.size() ||
           tokens_[current_].type == TokenType::End;
}

void Parser::skip_separators() {
    while (match(TokenType::Semicolon)) {}
}

[[noreturn]] void Parser::error(const Token& token, const std::string& message) const {
    throw std::runtime_error("Parse error at position " +
                             std::to_string(token.position) + ": " + message);
}

std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();
    skip_separators();

    while (!at_end()) {
        program->statements.push_back(parse_statement());
        if (!at_end() && !check(TokenType::Semicolon))
            error(peek(), "Expected ';' between statements.");
        skip_separators();
    }

    return program;
}

std::unique_ptr<Statement> Parser::parse_statement() {
    if (check(TokenType::Identifier) && peek().lexeme == "print") {
        advance();
        if (!match(TokenType::LeftParen))
            error(peek(), "Expected '(' after print.");

        auto value = parse_expression();
        if (!match(TokenType::RightParen))
            error(peek(), "Expected ')' after print argument.");

        return std::make_unique<PrintStatement>(std::move(value));
    }

    return parse_assignment_or_expression_statement();
}

std::unique_ptr<Statement> Parser::parse_assignment_or_expression_statement() {
    const std::size_t start = current_;
    auto left = parse_expression();

    if (match(TokenType::Equal)) {
        if (auto* target = dynamic_cast<IdentifierExpression*>(left.get())) {
            std::string name = target->name;
            auto value = parse_expression();
            return std::make_unique<AssignmentStatement>(std::move(name), std::move(value));
        }

        if (check(TokenType::Identifier)) {
            std::string name = advance().lexeme;
            return std::make_unique<AssignmentStatement>(std::move(name), std::move(left));
        }

        error(previous(), "Assignment target must be an identifier.");
    }

    if (current_ == start) error(peek(), "Expected an expression.");
    return std::make_unique<ExpressionStatement>(std::move(left));
}

std::unique_ptr<Expression> Parser::parse() {
    auto expression = parse_expression();
    if (!at_end()) error(peek(), "Unexpected token after expression.");
    return expression;
}

std::unique_ptr<Expression> Parser::parse_expression() {
    return parse_comparison();
}

std::unique_ptr<Expression> Parser::parse_comparison() {
    auto expression = parse_additive();

    while (match(TokenType::Equal) || match(TokenType::NotEqual) ||
           match(TokenType::Less) || match(TokenType::Greater) ||
           match(TokenType::LessEqual) || match(TokenType::GreaterEqual)) {
        const std::string op = previous().lexeme;
        auto right = parse_additive();
        expression = std::make_unique<BinaryExpression>(
            op, std::move(expression), std::move(right));
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parse_additive() {
    auto expression = parse_multiplicative();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        const std::string op = previous().lexeme;
        auto right = parse_multiplicative();
        expression = std::make_unique<BinaryExpression>(
            op, std::move(expression), std::move(right));
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parse_multiplicative() {
    auto expression = parse_unary();

    while (match(TokenType::Star) || match(TokenType::Slash) ||
           match(TokenType::Percent) || match(TokenType::Multiply) ||
           match(TokenType::Divide)) {
        const std::string op = previous().lexeme;
        auto right = parse_unary();
        expression = std::make_unique<BinaryExpression>(
            op, std::move(expression), std::move(right));
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parse_unary() {
    if (match(TokenType::Plus) || match(TokenType::Minus)) {
        const std::string op = previous().lexeme;
        return std::make_unique<UnaryExpression>(op, parse_unary());
    }

    return parse_postfix();
}

std::unique_ptr<Expression> Parser::parse_postfix() {
    auto expression = parse_primary();

    while (match(TokenType::Factorial)) {
        expression = std::make_unique<FactorialExpression>(std::move(expression));
    }

    return expression;
}

std::unique_ptr<Expression> Parser::parse_primary() {
    if (match(TokenType::Number)) {
        const double value = std::stod(previous().lexeme);
        return std::make_unique<NumberExpression>(value);
    }

    if (match(TokenType::Identifier)) {
        const std::string name = previous().lexeme;

        if (!match(TokenType::LeftParen))
            return std::make_unique<IdentifierExpression>(name);

        std::vector<std::unique_ptr<Expression>> arguments;
        if (!check(TokenType::RightParen)) {
            do {
                arguments.push_back(parse_expression());
            } while (match(TokenType::Comma));
        }

        if (!match(TokenType::RightParen))
            error(peek(), "Expected ')' after function arguments.");

        return std::make_unique<CallExpression>(name, std::move(arguments));
    }

    if (match(TokenType::LeftParen)) {
        auto expression = parse_expression();
        if (!match(TokenType::RightParen))
            error(peek(), "Expected ')' after expression.");
        return expression;
    }

    error(peek(), "Expected a number, identifier, or grouped expression.");
}

} // namespace mathlang
