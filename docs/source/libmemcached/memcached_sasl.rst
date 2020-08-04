SASL support
============

SYNOPSIS
--------

#include <libmemcached/memcached.h>
    Compile and link with -lmemcached

.. function:: void memcached_set_sasl_callbacks(memcached_st *ptr, const sasl_callback_t *callbacks)

    :param ptr: pointer to initialized `memcached_st` struct
    :param callbacks: pointer to `sasl_callbacks_t` holding the callbacks to use

.. function:: const sasl_callback_t *memcached_get_sasl_callbacks(memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: pointer to `sasl_callbacks_t` holding the callbacks currently used

.. function:: memcached_return_t memcached_set_sasl_auth_data(memcached_st *ptr, const char *username, const char *password)

    :param ptr: pointer to initialized `memcached_st` struct
    :param username:
    :param password:
    :returns: `memcached_return_t` indicating success

.. function:: memcached_return_t memcached_destroy_sasl_auth_data(memcached_st *ptr)

    :param ptr: pointer to initialized `memcached_st` struct
    :returns: `memcached_return_t` indicating success

DESCRIPTION
-----------

`libmemcached` allows you to plug in your own callbacks function used by
libsasl to perform SASL authentication.

Please note that SASL requires the memcached binary protocol, and you have
to specify the callbacks before you connect to the server.

:func:`memcached_set_sasl_auth_data` is a helper function defining
the basic functionality for you, but it will store the username and password
in memory. If you choose to use this method you have to call
`memcached_destroy_sasl_auth_data` before calling `memcached_free` to avoid a
memory leak. You should NOT call `memcached_destroy_sasl_auth_data` if you
specify your own callback function with `memcached_set_sasl_callbacks`.

RETURN VALUE
------------

:func:`memcached_get_sasl_callbacks` returns the callbacks currently used by
this memcached handle. :func:`memcached_set_sasl_auth_data` returns
`MEMCACHED_SUCCESS` upon success.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`
    :manpage:`memcached_strerror(3)`
    :manpage:`sasl_client_new(3)`
    :manpage:`sasl_callbacks(3)`

.. only:: html

    * :manpage:`memcached(1)`
    * :doc:`../libmemcached`
    * :doc:`memcached_strerror`
    * :manpage:`sasl_client_new(3)`
    * :manpage:`sasl_callbacks(3)`
