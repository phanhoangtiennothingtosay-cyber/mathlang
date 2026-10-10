#include "mathlang/parser.hpp"

#include <stdexcept>
#include <utility>

namespace mathlang {

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

const Token& Parser::peek() const {
    static const Token emptyToken{TokenType::EndOfFile, "", 0};
    if (tokens_.empty()) return emptyToken;
    if (current_ >= tokens_.size()) return tokens_.back();
    return tokens_[current_];
}

const Token& Parser::previous() const {
    static const Token emptyToken{TokenType::EndOfFile, "", 0};
    if (tokens_.empty() || current_ == 0) return emptyToken;
    return tokens_[current_ - 1];
}

const Token& Parser::advance() {
    if (!at_end()) ++current_;
    return previous();
}

bool Parser::check(TokenType type) const { return !at_end() && peek().type == type; }

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

bool Parser::at_end() const {
    return tokens_.empty() || current_ >= tokens_.size() ||
           tokens_[current_].type == TokenType::EndOfFile;
}

void Parser::skip_separators() { while (match(TokenType::Semicolon)) {} }

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
    if (check(TokenType::Identifier) && peek().text == "print" &&
        current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::LeftParen) {
        advance(); advance();
        auto value = parse_expression();
        if (!match(TokenType::RightParen)) error(peek(), "Expected ')' after print argument.");
        return std::make_unique<PrintStatement>(std::move(value));
    }
    return parse_assignment_or_expression_statement();
}

std::unique_ptr<Statement> Parser::parse_assignment_or_expression_statement() {
    if (check(TokenType::Identifier) && current_ + 1 < tokens_.size() &&
        tokens_[current_ + 1].type == TokenType::Equal) {
        const std::string name = advance().text;
        advance();
        auto value = parse_expression();
        return std::make_unique<AssignmentStatement>(name, std::move(value));
    }
    return std::make_unique<ExpressionStatement>(parse_expression());
}

std::unique_ptr<Expression> Parser::parse() {
    auto expression = parse_expression();
    if (!at_end()) error(peek(), "Unexpected token after expression.");
    return expression;
}

std::unique_ptr<Expression> Parser::parse_expression() { return parse_comparison(); }

std::unique_ptr<Expression> Parser::parse_comparison() {
    auto expression = parse_additive();
    while (match(TokenType::Equal) || match(TokenType::EqualEqual) ||
           match(TokenType::NotEqual) || match(TokenType::Less) ||
           match(TokenType::Greater) || match(TokenType::LessEqual) ||
           match(TokenType::GreaterEqual)) {
        const std::string op = previous().text;
        auto right = parse_additive();
        expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
    }
    return expression;
}

std::unique_ptr<Expression> Parser::parse_additive() {
    auto expression = parse_multiplicative();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        const std::string op = previous().text;
        auto right = parse_multiplicative();
        expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
    }
    return expression;
}

std::unique_ptr<Expression> Parser::parse_multiplicative() {
    auto expression = parse_unary();
    while (true) {
        if (match(TokenType::Multiply) || match(TokenType::Divide) || match(TokenType::Modulo)) {
            const std::string op = previous().text;
            auto right = parse_unary();
            expression = std::make_unique<BinaryExpression>(op, std::move(expression), std::move(right));
            continue;
        }

        const bool adjacent = previous().position + previous().text.size() == peek().position;
        const bool implicitMultiplication = check(TokenType::Identifier) ||
            check(TokenType::LeftParen) || (check(TokenType::Number) && adjacent);
        if (implicitMultiplication) {
            auto right = parse_unary();
            expression = std::make_unique<BinaryExpression>("*", std::move(expression), std::move(right));
            continue;
        }
        break;
    }
    return expression;
}

std::unique_ptr<Expression> Parser::parse_unary() {
    if (match(TokenType::Plus) || match(TokenType::Minus)) {
        const std::string op = previous().text;
        return std::make_unique<UnaryExpression>(op, parse_unary());
    }
    return parse_power();
}

std::unique_ptr<Expression> Parser::parse_power() {
    auto expression = parse_postfix();
    if (match(TokenType::Power)) {
        // Parsing the right operand as unary makes exponentiation right-associative
        // and lets expressions such as 2^-3 work as expected.
        auto right = parse_unary();
        expression = std::make_unique<BinaryExpression>("^", std::move(expression), std::move(right));
    }
    return expression;
}

std::unique_ptr<Expression> Parser::parse_postfix() {
    auto expression = parse_primary();
    while (match(TokenType::Factorial))
        expression = std::make_unique<FactorialExpression>(std::move(expression));
    return expression;
}

std::unique_ptr<Expression> Parser::parse_primary() {
    if (match(TokenType::Number)) {
        try {
            return std::make_unique<NumberExpression>(math::Number(previous().text));
        } catch (const std::exception&) {
            error(previous(), "Invalid numeric literal.");
        }
    }
    if (match(TokenType::Identifier)) {
        const std::string name = previous().text;
        if (!match(TokenType::LeftParen)) return std::make_unique<IdentifierExpression>(name);
        std::vector<std::unique_ptr<Expression>> arguments;
        if (!check(TokenType::RightParen)) {
            do { arguments.push_back(parse_expression()); } while (match(TokenType::Comma));
        }
        if (!match(TokenType::RightParen)) error(peek(), "Expected ')' after function arguments.");
        return std::make_unique<CallExpression>(name, std::move(arguments));
    }
    if (match(TokenType::LeftParen)) {
        auto expression = parse_expression();
        if (!match(TokenType::RightParen)) error(peek(), "Expected ')' after expression.");
        return expression;
    }
    error(peek(), "Expected a number, identifier, or grouped expression.");
}

} // namespace mathlang
