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
s32 alloc_register(Interpreter* interpreter) {
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
void free_register(Interpreter* interpreter, s32 reg) {
	if (interpreter->freereg[reg] != 0) {
		fprintf(stderr, "Error trying to free register %ld\n", reg);
		// assert(false);
		// exit(1);
	}
	interpreter->freereg[reg] = 1;
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

s32 get_label(Interpreter* interpreter) {
	return interpreter->label++;
}

// Add a global symbol to the symbol table.
// Return the slot number in the symbol table
s32 addglob(Interpreter* interpreter, const char* name, Type type) {
	Symbol symbol = {.name = strdup(name), .type=type, 0};

	return addglobsym(interpreter, symbol);
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

	freeall_registers(interpreter);
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

	interpreter->w_registers[0] = "%r8w";
	interpreter->w_registers[1] = "%r9w";
	interpreter->w_registers[2] = "%r10w";
	interpreter->w_registers[3] = "%r11w";

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

s32 interpreter_decode(Interpreter* interpreter, Ast_Node* node, s32 reg, Ast_Node* parent) {
	s32 leftreg, rightreg, retreg;
	leftreg = rightreg = retreg = -1;
	// Pains me to hack literal strings like this
	char buf[64];
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
		if (node->node_type == AST_TYPE_DECL)
			rightreg = reg;
		else
			rightreg = interpreter_decode(interpreter, node->right, -1, node);
	}

	if (node->left) {
		// @Temporary @Hack to persist the register returned by the right tree
		// parsing of the assignment node + node_type decl
		if (node->node_type == AST_TYPE_DECL) {
			leftreg = interpreter_decode(interpreter, node->left, reg, node);
		} else {
			leftreg = interpreter_decode(interpreter, node->left, rightreg, node);
		}
	}

	switch (node->node_type) {
		case AST_ARRAY_INDEX:
			reg = cg_deref_array(interpreter, rightreg, leftreg, node->left->type.size);
			if (parent->node_type == AST_ASSIGN) {
				return reg;
			} else {
				return cg_deref(interpreter, reg, node->left->type.size);
			}
		case AST_DEREF:
			if (parent->node_type == AST_ASSIGN && node->right->node_type == AST_LVIDENT) {
				return rightreg;
			} else {
				return cg_deref(interpreter, rightreg, node->type.ptr_size);
			}
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
		case AST_PREINC:
			name = node->right->token.name;
			// @Temporary account for the variable size;
			reg = cg_inc(interpreter, rightreg, 1);
			return cg_storglob(interpreter, reg, name, &node->right->type);
		case AST_PREDEC:
			name = node->right->token.name;
			// @Temporary account for the variable size;
			reg = cg_dec(interpreter, rightreg, 1);
			return cg_storglob(interpreter, reg, name, &node->right->type);
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
		case AST_ASSN_BINOP:
			name = node->left->token.name;
			switch (node->token.type) {
				case TOK_ADDASSN:
					reg = cg_add(interpreter, leftreg, rightreg);
					break;
				case TOK_SUBASSN:
					reg = cg_sub(interpreter, leftreg, rightreg);
					break;
				case TOK_DIVASSN:
					reg = cg_div(interpreter, leftreg, rightreg);
					break;
				case TOK_MULASSN:
					reg = cg_mul(interpreter, leftreg, rightreg);
					break;
			}
			return cg_storglob(interpreter, reg, name, &node->left->type);
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
			if (parent->node_type == AST_DEREF) {
				return cg_loadglob(interpreter, name, &node->type);
			}
			if (findglob(interpreter, name) == -1) {
				addglob(interpreter, name, node->type);
				// cg_globsym(interpreter, name, node->type);
			}
			return cg_storglob(interpreter, reg, name, &node->type);
		case AST_IDENT:
			name = node->token.name;
			if (findglob(interpreter, name) == -1) {
				if (parent != NULL && parent->node_type == AST_TYPE_DECL) {
					addglob(interpreter, name, node->type);
					// cg_globsym(interpreter, name, node->type);
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

			if (node->type.flags & TYPE_ARRAY) {
				return cg_address(interpreter, name);
			} else {
				return cg_loadglob(interpreter, name, &node->type);
			}
		case AST_ASSIGN:
			switch (node->left->node_type) {
				case AST_IDENT:
					reg = cg_storglob(interpreter, leftreg, node->left->token.name, &node->type);
					break;
				case AST_ARRAY_INDEX:
					reg = cg_storderef_array(interpreter, rightreg, leftreg, &node->left->left->type);
					break;
				case AST_DEREF:
					reg = cg_storderef(interpreter, rightreg, leftreg, &node->left->right->type);
					break;
				default:
					reg = rightreg;
					break;
			}
			freeall_registers(interpreter);
			return reg;
		case AST_LITERAL:
			switch (node->token.type) {
				case TOK_LIT_CHR:
				case TOK_LIT_INT:
				case TOK_TRUE:
				case TOK_FALSE:
					return cg_load(interpreter, node->token.integer_value);
				case TOK_NULL:
					return cg_load(interpreter, 0);
				case TOK_LIT_FLT:
					// @Todo ADDR
					assert(false);
					break;
				case TOK_LIT_STR:
					// @Refactor magic number.
					snprintf(buf, 64, "S%ld", node->label);
					reg = cg_loadglobstr(interpreter, buf);
					return reg;
			}
			break;
	}
	return 0;
}

s32 cg_inc(Interpreter* interpreter, s32 reg, u32 size) {
	fprintf(interpreter->output, "\t# cg_add\n");
	fprintf(interpreter->output, "\taddq\t$%ld, %s\n",
			size,
			interpreter->registers[reg]);
	return (reg);
}

s32 cg_dec(Interpreter* interpreter, s32 reg, u32 size) {
	fprintf(interpreter->output, "\t# cg_add\n");
	fprintf(interpreter->output, "\tsubq\t$%ld, %s\n",
			size,
			interpreter->registers[reg]);
	return (reg);
}

inline void fatalf(char* format, ...) {
	COLOR(TEXT_RED);
	va_list valist;
	va_start(valist, format);
	vfprintf(stderr, format, valist);
	va_end(valist);
	CLEAR;
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