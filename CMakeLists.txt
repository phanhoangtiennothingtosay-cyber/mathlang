cmake_minimum_required(VERSION 3.16)

project(mathlang VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# The math core uses Boost.Multiprecision's header-only decimal type.
include(CheckIncludeFileCXX)
check_include_file_cxx("boost/multiprecision/cpp_dec_float.hpp" MATHLANG_HAS_BOOST_MULTIPRECISION)
if(NOT MATHLANG_HAS_BOOST_MULTIPRECISION)
    message(FATAL_ERROR
        "MathLang's high-precision math core requires Boost headers. "
        "On Termux, install them with: pkg install boost")
endif()

add_executable(mathlang
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/semantic.cpp
    src/math.cpp
    src/runtime.cpp
    src/variables.cpp
)

target_include_directories(mathlang PRIVATE include)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(mathlang PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
