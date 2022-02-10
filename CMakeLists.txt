cmake_minimum_required(VERSION 3.21)
project(lang C)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        src/ast.c
        src/ast.h
        src/interpreter.c
        src/interpreter.h
        src/lexer.c
        src/lexer.h
        src/linked_list.c
        src/linked_list.h
        src/list.c
        src/list.h
        src/map.c
        src/map.h
        src/parser.c
        src/parser.h
        src/scope.c
        src/scope.h
        src/stack.c
        src/stack.h
        src/stdtypes.h
        src/string_buffer.c
        src/string_buffer.h
        src/token.c
        src/token.h
        src/type.c
        src/type.h
        )

add_executable(lang main.c ${SOURCE_FILES})
add_executable(vm vm.c ${SOURCE_FILES})

# TESTS
include(CTest)

add_executable(lexer_test test/lexer_test.c ${SOURCE_FILES})
target_include_directories(lexer_test PRIVATE src)
add_executable(parser_test test/parser_test.c ${SOURCE_FILES})
target_include_directories(parser_test PRIVATE src)
add_executable(list_test test/list_test.c ${SOURCE_FILES})
target_include_directories(list_test PRIVATE src)
add_executable(map_test test/map_test.c ${SOURCE_FILES})
target_include_directories(map_test PRIVATE src)
add_executable(linked_list_test test/linked_list_test.c ${SOURCE_FILES})
target_include_directories(linked_list_test PRIVATE src)

add_test(lexer_test lexer_test)
add_test(parser_test parser_test)
add_test(list_test list_test)
add_test(map_test map_test)
add_test(linked_list_test linked_list_test)
