#include "mathlang/lexer.hpp"

#include <cctype>

namespace mathlang {

Lexer::Lexer(std::string_view source)
    : source_(source) {}

char Lexer::current() const {
    if (position_ >= source_.size()) {
        return '\0';
    }

    return source_[position_];
}

char Lexer::peek(std::size_t offset) const {
    const std::size_t index = position_ + offset;

    if (index >= source_.size()) {
        return '\0';
    }

    return source_[index];
}

void Lexer::advance() {
    if (position_ < source_.size()) {
        ++position_;
    }
}

void Lexer::skip_whitespace() {
    while (std::isspace(
        static_cast<unsigned char>(current())
    )) {
        advance();
    }
}

Token Lexer::make_token(
    TokenType type,
    std::size_t start,
    std::size_t end
) {
    return {
        type,
        std::string(source_.substr(start, end - start)),
        start
    };
}

Token Lexer::invalid_token() {
    const std::size_t start = position_;
    advance();

    return make_token(
        TokenType::Invalid,
        start,
        position_
    );
}

Token Lexer::read_number() {
    const std::size_t start = position_;

    while (std::isdigit(
        static_cast<unsigned char>(current())
    )) {
        advance();
    }

    if (current() == '.') {
        advance();

        while (std::isdigit(
            static_cast<unsigned char>(current())
        )) {
            advance();
        }
    }

    return make_token(
        TokenType::Number,
        start,
        position_
    );
}

Token Lexer::read_identifier() {
    const std::size_t start = position_;

    while (
        std::isalnum(
            static_cast<unsigned char>(current())
        ) ||
        current() == '_'
    ) {
        advance();
    }

    return make_token(
        TokenType::Identifier,
        start,
        position_
    );
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (position_ < source_.size()) {
        skip_whitespace();

        if (position_ >= source_.size()) {
            break;
        }

        const std::size_t start = position_;
        const char c = current();

        if (std::isdigit(
            static_cast<unsigned char>(c)
        )) {
            tokens.push_back(read_number());
            continue;
        }

        if (
            std::isalpha(
                static_cast<unsigned char>(c)
            ) ||
            c == '_'
        ) {
            tokens.push_back(read_identifier());
            continue;
        }

        if (source_.substr(position_, 2) == "\xCF\x80") {
            position_ += 2;

            tokens.push_back(
                make_token(
                    TokenType::Identifier,
                    start,
                    position_
                )
            );

            continue;
        }

        if (source_.substr(position_, 2) == "\xC3\x97") {
            position_ += 2;

            tokens.push_back(
                make_token(
                    TokenType::Multiply,
                    start,
                    position_
                )
            );

            continue;
        }

        if (source_.substr(position_, 2) == "\xC3\xB7") {
            position_ += 2;

            tokens.push_back(
                make_token(
                    TokenType::Divide,
                    start,
                    position_
                )
            );

            continue;
        }

        if (source_.substr(position_, 3) == "\xE2\x89\xA0") {
            position_ += 3;

            tokens.push_back(
                make_token(
                    TokenType::NotEqual,
                    start,
                    position_
                )
            );

            continue;
        }

        if (source_.substr(position_, 3) == "\xE2\x89\xA4") {
            position_ += 3;

            tokens.push_back(
                make_token(
                    TokenType::LessEqual,
                    start,
                    position_
                )
            );

            continue;
        }

        if (source_.substr(position_, 3) == "\xE2\x89\xA5") {
            position_ += 3;

            tokens.push_back(
                make_token(
                    TokenType::GreaterEqual,
                    start,
                    position_
                )
            );

            continue;
        }

        switch (c) {
        case '+':
            advance();
            tokens.push_back(
                make_token(TokenType::Plus, start, position_)
            );
            break;

        case '-':
            advance();
            tokens.push_back(
                make_token(TokenType::Minus, start, position_)
            );
            break;

        case '*':
            advance();
            tokens.push_back(
                make_token(TokenType::Multiply, start, position_)
            );
            break;

        case '/':
            advance();
            tokens.push_back(
                make_token(TokenType::Divide, start, position_)
            );
            break;

        case '%':
            advance();
            tokens.push_back(
                make_token(TokenType::Modulo, start, position_)
            );
            break;

        case '!':
            advance();
            tokens.push_back(
                make_token(TokenType::Factorial, start, position_)
            );
            break;

        case '=':
            advance();
            tokens.push_back(
                make_token(TokenType::Equal, start, position_)
            );
            break;

        case '<':
            advance();

            tokens.push_back(
                make_token(TokenType::Less, start, position_)
            );
            break;

        case '>':
            advance();

            tokens.push_back(
                make_token(TokenType::Greater, start, position_)
            );
            break;

        case '(':
            advance();
            tokens.push_back(
                make_token(TokenType::LeftParen, start, position_)
            );
            break;

        case ')':
            advance();
            tokens.push_back(
                make_token(TokenType::RightParen, start, position_)
            );
            break;

        case '[':
            advance();
            tokens.push_back(
                make_token(TokenType::LeftBracket, start, position_)
            );
            break;

        case ']':
            advance();
            tokens.push_back(
                make_token(TokenType::RightBracket, start, position_)
            );
            break;

        case '{':
            advance();
            tokens.push_back(
                make_token(TokenType::LeftBrace, start, position_)
            );
            break;

        case '}':
            advance();
            tokens.push_back(
                make_token(TokenType::RightBrace, start, position_)
            );
            break;

        case ';':
            advance();
            tokens.push_back(
                make_token(TokenType::Semicolon, start, position_)
            );
            break;

        case ',':
            advance();
            tokens.push_back(
                make_token(TokenType::Comma, start, position_)
            );
            break;

        default:
            tokens.push_back(invalid_token());
            break;
        }
    }

    tokens.push_back({
        TokenType::EndOfFile,
        "",
        position_
    });

    return tokens;
}

} // namespace mathlang
