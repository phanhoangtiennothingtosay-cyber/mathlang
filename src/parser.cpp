#pragma once

#include "mathlang/ast.hpp"

#include <cstddef>
#include <vector>

namespace mathlang {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    ExpressionPtr parse();

private:
    const std::vector<Token>& tokens_;
    std::size_t position_ = 0;

    const Token& current() const;
    const Token& peek(std::size_t offset = 1) const;

    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token& consume(TokenType type);

    ExpressionPtr parse_expression();
    ExpressionPtr parse_comparison();
    ExpressionPtr parse_additive();
    ExpressionPtr parse_multiplicative();
    ExpressionPtr parse_unary();
    ExpressionPtr parse_postfix();
    ExpressionPtr parse_primary();

    bool starts_implicit_multiplication() const;

    ExpressionPtr make_number(const Token& token);
    ExpressionPtr make_identifier(const Token& token);
};

} // namespace mathlang
