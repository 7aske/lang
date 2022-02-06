//
// Created by nik on 1/31/22.
//

#ifndef LANG_STACK_H
#define LANG_STACK_H

#include "stdtypes.h"
#include <stdlib.h>
#include <string.h>

/**
 * Simple generic stack struct.
 *
 * @param capacity Max capacity.
 * @param count    Number of elements.
 * @param size     Size of the individual element in bytes.
 * @param data     Pointer to the heap-allocated data array.
 */
typedef struct stack  {
	u64 capacity;
	u64 count;
	u64 size;
	void* data;
	void* base;
} Stack;

#define stack_foreach(__stack, __Type, code) { \
for(int _i = ((__stack)->count - 1); _i >= 0; --_i) { \
    __Type it = (__Type) ((__stack)->base + (_i * (__stack)->size));\
    code\
}}

/**
 * Populates the stack allocated at with required data specified by size.
 *
 * @param stack Pointer to the stack struct(heap/stack allocated).
 * @param size  Size of the each element stack is supposed to hold in bytes.
 */
void  stack_new(Stack* stack, u64 size);

/**
 * Pushes a single element onto the stack.
 *
 * @param stack this
 * @param data  Element to be pushed to the stack.
 */
void  stack_push(Stack* stack, void* data);

/**
 * Copies one element from the stack to the pointer specified by dest
 * decrementing the stack size.
 *
 * @param stack this
 * @param dest  Location of where to copy the element from the stack.
 * @return      True if the stack was not empty before calling.
 */
bool  stack_pop(Stack* stack, void* dest);

/**
 * Looks up the top-most element from the stack.
 *
 * @param stack this
 * @return Returns the pointer to the top-most element of the stack.
 */
void* stack_peek(Stack* stack);

/**
 * Checks whether the stack is empty.
 *
 * @param stack this
 * @return True if the stack size is zero.
 */
bool  stack_is_empty(Stack* stack);

/**
 * Helper method to resize the stack if it is full.
 *
 * @param stack this
 */
void  stack_realloc(Stack* stack);

#endif //LANG_STACK_H
