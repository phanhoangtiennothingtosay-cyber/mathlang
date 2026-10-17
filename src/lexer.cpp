#include "mathlang/lexer.hpp"

#include <cctype>

namespace mathlang {

Lexer::Lexer(std::string_view source) : source_(source) {}

char Lexer::current() const {
    return position_ >= source_.size() ? '\0' : source_[position_];
}

char Lexer::peek(std::size_t offset) const {
    const std::size_t index = position_ + offset;
    return index >= source_.size() ? '\0' : source_[index];
}

void Lexer::advance() {
    if (position_ < source_.size()) ++position_;
}

void Lexer::skip_whitespace() {
    while (std::isspace(static_cast<unsigned char>(current()))) advance();
}
bool Lexer::skip_comment(std::vector<Token>& tokens) {
    const auto starts_with = [this](std::string_view marker) {
        return source_.substr(position_, marker.size()) == marker;
    };

    const auto skip_line = [this]() {
        while (position_ < source_.size() && current() != '\n') {
            advance();
        }
    };

    const auto skip_block =
        [this, &tokens](std::string_view opener,
                        std::string_view closer) {
            const std::size_t start = position_;

            position_ += opener.size();

            const std::size_t end = source_.find(closer, position_);

            if (end == std::string_view::npos) {
                tokens.push_back({
                    TokenType::Invalid,
                    "unterminated block comment",
                    start
                });

                position_ = source_.size();
                return;
            }

            position_ = end + closer.size();
        };

    // Lua long comment: --[[ ... ]]
    // Must be checked before the ordinary -- line comment.
    if (starts_with("--[[")) {
        skip_block("--[[", "]]");
        return true;
    }

    // Single-line comments: Python, C-style, SQL, Lua, and others.
    if (current() == '#' ||
        starts_with("//") ||
        starts_with("--")) {
        skip_line();
        return true;
    }

    // C / C++ / Java / JavaScript block comment.
    if (starts_with("/*")) {
        skip_block("/*", "*/");
        return true;
    }

    // Haskell block comment.
    if (starts_with("{-")) {
        skip_block("{-", "-}");
        return true;
    }

    // ML / OCaml / Pascal-style block comment.
    if (starts_with("(*")) {
        skip_block("(*", "*)");
        return true;
    }

    // HTML-style comment.
    if (starts_with("<!--")) {
        skip_block("<!--", "-->");
        return true;
    }

    return false;
}

Token Lexer::make_token(TokenType type, std::size_t start, std::size_t end) {
    return {type, std::string(source_.substr(start, end - start)), start};
}

Token Lexer::invalid_token() {
    const std::size_t start = position_;
    advance();
    return make_token(TokenType::Invalid, start, position_);
}

Token Lexer::read_number() {
    const std::size_t start = position_;
    while (std::isdigit(static_cast<unsigned char>(current()))) advance();

    if (current() == '.') {
        advance();
        while (std::isdigit(static_cast<unsigned char>(current()))) advance();
    }

    // Scientific notation is recognized only if an exponent digit exists.
    // This leaves "2e" valid as implicit multiplication (2 * e).
    if (current() == 'e' || current() == 'E') {
        std::size_t look = position_ + 1;
        if (look < source_.size() && (source_[look] == '+' || source_[look] == '-')) ++look;
        if (look < source_.size() && std::isdigit(static_cast<unsigned char>(source_[look]))) {
            advance();
            if (current() == '+' || current() == '-') advance();
            while (std::isdigit(static_cast<unsigned char>(current()))) advance();
        }
    }
    return make_token(TokenType::Number, start, position_);
}

Token Lexer::read_identifier() {
    const std::size_t start = position_;
    while (std::isalnum(static_cast<unsigned char>(current())) || current() == '_') advance();
    return make_token(TokenType::Identifier, start, position_);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (position_ < source_.size()) {
        skip_whitespace();
        if (position_ >= source_.size()) break;

        const std::size_t start = position_;
        const char c = current();
        if (std::isdigit(static_cast<unsigned char>(c)) ||
            (c == '.' && std::isdigit(static_cast<unsigned char>(peek())))) {
            tokens.push_back(read_number());
            continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(read_identifier());
            continue;
        }

        // UTF-8 mathematical symbols used by the language.
        if (source_.substr(position_, 2) == "\xCF\x80") {
            position_ += 2;
            tokens.push_back(make_token(TokenType::Identifier, start, position_));
            continue;
        }
        if (source_.substr(position_, 2) == "\xC3\x97") {
            position_ += 2;
            tokens.push_back(make_token(TokenType::Multiply, start, position_));
            continue;
        }
        if (source_.substr(position_, 2) == "\xC3\xB7") {
            position_ += 2;
            tokens.push_back(make_token(TokenType::Divide, start, position_));
            continue;
        }
        if (source_.substr(position_, 3) == "\xE2\x89\xA0") {
            position_ += 3;
            tokens.push_back(make_token(TokenType::NotEqual, start, position_));
            continue;
        }
        if (source_.substr(position_, 3) == "\xE2\x89\xA4") {
            position_ += 3;
            tokens.push_back(make_token(TokenType::LessEqual, start, position_));
            continue;
        }
        if (source_.substr(position_, 3) == "\xE2\x89\xA5") {
            position_ += 3;
            tokens.push_back(make_token(TokenType::GreaterEqual, start, position_));
            continue;
        }

        const auto remaining = source_.substr(position_, 2);
        if (remaining == "==") { position_ += 2; tokens.push_back(make_token(TokenType::EqualEqual, start, position_)); continue; }
        if (remaining == "!=") { position_ += 2; tokens.push_back(make_token(TokenType::NotEqual, start, position_)); continue; }
        if (remaining == "<=") { position_ += 2; tokens.push_back(make_token(TokenType::LessEqual, start, position_)); continue; }
        if (remaining == ">=") { position_ += 2; tokens.push_back(make_token(TokenType::GreaterEqual, start, position_)); continue; }

        switch (c) {
        case '+': advance(); tokens.push_back(make_token(TokenType::Plus, start, position_)); break;
        case '-': advance(); tokens.push_back(make_token(TokenType::Minus, start, position_)); break;
        case '*': advance(); tokens.push_back(make_token(TokenType::Multiply, start, position_)); break;
        case '/': advance(); tokens.push_back(make_token(TokenType::Divide, start, position_)); break;
        case '%': advance(); tokens.push_back(make_token(TokenType::Modulo, start, position_)); break;
        case '^': advance(); tokens.push_back(make_token(TokenType::Power, start, position_)); break;
        case '!': advance(); tokens.push_back(make_token(TokenType::Factorial, start, position_)); break;
        case '=': advance(); tokens.push_back(make_token(TokenType::Equal, start, position_)); break;
        case '<': advance(); tokens.push_back(make_token(TokenType::Less, start, position_)); break;
        case '>': advance(); tokens.push_back(make_token(TokenType::Greater, start, position_)); break;
        case '(': advance(); tokens.push_back(make_token(TokenType::LeftParen, start, position_)); break;
        case ')': advance(); tokens.push_back(make_token(TokenType::RightParen, start, position_)); break;
        case '[': advance(); tokens.push_back(make_token(TokenType::LeftBracket, start, position_)); break;
        case ']': advance(); tokens.push_back(make_token(TokenType::RightBracket, start, position_)); break;
        case '{': advance(); tokens.push_back(make_token(TokenType::LeftBrace, start, position_)); break;
        case '}': advance(); tokens.push_back(make_token(TokenType::RightBrace, start, position_)); break;
        case ';': advance(); tokens.push_back(make_token(TokenType::Semicolon, start, position_)); break;
        case ',': advance(); tokens.push_back(make_token(TokenType::Comma, start, position_)); break;
        default: tokens.push_back(invalid_token()); break;
        }
    }
    tokens.push_back({TokenType::EndOfFile, "", position_});
    return tokens;
}

} // namespace mathlang
