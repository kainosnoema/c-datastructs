#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef int8_t skiplist_height_t;
typedef uint32_t skiplist_length_t;

enum { SKIPLIST_HEIGHT_MAX = 15 };

typedef struct skiplist_t skiplist_t;
typedef struct skiplist_node_t skiplist_node_t;

struct skiplist_t {
  skiplist_node_t *head; // first of top
  skiplist_node_t *tail; // last of bottom
  skiplist_node_t *path[SKIPLIST_HEIGHT_MAX]; // tmp array for update path
  
  skiplist_height_t height;
  skiplist_length_t length;
};

struct skiplist_node_t {
  uint32_t key;
  void * value;
  
  skiplist_height_t height;
  skiplist_node_t *prev[SKIPLIST_HEIGHT_MAX]; // left neighbors
  skiplist_node_t *next[SKIPLIST_HEIGHT_MAX]; // right neighbors
};

static skiplist_node_t *skiplist_node_new(uint32_t key, void * value);

inline skiplist_node_t *skiplist_remove_node_range(skiplist_t *list,
                                                   skiplist_node_t *start,
                                                   skiplist_node_t *end);
                                                   
static skiplist_node_t *skiplist_path_find(skiplist_t *list, uint32_t key);

static skiplist_height_t skiplist_height_random();

skiplist_t *skiplist_new() {
  skiplist_t *list = malloc(sizeof(skiplist_t));
  if(list == NULL) {
    return NULL;
  }
  
  list->head = skiplist_node_new(0, NULL);
  list->tail = skiplist_node_new(0, NULL);
  
  list->head->next[0] = list->tail;
  list->tail->prev[0] = list->head;
  
  list->length = 0;
  list->height = 1;
  
  return list;
}

void skiplist_destroy(skiplist_t *list) {
  skiplist_node_t *node = list->head;
  skiplist_node_t *next;
  
  do {
    next = node->next[0];
    free(node);
  } while(node = next);
  
  free(list);
}

skiplist_node_t *skiplist_insert(skiplist_t *list, uint32_t key, void * value)
{
  skiplist_path_find(list, key);
  
  skiplist_node_t *new = skiplist_node_new(key, value);
  if(new == NULL) {
    return NULL;
  }
  
  // grow list height if needed
  int h;
  if (new->height > list->height ) {
    h = list->height;
    list->path[h] = list->head;
    list->head->next[h] = list->tail;
    list->tail->prev[h] = list->head;
    new->height = ++h;
  }
  
  h = new->height;
  while(--h >= 0) {
    new->next[h] = list->path[h]->next[h];
    list->path[h]->next[h] = new;
    new->prev[h] = list->path[h];
    new->next[h]->prev[h] = new;
  }
  
  list->length++;
  
  return new;
}

skiplist_node_t *skiplist_find_node(skiplist_t *list, uint32_t key)
{
  skiplist_node_t *result;

  result = skiplist_path_find(list, key)->next[0];
  if(result != list->tail) {
    return result; 
  }
  
  return NULL;
}

void *skiplist_find(skiplist_t *list, uint32_t key)
{
  skiplist_node_t *node = skiplist_find_node(list, key);
  if(node == NULL)
    return NULL;

  return node->value;
}

int skiplist_remove(skiplist_t *list, uint32_t key)
{
  skiplist_node_t *start = skiplist_find_node(list, key);
  skiplist_node_t *end = start;
  
  if(start == NULL) {
    return 0;
  }
  
  while(key == end->next[0]->key) {
    end = end->next[0];
  }
  
  skiplist_remove_node_range(list, start, end);
}

skiplist_node_t *skiplist_remove_node_range(skiplist_t *list,
                                            skiplist_node_t *start,
                                            skiplist_node_t *end)
{
  // reconnect links on either side of removed nodes
  int h = list->height;
  while(--h >= 0) {
    end->next[h]->prev[h] = start->prev[h];
    start->prev[h]->next[h] = end->next[h];
  }
  
  // shorten the height of the list if needed
  while(list->height > 0) {
    if(list->head->next[list->height - 1] != NULL)
      break;
    
    h = --list->height;
    list->head->next[h] = NULL;
    list->tail->prev[h] = NULL;
  }
  
  skiplist_node_t *next;
  while(next != end && (next = start)) {
    start = next->next[0];
    list->length--;
    free(next);
  }
}

skiplist_node_t *skiplist_remove_node(skiplist_t *list,
                                      skiplist_node_t *node)
{
  skiplist_remove_node_range(list, node, node);
}

skiplist_node_t *skiplist_head_node(skiplist_t *list)
{
  return list->head;
}

skiplist_node_t *skiplist_tail_node(skiplist_t *list)
{
  return list->tail;
}

skiplist_node_t *skiplist_node_next(skiplist_node_t *node)
{
  return node->next[0];
}

skiplist_node_t *skiplist_node_prev(skiplist_node_t *node)
{
  return node->prev[0];
}

uint32_t skiplist_node_get_key(skiplist_node_t *node)
{
  return node->key;
}

void *skiplist_node_get_value(skiplist_node_t *node)
{
  return node->value;
}

static skiplist_node_t *skiplist_path_find(skiplist_t *list, uint32_t key)
{
  skiplist_node_t *result = list->head;
  
  int i;
  for(i = list->height - 1; i >= 0; i--) {
    while(result->next[i] != list->tail && key > result->next[i]->key)
      result = result->next[i];
    list->path[i] = result;
  }
  
  return result;
}

static skiplist_node_t *skiplist_node_new(uint32_t key, void * value)
{
  skiplist_node_t *node;
  
  node = malloc(sizeof(skiplist_node_t));
  if(node == NULL) {
    return NULL;
  }
  
  node->key = key;
  node->value = value;
  node->height = skiplist_height_random();
  return node;
}

static skiplist_height_t skiplist_height_random()
{
  skiplist_height_t height = 0;
  static int bits = 0;
  static int reset = 0;
  int found = 0;
   
  while(!found) {
    if ( reset == 0 ) {
      bits = rand();
      reset = sizeof(int) * CHAR_BIT;
    }
    
    found = bits & 1;
    bits = bits >> 1;
    
    --reset;
    ++height;
  }
  
  if(height < SKIPLIST_HEIGHT_MAX)
    return height;
    
  return SKIPLIST_HEIGHT_MAX;
}

#endif // _SKIPLIST_H_