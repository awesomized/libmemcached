.. highlight:: perl


****
NAME
****


libmemcached_examples - Examples for libmemcached


***********
DESCRIPTION
***********


For full examples, test cases are found in tests/\*.c in the main
distribution. These are always up to date, and are used for each test run of
the library.

Creating and Freeing structure
==============================



.. code-block:: perl

   memcached_st *memc;
   memcached_return_t rc;
 
   memc= memcached_create(NULL);
   ...do stuff...
   memcached_free(memc);


The above code would create a connection and then free the connection when
finished.


Connecting to servers
=====================



.. code-block:: perl

   memcached_server_st *servers;
   memcached_st *memc= memcached_create(NULL);
   char servername[]= "0.example.com";
 
   servers= memcached_server_list_append(NULL, servername, 400, &rc);
 
   for (x= 0; x < 20; x++)
   {
     char buffer[SMALL_STRING_LEN];
 
     snprintf(buffer, SMALL_STRING_LEN, "%u.example.com", 400+x);
     servers= memcached_server_list_append(servers, buffer, 401, &rc);
   }
   rc= memcached_server_push(memc, servers);
   memcached_server_free(servers);
   memcached_free(memc);


In the above code you create a \ ``memcached_st``\  object that you then feed in a
single host into. In the for loop you build a \ ``memcached_server_st``\ 
pointer that you then later feed via memcached_server_push() into the
\ ``memcached_st``\  structure.

You can reuse the \ ``memcached_server_st``\  object with multile \ ``memcached_st``\ 
structures.


Adding a value to the server
============================



.. code-block:: perl

   char *key= "foo";
   char *value;
   size_t value_length= 8191;
   unsigned int x;
 
   value = (char*)malloc(value_length);
   assert(value);
 
   for (x= 0; x < value_length; x++)
   value[x] = (char) (x % 127);
 
   for (x= 0; x < 1; x++)
   {
     rc= memcached_set(memc, key, strlen(key), 
     value, value_length,
     (time_t)0, (uint32_t)0);
     assert(rc == MEMCACHED_SUCCESS);
   }
 
   free(value);


It is best practice to always look at the return value of any operation.


Fetching multiple values
========================



.. code-block:: perl

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



****
HOME
****


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_


******
AUTHOR
******


Brian Aker, <brian@tangent.org>


********
SEE ALSO
********


memcached(1)

