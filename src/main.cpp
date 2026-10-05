#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace mathlang {

enum class SourceCheck {
    Valid,
    WrongLanguage
};

bool has_mth_extension(std::string_view path) {
    constexpr std::string_view ext = ".mth";

    if (path.size() < ext.size()) {
        return false;
    }

    return path.substr(path.size() - ext.size()) == ext;
}

SourceCheck syntax_guard(std::string_view source) {
    constexpr std::string_view foreign_markers[] = {
        "#include",
        "std::",
        "int main(",
        "using namespace",
        "public static void",
        "System.out.",
        "console.log(",
        "def ",
        "import ",
        "package "
    };

    for (const auto marker : foreign_markers) {
        if (source.find(marker) != std::string_view::npos) {
            return SourceCheck::WrongLanguage;
        }
    }

    return SourceCheck::Valid;
}

bool read_source(const char* path, std::string& source) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();

    if (size < 0) {
        return false;
    }

    source.reserve(static_cast<std::size_t>(size));
    file.seekg(0, std::ios::beg);

    source.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );

    return true;
}

} // namespace mathlang

int main(int argc, char* argv[]) {
    using namespace mathlang;

    if (argc < 2) {
        std::cerr << "Usage: mathlang <file.mth>\n";
        return 1;
    }

    const std::string path = argv[1];

    if (!has_mth_extension(path)) {
        std::cerr << "Error: expected a .mth source file.\n";
        return 1;
    }

    std::string source;

    if (!read_source(path.c_str(), source)) {
        std::cerr << "Error: cannot read source file.\n";
        return 1;
    }

    if (syntax_guard(source) == SourceCheck::WrongLanguage) {
        std::cerr << "Hey, wrong programming language.\n";
        return 1;
    }

    std::cout << "mathlang: source accepted.\n";

    return 0;
}
