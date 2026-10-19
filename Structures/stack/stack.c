#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

struct Stack {
  int length;
  int *vals;
};

Stack *create_stack(void) {
  Stack *stack = malloc(sizeof(Stack));
  stack->length = 0;
  stack->vals = NULL;

  return stack;
}

void push(Stack **stack, int val) {
  int *new_array =
      malloc(sizeof(int) * ((long unsigned int)(*stack)->length + 1));
  for (int i = 0; i < (*stack)->length; ++i) {
    new_array[i] = (*stack)->vals[i];
  }
  new_array[(*stack)->length] = val;

  free((*stack)->vals);
  (*stack)->vals = NULL;

  (*stack)->vals = new_array;
  ++(*stack)->length;
}

void pop(Stack **stack) {
  if ((*stack)->length == 0) {
    return;
  } else if ((*stack)->length == 1) {
    free((*stack)->vals);
    (*stack)->vals = NULL;
    (*stack)->length = 0;

    return;
  }

  int *new_array =
      malloc(sizeof(int) * ((long unsigned int)(*stack)->length - 1));
  for (int i = 0; i < (*stack)->length - 1; ++i) {
    new_array[i] = (*stack)->vals[i];
  }

  free((*stack)->vals);
  (*stack)->vals = NULL;

  (*stack)->vals = new_array;
  --(*stack)->length;
}

void clean_stack(Stack **stack) {
  while ((*stack)->length > 0) {
    pop(stack);
  }

  free(*stack);
  *stack = NULL;
}

void print_stack(Stack *stack) {
  if (stack->length == 0) {
    printf("empty\n");
  } else {
    for (int i = 0; i < stack->length; ++i) {
      printf("%d ", stack->vals[i]);
    }
    printf("\n");
  }
}
