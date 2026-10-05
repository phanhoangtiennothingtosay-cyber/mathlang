#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace mathlang {

enum class TokenType {
    Number,
    Identifier,

    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    Factorial,

    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,

    LeftParen,
    RightParen,
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,

    Semicolon,
    Comma,

    EndOfFile,
    Invalid
};

struct Token {
    TokenType type;
    std::string text;
    std::size_t position;
};

class Lexer {
public:
    explicit Lexer(std::string_view source);

    std::vector<Token> tokenize();

private:
    std::string_view source_;
    std::size_t position_ = 0;

    char current() const;
    char peek(std::size_t offset = 1) const;

    void advance();

    void skip_whitespace();

    Token read_number();
    Token read_identifier();

    Token make_token(
        TokenType type,
        std::size_t start,
        std::size_t end
    );

    Token invalid_token();
};

} // namespace mathlang
