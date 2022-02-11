//
// Created by nik on 2/6/22.
//

#ifndef LANG_INTERPRETER_H
#define LANG_INTERPRETER_H

#include <stdarg.h>

#include "list.h"
#include "stack.h"
#include "token.h"
#include "ast.h"
#include "type.h"
#include "util.h"

#define VM_REG_SIZE 4

static char* cmplist[] =
	{"sete", "setne", "setg", "setl", "setge", "setle"};
static char* invcmplist[] =
	{"jne", "je", "jle", "jge", "jl", "jg"};

typedef struct symbol {
	const char* name;
	// @Warning Storing copies lol
	Type        type;
	s32         end_label;
} Symbol;

typedef struct interpreter {
	List  nodes;
	const char* code;
	const char* input_filename;
	FILE* output;
	int   freereg[VM_REG_SIZE];
	char* registers[VM_REG_SIZE];
	char* b_registers[VM_REG_SIZE];
	char* d_registers[VM_REG_SIZE];
	List  symbols; // @Optimization this should be by all means a hash table
	s32   label;
} Interpreter;

void interpreter_run(Interpreter* interpreter);

void interpreter_new(Interpreter* interpreter, List* nodes, FILE* file);

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent);

const char* resolve_pointer_var_name(Ast_Node* node);

void fatalf(char* format, ...);

#endif //LANG_INTERPRETER_H
