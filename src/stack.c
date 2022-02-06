//
// Created by nik on 1/31/22.
//

#include "stack.h"

inline void stack_realloc(Stack* stack) {
	if (stack->count == stack->capacity) {
		stack->capacity *= 2;
		stack->data = (void*) reallocarray(stack->data,
										   stack->capacity,
										   stack->size);
	}
}

inline void stack_new(Stack* stack, u64 size) {
	stack->capacity = 16;
	stack->size = size;
	stack->count = 0;
	stack->data = (void*) calloc(stack->capacity, stack->size);
	stack->base = stack->data;
}

inline void stack_push(Stack* stack, void* data) {
	// Before every push to the stack we verify that it has enough space.
	stack_realloc(stack);
	memcpy(stack->data + (stack->count++ * stack->size), data, stack->size);
}

inline bool stack_pop(Stack* stack, void* dest) {
	if (stack_is_empty(stack)) return false;
	// If we do not provide the destination of where to copy the popped element
	// we just decrement the "stack pointer".
	if (dest == NULL)  {
		stack->count--;
	} else {
		memcpy(dest, stack->data + (--stack->count * stack->size), stack->size);
	}
	return true;
}

inline void* stack_peek(Stack* stack) {
	if (stack_is_empty(stack)) return NULL;
	return stack->data + ((stack->count - 1) * stack->size);
}

inline bool stack_is_empty(Stack* stack) {
	return stack->count == 0;
}
