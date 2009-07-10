#include <libmemcached/memcached.h>

#include <string.h>
#include <string>

class Memcached
{
public:

  Memcached() 
    : 
      memc(),
      result()
  {
    memcached_create(&memc);
  }

  Memcached(memcached_st *clone) 
    : 
      memc(),
      result()
  {
    memcached_clone(&memc, clone);
  }

  ~Memcached()
  {
    memcached_free(&memc);
  }

  std::string fetch(const std::string& key, size_t *key_length, size_t *value_length)
  {
    uint32_t flags;
    memcached_return rc;
    std::string ret_val;

    char *value= memcached_fetch(&memc, const_cast<char *>(key.c_str()), key_length,
                                 value_length, &flags, &rc);
    if (value)
    {
      ret_val.assign(value);
    }
    return ret_val;
  }

  std::string get(const std::string& key, size_t *value_length) 
  {
    uint32_t flags;
    memcached_return rc;
    std::string ret_val;

    char *value= memcached_get(&memc, key.c_str(), key.length(),
                               value_length, &flags, &rc);
    if (value)
    {
      ret_val.assign(value);
    }
    return ret_val;
  }

  std::string get_by_key(const std::string& master_key, 
                         const std::string& key, 
                         size_t *value_length)
  {
    uint32_t flags;
    memcached_return rc;
    std::string ret_val;

    char *value= memcached_get_by_key(&memc, master_key.c_str(), master_key.length(), 
                                      key.c_str(), key.length(),
                                      value_length, &flags, &rc);
    if (value)
    {
      ret_val.assign(value);
    }
    return ret_val;
  }

  bool mget(char **keys, size_t *key_length, 
            unsigned int number_of_keys)
  {

    memcached_return rc= memcached_mget(&memc, keys, key_length, number_of_keys);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool set(const std::string& key, const char *value, size_t value_length)
  {
    memcached_return rc= memcached_set(&memc, key.c_str(), key.length(),
                                       value, value_length,
                                       time_t(0), uint32_t(0));
    return (rc == MEMCACHED_SUCCESS);
  }

  bool set_by_key(const std::string& master_key, 
                  const std::string& key, 
                  const char *value, size_t value_length)
  {
    memcached_return rc= memcached_set_by_key(&memc, master_key.c_str(), 
                                              master_key.length(),
                                              key.c_str(), key.length(),
                                              value, value_length,
                                              time_t(0),
                                              uint32_t(0));
    return (rc == MEMCACHED_SUCCESS);
  }

  bool increment(const std::string& key, unsigned int offset, uint64_t *value)
  {
    memcached_return rc= memcached_increment(&memc, key.c_str(), key.length(),
                                             offset, value);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool decrement(const char *key, unsigned int offset, uint64_t *value)
  {
    memcached_return rc= memcached_decrement(&memc, key, strlen(key),
                                             offset, value);
    return (rc == MEMCACHED_SUCCESS);
  }


  bool add(const char *key, const char *value, size_t value_length)
  {
    memcached_return rc= memcached_add(&memc, key, strlen(key), value, value_length, 0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool add_by_key(const char *master_key, const char *key, 
                  const char *value, size_t value_length)
  {
    memcached_return rc= memcached_add_by_key(&memc, master_key, strlen(master_key),
                                              key, strlen(key),
                                              value, value_length,
                                              0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool replace(const char *key, const char *value, 
               size_t value_length)
  {
    memcached_return rc= memcached_replace(&memc, key, strlen(key),
                                           value, value_length,
                                           0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool replace_by_key(const char *master_key, const char *key, 
                      const char *value, size_t value_length)
  {
    memcached_return rc= memcached_replace_by_key(&memc, master_key, strlen(master_key),
                                                  key, strlen(key),
                                                  value, value_length, 0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool prepend(const char *key, const char *value, 
               size_t value_length)
  {
    memcached_return rc= memcached_prepend(&memc, key, strlen(key),
                                           value, value_length, 0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool prepend_by_key(const char *master_key, const char *key, 
                      const char *value, size_t value_length)
  {
    memcached_return rc= memcached_prepend_by_key(&memc, master_key, strlen(master_key),
                                                  key, strlen(key),
                                                  value, value_length,
                                                  0,
                                                  0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool append(const char *key, const char *value, 
              size_t value_length)
  {
    memcached_return rc= memcached_append(&memc, key, strlen(key),
                                          value, value_length, 0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool append_by_key(const char *master_key, const char *key, 
                     const char *value, size_t value_length)
  {
    memcached_return rc= memcached_append_by_key(&memc,
                                                 master_key, strlen(master_key),
                                                 key, strlen(key),
                                                 value, value_length, 0, 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool cas(const char *key, const char *value, 
           size_t value_length, uint64_t cas_arg)
  {
    memcached_return rc= memcached_cas(&memc, key, strlen(key),
                                       value, value_length, 0, 0, cas_arg);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool cas_by_key(const char *master_key, const char *key, 
                  const char *value, size_t value_length, 
                  uint64_t cas_arg)
  {
    memcached_return rc= memcached_cas_by_key(&memc,
                                              master_key, strlen(master_key),
                                              key, strlen(key),
                                              value, value_length,
                                              0, 0, cas_arg);
    return (rc == MEMCACHED_SUCCESS);
  }

  // using 'remove' vs. 'delete' since 'delete' is a keyword 
  bool remove(const std::string& key)
  {
    memcached_return rc= memcached_delete(&memc, key.c_str(), key.length(), 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  bool delete_by_key(const char *master_key, const char *key)
  {
    memcached_return rc= memcached_delete_by_key(&memc, master_key, strlen(master_key),
                                                 key, strlen(key), 0);
    return (rc == MEMCACHED_SUCCESS);
  }

  const std::string lib_version() const
  {
    const char *ver= memcached_lib_version();
    const std::string version(ver);
    return version;
  }

private:
  memcached_st memc;
  memcached_result_st result;
};
