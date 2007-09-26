#include <memcached.h>

memcached_return memcached_string_check(memcached_string_st *string, size_t need)
{
  if (need > (string->current_size - (string->end - string->string)))
  {
    size_t current_offset= string->end - string->string;
    char *new_value;

    new_value= (char *)realloc(string->string, sizeof(char) * string->block_size);

    if (!new_value)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

    string->string= new_value;
    string->end= string->string + current_offset;

    string->current_size+= string->block_size;

    /* We zero the block structure we just realloced */
    memset((string + string->current_size) - string->block_size , 0, 
           sizeof(char) * string->block_size);
  }

  return MEMCACHED_SUCCESS;
}

memcached_string_st *memcached_string_init(memcached_st *ptr, size_t initial_size)
{
  memcached_string_st *string;

  /* Saving malloc calls :) */
  string= (memcached_string_st *)malloc(sizeof(memcached_string_st) + (sizeof(char) * initial_size));
  if (!string)
    return NULL;
  memset(string, 0, sizeof(memcached_string_st));
  string->block_size= initial_size;
  memcached_string_check(string, initial_size);

  return string;
}

memcached_return memcached_string_append_character(memcached_st *ptr, 
                                                   memcached_string_st *string, 
                                                   char character)
{
  memcached_string_check(string, 1);
  *string->end= ' ';
  string->end++;

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_string_append(memcached_st *ptr, memcached_string_st *string,
                                         char *value, size_t length)
{
  memcached_string_check(string, length);
  memcpy(string->end, value, length);
  string->end+= length;

  return MEMCACHED_SUCCESS;
}

#ifdef CRAP
size_t memcached_string_length(memcached_st *ptr, memcached_string_st *string)
{
  return (size_t)(string->end - string->string);
}

size_t memcached_string_size(memcached_st *ptr, memcached_string_st *string)
{
  return string->current_size;
}
#endif

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
