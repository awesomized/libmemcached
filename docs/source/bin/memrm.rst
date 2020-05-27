memrm
=====

SYNOPSIS
--------

memrm [options] <key ...>

.. program:: memrm

Remove key(s) from a collection of memcached servers

DESCRIPTION
-----------

:program:`memrm` removes items, specified by key, from :manpage:`memcached(1)` servers.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
.. include:: options/hash.rst
.. include:: options/sasl.rst

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
* :doc:`/libmemcached/memcached_delete`
