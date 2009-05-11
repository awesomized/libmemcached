#include "common.h"
#include "libmemcached/memcached_pool.h"
#include <pthread.h>

struct memcached_pool_st 
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  memcached_st *master;
  memcached_st **mmc;
  int firstfree;
  int size;
  int current_size;
};

/**
 * Lock a the pthread mutex and handle error conditions. If we fail to
 * lock the mutex there must be something really wrong and we cannot continue.
 */
static void mutex_enter(pthread_mutex_t *mutex) 
{
  int ret;
  do 
    ret= pthread_mutex_lock(mutex);
  while (ret == -1 && errno == EINTR);

  if (ret == -1) 
  {
    /*
    ** This means something is seriously wrong (deadlock or an internal
    ** error in the posix library. Print out an error message and abort
    ** the program.
    */
    fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(errno));
    fflush(stderr);
    abort();
  }
}

/**
 * Unlock a the pthread mutex and handle error conditions. 
 * All errors except EINTR is fatal errors and will terminate the program
 * with a coredump.
 */
static void mutex_exit(pthread_mutex_t *mutex) {
  int ret;
  do
    ret = pthread_mutex_unlock(mutex);
  while (ret == -1 && errno == EINTR);

  if (ret == -1) 
  {
    /*
    ** This means something is seriously wrong (deadlock or an internal
    ** error in the posix library. Print out an error message and abort
    ** the program.
    */
    fprintf(stderr, "pthread_mutex_unlock %s\n", strerror(errno));
    fflush(stderr);
    abort();
  }
}

/**
 * Grow the connection pool by creating a connection structure and clone the
 * original memcached handle.
 */
static int grow_pool(memcached_pool_st* pool) {
  memcached_st *obj = calloc(1, sizeof(*obj));
  if (obj == NULL)
    return -1;

  if (memcached_clone(obj, pool->master) == NULL)
  {
    free(obj);
    return -1;
  }

  pool->mmc[++pool->firstfree] = obj;
  pool->current_size++;

  return 0;
}

memcached_pool_st *memcached_pool_create(memcached_st* mmc, int initial, int max) 
{
  memcached_pool_st* ret = NULL;
  memcached_pool_st object = { .mutex = PTHREAD_MUTEX_INITIALIZER, 
                               .cond = PTHREAD_COND_INITIALIZER,
                               .master = mmc,
                               .mmc = calloc(max, sizeof(memcached_st*)),
                               .firstfree = -1,
                               .size = max, 
                               .current_size = 0 };

  if (object.mmc != NULL) 
  {
    ret = calloc(1, sizeof(*ret));
    if (ret == NULL) 
    {
      free(object.mmc);
      return NULL;
    } 

    *ret = object;

    /* Try to create the initial size of the pool. An allocation failure at
     * this time is not fatal..
     */
    for (int ii=0; ii < initial; ++ii)
      if (grow_pool(ret) == -1)
        break;
  }

  return ret;
}

memcached_st*  memcached_pool_destroy(memcached_pool_st* pool) 
{
  memcached_st *ret = pool->master;

  for (int ii = 0; ii <= pool->firstfree; ++ii) 
  {
    memcached_free(pool->mmc[ii]);
    free(pool->mmc[ii]);
    pool->mmc[ii] = NULL;
  }
  
  pthread_mutex_destroy(&pool->mutex);
  pthread_cond_destroy(&pool->cond);
  free(pool->mmc);
  free(pool);

  return ret;
}

memcached_st* memcached_pool_pop(memcached_pool_st* pool, bool block) {
  memcached_st *ret = NULL;
  mutex_enter(&pool->mutex);
  do 
  {
    if (pool->firstfree > -1)
      ret = pool->mmc[pool->firstfree--];
    else if (pool->current_size == pool->size) 
    {
      if (!block) 
      {
        mutex_exit(&pool->mutex);
        return NULL;
      }
      
      if (pthread_cond_wait(&pool->cond, &pool->mutex) == -1) 
      {
        /*
        ** This means something is seriously wrong (an internal error in the
        ** posix library. Print out an error message and abort the program.
        */
        fprintf(stderr, "pthread cond_wait %s\n", strerror(errno));
        fflush(stderr);
        abort();
      }
    } 
    else if (grow_pool(pool) == -1) 
    {
      mutex_exit(&pool->mutex);
      return NULL;
    }
  } 
  while (ret == NULL);

  mutex_exit(&pool->mutex);
  return ret;
}

void memcached_pool_push(memcached_pool_st* pool, memcached_st *mmc) 
{
  mutex_enter(&pool->mutex);
  pool->mmc[++pool->firstfree] = mmc;

  if (pool->firstfree == 0 && pool->current_size == pool->size) 
  {
    /* we might have people waiting for a connection.. wake them up :-) */
    pthread_cond_broadcast(&pool->cond);
  }
  mutex_exit(&pool->mutex);
}
