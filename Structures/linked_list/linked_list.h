#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Node Node;
typedef struct List List;

List *create_list(void);
void add_item(List **list, int val);
void remove_item(List **list, int index);
void clean_list(List **list);
void print_list(List *list);

#endif
