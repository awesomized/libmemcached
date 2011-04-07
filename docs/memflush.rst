.. highlight:: perl


memflush
********


Reset a server or list of servers


********
SYNOPSIS
********



.. code-block:: perl

   memflush [options]



***********
DESCRIPTION
***********


\ **memflush**\  resets the contents of memcached(1) servers.
This means all data in these servers will be deleted.

You can specify servers via the \ **--servers**\  option or via the
environment variable \ ``MEMCACHED_SERVERS``\ .

For a full list of operations run the tool with the \ **--help**\  option.


****
HOME
****


To find out more information please check:
`https://launchpad.net/libmemcached <https://launchpad.net/libmemcached>`_


******
AUTHOR
******


Brian Aker, <brian@tangent.org>

Mark Atwood <mark@fallenpegasus.com>


********
SEE ALSO
********


memcached(1) libmemcached(3)

