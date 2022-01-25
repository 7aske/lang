//
// Created by nik on 1/25/22.
//

#ifndef LANG_TOKEN_H
#define LANG_TOKEN_H
#include <assert.h>
#define STR(x) #x

typedef enum token {
	TOK_INVALID = -1,
	TOK_IF = 0,   // if
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
	TOK_ARRW,     // =>
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
	TOK_LIT_INT,
	TOK_LIT_FLT,
	__TOK_SIZE
	// keywords
} Token;

static const char* token_repr[] = {
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
	STR(TOK_ARRW),     // =>
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
	STR(TOK_LIT_FLT),
};


static const char* token_value[] = {
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
	"=>",
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
	NULL,NULL,NULL,NULL,NULL
};

Token resolve_token(char*, size_t);

#define SIZE_data_array(data) (sizeof(data)/sizeof((data)[0]))

static_assert(__TOK_SIZE == SIZE_data_array(token_repr),  "Missing token repr");
static_assert(__TOK_SIZE == SIZE_data_array(token_value), "Missing token value");

#endif //LANG_TOKEN_H
