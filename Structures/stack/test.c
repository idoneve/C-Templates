#include <stdio.h>

#include "stack.h"

int main(void) {
  Stack *stack = create_stack();

  for (int i = 0; i < 10; ++i) {
    push(&stack, i);
    print_stack(stack);
  }

  for (int i = 0; i < 10; ++i) {
    pop(&stack);
    print_stack(stack);
  }

  clean_stack(&stack);

  return 0;
}
