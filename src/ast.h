//
// Created by nik on 1/26/22.
//
/**
 * ast.h defines all possible AST node types and operator precedence of the nodes
 * that represent operators.
 */

#ifndef LANG_AST_H
#define LANG_AST_H

#include "token.h"
#include "lexer.h"

// All various types of AST nodes that have their functionality described
// by the language spec.
typedef enum ast_node_type {
	AST_NODE = -1,
	AST_ASSIGN,
	AST_IDENT,
	AST_LVIDENT,
	AST_LITERAL,
	AST_ARRAY_INDEX,

	AST_ARITHMETIC,
	AST_ASSN_BINOP,
	AST_RELATIONAL,
	AST_EQUALITY,
	AST_BOOLEAN,
	AST_NOT,
	AST_MINUS,
	AST_PREINC,
	AST_POSTINC,
	AST_PREDEC,
	AST_POSTDEC,
	AST_ADDR,
	AST_DEREF,

	AST_FOR,
	AST_BREAK,
	AST_CONTINUE,
	AST_IN,
	AST_ITER,

	AST_WHILE,
	AST_IF,
	AST_DEFAULT,
	AST_TERNARY,

	AST_BLOCK,
	AST_ARG_LIST,
	AST_TYPE_DEF,
	AST_TYPE_DECL,

	AST_FUNC_DEF,
	AST_FUNC_CALL,
	AST_FUNC_BODY,
	AST_FUNC_RET_TYPE,
	AST_FUNC_RETURN

} Ast_Node_Type;

// Operator precedence of operator notes.
#define AST_PREFIX_PRECEDENCE      120
#define AST_POSTFIX_PRECEDENCE     110
#define AST_NOT_PRECEDENCE         110
#define AST_PLUS_PRECEDENCE        110
#define AST_MINUS_PRECEDENCE       110
#define AST_MUL_DIV_MOD_PRECEDENCE 100
#define AST_ADD_SUB_PRECEDENCE     90
#define AST_RELATIONAL_PRECEDENCE  80
#define AST_EQUALITY_PRECEDENCE    70
#define AST_AND_PRECEDENCE         60
#define AST_OR_PRECEDENCE          50
#define AST_ASSIGN_PRECEDENCE      40

/**
 * Struct representing an AST node.
 *
 * Left, middle and right parameters are valid only if the node is not of
 * node_type AST_BLOCK_NODE.
 * @param left     Left child of the AST node.
 * @param middle   Middle child of the AST node (typically for if statement condition).
 * @param right    Right child of the AST node.
 * @param ret_type Return node_type specifier in a function definition;
 *
 * Nodes, size and capacity parameters are valid only if the node is of node_type
 * AST_BLOCK_NODE.
 * @param nodes    Heap-allocated list of pointers to the nodes that are "inside"
 *                 of the block node.
 * @param size     Number of nodes.
 * @param capacity Size of the heap-allocated array for storing nodes.
 *
 * @param precedence Operator precedence of the node.
 * @param type       AST node_type of the node.
 * @param token      Token that resulted in creation of this node. Not all node
 *                   will have their corresponding tokens.
 *
 */
typedef struct ast_node {
	union {
		// Regular nodes
		struct {
			struct ast_node* middle;
			struct ast_node* left;
			struct ast_node* right;
			struct ast_node* ret_type; // used for functions.
		};
		// Block nodes
		List nodes;
	};
	s32 precedence;
	Type type;
	Ast_Node_Type node_type;
	Token token;
	s32 label;
} Ast_Node;

/**
 * Function for dynamically allocating a AST node struct and configuring it
 * with appropriate initial data.
 *
 * @param token Token corresponding to the created Ast_Node.
 * @return Heap-allocated Ast_Node struct.
 */
Ast_Node* ast_node_new(Token* token);

/**
 * Function that resolves a AST node node_type based on the token node_type. Some token
 * types have strictly bound AST node types that they can create.
 *
 * @param token Token based on which the AST node node_type is parsed.
 * @return Ast_Node_Type corresponding to the Token node_type.
 */
Ast_Node_Type convert_token_to_ast_node_type(Token_Type token);

/**
 * Copies the AST node data pointed to by the node pointer into the location
 * pointed to by the pointer dest. Used for adding nodes to block statements.
 *
 * @param dest Location of where to add the node.
 * @param node Node to add.
 * @return Pointer of the newly added node relative to the dest pointer.
 */
Ast_Node* ast_block_node_add_node(Ast_Node* dest, Ast_Node* node);

#endif //LANG_AST_H
