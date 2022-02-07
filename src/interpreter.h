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

typedef struct symbol {
	char* name;
} Symbol;

typedef struct interpreter {
	Stack instructions;
	List  nodes;
	FILE* output;
	int   freereg[VM_REG_SIZE];
	char* registers[VM_REG_SIZE];
	List  symbols;
} Interpreter;

void interpreter_run(Interpreter* interpreter);

void interpreter_new(Interpreter* interpreter, List* nodes, FILE* file);

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg);

void interpreter_push(Interpreter* interpreter, Instruction instruction);

#endif //LANG_INTERPRETER_H
