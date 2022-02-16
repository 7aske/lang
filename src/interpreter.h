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
#include "map.h"

#define VM_REG_SIZE 4

#define VERIFY_REGISTERS(__func, __r1, __r2) \
if ((__r1 < 0 || __r1 > VM_REG_SIZE) || (__r2 < 0 || __r2 > VM_REG_SIZE)) {  \
REPORT_LINE();\
fatalf("Invalid registers `%d`,`%d` in `%s`\n", r1, r2, __func);\
}

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
	char* w_registers[VM_REG_SIZE];
	List  symbols; // @Optimization this should be by all means a hash table
	s32   label;
} Interpreter;

s32 alloc_register(Interpreter* interpreter);

void freeall_registers(Interpreter* interpreter);

void free_register(Interpreter* interpreter, s32 reg);

void interpreter_run(Interpreter* interpreter);

void interpreter_new(Interpreter* interpreter, List* nodes, FILE* file);

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent);

const char* resolve_pointer_var_name(Ast_Node* node);

void fatalf(char* format, ...);

s32 get_label(Interpreter* interpreter);

// codegen
// @formatter:off
s32  cg_compare_jump(Interpreter* interpreter, Token_Type type, s32 r1, s32 r2, s32 label);
s32  cg_add(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_address(Interpreter* interpreter, const char* name);
s32  cg_compare(Interpreter* interpreter, s32 r1, s32 r2, char* op);
s32  cg_compare_set(Interpreter* interpreter, Token_Type type, s32 r1, s32 r2);
s32  cg_dec(Interpreter* interpreter, s32 reg, u32 size);
s32  cg_deref(Interpreter* interpreter, s32 reg, s32 size);
s32  cg_deref_array(Interpreter* interpreter, s32 offset_reg, s32 reg, s32 size);
s32  cg_div(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_equal(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_funccall(Interpreter* interpreter, s32 reg, const char* name);
s32  cg_genglobstr(Interpreter* interpreter, char* name, char* value);
s32  cg_globstr(Interpreter* interpreter, char* name, char* value);
s32  cg_greaterequal(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_greaterthan(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_inc(Interpreter* interpreter, s32 reg, u32 size);
s32  cg_lessequal(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_lessthan(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_load(Interpreter* interpreter, s64 value);
s32  cg_loadglob(Interpreter* interpreter, const char* name, Type* type);
s32  cg_loadglobstr(Interpreter* interpreter, char* name);
s32  cg_mul(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_notequal(Interpreter* interpreter, s32 r1, s32 r2);
s32  cg_shlconst(Interpreter* interpreter, s32 r, s32 val);
s32  cg_shrconst(Interpreter* interpreter, s32 r, s32 val);
s32  cg_storderef(Interpreter* interpreter, s32 r1, s32 r2, Type* type);
s32  cg_storderef_array(Interpreter* interpreter, s32 r1, s32 r2, Type* type);
s32  cg_storglob(Interpreter* interpreter, s32 r, const char* name, Type* type);
s32  cg_sub(Interpreter* interpreter, s32 r1, s32 r2);
void cg_funcpostamble(Interpreter* interpreter);
void cg_funcpreamble(Interpreter* interpreter, const char* name);
void cg_globsym(Interpreter* interpreter, const char* name, Type type);
void cg_jump(Interpreter* interpreter, s32 label);
void cg_label(Interpreter* interpreter, s32 label);
void cg_postamble(Interpreter* interpreter);
void cg_preamble(Interpreter* interpreter);
void cg_return(Interpreter* interpreter, s32 reg, Symbol* symbol);
// @formatter:on

#endif //LANG_INTERPRETER_H
