
cmake_minimum_required(VERSION 3.16)

project(mathlang VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
