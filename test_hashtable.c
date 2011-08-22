#include <stdio.h>
#include "test.h"
#include "hashtable.h"

unsigned correct = 0;
unsigned totaltests = 0;

void test_hashtable();
void test_hashtable_uint();

int main() {
  printf("Hashtable Autotester Running\n"
         "---------------------------\n");
  
  printf("\n== hashtable ==\n");
  
  test_hashtable();
  
  printf("\n== hashtable_uint ==\n");
  
  test_hashtable_uint();
  
  printf("\n");
  
  printf("---------------------------\n");
  printf("You got %i/%i tests correct.\n", correct, totaltests);
}

void test_hashtable()
{
  hashtable_t *hash = hashtable_new();
  
  int len = 2;
  char *(values[2]) = {"1234567890", "0987654321"};
  char *(keys[2]) = {"some_long_key", "another_key"};

  int i;
  for(i=0; i<len; i++) {
    totaltests++;
    hashtable_set(hash, keys[i], values[i]);
  }
  
  int c = hashtable_count(hash);
  totaltests++;
  if(c == 2) {
    correct++;
    printf_pass("count: %i \n", c);
  } else {
    printf_fail("count: %i (should be 2) \n", c);
  }

  for(i=0; i<len; i++) {
    char * value = (char *) hashtable_get(hash, keys[i]);
    if(value == values[i]) {
      correct++;
      printf_pass("%s => \"%s\"\n", keys[i], value);
    } else
      printf_fail("%s => \"%s\" (should be \"%s\")\n", keys[i], value, values[i]);
  }
  
  totaltests++;
  hashtable_unset(hash, keys[0]);
  char * value = (char *) hashtable_get(hash, keys[0]);
  if(value == NULL) {
    correct++;
    printf_pass("(unset) %s => \"%s\"\n", keys[0], value);
  } else
    printf_fail("(unset) %s => \"%s\" (should be \"%s\")\n", keys[0], value, NULL);
  
  hashtable_destroy(hash);
}

void test_hashtable_uint()
{
  hashtable_t *hash = hashtable_uint_new();
  
  int len = 2;
  char *(values[2]) = {"1234567890", "0987654321"};
  uint32_t keys[2] = {0, 1};
  
  int i;
  for(i=0; i<len; i++) {
    totaltests++;
    hashtable_uint_set(hash, keys[i], values[i]);
  }
  
  int c = hashtable_count(hash);
  totaltests++;
  if(c == 2) {
    correct++;
    printf_pass("count: %i \n", c);
  } else {
    printf_fail("count: %i (should be 2) \n", c);
  }
  
  for(i=0; i<len; i++) {
    char * value = (char *) hashtable_uint_get(hash, keys[i]);
    if(value == values[i]) {
      correct++;
      printf_pass("%i => \"%s\"\n", keys[i], value);
    } else
      printf_fail("%i => \"%s\" (should be \"%s\")\n", keys[i], value, values[i]);
  }
  
  totaltests++;
  hashtable_uint_unset(hash, keys[0]);
  char * value = (char *) hashtable_uint_get(hash, keys[0]);
  if(value == NULL) {
    correct++;
    printf_pass("(unset) %i => \"%s\"\n", keys[0], value);
  } else
    printf_fail("(unset) %i => \"%s\" (should be \"%s\")\n", keys[0], value, NULL);
  
  hashtable_destroy(hash);
}
