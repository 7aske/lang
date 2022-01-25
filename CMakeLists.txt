cmake_minimum_required(VERSION 3.21)
project(lang C)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c
        src/string_buffer.h
        src/string_buffer.c
        src/lexer.h
        src/lexer.c
        src/token.h
        src/token.c
        )

add_executable(lang ${SOURCE_FILES})
