memerror
========

SYNOPSIS
--------

.. program:: memerror

memerror [options] <error code>

Translate a memcached error code into a string.

DESCRIPTION
-----------

:program:`memerror` translates an error code from `libmemcached` into a human
readable string.

OPTIONS
-------

.. include:: options/all.rst

ENVIRONMENT
-----------

.. envvar:: MEMCACHED_SERVERS

    Specify the list of servers.

SEE ALSO
--------

.. only:: man

    :manpage:`memcached(1)`
    :manpage:`libmemcached(3)`

.. only:: html

    * :doc:`/libmemcached`
    * :doc:`/libmemcached/index_errors`
