memflush
========

SYNOPSIS
--------

memflush [options]

.. program:: memflush

Reset a server or list of servers

DESCRIPTION
-----------

:program:`memflush`  resets the contents of :manpage:`memcached(1)` servers.

.. warning::

    This means that all data in the specified servers will be deleted.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
.. include:: options/sasl.rst
.. include:: options/expire.rst

.. note::

    Using an expiration time (period), all keys, which have not bean updated until expiration will cease to exist.

    Quoting the `memcached protocol documentation`_, it states:

        Its effect is to invalidate all
        existing items immediately (by default) or after the expiration
        specified.  After invalidation none of the items will be returned in
        response to a retrieval command (unless it's stored again under the
        same key *after* flush_all has invalidated the items).

        The most precise
        definition of what flush_all does is the following: it causes all
        items whose update time is earlier than the time at which flush_all
        was set to be executed to be ignored for retrieval purposes.

        The intent of flush_all with a delay, was that in a setting where you
        have a pool of memcached servers, and you need to flush all content,
        you have the option of not resetting all memcached servers at the
        same time (which could e.g. cause a spike in database load with all
        clients suddenly needing to recreate content that would otherwise
        have been found in the memcached daemon).

.. _memcached protocol documentation: https://github.com/memcached/memcached/blob/master/doc/protocol.txt

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
* :doc:`/libmemcached/memcached_flush`
