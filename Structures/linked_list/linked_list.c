#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

struct Node {
  int val;
  Node *next;
};

struct List {
  Node *head;
  int length;
};

List *create_list(void) {
  List *list = malloc(sizeof(List));
  list->head = NULL;
  list->length = 0;

  return list;
}

void add_item(List **list, int val) {
  if ((*list)->length == 0) {
    (*list)->head = malloc(sizeof(Node));
    (*list)->head->val = val;
    (*list)->head->next = NULL;
  } else {
    Node *curr = (*list)->head;
    while (curr->next != NULL) {
      curr = curr->next;
    }
    curr->next = malloc(sizeof(Node));
    curr->next->val = val;
    curr->next->next = NULL;
  }
  ++(*list)->length;
}

void remove_item(List **list, int index) {
  if ((*list)->length < index) {
    return;
  } else if (index == 0) {
    Node *remove = (*list)->head;
    (*list)->head = (*list)->head->next;
    --(*list)->length;
    free(remove);
  } else {
    Node **curr = &(*list)->head;
    for (int i = 0; i < index - 1; ++i) {
      curr = &(*curr)->next;
    }

    Node *remove = (*curr)->next;
    (*curr)->next = (*curr)->next->next;
    --(*list)->length;
    free(remove);
  }
}

void clean_list(List **list) {
  while ((*list)->length > 0) {
    remove_item(list, 0);
  }
  free(*list);
  *list = NULL;
}

void print_list(List *list) {
  Node *curr = list->head;
  if (curr == NULL) {
    printf("empty\n");
    return;
  }

  while (curr->next != NULL) {
    printf("%d ", curr->val);
    curr = curr->next;
  }
  printf("%d\n", curr->val);
}
