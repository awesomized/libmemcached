/*
  Code to generate data to be pushed into memcached
*/

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

typedef struct pairs_st pairs_st;

struct pairs_st {
  char *key;
  size_t key_length;
  char *value;
  size_t value_length;
};

pairs_st *pairs_generate(unsigned long long number_of);
void pairs_free(pairs_st *pairs);

#endif
