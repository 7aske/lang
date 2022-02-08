//
// Created by nik on 2/6/22.
//

#ifndef LANG_INTERPRETER_H
#define LANG_INTERPRETER_H

#include "list.h"
#include "stack.h"
#include "bytecode.h"
#include "ast.h"

#define VM_REG_SIZE 4

static char* cmplist[] =
	{"sete", "setne", "setg", "setl", "setge", "setle"};
static char* invcmplist[] =
	{"jne", "je", "jle", "jge", "jl", "jg"};

typedef struct symbol {
	char* name;
} Symbol;

typedef struct interpreter {
	Stack instructions;
	List  nodes;
	FILE* output;
	int   freereg[VM_REG_SIZE];
	char* registers[VM_REG_SIZE];
	char* b_registers[VM_REG_SIZE];
	List  symbols;
	s32   label;
} Interpreter;

void interpreter_run(Interpreter* interpreter);

void interpreter_new(Interpreter* interpreter, List* nodes, FILE* file);

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent);

void interpreter_push(Interpreter* interpreter, Instruction instruction);

#endif //LANG_INTERPRETER_H
