#include "mathlang/lexer.hpp"
#include "mathlang/parser.hpp"
#include "mathlang/runtime.hpp"
#include "mathlang/semantic.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: mathlang <file.mth>\n";
        return 1;
    }

    const std::string path = argv[1];

    if (
        path.size() < 4 ||
        path.substr(path.size() - 4) != ".mth"
    ) {
        std::cerr << "Error: expected a .mth source file.\n";
        return 1;
    }

    std::ifstream file(path, std::ios::binary);

    if (!file) {
        std::cerr << "Error: cannot read source file.\n";
        return 1;
    }

    const std::string source(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    mathlang::Lexer lexer(source);
    const auto tokens = lexer.tokenize();

    for (const auto& token : tokens) {
        if (token.type == mathlang::TokenType::Invalid) {
            std::cerr
                << "Syntax Error at position "
                << token.position
                << '\n';

            return 1;
        }
    }

    try {
        mathlang::Parser parser(tokens);
        auto expression = parser.parse();

        mathlang::SemanticAnalyzer semantic;
        const auto semantic_result =
            semantic.analyze(*expression);

        if (!semantic_result.valid) {
            std::cerr
                << "Semantic Error: "
                << semantic_result.error
                << '\n';

            return 1;
        }

        mathlang::Runtime runtime;
        const auto result =
            runtime.evaluate(*expression);

        if (!result.valid) {
            std::cerr
                << "Runtime Error: "
                << result.error
                << '\n';

            return 1;
        }

        switch (result.value.type) {
        case mathlang::RuntimeValue::Type::Number:
            std::cout
                << result.value.number
                << '\n';
            break;

        case mathlang::RuntimeValue::Type::Boolean:
            std::cout
                << (result.value.boolean ? "true" : "false")
                << '\n';
            break;

        case mathlang::RuntimeValue::Type::Undefined:
            std::cerr
                << "Runtime Error: undefined value.\n";
            return 1;
        }

    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
