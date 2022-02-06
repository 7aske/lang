#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by nik on 2/6/22.
//

#include "interpreter.h"

void interpreter_run(Interpreter* interpreter) {
	list_foreach(&interpreter->nodes, Ast_Node**, {
		interpreter_decode(interpreter, *it);
	})
}

void interpreter_new(Interpreter* interpreter, List* nodes) {
	memcpy(&interpreter->nodes, nodes, sizeof(List));
	stack_new(&interpreter->instructions, sizeof(Instruction));
}

Decode_Result interpreter_decode(Interpreter* interpreter, Ast_Node* node) {
	Decode_Result decode_result = {0};
	Instruction instruction= {0};
	Decode_Result result;
	switch (node->type) {
		case AST_ARITHMETIC:
			switch (node->token.type) {
				case TOK_ADD:
					decode_result.type = DRT_INSTRUCTION;
					instruction.bytecode = BC_ADD;
					interpreter_push(interpreter, instruction);

					result = interpreter_decode(interpreter,node->left);
					Instruction left;
					left.bytecode = BC_LOAD;
					left.args[0] = R0;
					left.args[1] = result.value;
					interpreter_push(interpreter, left);

					result = interpreter_decode(interpreter,node->right);
					Instruction right;
					right.bytecode = BC_LOAD;
					right.args[0] = R1;
					right.args[1] = result.value;
					interpreter_push(interpreter, right);
					break;
			}
		break;
		case AST_LITERAL:
			decode_result.type = DRT_VALUE;
			switch (node->token.type) {
				case TOK_LIT_INT:
					memcpy(&decode_result.value,&node->token.integer_value, sizeof(u64));
					break;
				case TOK_LIT_FLT:
					memcpy(&node->token.float64_value, &decode_result.value, sizeof(u64));
					break;
				case TOK_LIT_CHR:
					memcpy(&node->token.integer_value,&decode_result.value, sizeof(u64));
					break;
				case TOK_LIT_STR:
					// @Todo ADDR
					break;
				case TOK_TRUE:
					memcpy(&decode_result.value,&node->token.integer_value, sizeof(u64));
					break;
				case TOK_FALSE:
					memcpy(&decode_result.value,&node->token.integer_value, sizeof(u64));
					break;
				case TOK_NULL:
					// TOTALLY FORGOT ABOUT NULL? Are we Kotlin yet?
					break;
			}
			break;
	}
	return decode_result;
}

void interpreter_push(Interpreter* interpreter, Instruction instruction) {
	stack_push(&interpreter->instructions, &instruction);
}

#pragma clang diagnostic pop