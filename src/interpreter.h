//
// Created by nik on 2/6/22.
//

#ifndef LANG_INTERPRETER_H
#define LANG_INTERPRETER_H

#include "list.h"
#include "stack.h"
#include "bytecode.h"
#include "ast.h"

typedef struct interpreter {
	Stack instructions;
	List  nodes;
} Interpreter;

#define VM_REG_SIZE 32

#define R0 0
#define R1 1
#define R2 2
#define R3 3

typedef struct vm {
	List instructions;
	u64  registers[VM_REG_SIZE];
} Vm;

void interpreter_run(Interpreter* interpreter);

void interpreter_new(Interpreter* interpreter, List* nodes);

Decode_Result interpreter_decode(Interpreter* interpreter, Ast_Node* node);

void interpreter_push(Interpreter* interpreter, Instruction instruction);

#endif //LANG_INTERPRETER_H
