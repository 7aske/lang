//
// Created by nik on 2/16/22.
//

#include "interpreter.h"

s32 alloc_register(Interpreter* p_interpreter);

// Shift a register left by a constant
s32 cg_shlconst(Interpreter* interpreter, s32 r, s32 val) {
	fprintf(interpreter->output, "\tsalq\t$%ld, %s\n", val, interpreter->registers[r]);
	return (r);
}

// Shift a register left by a constant
s32 cg_shrconst(Interpreter* interpreter, s32 r, s32 val) {
	fprintf(interpreter->output, "\tsarq\t$%ld, %s\n", val, interpreter->registers[r]);
	return (r);
}

// Print out the assembly preamble
void cg_preamble(Interpreter* interpreter) {
}

// Print out the assembly postamble
void cg_postamble(Interpreter* interpreter) {
	fprintf(interpreter->output, "\t# cg_postamble\n");
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
	VERIFY_REGISTERS(__FUNCTION__, r1, r2);
	fprintf(interpreter->output, "\t# cg_sub\n");
	fprintf(interpreter->output, "\tsubq\t%s, %s\n",
			interpreter->registers[r2],
			interpreter->registers[r1]);
	free_register(interpreter, r2);
	return (r1);
}

// Multiply two registers together and return
// the number of the register with the result
s32 cg_mul(Interpreter* interpreter, s32 r1, s32 r2) {
	VERIFY_REGISTERS(__FUNCTION__, r1, r2);
	fprintf(interpreter->output, "\t# cg_mul\n");
	fprintf(interpreter->output, "\timulq\t%s, %s\n",
			interpreter->registers[r1],
			interpreter->registers[r2]);
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
	fprintf(interpreter->output, "\tidivq\t%s\n",
			interpreter->registers[r2]);
	fprintf(interpreter->output, "\tmovq\t%%rax,%s\n",
			interpreter->registers[r1]);
	free_register(interpreter, r2);
	return (r1);
}

void cg_globsym(Interpreter* interpreter, const char* name, Type type) {
	s32 size = (s32) type.size;

	fprintf(interpreter->output, "\t.data\n" "\t.globl\t%s\n", name);
	fprintf(interpreter->output, "%s:\n", name);

	for (int i = 0; i < type.elements; i++) {
		switch (type.size) {
			case 1:
				fprintf(interpreter->output, "\t.byte\t0\n");
				break;
			case 2:
				fprintf(interpreter->output, "\t.word\t0\n");
				break;
			case 4:
				fprintf(interpreter->output, "\t.long\t0\n");
				break;
			case 8:
				fprintf(interpreter->output, "\t.quad\t0\n");
				break;
			default:
				fatalf("Unknown typesize in cg_globsym: %d", size);
		}
	}
}

// Store a register's value into a variable
s32 cg_storglob(Interpreter* interpreter, s32 r, const char* name, Type* type) {
	if (r == -1) return r;
	fprintf(interpreter->output, "\t# cg_storglob\n");

	if (type->size >= TYPE_INT_SIZE) {
		fprintf(interpreter->output, "\tmovq\t%s, %s(%%rip)\n",
				interpreter->registers[r], name);
	} else {
		fprintf(interpreter->output, "\tmovb\t%s, %s(%%rip)\n",
				interpreter->b_registers[r], name);
	}
	if (!interpreter->freereg[r]) {
		free_register(interpreter, r);
	}
	return (r);
}

// Load a value from a variable into a register.
// Return the number of the register
s32 cg_loadglob(Interpreter* interpreter, const char* name, Type* type) {
	fprintf(interpreter->output, "\t# cg_loadglob\n");
	// Get a new register
	s32 r = alloc_register(interpreter);

	// Print out the code to initialize it
	if (type->size >= TYPE_INT_SIZE) {
		fprintf(interpreter->output, "\tmovq\t%s(%%rip), %s\n",
				name, interpreter->registers[r]);
	} else {
		fprintf(interpreter->output, "\tmovzbq\t%s(%%rip), %s\n",
				name, interpreter->registers[r]);
	}
	return (r);
}

// Compare two registers.
s32 cg_compare(Interpreter* interpreter, s32 r1, s32 r2, char* op) {
	fprintf(interpreter->output, "\t# cg_compare %s\n", op);
	fprintf(interpreter->output, "\tcmpq\t%s, %s\n",
			interpreter->registers[r2],
			interpreter->registers[r1]);
	fprintf(interpreter->output, "\t%s\t%s\n",
			op, interpreter->b_registers[r2]);
	fprintf(interpreter->output, "\tandq\t$255,%s\n",
			interpreter->registers[r2]);
	free_register(interpreter, r1);
	return (r2);
}

// Compare register with a constant
s32 cg_compare_constant(Interpreter* interpreter, s32 reg, s64 val, char* op) {
	fprintf(interpreter->output, "\t# cg_compare %s\n", op);
	fprintf(interpreter->output, "\tcmpq\t%s, $%lld\n",
			interpreter->registers[reg],
			val);
	fprintf(interpreter->output, "\t%s\t%s\n",
			op, interpreter->b_registers[reg]);
	fprintf(interpreter->output, "\tandq\t$255,%s\n",
			interpreter->registers[reg]);
	return (reg);
}

s32 cg_equal(Interpreter* interpreter, s32 r1, s32 r2) {
	return (cg_compare(interpreter, r1, r2, "sete"));
}

s32 cg_notequal(Interpreter* interpreter, s32 r1, s32 r2) {
	return (cg_compare(interpreter, r1, r2, "setne"));
}

s32 cg_lessthan(Interpreter* interpreter, s32 r1, s32 r2) {
	return (cg_compare(interpreter, r1, r2, "setl"));
}

s32 cg_greaterthan(Interpreter* interpreter, s32 r1, s32 r2) {
	return (cg_compare(interpreter, r1, r2, "setg"));
}

s32 cg_lessequal(Interpreter* interpreter, s32 r1, s32 r2) {
	return (cg_compare(interpreter, r1, r2, "setle"));
}

s32 cg_greaterequal(Interpreter* interpreter, s32 r1, s32 r2) {
	return (cg_compare(interpreter, r1, r2, "setge"));
}

// Generate a label
void cg_label(Interpreter* interpreter, s32 label) {
	fprintf(interpreter->output, "\t# cg_label L%ld\n", label);
	fprintf(interpreter->output, "L%ld:\n", label);
}

// Generate a jump to a label
void cg_jump(Interpreter* interpreter, s32 label) {
	fprintf(interpreter->output, "\t# cg_jump L%ld\n", label);
	fprintf(interpreter->output, "\tjmp\tL%ld\n", label);
}

// Compare two registers and set if true.
s32 cg_compare_set(Interpreter* interpreter, Token_Type type, s32 r1, s32 r2) {
	s32 op = type - TOK_EQ;
	fprintf(interpreter->output, "\t# cg_compare_set\n");
	fprintf(interpreter->output, "\tcmpq\t%s, %s\n",
			interpreter->registers[r2],
			interpreter->registers[r1]);
	fprintf(interpreter->output, "\t%s\t%s\n",
			cmplist[op],
			interpreter->b_registers[r2]);
	fprintf(interpreter->output, "\tmovzbq\t%s, %s\n",
			interpreter->b_registers[r2],
			interpreter->registers[r2]);
	free_register(interpreter, r1);
	return (r2);
}

// Compare two registers and jump if false.
s32 cg_compare_jump(Interpreter* interpreter, Token_Type type, s32 r1, s32 r2, s32 label) {
	s32 op = type - TOK_EQ;
	fprintf(interpreter->output, "\t# cg_compare_jump\n");
	fprintf(interpreter->output, "\tcmpq\t%s, %s\n",
			interpreter->registers[r2],
			interpreter->registers[r1]);
	fprintf(interpreter->output, "\t%s\tL%ld\n",
			invcmplist[op],
			label);
	freeall_registers(interpreter);
	return (-1);
}

// Compare two registers and jump if false.
s32 cg_compare_jump_constant(Interpreter* interpreter, Token_Type type, s32 reg, s64 val, s32 label) {
	s32 op = type - TOK_EQ;
	fprintf(interpreter->output, "\t# cg_compare_jump\n");
	fprintf(interpreter->output, "\tcmpq\t%s, $%lld\n",
			interpreter->registers[reg],
			val);
	fprintf(interpreter->output, "\t%s\tL%ld\n",
			invcmplist[op],
			label);
	freeall_registers(interpreter);
	return (-1);
}

void cg_funcpreamble(Interpreter* interpreter, const char* name) {
	fprintf(interpreter->output, "# func decl\n");
	fprintf(interpreter->output,
			"\t.text\n"
			"\t.globl\t%s\n"
			"\t.type\t%s, @function\n"
			"%s:\n"
			"\tpushq\t%%rbp\n"
			"\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

void cg_funcpostamble(Interpreter* interpreter) {
	fprintf(interpreter->output, "# func decl end\n");
	fputs("\tpopq\t%rbp\n"
		  "\tret\n", interpreter->output);
}

s32 cg_funccall(Interpreter* interpreter, s32 reg, const char* name) {
	s32 retreg = alloc_register(interpreter);
	fprintf(interpreter->output, "\t# cg_funccall\n");
	if (reg != -1)
		fprintf(interpreter->output, "\tmovq\t%s, %%rdi\n",
				interpreter->registers[reg]);
	fprintf(interpreter->output, "\tcall\t%s\n",
			name);
	fprintf(interpreter->output, "\tmovq\t%%rax, %s\n",
			interpreter->registers[retreg]);
	// free_register(interpreter, retreg);
	freeall_registers(interpreter);
	return retreg;
}

void cg_return(Interpreter* interpreter, s32 reg, Symbol* symbol) {
	// @ToDo type size of return
	fprintf(interpreter->output, "\t# cg_return\n");
	fprintf(interpreter->output, "\tmovq\t%s, %%rax\n",
			interpreter->registers[reg]);
	cg_jump(interpreter, symbol->end_label);
}

s32 cg_address(Interpreter* interpreter, const char* name) {
	s32 r = alloc_register(interpreter);

	fprintf(interpreter->output, "\t# cg_address %s\n", name);
	fprintf(interpreter->output, "\tleaq\t%s(%%rip), %s\n", name, interpreter->registers[r]);
	return r;
}

s32 cg_deref_array(Interpreter* interpreter, s32 offset_reg, s32 reg, s32 size) {
	fprintf(interpreter->output, "\t# cg_deref_array\n");

	// Get the offset from the base of the array.
	fprintf(interpreter->output, "\timulq\t$%ld, %s\n",
			size,
			interpreter->registers[offset_reg]);
	// Add the offset to the base address.
	fprintf(interpreter->output, "\taddq\t%s, %s\n",
			interpreter->registers[offset_reg],
			interpreter->registers[reg]);
	free_register(interpreter, offset_reg);
	return reg;

}

s32 cg_deref(Interpreter* interpreter, s32 reg, s32 size) {
	// Symbol* symbol = findglobsym(interpreter, name);
	// assert(symbol != NULL);

	fprintf(interpreter->output, "\t# cg_deref\n");
	// Dereference 64bits of data.
	fprintf(interpreter->output, "\tmovq\t(%s), %s\n",
			interpreter->registers[reg],
			interpreter->registers[reg]);

	// Mask of the bits we don't need
	if (size * 8 != 0) {
		// This is probably very bad and inefficient, but it discards the part of
		// the register that we don't care about.
		cg_shlconst(interpreter, reg, 64 - (size * 8));
		cg_shrconst(interpreter, reg, 64 - (size * 8));
	}
	return reg;
}

s32 cg_globstr(Interpreter* interpreter, char* name, char* value) {
	char* cptr;

	fprintf(interpreter->output, "%s:\n", name);
	for (cptr = value; *cptr; cptr++) {
		fprintf(interpreter->output, "\t.byte\t%d\n", *cptr);
	}
	// Terminate string.
	fprintf(interpreter->output, "\t.byte\t0\n");
}

// Given the label number of a global string,
// load its address into a new register.
s32 cg_loadglobstr(Interpreter* interpreter, char*name) {
	// Get a new register
	s32 reg = alloc_register(interpreter);
	fprintf(interpreter->output, "\tleaq\t%s(%%rip), %s\n",
			name,
			interpreter->registers[reg]);
	return (reg);
}

s32 cg_genglobstr(Interpreter* interpreter, char* name, char* value) {
	cg_globstr(interpreter, name, value);
	return -1;
}

// Store through a dereferenced pointer
s32 cg_storderef_array(Interpreter* interpreter, s32 r1, s32 r2, Type* type) {
	fprintf(interpreter->output, "\t# cg_storderef_array\n");
	fprintf(interpreter->output, "\tmovq\t%s, (%s)\n",
			interpreter->registers[r1],
			interpreter->registers[r2]);
	free_register(interpreter, r2);
	return (r1);
}

// Store through a dereferenced pointer
s32 cg_storderef(Interpreter* interpreter, s32 r1, s32 r2, Type* type) {
	fprintf(interpreter->output, "\t# cg_storderef\n");
	if (type->size > 1) {
		fprintf(interpreter->output, "\tmovq\t%s, (%s)\n",
				interpreter->registers[r1],
				interpreter->registers[r2]);
	} else {
		fprintf(interpreter->output, "\tmovb\t%s, (%s)\n",
				interpreter->b_registers[r1],
				interpreter->registers[r2]);
	}
	free_register(interpreter, r2);
	return (r1);
}
