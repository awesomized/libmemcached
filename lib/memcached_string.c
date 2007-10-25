#include "common.h"

memcached_return memcached_string_check(memcached_string_st *string, size_t need)
{
  if (need > (size_t)(string->current_size - (size_t)(string->end - string->string)))
  {
    size_t current_offset= string->end - string->string;
    char *new_value;
    size_t adjust= (need - (size_t)(string->current_size - (size_t)(string->end - string->string))) / string->block_size;

    adjust++;

    new_value= (char *)realloc(string->string, 
                               sizeof(char) * ((adjust * string->block_size) + string->current_size));

    if (new_value == NULL)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

    string->string= new_value;
    string->end= string->string + current_offset;

    string->current_size+= (string->block_size * adjust);

    /* We zero the block structure we just realloced */
    memset((string->string + current_offset), 0, 
           sizeof(char) * string->block_size);
  }

  return MEMCACHED_SUCCESS;
}

memcached_string_st *memcached_string_create(memcached_st *ptr, size_t initial_size)
{
  memcached_return rc;
  memcached_string_st *string;

  /* Saving malloc calls :) */
  string= (memcached_string_st *)malloc(sizeof(memcached_string_st));
  if (!string)
    return NULL;
  string->end= string->string;
  memset(string, 0, sizeof(memcached_string_st));
  string->block_size= initial_size;

  rc=  memcached_string_check(string, initial_size);
  if (rc != MEMCACHED_SUCCESS)
  {
    free(string);
    return NULL;
  }

  WATCHPOINT_ASSERT(string->string == string->end);

  return string;
}

memcached_return memcached_string_append_character(memcached_st *ptr, 
                                                   memcached_string_st *string, 
                                                   char character)
{
  memcached_return rc;

  rc=  memcached_string_check(string, 1);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  *string->end= ' ';
  string->end++;

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_string_append(memcached_st *ptr, memcached_string_st *string,
                                         char *value, size_t length)
{
  memcached_return rc;

  rc= memcached_string_check(string, length);

  if (rc != MEMCACHED_SUCCESS)
    return rc;
  
  WATCHPOINT_ASSERT(string->string);
  WATCHPOINT_ASSERT(string->end >= string->string && string->end <= string->string + string->current_size);

  memcpy(string->end, value, length);
  string->end+= length;

  return MEMCACHED_SUCCESS;
}

size_t memcached_string_backspace(memcached_st *ptr, memcached_string_st *string, size_t remove)
{
  if (string->end - string->string  > remove)
  {
    size_t difference;

    difference= string->end - string->string;
    string->end= string->string;

    return difference;
  }
  string->end-= remove;

  return remove;
}

memcached_return memcached_string_reset(memcached_st *ptr, memcached_string_st *string)
{
  string->end= string->string;
  
  return MEMCACHED_SUCCESS;
}

void memcached_string_free(memcached_st *ptr, memcached_string_st *string)
{
  free(string->string);
  free(string);
}
