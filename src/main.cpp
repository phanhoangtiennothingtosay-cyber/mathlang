
#include "mathlang/lexer.hpp"
#include "mathlang/parser.hpp"
#include "mathlang/runtime.hpp"
#include "mathlang/semantic.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>

namespace {

int print_value(const mathlang::RuntimeValue& value) {
    switch (value.type) {
    case mathlang::RuntimeValue::Type::Number:
        std::cout << std::setprecision(15) << value.number << '\n';
        return 0;
    case mathlang::RuntimeValue::Type::Boolean:
        std::cout << (value.boolean ? "true" : "false") << '\n';
        return 0;
    case mathlang::RuntimeValue::Type::Undefined:
        std::cerr << "Runtime Error: undefined value.\n";
        return 1;
    }
    std::cerr << "Runtime Error: invalid value type.\n";
    return 1;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: mathlang <file.mth>\n";
        return 1;
    }

    const std::string path = argv[1];
    if (path.size() < 4 || path.substr(path.size() - 4) != ".mth") {
        std::cerr << "Error: expected a .mth source file.\n";
        return 1;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Error: cannot read source file: " << path << '\n';
        return 1;
    }

    const std::string source{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    try {
        mathlang::Lexer lexer(source);
        const auto tokens = lexer.tokenize();

        for (const auto& token : tokens) {
            if (token.type == mathlang::TokenType::Invalid) {
                std::cerr << "Syntax Error at position " << token.position
                          << ": invalid token '" << token.text << "'.\n";
                return 1;
            }
        }

        mathlang::Parser parser(tokens);
        const auto expression = parser.parse();

        mathlang::SemanticAnalyzer semantic;
        const auto semanticResult = semantic.analyze(*expression);
        if (!semanticResult.valid) {
            std::cerr << "Semantic Error: " << semanticResult.error << '\n';
            return 1;
        }

        mathlang::Runtime runtime;
        const auto result = runtime.evaluate(*expression);
        if (!result.valid) {
            std::cerr << "Runtime Error: " << result.error << '\n';
            return 1;
        }

        return print_value(result.value);
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
