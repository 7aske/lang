cmake_minimum_required(VERSION 3.21)
project(lang C)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        src/string_buffer.h
        src/string_buffer.c
        src/lexer.h
        src/lexer.c
        src/token.h
        src/token.c
        )

add_executable(lang main.c ${SOURCE_FILES})

# TESTS
include(CTest)
add_executable(lexer_test test/lexer_test.c ${SOURCE_FILES})
target_include_directories(lexer_test PRIVATE src)

add_test(lexer_test lexer_test)