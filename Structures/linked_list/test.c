#include "linked_list.h"
#include <stdio.h>

int main(void) {
  List *list = create_list();
  for (int i = 0; i < 10; ++i) {
    add_item(&list, i);
    print_list(list);
  }

  for (int i = 9; i >= 0; --i) {
    remove_item(&list, i);
    print_list(list);
  }

  clean_list(&list);

  return 0;
}
