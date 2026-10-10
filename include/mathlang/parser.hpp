
#pragma once

#include "mathlang/ast.hpp"
#include "mathlang/lexer.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace mathlang {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<Program> parse_program();
    std::unique_ptr<Expression> parse();

private:
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();

    bool check(TokenType type) const;
    bool match(TokenType type);
    bool at_end() const;

    void skip_separators();

    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Statement> parse_assignment_or_expression_statement();

    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_comparison();
    std::unique_ptr<Expression> parse_additive();
    std::unique_ptr<Expression> parse_multiplicative();
    std::unique_ptr<Expression> parse_unary();
    std::unique_ptr<Expression> parse_postfix();
    std::unique_ptr<Expression> parse_primary();

    [[noreturn]] void error(
        const Token& token,
        const std::string& message
    ) const;

    const std::vector<Token>& tokens_;
    std::size_t current_ = 0;
};

} // namespace mathlang
