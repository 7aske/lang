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
	AST_LITERAL,

	AST_ARITHMETIC,
	AST_RELATIONAL,
	AST_EQUALITY,
	AST_BOOLEAN,
	AST_NOT,
	AST_MINUS,
	AST_PREINC,
	AST_POSTINC,
	AST_PREDEC,
	AST_POSTDEC,

	AST_FOR,
	AST_BREAK,
	AST_CONTINUE,

	AST_IF,
	AST_DEFAULT,
	AST_TERNARY,

	AST_BLOCK,
	AST_TYPE_DECL

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
 * type AST_BLOCK_NODE.
 * @param left   Left child of the AST node.
 * @param middle Middle child of the AST node (typically for if statement condition).
 * @param right  Right child of the AST node.
 *
 * Nodes, size and capacity parameters are valid only if the node is of type
 * AST_BLOCK_NODE.
 * @param nodes    Heap-allocated list of pointers to the nodes that are "inside"
 *                 of the block node.
 * @param size     Number of nodes.
 * @param capacity Size of the heap-allocated array for storing nodes.
 *
 * @param precedence Operator precedence of the node.
 * @param type       AST type of the node.
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
		};
		// Block nodes
		struct {
			struct ast_node** nodes;
			u32 size;
			u32 capacity;
		};
	};
	s32 precedence;
	Ast_Node_Type type;
	Token token;
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
 * Function that resolves a AST node type based on the token type. Some token
 * types have strictly bound AST node types that they can create.
 *
 * @param token Token based on which the AST node type is parsed.
 * @return Ast_Node_Type corresponding to the Token type.
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
