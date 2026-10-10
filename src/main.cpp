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
        // 61 significant digits show the 60 decimal places in MathLang's PI literal.
        std::cout << std::setprecision(61) << std::defaultfloat << value.number << '\n';
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

void print_help() {
    std::cout << "MathLang Math Core Big Update\n"
              << "Usage: mathlang <file.mth>\n\n"
              << "Operators: + - * / % ^ ! and numeric comparisons\n"
              << "Constants: pi (π), tau (τ), e, phi (φ), golden_ratio\n"
              << "Functions: abs, sqrt, cbrt, exp, ln, log, log10, log2,\n"
              << "          sin, cos, tan, asin, acos, atan, sinh, cosh, tanh,\n"
              << "          asinh, acosh, atanh, floor, ceil, round, trunc, sign,\n"
              << "          min, max, pow, hypot, atan2, clamp, deg, rad, fact\n"
              << "Angles used by trig functions are in radians.\n";
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        print_help();
        return 0;
    }
    if (argc != 2) {
        std::cerr << "Usage: mathlang <file.mth> (or --help)\n";
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
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()
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
        const auto semantic_result = semantic.analyze(*expression);
        if (!semantic_result.valid) {
            std::cerr << "Semantic Error: " << semantic_result.error << '\n';
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
