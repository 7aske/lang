//
// Created by nik on 1/26/22.
//
/**
 * Parser has a purpose of parsing the list of tokens extracted by the Lexer
 * into a tree structure called an Abstract Syntax Tree(AST). AST represents the
 * code flow and all possible execution paths of the program. AST is parsed,
 * in this case recursively, by looking at current and next TOKEN in the list
 * and making decisions accordingly.
 *
 * AST nodes are split into few categories:
 *  > Expressions - expressions are relatively simple operations written in the
 *                  code. For example a binary expression would write as a||b where
 *                  we have three tokens: a and b which are identifiers, and ||
 *                  which is a boolean or operator. All three together define a
 *                  boolean binary expression.
 *  > Statements  - Statements are more complex expression in the sense that they
 *                  are consisted of one or more expressions written in specific
 *                  way. For example we can have an if statement written as:
 *
 *                  if a == 2 {
 *                    print("a is 2");
 *                  } else {
 *                    print("a is not 2");
 *                  }
 *
 *                  In this example we have an if statement consisted of a boolean
 *                  expression for the condition and two block statements for
 *                  each of the branching options (true or false). We treat block
 *                  nodes as statements rather than expressions as expressions should
 *                  have an evaluation value and block statements do not.
 *
 */

#ifndef LANG_PARSER_H
#define LANG_PARSER_H
#include <stdarg.h>

#include "config.h"
#include "ast.h"
#include "interpreter.h"
#include "lexer.h"
#include "map.h"
#include "scope.h"
#include "stack.h"
#include "util.h"

// Utility to pop and element from the parser node stack.
#define PARSER_POP(dest) stack_pop(&parser->node_stack, dest)

// Utility to push an element to the parser node stack.
#define PARSER_PUSH(src) stack_push(&parser->node_stack, src)
#define PARSER_PEEK(src) stack_peek(&parser->node_stack)

// Utility to pop and element from the parser node stack.
#define FUNCTION_POP(dest) stack_pop(&parser->function_stack, dest)
#define FUNCTION_PEEK() stack_peek(&parser->function_stack)

// Utility to push an element to the function node stack.
#define FUNCTION_PUSH(src) stack_push(&parser->function_stack, src)

#define REPORT_ERROR(token,...) \
parser_report_error(parser, token, __VA_ARGS__);\
COLOR(TEXT_YELLOW);\
fprintf(stderr, "ERROR: ");\
REPORT_LINE(__VA_ARGS__);\
CLEAR

#define PARSER_ERROR_BUFLEN 256
/**
 * Struct representing a parser error.
 *
 * @param source Source token that during parsing was the location of the error.
 * @param text   Heap-allocated error text for the given error.
 */
typedef struct parser_error_report {
	char   text[PARSER_ERROR_BUFLEN];
	Token* source;
} Parser_Error_Report;

/**
 * Struct containing all of the parser state during token parsing.
 *
 * @struct code Struct encapsulating data related to to the program text
 *              being parsed.
 * @param code.size Character length of the code segment
 * @param code.text Actual text data of the program being parsed.
 *
 * @struct error Struct encapsulating data related to parser generated errors.
 * @param error.size     Number of generated errors.
 * @param error.capacity Size of the heap-allocated array for storing error
 *                       reports
 * @param error.reports  Heap-allocated array for storing Parser_Error_Report
 *                       structs.
 *
 * @param node_stack Struct for holding the stack of currently parsed/created
 *                   AST nodes. Useful when parsing binary operations and parenthesis
 *                   related AST nodes.
 * @param function_stack Struct for holding the stack of currently parsing function
 *                       definitions. Used to set the name of the function that is
 *                       being returned by the returned by the return statement.
 */
typedef struct parser {
	// @formatter:off
	struct {
		u64         size;
		char*       text;
		const char* filename;
	} code;

	List  errors;
	Map   symbols;
	Stack node_stack;
	Stack function_stack;
	Stack scopes;
	// @formatter:on
} Parser;

/**
 * @param nodes List of parsed nodes.
 * @param errors List of parser generated errors.
 */
typedef struct parser_result {
	List nodes;
	List errors;
} Parser_Result;

// Returns the current token and increments the token point
#define NEXT_TOKEN(ptrptr) (*(ptrptr))++
// Checks whether next token in the token list is of node_type __token.
#define IS_PEEK_OF_TYPE(__ptrptr, __token) (((*(__ptrptr)) + 1)->type == (__token))
#define IS_AT_OF_TYPE(__ptrptr, __offset, __token) (((*(__ptrptr)) + (__offset))->type == (__token))
// Checks whether previous token in the token list is of node_type __token.
#define IS_PREV_OF_TYPE(__ptrptr, __token) (((*(__ptrptr)) - 1)->type == (__token))
// Checks whether current token in the token list is of node_type __token.
#define IS_CURR_OF_TYPE(__ptrptr, __token) ((*(__ptrptr))->type == (__token))
// Prints __end number of __str to stderr.

/**
 * Enum representing the possible outcomes of parsing an AST node.
 */
typedef enum ast_result_type {
	AST_NO_ERROR = 0,
	AST_ERROR
} Ast_Error;

/**
 * Struct representing the result of a AST node parsing operation.
 *
 * @param error Optionally this would be set to something other than AST_NO_ERROR.
 *              In that case the user must handle provided error accordingly.
 * @param node  Heap-allocated node resulting from the parsing. Will be NULL in
 *              most cases when there is an error. Should be discard or freed if
 *              there is an error.
 */
typedef struct ast_result {
	Ast_Error error;
	Ast_Node* node;
} Ast_Result;

/**
 * Populates an instance of the Parser struct pointer to by the dest parameter.
 *
 * @param dest Location of the allocated Parser struct to be populated with
 *             corresponding data.
 * @param code Source code of the program being parser required for displaying
 *             error messages.
 */
void parser_new(Parser* dest, char* code);

/**
 * Utility method for reporting a new error.
 *
 * @param parser  this
 * @param format  Text format that gets converted to Parser_Error_Report struct
 *                and gets put into the parser.error.reports member.
 */
void parser_report_error(Parser* parser, Token*, const char* format, ...);

/**
 * Entry point of the Parser struct. Responsible for performing parse operations
 * on the tokens contained in the lexer result.
 * @param parser this
 * @param lexer Lexer with parsed tokens.
 * @return Parser_Result containing parsed nodes and possible errors.
 */
Parser_Result parser_parse(Parser* parser, Lexer* lexer);

/**
 * Generic parse statement function. Usually an entry point at the start of parsing
 * a new block of code. This function does the first few decisions as to which
 * function should be called next.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsing the current block of expressions.
 */
Ast_Result parse_statement(Parser* parser, Token** token);

/**
 * Function that parses tokens into an if statement. Always should be called
 * by the parse_statement function.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsing the current block of expressions.
 */
Ast_Result parse_if_statement(Parser* this, Token** token);

/**
 * Generic parse expression function. It looks at current and tokens around
 * current to make decisions on which expression parsing functions to call next.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsing the current block of expressions.
 */
Ast_Result parse_expression(Parser* parser, Token** token);

/**
 * Function for parsing a boolean AND or OR node.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed boolean node.
 */
Ast_Result parse_boolean_node(Parser* parser, Token** token);

/**
 * Function for parsing a binary node. Binary nodes can be of node_type: arithmetic,
 * relational, equality etc.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed boolean node.
 */
Ast_Result parse_binary_operation_node(Parser* parser, Token** token);
Ast_Result parse_assn_binop_node(Parser* parser, Token** token);

/**
 * Function for parsing a block statement.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed block statement.
 */
Ast_Result parse_block_node(Parser*, Token**);

/**
 * Frees parser allocated data (except nodes) from the memory.
 *
 * @param parser this
 */
void parser_free(Parser* parser);

/**
 * Fixes precedence of the node and its right child according to the values of
 * the precedence attribute of corresponding nodes. Nodes with lower precedence
 * tend to climb the AST.
 *
 * @return New valid node with fixed precedence.
 */
Ast_Node* fix_precedence(Ast_Node* node);

/**
 * Wrapper that is used for creating nodes parsed by the parser. Reason this
 * function is used is that this function also pushes the created node tot the stack
 * which is required for properly parsing parenthesis.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result struct with the newly created node.
 */
Ast_Result parser_create_node(Parser* parser, Token** token);

/**
 * Same as parser_create_node(Parser*, Token**) doesn't increment the token
 * pointer. Can be used when creating AST nodes that do not strictly correspond
 * to parsed lexer tokens.
 *
 * @param parser this
 * @param token Pointer to the token we want to craete node for.
 * @return Ast_Result struct with the newly created node.
 */
Ast_Result parser_create_node_no_inc(Parser* parser, Token* token);

/**
 * Function for parsing argument list for function function calls and
 * function declarations and definitions.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed argument list node.
 */
Ast_Result parse_argument_list(Parser* parser, Token** token);

/**
 * Function for parsing variable node_type declaration.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed node_type declaration node.
 */
Ast_Result parse_type_decl_node(Parser* parser, Token** token);


/**
 * Function for parsing assignment node.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed assignment node.
 */
Ast_Result parse_assignment_node(Parser* parser, Token** token);

/**
 * Function for parsing function definitions.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed function definition node.
 */
Ast_Result parse_fn_statement(Parser* parser, Token** token);

/**
 * Function for parsing function return node_type.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed function return node_type.
 */
Ast_Result parse_ret_type_node(Parser* parser, Token** token);

/**
 * Function for parsing function call AST node.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed function call node.
 */
Ast_Result parse_function_call(Parser* parser, Token** token);

/**
 * Function for parsing for statement.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed for statement node.
 */
Ast_Result parse_for_statement(Parser* parser, Token** token);

/**
 * Function for parsing in node.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed in node.
 */
Ast_Result parse_in_node(Parser* parser, Token** token);

/**
 * Function for parsing iter node.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed iter node.
 */
Ast_Result parse_iter_node(Parser* parser, Token** token);

/**
 * Function for parsing while node.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed while node.
 */
Ast_Result parse_while_statement(Parser* parser, Token** token);

/**
 * Function for parsing return statement.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed return statement.
 */
Ast_Result parse_return_statement(Parser* parser, Token** token);

/**
 * Function for parsing prefixed identifier nodes. Prefixes are used
 * for addr, deref, minus and pre dec/inc oprations.
 *
 * @param parser this
 * @param token Pointer to the list lexer of tokens.
 * @return Ast_Result of parsed prefix node.
 */
Ast_Result parse_prefix(Parser* parser, Token** token);
Ast_Result parse_suffix(Parser* parser, Token** token);
Scope* parser_peek_scope(Parser* parser);
Scope* parser_push_scope(Parser* parser);
bool   parser_pop_scope(Parser* parser, Scope* dest);
bool   parser_is_defined(Parser* parser, const char* var_name);
Ast_Result parse_array_index(Parser* p_parser, Token** p_token);

const Type* resolve_pointer_type(Ast_Node* node);

#endif //LANG_PARSER_H

