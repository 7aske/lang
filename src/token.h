//
// Created by nik on 1/25/22.
//
/**
 *
 * Token is a smallest chunk of information that can be extracted from program
 * text. All instructions in the language consist of one or more tokens. Tokens
 * represent concepts as arithmetic/boolean operations, user-defined types,
 * various types of literals etc.
 *
 * token.h defines all of the tokens that the lexer should be able to parse.
 * Additionally, token.h provides a list of both code and debug string representations
 * of each individual token. Tokens that do not have string representations are
 * ones that can be user-defined (literals, identifiers...).
 */

#ifndef LANG_TOKEN_H
#define LANG_TOKEN_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stdtypes.h"

// Returns enum or #define as a string
#define STR(x) #x

// Enumeration of all possible token types. When adding keywords or operators
// they must be places above __KEYWORD_END marker enum. When adding tokens that
// do not have string representations they must be added between __KEYWORD_END
// and __TOK_SIZE. __KEYWORD_END and __TOK_SIZE are utility makers that
// help when iterating over all existing enum values.
typedef enum token_type {
	TOK_INVALID = 0,
	TOK_IF,       // if
	TOK_ELSE,     // else
	TOK_FOR,      // for
	TOK_FOREACH,  // foreach
	TOK_IN,       // in
	TOK_CONT,     // continue
	TOK_BREAK,    // break
	TOK_TRUE,     // true
	TOK_FALSE,    // false
	TOK_NULL,     // null
	TOK_IMPRT,    // import
	TOK_INCL,     // include
	TOK_AS,       // as
	TOK_FN,       // fn
	TOK_RETURN,   // return
	TOK_FAT_ARRW, // =>
	TOK_THIN_ARRW,// ->
	TOK_ASSN,     // =
	TOK_DQM,      // ??
	TOK_QM,       // ?
	TOK_COL,      // :
	TOK_ADD,      // +
	TOK_SUB,      // -
	TOK_DIV,      // /
	TOK_MUL,      // *
	TOK_MOD,      // %
	TOK_AND,      // &&
	TOK_OR,       // ||
	TOK_NOT,      // !
	TOK_EQ,       // ==
	TOK_NE,       // !=
	TOK_GT,       // >=
	TOK_LT,       // <=
	TOK_GE,       // >
	TOK_LE,       // <
	TOK_INC,      // ++
	TOK_DEC,      // --
	TOK_DOT,      // .
	TOK_COMMA,    // ,
	TOK_SCOL,     // ;
	TOK_LPAREN,   // (
	TOK_RPAREN,   // )
	TOK_LBRACE,   // {
	TOK_RBRACE,   // }
	TOK_LBRACK,   // [
	TOK_RBRACK,   // ]
	__TOK_KEYWORD_END,
	TOK_IDEN,
	TOK_LIT_STR,
	TOK_LIT_CHR,
	TOK_LIT_INT,
	TOK_LIT_FLT,
	__TOK_SIZE
} Token_Type;

/**
 * Struct representing a lexer-processed token.
 *
 * @param type Type of the token.
 * @param c0 Token start column.
 * @param r0 Token start row.
 * @param c1 Token end   column.
 * @param r1 Token end   row.
 * @param integer_value Integer value of the token.
 * @param float_value   Floating point value of the token.
 * @struct string_value String value of the token.
 * @struct string_value.size Size of the string value buffer.
 * @struct string_value.data Heap-allocated string value buffer.
 */
typedef struct token {
	Token_Type type;
	u32 c0;
	u32 r0;
	u32 c1;
	u32 r1;

	union {
		s64 integer_value;
		// @Incomplete implement 32bit float
		// float32 float32_value;
		float64 float64_value;
		struct { u32 size; char* data; } string_value;
	};
} Token;

// Debug string representation of token_type enum
static const char* token_repr[] = {
	STR(TOK_INVALID),
	STR(TOK_IF),       // if
	STR(TOK_ELSE),     // else
	STR(TOK_FOR),      // for
	STR(TOK_FOREACH),  // foreach
	STR(TOK_IN),       // in
	STR(TOK_CONT),     // continue
	STR(TOK_BREAK),    // break
	STR(TOK_TRUE),     // true
	STR(TOK_FALSE),    // false
	STR(TOK_NULL),     // null
	STR(TOK_IMPRT),    // import
	STR(TOK_INCL),     // include
	STR(TOK_AS),       // as
	STR(TOK_FN),       // fn
	STR(TOK_RETURN),   // return
	STR(TOK_FAT_ARRW), // =>
	STR(TOK_THIN_ARRW),// ->
	STR(TOK_ASSN),     // =
	STR(TOK_DQM),      // ??
	STR(TOK_QM),       // ?
	STR(TOK_COL),      // :
	STR(TOK_ADD),      // +
	STR(TOK_SUB),      // -
	STR(TOK_DIV),      // /
	STR(TOK_MUL),      // *
	STR(TOK_MOD),      // %
	STR(TOK_AND),      // &&
	STR(TOK_OR),       // ||
	STR(TOK_NOT),      // !
	STR(TOK_EQ),       // ==
	STR(TOK_NE),       // !=
	STR(TOK_GT),       // >=
	STR(TOK_LT),       // <=
	STR(TOK_GE),       // >
	STR(TOK_LE),       // <
	STR(TOK_INC),      // ++
	STR(TOK_DEC),      // --
	STR(TOK_DOT),      // .
	STR(TOK_COMMA),    // ,
	STR(TOK_SCOL),     // ;
	STR(TOK_LPAREN),   // (
	STR(TOK_RPAREN),   // )
	STR(TOK_LBRACE),   // {
	STR(TOK_RBRACE),   // }
	STR(TOK_LBRACK),   // [
	STR(TOK_RBRACK),   // ]
	STR(__TOK_KEYWORD_END),
	STR(TOK_IDEN),
	STR(TOK_LIT_STR),
	STR(TOK_LIT_INT),
	STR(TOK_LIT_CHR),
	STR(TOK_LIT_FLT),
};

// Code values of all token types.
static const char* token_value[] = {
	NULL, // TOK_INVALID
	"if",
	"else",
	"for",
	"foreach",
	"in",
	"continue",
	"break",
	"true",
	"false",
	"null",
	"import",
	"include",
	"as",
	"fn",
	"return",
	"=>",
	"->",
	"=",
	"??",
	"?",
	":",
	"+",
	"-",
	"/",
	"*",
	"%",
	"&&",
	"||",
	"!",
	"==",
	"!=",
	">=",
	"<=",
	">",
	"<",
	"++",
	"--",
	".",
	",",
	";",
	"(",
	")",
	"{",
	"}",
	"[",
	"]",
	// Literals do not have their representations.
	NULL, NULL, NULL, NULL, NULL, NULL
};

/**
 * Resolves arbitrary text contained in the buffer into a token.
 *
 * @param buffer Code text buffer.
 * @param size   Size of the text buffer.
 * @return       Resolved token or TOK_INVALID if the text cannot be resolved.
 */
Token_Type resolve_token(char* buffer, u32 size);

/**
 * Resolves an operator type token from the text pointed by the pointer ptr.
 *
 * @param ptr Pointer to the text buffer containing the text to be parsed.
 * @return Resolved operator token or TOK_INVALID if the text cannot be resolved.
 */
Token_Type resolve_operator(const char* ptr);

#define SIZE_data_array(data) (sizeof(data)/sizeof((data)[0]))

// Static check for whether value array and repr array have corresponding values
// for all defined tokens.
static_assert(__TOK_SIZE == SIZE_data_array(token_repr), "Missing token repr");
static_assert(__TOK_SIZE == SIZE_data_array(token_value), "Missing token value");

#endif //LANG_TOKEN_H
