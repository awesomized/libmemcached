
ChangeLog v1.0
==============

v 1.0.18
--------

..

   released 2014-02-09



* MEMCACHED_BEHAVIOR_RETRY_TIMEOUT can now be set to zero.
* Numerous bug fixes.

v 1.0.17
--------

..

   released 2013-04-03



* Remove c++ namespace that was being exposed (the API should be plug compatible)..
* Fix cases where --servers wasn't behaving the same in all clients.

v 1.0.16
--------

..

   released 2013-02-01



* Added support to do two part shutdown of socket.
* Fixes for Fedora 18.
* Fix for binary memcached_touch()

v 1.0.15
--------

..

   released 2012-12-17



* Added support for Murmur3 (HASHKIT_HASH_MURMUR3)
* Portability fixes.

v 1.0.14
--------

..

   released 2012-11-14



* CLIENT_ERROR fixed to not be treated as a fatal error.
* Compiler fixes for older Ubuntu releases.

v 1.0.13
--------

..

   released 2012-10-19



* Fix bug that caused version string to not be exported correctly.

v 1.0.12
--------

..

   released 2012-10-09



* Added memcached_result_take_value().
* Added ax_libmemcached.m4

v 1.0.11
--------

..

   released 2012-09-17



* Removed custom version of memcached.
* Updated hardening rules.
* Fixed a case where the return error from a socket connection differred from that of a TCP/IP socket.

v 1.0.10
--------

..

   released 2012-07-30



* --disable-assert has been removed from configure, and --enable-assert has been added in its place.
* Compiling fixes for Clang on OSX Mountain Lion.

v 1.0.9
-------

..

   released 2012-07-05



* Faster close on socket.
* Instance allocation is now seperated from server interface. 
  This should allow for a better preservation of ABI compliance from now on.
* Fix close on exec bug.
* Numerous other bug fixes.

v 1.0.8
-------

..

   released 2012-05-22



* Added support for setting options via ENV variable LIBMEMCACHED
* Fix corner case on last used result.

v 1.0.7
-------

..

   released 2012-04-28



* Add API call for exist calls.
* Update all license files to be BSD.

v 1.0.6
-------

..

   released 2012-04-08



* Fixes for gcc 4.7, lp:961812
* Fix for restart issue that happens under testing.
* Fix for lp:962815.
* Support for transparent AES encryption.

v 1.0.5
-------

..

   released 2012-03-14



* Fixes for OSX.
* Version is now parsed directly in the parser, which makes buffered operations now work with it..
* memstat has been extended so that it can be used to find the version of the server.
* Update documentation.
* Fixes for compile issues on Debian and Ubuntu

v 1.0.4
-------

..

   released 2012-01-27



* Fix for memcached_dump().
* Additional testing for memcached_stat_execute().

v 1.0.3
-------

..

   released 2012-01-09



* Increased size of sort buffer used during Ketama.
* Added support for new behavior to handle dead servers.
* Overall haul of UDP IO.
* Fixed C compile issue with memcached_exist()
* Numerous bug fixes.
* Clang support for OSX.
* All commands now using vector send support.

v 1.0.2
-------

..

   released 2011-10-24



* Dropped libmemcached/memcached_util.h (undocumented header file)
* Added memcached_touch() and memcached_touch_by_key()
* UDP support restructured to toggle on a complete memcached_st structure.

----

See :doc:`ChangeLog-0 <./ChangeLog-0>` for changes prior v1.0.
