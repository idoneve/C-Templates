#include "queue.h"

#include <stdio.h>

int main(void) {
  Queue *queue = create_queue();

  for (int i = 0; i < 5; ++i) {
    print_queue(queue);
    push_head(&queue, i);
  }
  print_queue(queue);
  for (int i = 0; i < 5; ++i) {
    push_tail(&queue, i);
    print_queue(queue);
  }

  for (int i = 0; i < 5; ++i) {
    print_queue(queue);
    pop_tail(&queue);
  }
  print_queue(queue);
  for (int i = 0; i < 5; ++i) {
    pop_head(&queue);
    print_queue(queue);
  }

  clean_queue(&queue);

  return 0;
}
