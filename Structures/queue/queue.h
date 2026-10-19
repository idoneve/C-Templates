#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue Queue;

Queue *create_queue(void);
void push_head(Queue **queue, int val);
void push_tail(Queue **queue, int val);
void pop_head(Queue **queue);
void pop_tail(Queue **queue);
void clean_queue(Queue **queue);
void print_queue(Queue *queue);

#endif
