/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2010 Brian Aker All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <libmemcached/common.h>
#include <libmemcached/memcached_util.h>

#include <cassert>
#include <cerrno>
#include <pthread.h>
#include <memory>

static bool grow_pool(memcached_pool_st* pool);

struct memcached_pool_st
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  memcached_st *master;
  memcached_st **server_pool;
  int firstfree;
  const uint32_t size;
  uint32_t current_size;
  bool _owns_master;

  memcached_pool_st(memcached_st *master_arg, size_t max_arg) :
    master(master_arg),
    server_pool(NULL),
    firstfree(-1),
    size(max_arg),
    current_size(0),
    _owns_master(false)
  {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
  }

  bool init(uint32_t initial)
  {
    server_pool= new (std::nothrow) memcached_st *[size];
    if (not server_pool)
      return false;

    /*
      Try to create the initial size of the pool. An allocation failure at
      this time is not fatal..
    */
    for (unsigned int x= 0; x < initial; ++x)
    {
      if (not grow_pool(this))
        break;
    }

    return true;
  }

  ~memcached_pool_st()
  {
    for (int x= 0; x <= firstfree; ++x)
    {
      memcached_free(server_pool[x]);
      server_pool[x] = NULL;
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    delete [] server_pool;
    if (_owns_master)
    {
      memcached_free(master);
    }
  }

  void increment_version()
  {
    ++master->configure.version;
  }

  bool compare_version(const memcached_st *arg) const
  {
    return (arg->configure.version == version());
  }

  int32_t version() const
  {
    return master->configure.version;
  }
};

static memcached_return_t mutex_enter(pthread_mutex_t *mutex)
{
  int ret;
  do
  {
    ret= pthread_mutex_lock(mutex);
  } while (ret == -1 && errno == EINTR);

  return (ret == -1) ? MEMCACHED_ERRNO : MEMCACHED_SUCCESS;
}

static memcached_return_t mutex_exit(pthread_mutex_t *mutex)
{
  int ret;
  do
  {
    ret= pthread_mutex_unlock(mutex);
  } while (ret == -1 && errno == EINTR);

  return (ret == -1) ? MEMCACHED_ERRNO : MEMCACHED_SUCCESS;
}

/**
 * Grow the connection pool by creating a connection structure and clone the
 * original memcached handle.
 */
static bool grow_pool(memcached_pool_st* pool)
{
  memcached_st *obj;
  if (not (obj= memcached_clone(NULL, pool->master)))
  {
    return false;
  }

  pool->server_pool[++pool->firstfree]= obj;
  pool->current_size++;
  obj->configure.version= pool->version();

  return true;
}

static inline memcached_pool_st *_pool_create(memcached_st* master, uint32_t initial, uint32_t max)
{
  if (! initial || ! max || initial > max)
  {
    errno= EINVAL;
    return NULL;
  }

  memcached_pool_st *object= new (std::nothrow) memcached_pool_st(master, max);
  if (not object)
  {
    errno= ENOMEM; // Set this for the failed calloc
    return NULL;
  }

  /*
    Try to create the initial size of the pool. An allocation failure at
    this time is not fatal..
  */
  if (not object->init(initial))
  {
    delete object;
    return NULL;
  }

  return object;
}

memcached_pool_st *memcached_pool_create(memcached_st* master, uint32_t initial, uint32_t max)
{
  return _pool_create(master, initial, max);
}

memcached_pool_st * memcached_pool(const char *option_string, size_t option_string_length)
{
  memcached_st *memc= memcached(option_string, option_string_length);

  if (not memc)
    return NULL;

  memcached_pool_st *self;
  self= memcached_pool_create(memc, memc->configure.initial_pool_size, memc->configure.max_pool_size);
  if (not self)
  {
    memcached_free(memc);
    errno= ENOMEM;
    return NULL;
  }
  errno= 0;

  self->_owns_master= true;

  return self;
}

memcached_st*  memcached_pool_destroy(memcached_pool_st* pool)
{
  if (not pool)
    return NULL;

  // Legacy that we return the original structure
  memcached_st *ret= NULL;
  if (pool->_owns_master)
  { }
  else
  {
    ret= pool->master;
  }

  delete pool;

  return ret;
}

memcached_st* memcached_pool_pop(memcached_pool_st* pool,
                                 bool block,
                                 memcached_return_t *rc)
{
  assert(pool);
  assert(rc);
  if (not pool ||  not rc)
  {
    errno= EINVAL;
    return NULL;
  }

  if ((*rc= mutex_enter(&pool->mutex)) != MEMCACHED_SUCCESS)
  {
    return NULL;
  }

  memcached_st *ret= NULL;
  do
  {
    if (pool->firstfree > -1)
    {
      ret= pool->server_pool[pool->firstfree--];
    }
    else if (pool->current_size == pool->size)
    {
      if (not block)
      {
        *rc= mutex_exit(&pool->mutex); // this should be a different error
        return NULL;
      }

      if (pthread_cond_wait(&pool->cond, &pool->mutex) == -1)
      {
        int err= errno;
        mutex_exit(&pool->mutex);
        errno= err;
        *rc= MEMCACHED_ERRNO;
        return NULL;
      }
    }
    else if (not grow_pool(pool))
    {
      (void)mutex_exit(&pool->mutex);
      *rc= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
      return NULL;
    }
  }
  while (ret == NULL);

  *rc= mutex_exit(&pool->mutex);

  return ret;
}

memcached_return_t memcached_pool_push(memcached_pool_st* pool, memcached_st *released)
{
  if (not pool)
    return MEMCACHED_INVALID_ARGUMENTS;

  memcached_return_t rc= mutex_enter(&pool->mutex);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  /* Someone updated the behavior on the object.. */
  if (not pool->compare_version(released))
  {
    memcached_free(released);
    if (not (released= memcached_clone(NULL, pool->master)))
    {
      rc= MEMCACHED_SOME_ERRORS;
    }
  }

  pool->server_pool[++pool->firstfree]= released;

  if (pool->firstfree == 0 && pool->current_size == pool->size)
  {
    /* we might have people waiting for a connection.. wake them up :-) */
    pthread_cond_broadcast(&pool->cond);
  }

  memcached_return_t rval= mutex_exit(&pool->mutex);
  if (rc == MEMCACHED_SOME_ERRORS)
    return rc;

  return rval;
}


memcached_return_t memcached_pool_behavior_set(memcached_pool_st *pool,
                                               memcached_behavior_t flag,
                                               uint64_t data)
{
  if (not pool)
    return MEMCACHED_INVALID_ARGUMENTS;

  memcached_return_t rc= mutex_enter(&pool->mutex);
  if (rc != MEMCACHED_SUCCESS)
    return rc;

  /* update the master */
  rc= memcached_behavior_set(pool->master, flag, data);
  if (rc != MEMCACHED_SUCCESS)
  {
    mutex_exit(&pool->mutex);
    return rc;
  }

  pool->increment_version();
  /* update the clones */
  for (int xx= 0; xx <= pool->firstfree; ++xx)
  {
    rc= memcached_behavior_set(pool->server_pool[xx], flag, data);
    if (rc == MEMCACHED_SUCCESS)
    {
      pool->server_pool[xx]->configure.version= pool->version();
    }
    else
    {
      memcached_free(pool->server_pool[xx]);
      if (not (pool->server_pool[xx]= memcached_clone(NULL, pool->master)))
      {
        /* I'm not sure what to do in this case.. this would happen
          if we fail to push the server list inside the client..
          I should add a testcase for this, but I believe the following
          would work, except that you would add a hole in the pool list..
          in theory you could end up with an empty pool....
        */
      }
    }
  }

  return mutex_exit(&pool->mutex);
}

memcached_return_t memcached_pool_behavior_get(memcached_pool_st *pool,
                                               memcached_behavior_t flag,
                                               uint64_t *value)
{
  if (! pool)
    return MEMCACHED_INVALID_ARGUMENTS;

  memcached_return_t rc= mutex_enter(&pool->mutex);
  if (rc != MEMCACHED_SUCCESS)
  {
    return rc;
  }

  *value= memcached_behavior_get(pool->master, flag);

  return mutex_exit(&pool->mutex);
}
