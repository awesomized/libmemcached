memping
=======

SYNOPSIS
--------

|client_prefix|\ping [options] [server]

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

.. option:: -s|--servers <list of servers>

    Specify the list of servers as *hostname[:port][,hostname[:port]...]*.


.. include:: common/env.rst

NOTES
-----

.. include:: common/note_program_prefix.rst

SEE ALSO
--------

.. only:: man

  :manpage:`memcached(1)`
  :manpage:`libmemcached(3)`

.. only:: html

* :doc:`/libmemcached`
