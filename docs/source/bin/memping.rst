memping
=======

SYNOPSIS
--------

memping [options] [server]

.. program:: memping

Test for availability of a server

DESCRIPTION
-----------

:program:`memping` can be used to ping a memcached server to see if it is accepting connections.

OPTIONS
-------

.. include:: options/all.rst
.. include:: options/common.rst
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
