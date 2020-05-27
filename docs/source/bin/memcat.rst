memcat
======

SYNOPSIS
--------

.. program:: memcat

memcat [options] key [key...]

Read and output the value of one key or the values of a set of keys.

DESCRIPTION
-----------

:program:`memcat` reads and outputs the value of a single or a set of keys
stored in a :manpage:`memcached(1)` server.

If any key is not found an error is returned.

It is similar to the standard UNIX :manpage:`cat(1)` utility.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
.. include:: options/hash.rst

.. option:: --flag

    Display stored flags.

ENVIRONMENT
-----------

.. envvar:: MEMCACHED_SERVERS

    Specify a list of servers.

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)`
  :manpage:`libmemcached(3)`
  :manpage:`libmemcached_configuration(3)`

.. only:: html

  * :doc:`/libmemcached`
  * :doc:`/libmemcached/configuration`
