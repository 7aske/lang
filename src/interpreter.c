#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 2/6/22.
//

#include "interpreter.h"

#define VERIFY_REGISTERS(__func, __r1, __r2) \
if ((__r1 < 0 || __r1 > VM_REG_SIZE) || (__r2 < 0 || __r2 > VM_REG_SIZE)) {  \
REPORT_LINE();\
fatalf("Invalid registers `%d`,`%d` in `%s`\n", r1, r2, __func);\
}

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
		fprintf(stderr, "Error trying to free register %ld\n", reg);
		assert(false);
		exit(1);
	}
	interpreter->freereg[reg] = 1;
}

// Print out the assembly preamble
void cg_preamble(Interpreter* interpreter) {
	freeall_registers(interpreter);
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
	VERIFY_REGISTERS(__FUNCTION__ , r1, r2);
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

// Call printint() with the given register
s32 cg_print_int(Interpreter* interpreter, s32 reg) {
	if (reg == -1) return -1;
	fprintf(interpreter->output, "\t# cg_print_int\n");
	fprintf(interpreter->output, "\tmovq\t%s, %%rdi\n",
			interpreter->registers[reg]);
	fprintf(interpreter->output, "\tcall\tprintint\n");
	// @Temporary
	if (!interpreter->freereg[reg]) {
		free_register(interpreter, reg);
	}

	return reg;
}

// Determine if the symbol s is in the global symbol table.
// Return its slot position or -1 if not found.
int findglob(Interpreter* interpreter, const char* s) {
	int i;
	for (i = 0; i < interpreter->symbols.count; i++) {
		Symbol* symbol = list_get(&interpreter->symbols, i);
		if (*s == *symbol->name && strcmp(s, symbol->name) == 0)
			return (i);
	}
	return (-1);
}

Symbol* findglobsym(Interpreter* interpreter, const char* s) {
	int i;
	for (i = 0; i < interpreter->symbols.count; i++) {
		Symbol* symbol = list_get(&interpreter->symbols, i);
		if (*s == *symbol->name && strcmp(s, symbol->name) == 0)
			return symbol;
	}
	return NULL;
}

s32 addglobsym(Interpreter* interpreter, Symbol symbol) {
	s32 y;

	// If this is already in the symbol table, return the existing slot
	if ((y = findglob(interpreter, symbol.name)) != -1)
		return y;

	// Otherwise, get a new slot, fill it in and
	// return the slot number
	list_push(&interpreter->symbols, &symbol);
	return interpreter->symbols.count - 1;
}

// Add a global symbol to the symbol table.
// Return the slot number in the symbol table
s32 addglob(Interpreter* interpreter, const char* name, Type type) {
	Symbol symbol = {.name = strdup(name), .type=type, 0};

	return addglobsym(interpreter, symbol);
}

// Generate a global symbol
void cg_globsym(Interpreter* interpreter, const char* name, Type type) {
	s32 size = (type.size > TYPE_CHAR_SIZE ? TYPE_LONG_SIZE : TYPE_CHAR_SIZE);
	fprintf(interpreter->output, "\t# cg_globsym %s\n", name);
	fprintf(interpreter->output, "\t.comm\t%s,%ld,%ld\n",
			name, size, size);
}

// Store a register's value into a variable
s32 cg_storglob(Interpreter* interpreter, s32 r, const char* name) {
	if (r == -1) return r;
	fprintf(interpreter->output, "\t# cg_storglob\n");

	Symbol* symbol = findglobsym(interpreter, name);
	assert(symbol != NULL);

	if (symbol->type.size >= TYPE_INT_SIZE) {
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
s32 cg_loadglob(Interpreter* interpreter, const char* name) {
	fprintf(interpreter->output, "\t# cg_loadglob\n");
	// Get a new register
	s32 r = alloc_register(interpreter);

	Symbol* symbol = findglobsym(interpreter, name);
	assert(symbol != NULL);

	// Print out the code to initialize it
	if (symbol->type.size >= TYPE_INT_SIZE){
		fprintf(interpreter->output, "\tmovq\t%s(%%rip), %s\n",
				name, interpreter->registers[r]);
	} else {
		fprintf(interpreter->output, "\tmovzbq\t%s(%%rip), %s\n",
				name, interpreter->registers[r]);
	}
	return (r);
}

// Compare two registers.
static s32 cg_compare(Interpreter* interpreter, s32 r1, s32 r2, char* op) {
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

s32 get_label(Interpreter* interpreter) {
	return interpreter->label++;
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
int cg_compare_jump(Interpreter* interpreter, Token_Type type, s32 r1, s32 r2, s32 label) {
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

void interpreter_run(Interpreter* interpreter) {
	cg_preamble(interpreter);
	list_foreach(&interpreter->nodes, Ast_Node**, {
		interpreter_decode(interpreter, *it, -1, NULL);
	})
	cg_postamble(interpreter);
}

void interpreter_new(Interpreter* interpreter, List* nodes, FILE* output) {
	memcpy(&interpreter->nodes, nodes, sizeof(List));

	interpreter->registers[0] = "%r8";
	interpreter->registers[1] = "%r9";
	interpreter->registers[2] = "%r10";
	interpreter->registers[3] = "%r11";

	interpreter->b_registers[0] = "%r8b";
	interpreter->b_registers[1] = "%r9b";
	interpreter->b_registers[2] = "%r10b";
	interpreter->b_registers[3] = "%r11b";

	interpreter->d_registers[0] = "%r8d";
	interpreter->d_registers[1] = "%r9d";
	interpreter->d_registers[2] = "%r10d";
	interpreter->d_registers[3] = "%r11d";

	interpreter->output = output;
	interpreter->label = 0;
	list_new(&interpreter->symbols, sizeof(Symbol));
	// for (int i = 1; i < PRIMITIVE_TYPES_LEN; ++i) {
	// 	Symbol symbol = {
	// 		.name = primitive_types[i].name,
	// 		.type = primitive_types[i],
	// 		.end_label = 0
	// 	};
	// 	list_push(&interpreter->symbols, &symbol);
	// 	cg_globsym(interpreter, symbol.name, *symbol.type.type);
	// }
}

s32 interpreter_decode_if(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent) {
	s32 retreg;
	s32 l_false, l_end;

	l_false = get_label(interpreter);
	if (node->right)
		l_end = get_label(interpreter);

	fprintf(interpreter->output, "\t# if\n");
	interpreter_decode(interpreter, node->middle, l_false, node);
	freeall_registers(interpreter);

	fprintf(interpreter->output, "\t# if true\n");
	retreg = interpreter_decode(interpreter, node->left, l_false, node);
	freeall_registers(interpreter);

	if (node->right)
		cg_jump(interpreter, l_end);

	cg_label(interpreter, l_false);

	if (node->right) {
		fprintf(interpreter->output, "\t# if false\n");
		retreg = interpreter_decode(interpreter, node->right, l_end, node);
		freeall_registers(interpreter);
		cg_label(interpreter, l_end);
	}

	return retreg;
}

s32 interpreter_decode_while(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent) {
	s32 retreg;
	s32 l_start, l_end;

	l_start = get_label(interpreter);
	l_end = get_label(interpreter);
	cg_label(interpreter, l_start);

	fprintf(interpreter->output, "\t# while condition\n");
	interpreter_decode(interpreter, node->middle, l_end, node);
	freeall_registers(interpreter);

	fprintf(interpreter->output, "\t# while body\n");
	retreg = interpreter_decode(interpreter, node->left, -1, node);
	freeall_registers(interpreter);

	cg_jump(interpreter, l_start);
	cg_label(interpreter, l_end);
	return retreg;
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

s32 cg_deref(Interpreter* interpreter, s32 reg, const char* name) {
	Symbol* symbol = findglobsym(interpreter, name);
	assert(symbol != NULL);

	fprintf(interpreter->output, "\t# cg_deref\n");
	s32 size = (symbol->type.size > TYPE_CHAR_SIZE ? TYPE_LONG_SIZE : TYPE_CHAR_SIZE);
	if (size > 1) {
		fprintf(interpreter->output, "\tmovq\t(%s), %s\n",
				interpreter->registers[reg],
				interpreter->registers[reg]);
	} else {
		fprintf(interpreter->output, "\tmovzbq\t(%s), %s\n",
				interpreter->registers[reg],
				interpreter->registers[reg]);
	}
	return reg;
}

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent) {
	s32 leftreg, rightreg, retreg;
	const char* name;
	if (node->node_type == AST_IF) {
		return interpreter_decode_if(interpreter, node, reg, parent);
	} else if (node->node_type == AST_WHILE) {
		return interpreter_decode_while(interpreter, node, reg, parent);
	} else if (node->node_type == AST_FUNC_DEF) {
		name = node->left->token.name;
		s32 end_label = get_label(interpreter);
		addglobsym(interpreter, (Symbol) {.name=name, .end_label=end_label, .type=node->type});
		cg_funcpreamble(interpreter, name);
		interpreter_decode(interpreter, node->right, -1, node);
		cg_label(interpreter, end_label);
		cg_funcpostamble(interpreter);
		return -1;
	} else if (node->node_type == AST_BLOCK) {
		for (s32 i = 0; i < node->nodes.count; ++i) {
			retreg = interpreter_decode(interpreter,
										*list_get_as(&node->nodes, i, Ast_Node**),
										retreg,
										node);
		}
		return retreg;
	}

	if (node->right) {
		// Do not parse the right size of the type declaration
		if (node->node_type == AST_TYPE_DECL || node->right->node_type == AST_DEREF)
			rightreg = reg;
		else
			rightreg = interpreter_decode(interpreter, node->right, -1, node);
	}

	if (node->left) {
		// @Temporary @Hack to persist the register returned by the right tree
		// parsing of the assignment node + node_type decl
		if (node->node_type == AST_TYPE_DECL && rightreg == -1) rightreg = reg;
		leftreg = interpreter_decode(interpreter, node->left, rightreg, node);
	}

	switch (node->node_type) {
		case AST_DEREF:
			return cg_deref(interpreter, leftreg, resolve_pointer_var_name(node));
		case AST_ADDR:
			return cg_address(interpreter, resolve_pointer_var_name(node));
		case AST_FUNC_RETURN:
			name = node->middle->left->token.name;
			cg_return(interpreter, rightreg, findglobsym(interpreter, name));
			return -1;
		case AST_FUNC_CALL:
			name = node->left->token.name;
			if (node->middle->nodes.count != 0) {
				// @ToDo parse argument list
				leftreg = interpreter_decode(interpreter,
											 *list_get_as(&node->middle->nodes, 0, Ast_Node**),
											 -1,
											 parent);
			} else {
				leftreg = -1;
			}
			return cg_funccall(interpreter, leftreg, name);
		case AST_FUNC_DEF:
			assert(false);
			break;
		case AST_EQUALITY:
			switch (node->token.type) {
				case TOK_EQ:
				case TOK_NE:
					if (parent != NULL &&
						(parent->node_type == AST_IF || parent->node_type == AST_WHILE))
						return cg_compare_jump(interpreter, node->token.type,
											   leftreg, rightreg, reg);
					else
						return cg_compare_set(interpreter, node->token.type,
											  leftreg, rightreg);
			}
			break;
		case AST_RELATIONAL:
			switch (node->token.type) {
				case TOK_GT:
				case TOK_GE:
				case TOK_LT:
				case TOK_LE:
					if (parent != NULL &&
						(parent->node_type == AST_IF || parent->node_type == AST_WHILE))
						return cg_compare_jump(interpreter, node->token.type,
											   leftreg, rightreg, reg);
					else
						return cg_compare_set(interpreter, node->token.type,
											  leftreg, rightreg);
			}
			break;
		case AST_ARITHMETIC:
			switch (node->token.type) {
				case TOK_ADD:
					return cg_add(interpreter, leftreg, rightreg);
				case TOK_SUB:
					return cg_sub(interpreter, leftreg, rightreg);
				case TOK_DIV:
					return cg_div(interpreter, leftreg, rightreg);
				case TOK_STAR:
					return cg_mul(interpreter, leftreg, rightreg);
			}
			break;
		case AST_TYPE_DECL:
			return reg;
		case AST_LVIDENT:
			name = node->token.name;
			if (findglob(interpreter, name) == -1) {
				addglob(interpreter, name, node->type);
				cg_globsym(interpreter, name, node->type);
			}
			return cg_storglob(interpreter, reg, name);
		case AST_IDENT:
			name = node->token.name;
			if (findglob(interpreter, name) == -1) {
				if (parent != NULL && parent->node_type == AST_TYPE_DECL) {
					addglob(interpreter, name, node->type);
					cg_globsym(interpreter, name, node->type);
				} else {
					COLOR(TEXT_YELLOW);
					fprintf(stderr, "WARNING: Possible undefined reference to `%s`. %s:%lu:%lu\n",
							name,
							interpreter->input_filename,
							node->token.r0,
							node->token.c0);
					CLEAR;
					print_token_source_code_location(interpreter->code, &node->token);
				}
				return -1;
			}
			return cg_loadglob(interpreter, name);
		case AST_ASSIGN:
			freeall_registers(interpreter);
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
					return cg_load(interpreter, 0);
			}
			break;
	}
	return 0;
}

inline void fatalf(char* format, ...) {
	va_list valist;
	va_start(valist, format);
	vfprintf(stderr, format, valist);
	va_end(valist);
	assert(false);
	exit(EXIT_FAILURE);
}

const char* resolve_pointer_var_name(Ast_Node* node) {
	while (node != NULL && (node->node_type == AST_DEREF || node->node_type == AST_ADDR))
		node = node->right;
	if (node == NULL)
		return NULL;
	return node->token.name;
}

#pragma clang diagnostic pop