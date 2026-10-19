#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

struct Queue {
  int length;
  int *vals;
};

Queue *create_queue(void) {
  Queue *queue = malloc(sizeof(Queue));
  queue->length = 0;
  queue->vals = NULL;

  return queue;
}

void push_head(Queue **queue, int val) {
  int *new_array =
      malloc(sizeof(int) * ((long unsigned int)(*queue)->length + 1));
  new_array[0] = val;
  for (int i = 1; i <= (*queue)->length; ++i) {
    new_array[i] = (*queue)->vals[i - 1];
  }

  free((*queue)->vals);
  (*queue)->vals = NULL;

  (*queue)->vals = new_array;
  ++(*queue)->length;
}

void push_tail(Queue **queue, int val) {
  int *new_array =
      malloc(sizeof(int) * ((long unsigned int)(*queue)->length + 1));
  for (int i = 0; i < (*queue)->length; ++i) {
    new_array[i] = (*queue)->vals[i];
  }
  new_array[(*queue)->length] = val;

  free((*queue)->vals);
  (*queue)->vals = NULL;

  (*queue)->vals = new_array;
  ++(*queue)->length;
}

void pop_head(Queue **queue) {
  if ((*queue)->length == 0) {
    return;
  } else if ((*queue)->length == 1) {
    free((*queue)->vals);
    (*queue)->vals = NULL;
    (*queue)->length = 0;

    return;
  }

  int *new_array =
      malloc(sizeof(int) * ((long unsigned int)(*queue)->length - 1));
  for (int i = 1; i < (*queue)->length; ++i) {
    new_array[i - 1] = (*queue)->vals[i];
  }

  free((*queue)->vals);
  (*queue)->vals = NULL;

  (*queue)->vals = new_array;
  --(*queue)->length;
}

void pop_tail(Queue **queue) {
  if ((*queue)->length == 0) {
    return;
  } else if ((*queue)->length == 1) {
    free((*queue)->vals);
    (*queue)->vals = NULL;
    (*queue)->length = 0;

    return;
  }

  int *new_array =
      malloc(sizeof(int) * ((long unsigned int)(*queue)->length - 1));
  for (int i = 0; i < (*queue)->length - 1; ++i) {
    new_array[i] = (*queue)->vals[i];
  }

  free((*queue)->vals);
  (*queue)->vals = NULL;

  (*queue)->vals = new_array;
  --(*queue)->length;
}

void clean_queue(Queue **queue) {
  while ((*queue)->length > 0) {
    pop_head(queue);
  }

  free(*queue);
  *queue = NULL;
}

void print_queue(Queue *queue) {
  if (queue->length == 0) {
    printf("empty\n");
  } else {
    for (int i = 0; i < queue->length; ++i) {
      printf("%d ", queue->vals[i]);
    }
    printf("\n");
  }
}
