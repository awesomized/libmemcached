# ChangeLog v0.x

## v 0.53
> released 2011-09-27

* Fix for FreeBSD/OpenBSD and -lm
* Added memcached_exist()
* Fix for memory when using config test.
* CLI gained --quiet

## v 0.52
> released 2011-09-12

* Build fixes for Ubuntu/Suse.
* Fixes for OSX Lion.
* Bug fix for looping back through dns lookups under certain failures.
* Fixes related to dead server failures.

## v 0.51
> released 2011-07-21

* memcached_callback_set() now takes its data argument as const
* Update to tests.
* Fix in parser for port number.

## v 0.50
> released 2011-06-20

* Updates to C++ interface 
* Custom free allocators need to now check for value before calling free.
* memcached_fetch_result() now uses the internal result when available (about 25 to 50% faster).
* Fix for stats structure.
* Updates to documentation.
* memcached_fetch_result() now uses the internal result when available (about 25 to 50% faster).

## v 0.49
> released 2011-04-14

* Fix calls to auto methods so that if value is not passed in nothing bad happens.
* New parser calls for generating memcached_st objects.
* New error system.
* New flow control for messages means faster get/set calls.
* Added new documentation system.
* A behavior change has been now made that if you specify a weight for any server, we enable the weight flag and do weight balancing.  
* Added MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS to simplify the setting of AUTO REJECT for servers.

## v 0.48
> released 2011-03-16

* Fix memory leak in server parse.
* Move test framework out to be its own library (easier to work with Gearman).


## v 0.47
> released 2011-02-24

* Additional fixes for OpenBSD.
* Bug fix 677609, 456080.
* SIGPIPE fix for Linux send(). 
* memcapable can now test ascii or binary based on flags.
* Additional build fixes for SASL.


## v 0.46
> released 2011-02-14

* Fixes a number of corner case bugs.
* Fixes related to OpenBSD.
* Better testing for protocol version.
* Removes special case infinite wait on blocking setup.

## v 0.45
> released 2011-02-09

* Add support for systemtap

## v 0.44
> released 2010-09-23

* Windows bug fixes.
* Hudson port support in test harness.
* Improved portability of test hanrness.
* SASL fixes.

## v 0.43
> released 2010-07-28

* Added --args to memstat so that a greater range of values can be returned.
* Prelimanary support for Windows.
* memcached_stat_execute() merged. 

## v 0.42
> released 2010-07-06

* Mistake in libtool caused issue with library version

## v 0.41
> released 2010-06-30

* Added --file for memcat.
* Added limemcached_ping() to libmemcached_util
* Bugfix for some cases where connect would have issues with timeout.
* Wrong value for errno given as error on an IO failure inside of poll.
* Bug fix for issue where multiple interfaces with bad DNS were not being caught.

## v 0.40
> released 2010-04-23

* Placed retry logic in for busted resolvers
* Add an ignore for SIGPIPE to solve OSX issues.
* A couple of fixed for memcached_light server.
* Updated to debug mode to track io_wait

## v 0.39
> released 2010-04-06

* Add support for prefix keys to binary protocol.
* Remove the undocumented call memcached_server_remove().
* The undocumented call memcached_server_by_key() now returns const.
* memcached_server_error_reset() has been deprecated.
* memcached_server_list() has been deprecated. Use memcached_server_cursor() to walk the servers found in a memcached_st() structure.
* memcached_verbosity() can now be run concurrently with other operations.
* SASL support.
* Fixes memory leak found in EJECT HOSTS.

## v 0.38
> released 2010-02-10

* C++ interface for libhashkit.
* Modified memcached_set_memory_allocators() so that it requires a context pointer.
* memcached_clone() now runs 5 times faster.
* Functions used for callbacks are now given const memcached_st.
* Added MEMCACHED_BEHAVIOR_CORK.
* memslap now creates a configuration file at ~/.memslap.cnf
* memcached_purge() now calls any callbacks registered during get execution.
* Many fixes to memslap.
* Updates for memcapable.
* Compile fixes for OpenBSD.
* Fix for possible recursive decent on IO failure.

## v 0.37
> released 2010-01-12

* Fixed build for libhashkit.
* Fixed install path regression.  
* Modified RPM to strict check install. 
* Added documentation for memcached_server_cursor();
* Added memcached_servers_reset().
* Modified memcached_st to remove dead cursor_server member.

## v 0.36
> released 2010-01-07

* Merged in new memslap utility.
* All of constants.h has been updated to match style (all old identifiers continue to work).
* Added first pass for libhashkit.
* Updated test Framework/extended tests.
* Random read support during replication added.
* Modified use_sort so that the option can be applied to any distribution type.
* We removed the MEMCACHED_BEHAVIOR_KETAMA_COMPAT_MODE added in 0.35. Instead use memcached_behavior_set_distribution().

## v 0.35
> released 2009-11-09

* Added support for by_key operations for inc/dec methods.
* Added mget test to memslap.
* Support for compatible ketama for SpyMemcached
* Update C++ interface.
* Fix for memcp

## v 0.34
> released 2009-10-13

* Added support for setting behavior flags on a connection pool.
* Don't increment server_failure_counter on normal disconnects.
* Added prototype for a callback based protocol parser (server side) with examples so that you could let your own application speak the memcached protocol
* Updated memcapable to test ASCII protocol.
* Changed behavior so that server can be removed at first sign of failure.
* Added memcached_server_get_last_disconnect() call

## v 0.33
> released 2009-09-23

* Added memcapable to test servers for binary compatibility.
* Updated C++ interface. Added basic support for C++ exceptions. Added multiple constructors the memcached client object. The C++ interface now takes parameters which are C++ types (such as std::string).
* Several bug fixes for binary protocol support.
* Fixed crashing issue with dumping from memcachd server (server internals were changed without documenting change).

## v 0.32
> released 2009-09-15

* Change of behavior where linger is only modified for no-block and then it is set to zero.
* Added Twitter's memcached_server_error() functions.
* Fix for OSX compiles in development builds.
* Updated C++ interface.
* Updated memcached_mget and memcached_mget_by_key to take a size_t as a parameter instead of an unsigned int for number_of_keys.

## v 0.31
> released 2009-07-10

* Added support or HA via replication.
* malloc() removed for server key usage.
* Update build system.
* Added support for memcached_set_memory_allocators().
* Fixed bug in configure.ac for have_htoll.

## v 0.30
> released 2009-06-01

* Added memcachd_dump command (and framework for memdump tool).
* Realigned all structures to remove padding (and line up important bits for 64bit caches.
* Remove some of sprintf() in storage calls().
* Removed printf() in stat call for unknown stat member.
* memcached_generate_hash() function added.
* Added tests to make sure all hash functions are stable.

## v 0.29
> released 2009-05-19

* Fixed malloc usage to calloc for spots where we need zero filled memory. 
* All code warnings now treated as errors.
* Fixes for debian packaging.
* Added new pooling mechanism.
* MEMCACHED_BEHAVIOR_NO_BLOCK no longer also sets MEMCACHED_BEHAVIOR_BUFFER_REQUESTS.
* Updated generic rpm.

## v 0.28
> released 2009-04-15

* Fixed bug in init sructure (reapplied)
* Fixed bug in get/set by key (nikkhils@gmail.com)

## v 0.27
> released 2009-03-30

* Added new UDP fire-forget mode.
* Reworked performance for mget() to better make use of async protocol
* Cleaned up execution of fetch (just one set of code now)
* Fixed Jenkin's for big endian hosts.
* Updates for memstat to determine network latency.
* Updates for binary protocol.
* Many updates to documentation.

## v 0.26
> released 2009-01-29

* Fix for decrement on hash key
* Fixed assert that was catching bad memset() call in host_reset()
* Fix purge issue for blocked IO which has been stacked.

## v 0.25
> released 2008-11-28

* Jenkins HASH added.
* Update of Murmur hash code
* Support explicit weights (Robey Pointer, Evan Weaver)
* Bugfix for ketama continuum (Robey Pointer)
* New behavior MEMCACHED_BEHAVIOR_HASH_WITH_PREFIX_KEY (Robey Pointer)
* Don't ever call stats for weighting servers, because it is unstable.

## v 0.24
> released 2008-09-16

* Cleanup compile warnings.
* Fix issues in partitioning by keys.
* Fixed "fail case" to make sure when calling memcached_clone() no memcached_st is over written.
* New memcached_server_by_key() method for finding a server from a key.
* memcached_server_free() was added for freeing server structures.


## v 0.23
> released 2008-09-07

* Added strings.h header for Solaris 9
* Solaris 64bit fix.
* Support for weighted Ketama from Yin Chen.
* Fix for Chinese 
* Fix for 0 length key to trigger bad key.
* Added behaviors MEMCACHED_BEHAVIOR_SND_TIMEOUT, MEMCACHED_BEHAVIOR_RCV_TIMEOUT
* Support for Binary Protocol added

## v 0.22
> released 2008-07-14

* Fix where master key was no being checked for "bad key"
* Fixed bugs in stats output (thread output was wrong)
* Clarified MEMCACHED_BAD_KEY_PROVIDED is return for bad prefix key.
* Found a bug in Flags return (Jacek Ostrowski)
* Fixed issue with compiling on Visual Studio

## v 0.21
> released 2008-05-24

* Change of char * to const char * for all key based functions.
* New  MEMCACHED_CALLBACK_PREFIX_KEY added. You can now create domains for values.
* Fixed bug introducd in last version on memcp
* Fix for death of file io to call shutdown()

## v 0.20
> released 2008-05-05

* New consistent distribution tests.
* Found a memory leak when a server constantly fails.
* Fix in watchpoint macro
* Changed default timeout to 1 second for poll timeouts
* Wheel uses less memory/dynamic allocation for size (no longer limited to 512 hosts by default.
* memslap memory leak fix
* Added Ketama distribution
* Fix assert.h compile problem on CentOS

## v 0.19
> released 2008-04-09

* Documentation fix in libmemcached.
* Fixed bug where sort was always occuring on hosts
* Logic fix in branch prediction (thanks Jay!)
* Read through cached support.
* Fixed for cas by key operation.
* Fix for memcached_server_st list structures to have correct count.
* Added callback MEMCACHED_CALLBACK_DELETE_TRIGGER
* Removed function call in favor of macro (aka cut out some instructions)


## v 0.18
> released 2008-03-17

* Fix plus tests for non-zero value objects and flags.
* MEMCACHED_HASH_MURMUR added for murmur algorithm provided.
* MEMCACHED_BEHAVIOR_RETRY_TIMEOUT added to keep connecting from looping on timeout.
* gcc branch prediction optimizations
* Refactored entire tree to make include files cleaner
* Fixed leaked socket.

## v 0.17
> released 2008-02-27

* MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT added for connect timeout in non-block mode.
* Incompatible change in memcached_behavior_set() api. We now use a uint64_t, instead of a pointer.
* Fix for storage of values for zero.
* memcached_server_cursor() function added to API for cycling through servers.

## v 0.16
> released 2008-02-18

* Work on the UDP protocol
* Added get_by_key, set_by_key tests for C++ API
* Fix for limit_maxbytes to be 64bit in stats
* Added Atom Smasher test (scale baby, scale!)
* Servers are now sorted, meaning that servers are now ordered so that clients with the same lists, will have same distribution. (Idea from Ross McFarland). MEMCACHED_BEHAVIOR_SORT_HOSTS was added to enable this support.
* Added MEMCACHED_BAD_KEY_PROVIDED error for auto, set, and get operations.  MEMCACHED_BEHAVIOR_VERIFY_KEY was added to enable this feature.
* More error messages on command line tools.
* Fixed bugs in memcached_cas() operator.
* Fix to loop through interfaces

## v 0.15
> released 2008-01-29

* More work on the C++ API.
* Bug fixes around block corner cases.
* Slight performance increase in both read() and write().

## v 0.14
> released 2008-01-22

* For for bug found by Evan Weaver where increment() was not returning propper error of value was not found.
* Fix for bad null pointer on flag by Toru Maesaka.
* Refactor of all IO to just pass in the active server
* Problem configuring (PKG_CHECK_MODULES) fixed by removal of "rpath" in support/libmemcached.pc.in (Thanks to Ross McFarland).
* Added memcached_callback_get()/set()
* First prototype of C++ interface
* Updated docs for uint16_t changes in previous release

## v 0.13
> released 2008-01-13

* MEMCACHED_BEHAVIOR_USER_DATA added to store user pointer.
* Fix for failure to connect to invalidate socket.
* Patch from Marc Rossi to add --hash option for memcp, memrm, and memcat.
* Kevin's patch for fixing EOF issues during a read.
* Toru Maesaka patch for stats mismatch
* Fix for when CRC return 0
* Fixed uint16_t issues around flags. Turns out the documentation on the protocol was wrong.
* Lingering socket fixes for FreeBSD.
* Patches from Kevin Dalley for FreeBSD 4.0
* Added multi delete functions.
* All get key returns have C style null termination
* If memcached_server_list_append is passed NULLs instead of pointers it returns NULL.  
* Added memcached_fetch_execute() method
* Found a bug where memcached_fetch() was not null terminating the result value.
* memcached_behavior() now has the ability to set "buffering" so that data is not automatically flushed.
* Behavior change, buffered commands now return MEMCACHED_BUFFERED

## v 0.12
> released 2007-12-11

* Updates for consistent hashing
* IPV6 support
* Static allocation for hostname (performance)
* Fixed bug where in non-block mode all data might not have been sent on close().
* Refactor of memcached_get() to use common code.
* Change in value fetch, MEMCACHED_END is now returned when keys are no longer in the pipe.
* Fixed bug where key could be out of range of characters
* Added _by_key() methods to allow partitioning of values to particular servers.
* MEMCACHED_DEFAILT_TIMEOUT is now set to a non -1 value.
* Performance improvements in get operations.

## v 0.11
> released 2007-11-26

* Added option to memcache_behavior_set() so that poll() can be timed out.
* Fixed memory leak in case of using memcached_fetch_result() where no value was returned.
* Bug fixed in memcached_connect() which would cause servers that did not need to be enabled to be enabled (performance issue).
* Rewrote bounds checking code for get calls.
* "make test" now starts its own memcached servers.
* Added Hseih hash (MEMCACHED_HASH_HSIEH), which is showing about 7% performance over standard hash.

## v 0.10
> released 2007-11-21

* Added append binary test.
* Added MEMCACHED_BEHAVIOR_CACHE_LOOKUPS behavior so that you can save on multiple DNS lookups.
* Added CAS support, though this is optional and must be enabled during runtime.
* Added the utility memerror to create human readable error strings from memcached errors (aka convert ints to strings)
* Fixed type in MEMCACHED_HOST_LOOKUP_FAILURE
* Fixed bug where hostname might not be null terminated
* Moved to using gethostbyname_r() on Linux to solve thread safety issue
* Added -rpath support for pkg-config
* Documentation fix for hash setting using memcached_behavior_set()

## v 0.9
> released 2007-11-15

* fix for when no servers are definied.
* different buffers are now kept for different connections to speed up async efforts
* Modified increment/decrement functions to return uint64_t values
* Fixed bug in cases where zero length keys were provided
* Thread cleanup issue in memslap
* No hostname lookup on reconnect
* Fix for flag settings (was doing hex by accident!)
* Support for 1.2.4 server additions "prepend" and "append" added.
* Added memcached_version()... not sure if I will make this public or not.

## v 0.8
> released 2007-11-05

* Adding support for CRC hash method 
* Adding support for UNIX sockets
* Added additional HASHing methods of FNV1_64,FNV1A_64, FNV1_32, FNV1A_32
* Added pkgconfig support (PKG_CHECK_MODULES)
* Fixed conflict with defined type in MySQL
* Added memcached_result_st structure and functions to manipulate it.

## v 0.7
> released 2007-10-30

* Poved to poll() from select()
* Fixes in internal string class for allocation of large numbers of strings.
* memcached_mget() function now sends keys as it parses them instead of building strings as it goes.
* Propper flush now for making sure we get all IO sent even when in non-block mode.
* Added --enable-debug rule for configure
* All asserts() removed (hey this is going into production!)


## v 0.6
> released 2007-10-17

* get value returns are now null terminated (request by Cal Heldenbrand) 
* Fixed connections for more hosts then two.
* Rewrite of the read/write IO systems to handle different sorts of host failures.
* Added man pages for all functions and tools
* Raised buffer size for readinng/writing to 16K
* You can now optionally set the socket size for recv/send via memached_behavior_set/get.

## v 0.5
> released 2007-10-09

* Ruby maintainer mentioned TCP_NODELAY patch he had added. Added this to C
  library as well. (Eric Hodel drbrain@segment7.net)
* Added support script for set_benchmark
* Updated memslap to allow testing of TCP_NODELAY
* Updated memslap to support --flush (aka dump memcache servers before testing)
* Fixed bug in multiple hosts not being activated
* Added environmental variable MEMCACHED_SERVERS which can be used to set the servers list.
* fixed memcached_stat method (and now memstat works)
* server connect now happens on demand.
* Help for all command line applications

## v 0.4
> released 2007-10-03

* Added buffered IO to write calls for keys
* Added buffered IO for reads
* memstat was broken (bad if/else on connect)
* New non-blocking IO (not default yet). Mucho faster
* Refactor of test system.
* memslap crash solved

## v 0.3
> released 2007-10-01

* Jeff Fisher <guppy@techmonkeys.org> provided a spec file
* Added "make rpm" around dist file
* Added support for Solaris
* Added support for DTrace
* Fixed read to be recv and write to be send
* Bug fix where memstat would core if no server was found
* Added memslap tool (load generator)
* Numerous bug fixes in library
* Added calls to library for creating host lists (see text cases to understand how to use this).

## v 0.2
> released 2007-09-27

* First public version
