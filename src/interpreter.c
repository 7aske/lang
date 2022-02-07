#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 2/6/22.
//

#include "interpreter.h"

// Set all registers as available
void freeall_registers(Interpreter* interpreter) {
	s32 i;
	#pragma GCC unroll 1
	for (i = 0; i < VM_REG_SIZE; ++i) {
		interpreter->freereg[i] = 1;
	}
}

// Allocate a free register. Return the number of
// the register. Die if no available registers.
static s32 alloc_register(Interpreter* interpreter) {
	#pragma GCC unroll 1
	for (s32 i = 0; i < 4; i++) {
		if (interpreter->freereg[i]) {
			interpreter->freereg[i] = 0;
			return (i);
		}
	}
	fprintf(stderr, "Out of registers!\n");
	assert(false);
	exit(1);
}

// Return a register to the list of available registers.
// Check to see if it's not already there.
static void free_register(Interpreter* interpreter, s32 reg) {
	if (interpreter->freereg[reg] != 0) {
		fprintf(stderr, "Error trying to free register %d\n", reg);
		exit(1);
	}
	interpreter->freereg[reg] = 1;
}

// Print out the assembly preamble
void cg_preamble(Interpreter* interpreter) {
	freeall_registers(interpreter);
	fputs(
		"\t.text\n"
		".LC0:\n"
		"\t.string\t\"%d\\n\"\n"
		"printint:\n"
		"\tpushq\t%rbp\n"
		"\tmovq\t%rsp, %rbp\n"
		"\tsubq\t$16, %rsp\n"
		"\tmovl\t%edi, -4(%rbp)\n"
		"\tmovl\t-4(%rbp), %eax\n"
		"\tmovl\t%eax, %esi\n"
		"\tleaq	.LC0(%rip), %rdi\n"
		"\tmovl	$0, %eax\n"
		"\tcall	printf@PLT\n"
		"\tnop\n"
		"\tleave\n"
		"\tret\n"
		"\n"
		"\t.globl\tmain\n"
		"\t.type\tmain, @function\n"
		"main:\n"
		"\tpushq\t%rbp\n"
		"\tmovq	%rsp, %rbp\n",
		interpreter->output);
}

// Print out the assembly postamble
void cg_postamble(Interpreter* interpreter) {
	fputs("\tmovl	$0, %eax\n"
		  "\tpopq	%rbp\n"
		  "\tret\n",
		  interpreter->output);
}

// Load an integer literal value into a register.
// Return the number of the register
s32 cg_load(Interpreter* interpreter, s64 value) {

	// Get a new register
	s32 r = alloc_register(interpreter);

	// Print out the code to initialise it
	fprintf(interpreter->output, "\t# cg_load\n");
	fprintf(interpreter->output, "\tmovq\t$%lld, %s\n", value,
			interpreter->registers[r]);
	return (r);
}

// Add two registers together and return
// the number of the register with the result
s32 cg_add(Interpreter* interpreter, s32 r1, s32 r2) {
	fprintf(interpreter->output, "\t# cg_add\n");
	fprintf(interpreter->output, "\taddq\t%s, %s\n",
			interpreter->registers[r1],
			interpreter->registers[r2]);
	free_register(interpreter, r1);
	return (r2);
}

// Subtract the second register from the first and
// return the number of the register with the result
s32 cg_sub(Interpreter* interpreter, s32 r1, s32 r2) {
	fprintf(interpreter->output, "\t# cg_sub\n");
	fprintf(interpreter->output, "\tsubq\t%s, %s\n", interpreter->registers[r2],
			interpreter->registers[r1]);
	free_register(interpreter, r2);
	return (r1);
}

// Multiply two registers together and return
// the number of the register with the result
s32 cg_mul(Interpreter* interpreter, s32 r1, s32 r2) {
	fprintf(interpreter->output, "\t# cg_mul\n");
	fprintf(interpreter->output, "\timulq\t%s, %s\n",
			interpreter->registers[r1], interpreter->registers[r2]);
	free_register(interpreter, r1);
	return (r2);
}

// Divide the first register by the second and
// return the number of the register with the result
s32 cg_div(Interpreter* interpreter, s32 r1, s32 r2) {
	fprintf(interpreter->output, "\t# cg_div\n");
	fprintf(interpreter->output, "\tmovq\t%s,%%rax\n",
			interpreter->registers[r1]);
	fprintf(interpreter->output, "\tcqo\n");
	fprintf(interpreter->output, "\tidivq\t%s\n", interpreter->registers[r2]);
	fprintf(interpreter->output, "\tmovq\t%%rax,%s\n",
			interpreter->registers[r1]);
	free_register(interpreter, r2);
	return (r1);
}

// Call printint() with the given register
void cg_print_int(Interpreter* interpreter, s32 reg) {
	if (reg == -1) return;
	fprintf(interpreter->output, "\t# cg_print_int\n");
	fprintf(interpreter->output, "\tmovq\t%s, %%rdi\n",
			interpreter->registers[reg]);
	fprintf(interpreter->output, "\tcall\tprintint\n");
	free_register(interpreter, reg);
}

// Determine if the symbol s is in the global symbol table.
// Return its slot position or -1 if not found.
int findglob(Interpreter* interpreter, const char *s) {
	int i;
	for (i = 0; i < interpreter->symbols.count; i++) {
		Symbol* symbol = list_get(&interpreter->symbols, i);
		if (*s == *symbol->name && strcmp(s, symbol->name) == 0)
			return (i);
	}
	return (-1);
}

// Add a global symbol to the symbol table.
// Return the slot number in the symbol table
s32 addglob(Interpreter* interpreter, const char* name) {
	s32 y;

	// If this is already in the symbol table, return the existing slot
	if ((y = findglob(interpreter, name)) != -1)
		return (y);

	// Otherwise get a new slot, fill it in and
	// return the slot number
	Symbol symbol = {.name = strdup(name)};
	list_push(&interpreter->symbols, &symbol);
	return interpreter->symbols.count - 1;
}

// Generate a global symbol
void cg_globsym(Interpreter* interpreter, const char* name) {
	fprintf(interpreter->output, "\t# cg_globsym\n");
	fprintf(interpreter->output, "\t.comm\t%s,8,8\n", name);
}

// Store a register's value into a variable
s32 cg_storglob(Interpreter* interpreter, s32 r, char* name) {
	if (r == -1) return r;
	fprintf(interpreter->output, "\t# cg_storglob\n");
	fprintf(interpreter->output, "\tmovq\t%s, %s(%%rip)\n",
			interpreter->registers[r], name);
	free_register(interpreter, r);
	return (r);
}

// Load a value from a variable into a register.
// Return the number of the register
s32 cg_loadglob(Interpreter* interpreter, char* name) {
	fprintf(interpreter->output, "\t# cg_loadglob\n");
	// Get a new register
	s32 r = alloc_register(interpreter);

	// Print out the code to initialise it
	fprintf(interpreter->output, "\tmovq\t%s(%%rip), %s\n",
			name, interpreter->registers[r]);
	return (r);
}

void interpreter_run(Interpreter* interpreter) {
	cg_preamble(interpreter);
	int reg;
	list_foreach(&interpreter->nodes, Ast_Node**, {
		reg = interpreter_decode(interpreter, *it, -1);
	})
	cg_print_int(interpreter, reg);
	cg_postamble(interpreter);
}

void interpreter_new(Interpreter* interpreter, List* nodes, FILE* output) {
	memcpy(&interpreter->nodes, nodes, sizeof(List));
	stack_new(&interpreter->instructions, sizeof(Instruction));
	interpreter->registers[0] = "%r8";
	interpreter->registers[1] = "%r9";
	interpreter->registers[2] = "%r10";
	interpreter->registers[3] = "%r11";
	interpreter->output = output;
	list_new(&interpreter->symbols, sizeof(Symbol));
}

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg) {
	s32 leftreg, rightreg;
	const char* name;
	if (node->left) {
		leftreg = interpreter_decode(interpreter, node->left, -1);
	}
	if (node->right) {
		if (node->type == AST_TYPE_DECL) rightreg = -1;
		else rightreg = interpreter_decode(interpreter, node->right, leftreg);
	}

	switch (node->type) {
		case AST_ARITHMETIC:
			switch (node->token.type) {
				case TOK_ADD:
					return cg_add(interpreter, leftreg, rightreg);
				case TOK_SUB:
					return cg_sub(interpreter, leftreg, rightreg);
				case TOK_DIV:
					return cg_div(interpreter, leftreg, rightreg);
				case TOK_MUL:
					return cg_mul(interpreter, leftreg, rightreg);
			}
			break;
		case AST_TYPE_DECL:
			name = node->left->token.string_value.data;
			if (findglob(interpreter, name) == -1) {
				cg_globsym(interpreter, name);
				return -1;
			}
			return cg_storglob(interpreter, leftreg, node->token.string_value.data);
		case AST_LVIDENT:
			return cg_storglob(interpreter, reg, node->token.string_value.data);
		case AST_IDENT:
			return cg_loadglob(interpreter, node->token.string_value.data);
		case AST_ASSIGN:
			return rightreg;
		case AST_LITERAL:
			switch (node->token.type) {
				case TOK_LIT_CHR:
				case TOK_LIT_INT:
				case TOK_TRUE:
				case TOK_FALSE:
					return cg_load(interpreter, node->token.integer_value);
				case TOK_LIT_FLT:
					break;
				case TOK_LIT_STR:
					// @Todo ADDR
					break;
				case TOK_NULL:
					// TOTALLY FORGOT ABOUT NULL? Are we Kotlin yet?
					break;
			}
			break;
	}
	return 0;
}

void interpreter_push(Interpreter* interpreter, Instruction instruction) {
	stack_push(&interpreter->instructions, &instruction);
}

#pragma clang diagnostic pop