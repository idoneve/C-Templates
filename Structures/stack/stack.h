#ifndef STACK_H
#define STACK_H

typedef struct Stack Stack;

Stack *create_stack(void);
void push(Stack **stack, int val);
void pop(Stack **stack);
void clean_stack(Stack **stack);
void print_stack(Stack *stack);

#endif
