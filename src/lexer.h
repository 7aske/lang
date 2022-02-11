//
// Created by nik on 1/25/22.
//
/**
 * Lexer is a struct designed to resolve source code text into compiler-understandable
 * tokens that can be used to generate the AST. Lexer reads one character at the
 * tome from the source code and makes decisions about how to lex the text. For
 * example source code written as `a: string = "This is a string!";` will bre lexed
 * as follows: TOK_IDEN TOK_COL TOK_IDEN TOK_ASSN TOK_LIT_STRING TOK_SCOL. This
 * sequence can be later used to generate the AST of the assignment expression.
 */

#ifndef LANG_LEXER_H
#define LANG_LEXER_H

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "token.h"
#include "stdtypes.h"
#include "string_buffer.h"
#include "list.h"
#include "util.h"


/**
 * Enum representing the possible outcomes of parsing a code token.
 */
typedef enum lexer_error {
	LEXER_FAILED = -1,
} Lexer_Error;

// @Refactor this can be refactored into a generic list.
/**
 * Struct representing the return value of the complete lexer operation the
 * program source code.
 *
 * @param data     Heap-allocated array of parsed tokens.
 * @param size     Count of Tokens in the data array.
 * @param capacity Maximum capacity of the data array.
 */
typedef struct lexer_result {
	List tokens;
} Lexer_Result;

// @Refactor this is essentially the same as the Parser_Error_Report struct.
// Can be refactored into a generic list.
/**
 * Struct representing a lexer error.
 *
 * @param type Type of the token that caused an error.
 * @param text Static error text for the given error.
 * @param col  Column in the line where the lexing error occurred.
 * @param row  Row of the line where the lexing error occurred.
 */
typedef struct lexer_error_report {
	char* text;
	Token_Type type;
	// @Incomplete add end col and end row
	u32 col;
	u32 row;
} Lexer_Error_Report;

// @Refactor this is essentially the same as the Parser struct.
/**
 * Structs encapsulating the state of the lexing operation.
 *
 * @struct code Struct encapsulating data related to to the program text
 *              being lexer.
 * @param code.size Character length of the code segment
 * @param code.text Actual text data of the program being parsed.
 *
 * @struct errors List of lexer generated errors.
 */
typedef struct lexer {
	struct {
		u64 size;
		char* text;
	} code;

	List tokens;
	List errors;
} Lexer;

// Returns the next token without incrementing the pointer.
#define PEEK_NEXT(ptr) (*((ptr) + 1))
#define PEEK_PREV(ptr) (*((ptr) - 1))

/**
 * Populates an instance of the Lexer struct pointer to by the dest parameter.
 *
 * @param dest Location of the allocated Lexer struct to be populated with
 *             corresponding data.
 * @param code Source code of the program to lex.
 */
void lexer_new(Lexer* dest, char* code);

/**
 * Performs lexing operations on the code in the Lexer struct.
 *
 * @param lexer this
 * @param lexer_result Location of where parsed tokens are stored. This is an
 *                     output pointer.
 * @return number of errors that occurred during lexing.
 */
u32 lexer_lex(Lexer* lexer);

/**
 * @param ptr Pointer to the current character being parsed.
 * @return True if the current character a valid start of an identifier token.
 */
bool lexer_startof_iden(const char* ptr);

/**
 * @param ptr Pointer to the current character being parsed.
 * @return True if the current character a valid start of a number literal token.
 */
bool lexer_startof_number(const char* ptr);

/**
 * @param ptr Pointer to the current character being parsed.
 * @return True if the current character a valid start of a string literal token.
 */
bool lexer_startof_string(const char* ptr);

/**
 * @param ptr pointer to the current character being parsed.
 * @return true if the current character a valid of start a character literal token.
 */
bool lexer_startof_char(const char* ptr);

/**
 * @param ptr pointer to the current character being parsed.
 * @return true if the current character a valid of start a line comment.
 */
bool lexer_startof_linecomment(const char* ptr);

/**
 * @param ptr Pointer to the current character being parsed.
 * @return True if the current character is a valid character of an identifier token.
 */
bool lexer_is_iden(const char* ptr);

/**
 * @param ptr Pointer to the current character being parsed.
 * @return True if the current character is a valid character of a number literal token.
 */
bool lexer_is_number(const char* ptr);

/**
 * @param ptr String to check whether is a float valid float.
 * @return True the string starting with ptr is a valid floating point number.
 */
bool lexer_is_float(const char* ptr);

/**
 * Wrapper for initializing a Token at the location pointed to by dest.
 *
 * @param dest Location of where the token will be initialized.
 * @param type Type of the token (Token_Type).
 * @param size Size of the code chunk related to the created token. Used for
 *             creating a buffer to hold the actual code text data.
 * @param col  Column of the code text where the token was parsed from.
 * @param row  Row of the code text where the token was parsed from.
 */
void lexer_token_new(Token* dest, Token_Type type, u32 size, u32 col, u32 row);

/**
 * Copies the data from the text pointed to by ptr into the buffer while
 * incrementing the pointer. Used for "eating" a character literal from the source
 * code of the program.
 *
 * @param ptr Location of the source code iteration pointer.
 * @param buffer String_Buffer used for storing the data.
 * @return Lexer_Error struct that shows if any error occurred.
 */
Lexer_Error lexer_eat_char(char** ptr, String_Buffer* buffer);

/**
 * Copies the data from the text pointed to by ptr into the buffer while
 * incrementing the pointer. Used for "eating" a string literal from the source
 * code of the program. Validates whether the string literal is properly closed
 * with corresponding quotes.
 *
 * @param ptr Location of the source code iteration pointer.
 * @param buffer String_Buffer used for storing the data.
 * @return Lexer_Error struct that shows if any error occurred.
 */
Lexer_Error lexer_eat_string(char** ptr, String_Buffer* buffer);

/**
 * Copies the data from the text pointed to by ptr into the buffer while
 * incrementing the pointer. Used for "eating" an identifier from the source
 * code of the program.
 *
 * @param ptr Location of the source code iteration pointer.
 * @param buffer String_Buffer used for storing the data.
 * @return Lexer_Error struct that shows if any error occurred.
 */
u32 lexer_eat_iden(char** ptr, String_Buffer* buffer);

/**
 * Copies the data from the text pointed to by ptr into the buffer while
 * incrementing the point. Used for "eating" a number literal from the source
 * code of the program.
 *
 * @param ptr Location of the source code iteration pointer.
 * @param buffer String_Buffer used for storing the data.
 * @return Lexer_Error struct that shows if any error occurred.
 */
u32 lexer_eat_number(char**, String_Buffer*);

/**
 * Functions used for reporting Lexer errors.
 *
 * @param lexer this.
 * @param token_type Type of the token that caused and error.
 * @param col Column in the source code line where the error occurred.
 * @param row Row in the source code line where the error occurred.
 * @param message Static message string that more verbosely describes the error.
 */
void lexer_report_error(Lexer* lexer, Token_Type token_type, u32 col, u32 row, char* message);

/**
 *  Frees all the allocated data used by the lexer.
 *
 *  @param lexer this
 */
void lexer_free(Lexer* lexer);

#endif //LANG_LEXER_H
