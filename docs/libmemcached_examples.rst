========
Examples
========

Examples for libmemcached

-----------
DESCRIPTION
-----------


For full examples, test cases are found in tests/\*.c in the main
distribution. These are always up to date, and are used for each test run of
the library.


---------------------
Connecting to servers
---------------------



.. code-block:: c

   const char *config_string= "--SERVER=host10.example.com --SERVER=host11.example.com --SERVER=host10.example.com"
   memcached_st *memc= memcached(config_string, strlen(config_string);
   {
    ...
   }
   memcached_free(memc);


In the above code you create a \ ``memcached_st``\  object with three server by making use of :manpage:`memcached_create_with_options(3)`.


--------------------------
Creating a pool of servers
--------------------------



.. code-block:: c

  const char *config_string= "--SERVER=host10.example.com --SERVER=host11.example.com --SERVER=host10.example.com"; 
  
  memcached_pool_st* pool= memcached_pool(config_string, strlen(config_string));

  memcached_return_t rc;

  memcached_st *memc= memcached_pool_pop(pool, false, &rc);

  .... do work

  /*
    Release the memc_ptr that was pulled from the pool
  */
  memcached_pool_push(pool, memc);

  /*
    Destroy the pool.
  */
  memcached_pool_destroy(pool);



In the above code you create a \ ``memcached_pool_st``\  object with three
server by making use of :manpage:`memcached_pool(3)`.

When memcached_pool_destroy() all memory will be released that is associated
with the pool.


----------------------------
Adding a value to the server
----------------------------



.. code-block:: c

   char *key= "foo";
   char *value= "value";

   memcached_return_t rc= memcached_set(memc, key, strlen(key), value, value_length, (time_t)0, (uint32_t)0);

   if (rc != MEMCACHED_SUCCESS)
   {
   ... // handle failure
   }


It is best practice to always look at the return value of any operation.


------------------------
Fetching multiple values
------------------------



.. code-block:: c

   memcached_return_t rc;
   char *keys[]= {"fudge", "son", "food"};
   size_t key_length[]= {5, 3, 4};
   unsigned int x;
   uint32_t flags;
 
   char return_key[MEMCACHED_MAX_KEY];
   size_t return_key_length;
   char *return_value;
   size_t return_value_length;
 
   rc= memcached_mget(memc, keys, key_length, 3);
 
   x= 0;
   while ((return_value= memcached_fetch(memc, return_key, &return_key_length, 
                                         &return_value_length, &flags, &rc)))
   {
     free(return_value);
     x++;
   }


Notice that you freed values returned from memcached_fetch(). The define
\ ``MEMCACHED_MAX_KEY``\  is provided for usage.



----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


--------
SEE ALSO
--------


:manpage:`memcached(1)`

