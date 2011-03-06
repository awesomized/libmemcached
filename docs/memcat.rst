.. highlight:: perl


****
NAME
****


memcat - Copy a set of keys to stdout


********
SYNOPSIS
********



.. code-block:: perl

   memcat [options] key key ...



***********
DESCRIPTION
***********


\ **memcat**\  outputs to stdout the value a single or multiple set of keys
stored in a memcached(1) server. If any key is not found an error is returned.

It is similar to the standard UNIX cat(1) utility.

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

