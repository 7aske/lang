//
// Created by nik on 2/6/22.
//

#ifndef LANG_INTERPRETER_H
#define LANG_INTERPRETER_H

#include "list.h"
#include "stack.h"
#include "token.h"
#include "bytecode.h"
#include "ast.h"

#define VM_REG_SIZE 4

static char* cmplist[] =
	{"sete", "setne", "setg", "setl", "setge", "setle"};
static char* invcmplist[] =
	{"jne", "je", "jle", "jge", "jl", "jg"};

typedef struct symbol {
	const char* name;
	u32         size;
	Primitive   p_type;
	s32         c_type;
	s32         end_label;
} Symbol;

typedef struct interpreter {
	Stack instructions;
	List  nodes;
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

void interpreter_push(Interpreter* interpreter, Instruction instruction);

#endif //LANG_INTERPRETER_H
