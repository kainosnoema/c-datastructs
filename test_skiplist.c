#include <stdio.h>
#include "test.h"
#include "skiplist.h"

unsigned correct = 0;
unsigned totaltests = 0;

int main() {
  printf("Skiplist Autotester Running\n"
         "---------------------------\n");  
  
  skiplist_t *list = skiplist_new();
  
  skiplist_insert(list, 1, "value 1");
  skiplist_insert(list, 5, "value 5");
  skiplist_insert(list, 3, "value 3 a");
  skiplist_insert(list, 4, "value 4");
  skiplist_insert(list, 2, "value 2");
  
  char * value = (char *) skiplist_find(list, 1);
  printf("skiplist_find: %s\n", value);
  
  skiplist_remove(list, 2);
  skiplist_remove(list, 4);
  skiplist_insert(list, 2, "value 2");
  skiplist_insert(list, 4, "value 4");
  skiplist_insert(list, 3, "value 3 b");

  skiplist_remove(list, 3);

  skiplist_node_t *node;

  node = skiplist_head_node(list);
  printf("skiplist_node_next:\n");
  do {
    value = (char *) skiplist_node_get_value(node);
    printf("  %s\n", value);
  } while(node = skiplist_node_next(node));
  
  node = skiplist_tail_node(list);
  printf("skiplist_node_prev:\n");
  do {
    value = (char *) skiplist_node_get_value(node);
    printf("  %s\n", value);
  } while(node = skiplist_node_prev(node));
  
  skiplist_destroy(list);
  
  printf("---------------------------\n");  
  printf("You got %i/%i tests correct.\n", correct, totaltests);
}
