=================================
memcat - "cat" data from a server
=================================



--------
SYNOPSIS
--------

memcat [options] key

Copy a set of keys to stdout


.. program:: memcat


-----------
DESCRIPTION
-----------


:program:`memcat` outputs to stdout the value a single or multiple set of keys
stored in a memcached(1) server. If any key is not found an error is returned.

It is similar to the standard UNIX cat(1) utility.


-------
OPTIONS
-------


You can specify servers via the option:

.. option:: --servers

or via the environment variable:

.. envvar:: `MEMCACHED_SERVERS`

For a full list of operations run the tool with the option:

.. option:: --help


--------
SEE ALSO
--------


:manpage:`memcached(1)` :manpage:`libmemcached(3)`

