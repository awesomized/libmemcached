=======
memdump
=======


Dump a list of keys from a server.


********
SYNOPSIS
********



.. code-block:: perl

   memdump [options]



***********
DESCRIPTION
***********


\ **memdump**\  currently dumps a list of "keys" from all servers that 
it is told to fetch from. Because memcached does not guarentee to
provide all keys it is not possible to get a complete "dump".

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


********
SEE ALSO
********


memcached(1) libmemcached(3)

