=================================
memrm - Remove data from a server
=================================


memrm - Remove a key(s) from a collection of memcached servers


--------
SYNOPSIS
--------

:program: `memrm`

.. program:: memrm

.. option:: --help



-----------
DESCRIPTION
-----------


\ **memrm**\  removes items, specified by key, from memcached(1) servers.

You can specify servers via the \ **--servers**\  option or via the
environment variable \ ``MEMCACHED_SERVERS``\ .

For a full list of operations run the tool with the \ **--help**\  option.


----
HOME
----


To find out more information please check:
`http://libmemcached.org/ <http://libmemcached.org/>`_


------
AUTHOR
------


Brian Aker, <brian@tangent.org>

Mark Atwood, <mark@fallenpegasus.com>


--------
SEE ALSO
--------


memcached(1) libmemcached(3)

