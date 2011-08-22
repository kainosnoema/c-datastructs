#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

enum { HASHTABLE_BUCKET_COUNT = 256 };

typedef struct hashtable_pair_t hashtable_pair_t;
typedef struct hashtable_bucket_t hashtable_bucket_t;
typedef struct hashtable_t hashtable_t;

struct hashtable_pair_t {
  void *key;
  void *value;
  hashtable_pair_t *next;
};

struct hashtable_bucket_t {
  int pair_count;
  hashtable_pair_t *pairs;
};

struct hashtable_t {
  int keys_are_uint;
  int pair_count;
  int bucket_count;
  hashtable_bucket_t *buckets;
};

static hashtable_pair_t * bucket_get_pair(hashtable_bucket_t *bucket, const void *key, int key_is_uint);

static int bucket_add_pair(hashtable_bucket_t *bucket, const void *key, const void *value, int key_is_uint);

static hashtable_pair_t * bucket_destroy_pair(hashtable_bucket_t *bucket, const void *key, int key_is_uint);

static unsigned long hash(const void *key, int key_is_uint);

hashtable_t * hashtable_new()
{
  hashtable_t *table;
  
  table = malloc(sizeof(hashtable_t));
  if (table == NULL) {
    return NULL;
  }
  table->keys_are_uint = 0;
  table->bucket_count = HASHTABLE_BUCKET_COUNT;
  table->buckets = malloc(table->bucket_count * sizeof(hashtable_bucket_t));
  if (table->buckets == NULL) {
    free(table);
    return NULL;
  }
  memset(table->buckets, 0, table->bucket_count * sizeof(hashtable_bucket_t));
  return table;
}

hashtable_t * hashtable_uint_new()
{
  hashtable_t *table = hashtable_new();
  table->keys_are_uint = 1;
  return table;
}

void hashtable_destroy(hashtable_t *table)
{
  unsigned int i, n;
  hashtable_bucket_t *bucket;
  hashtable_pair_t *pair;

  if (table == NULL) {
    return;
  }
  
  n = table->bucket_count;
  bucket = table->buckets;
  i = 0;
  while (i < n) {
    pair = bucket->pairs;
    hashtable_pair_t *next;
    while((pair != NULL) && (next = pair)) {
      pair = next->next;
      free(next);
    };
    bucket++;
    i++;
  }
  free(table->buckets);
  free(table);
}

void *hashtable_get(const hashtable_t *table, const char *key)
{
  int key_is_uint = table->keys_are_uint;
  unsigned int index;
  hashtable_bucket_t *bucket;
  hashtable_pair_t *pair;

  if (table == NULL) {
    return NULL;
  }

  if(!key_is_uint && key == NULL) {
    return NULL;
  }

  index = hash(key, key_is_uint) % table->bucket_count;
  bucket = &(table->buckets[index]);
  pair = bucket_get_pair(bucket, key, key_is_uint);
  
  if(pair == NULL) {
    return NULL;
  }

  return pair->value;
}

void *hashtable_uint_get(const hashtable_t *table, const uint32_t key)
{
  if (table == NULL) {
    return NULL;
  }

  if(!table->keys_are_uint) {
    return NULL;
  }

  return hashtable_get(table, (void *) key);
}


int hashtable_exists(const hashtable_t *table, const char *key)
{
  int key_is_uint = table->keys_are_uint;
  unsigned int index;
  hashtable_bucket_t *bucket;
  hashtable_pair_t *pair;

  if (table == NULL) {
    return 0;
  }

  if(!table->keys_are_uint && key == NULL) {
    return 0;
  }
  
  index = hash(key, key_is_uint) % table->bucket_count;
  bucket = &(table->buckets[index]);
  pair = bucket_get_pair(bucket, key, key_is_uint);
  if (pair == NULL) {
    return 0;
  }
  
  return 1;
}

int hashtable_uint_exists(const hashtable_t *table, const uint32_t key)
{
  if (table == NULL) {
    return 0;
  }

  if(!table->keys_are_uint) {
    return 0;
  }

  return hashtable_exists(table, (void *) key);
}

int hashtable_set(const hashtable_t *table, const char *key, const void *value)
{
  int key_is_uint = table->keys_are_uint;
  unsigned int index;
  hashtable_bucket_t *bucket;
  hashtable_pair_t *pair;

  if (table == NULL || value == NULL) {
    return 0;
  }
  
  if(!table->keys_are_uint && key == NULL) {
    return 0;
  }
  
  index = hash(key, key_is_uint) % table->bucket_count;
  bucket = &(table->buckets[index]);
  pair = bucket_get_pair(bucket, key, key_is_uint);
  if (pair != NULL) {
    pair->value = (void *) value;
    return 1;
  }
  
  return bucket_add_pair(bucket, key, value, key_is_uint);
}

int hashtable_uint_set(const hashtable_t *table, const uint32_t key, const void *value)
{
  if (table == NULL) {
    return 0;
  }
  
  if(!table->keys_are_uint) {
    return 0;
  }
  
  return hashtable_set(table, (void *) key, value);
}

void *hashtable_unset(const hashtable_t *table, const char *key)
{
  int key_is_uint = table->keys_are_uint;
  unsigned int index;
  hashtable_bucket_t *bucket;
  hashtable_pair_t *pair;

  if (table == NULL) {
    return 0;
  }
  
  if(!key_is_uint && key == NULL) {
    return 0;
  }

  index = hash(key, key_is_uint) % table->bucket_count;
  bucket = &(table->buckets[index]);
  pair = bucket_destroy_pair(bucket, key, key_is_uint);
  if (pair == NULL) {
    return NULL;
  }
  
  return pair->value;
}

void *hashtable_uint_unset(const hashtable_t *table, const uint32_t key)
{
  if (table == NULL) {
    return 0;
  }
  
  if(!table->keys_are_uint) {
    return 0;
  }
  
  return hashtable_unset(table, (void *) key);
}

int hashtable_count(const hashtable_t *table)
{
  unsigned int i, j, n, m;
  unsigned int count;
  hashtable_bucket_t *bucket;
  hashtable_pair_t *pair;

  if (table == NULL) {
    return 0;
  }
  
  bucket = table->buckets;
  n = table->bucket_count;
  i = 0;
  count = 0;
  while (i < n) {
    pair = bucket->pairs;
    m = bucket->pair_count;
    j = 0;
    while (j < m) {
      count++;
      pair++;
      j++;
    }
    bucket++;
    i++;
  }
  return count;
}

static int pair_has_key(hashtable_pair_t *pair, const void *key, int key_is_uint)
{
  if(pair == NULL) {
    return 0;
  }
  
  if(!key_is_uint && pair->key == NULL) {
    return 0;
  }
  
  if (key_is_uint && (pair->key == key)) {
    return 1;
  } else if((strcmp((char *) pair->key, key) == 0)) {
    return 1;
  }
  
  return 0;
}

static int bucket_add_pair(hashtable_bucket_t *bucket, const void *key, const void *value, int key_is_uint)
{
  hashtable_pair_t * pair = malloc(sizeof(hashtable_pair_t));
  if (pair == NULL) {
    return 0;
  }
  
  pair->key = (void *) key;
  pair->value = (void *) value;
  
  pair->next = bucket->pairs;
  bucket->pairs = pair;
  
  bucket->pair_count++;

  return 1;
}

/*
 * Returns a pair matching the provided char or uint key,
 * or null if no pair exists in the bucket.
 */
static hashtable_pair_t * bucket_get_pair(hashtable_bucket_t *bucket, const void *key, int key_is_uint)
{
  if (bucket->pair_count == 0) {
    return NULL;
  }
  
  hashtable_pair_t *pair = bucket->pairs;
  do {
    if (pair_has_key(pair, key, key_is_uint)) {
      return pair;
    }
  } while(pair = pair->next);
  
  return NULL;
}

static hashtable_pair_t * bucket_destroy_pair(hashtable_bucket_t *bucket, const void *key, int key_is_uint)
{
  if (bucket->pair_count == 0) {
    return 0;
  }
  
  hashtable_pair_t *prev = NULL;
  hashtable_pair_t *pair = bucket->pairs;
  do {
    if (pair_has_key(pair, key, key_is_uint)) {
      void *value = pair->value;
      if(prev == NULL) {
        bucket->pairs = pair->next;
      } else {
        prev->next = pair->next;
      }
      free(pair);
      bucket->pair_count--;
      return value;
    }
  } while((prev = pair) && (pair = pair->next));
  
  return NULL;
}

/*
 * Returns a hash code for the provided char or uint.
 */
static unsigned long hash(const void *key, int key_is_uint)
{
  if(key_is_uint) {
    unsigned long hash = (unsigned long) key;

    hash = (hash ^ 61) ^ (hash >> 16);
    hash = hash + (hash << 3);
    hash = hash ^ (hash >> 4);
    hash = hash * 0x27d4eb2d;
    hash = hash ^ (hash >> 15);

    return hash;
  } else {
    unsigned long hash = 5381;

    char *str = (char *) key;
    int c;
    while (c = *str++) {
      hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
  }
}

#endif