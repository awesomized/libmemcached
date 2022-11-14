.. role:: raw-html-m2r(raw)
   :format: html


ChangeLog v1.1
==============

v 1.1.3
-------

..

   released 2022-11-09



* Fix `gh #130 <https://github.com/awesomized/libmemcached/issues/130>`_
  with `gh #124 <https://github.com/awesomized/libmemcached/issues/124>`_\ :
  Server response count can underflow.

v 1.1.2
-------

..

   released 2022-08-10



* Fix handling of negative expiration values, which are somehow allowed by legacy.\ :raw-html-m2r:`<br>`
  See also `gh #125 <https://github.com/awesomized/libmemcached/issues/125>`_\ ,
  and `gh #76 <https://github.com/awesomized/libmemcached/issues/76>`_.
* Fix `gh #122 <https://github.com/awesomized/libmemcached/issues/122>`_\ :
  If libcrypto implementation of AES is used, do not compile internal.
* Fix missing include of :raw-html-m2r:`<array>` in tests.
* Fix warnings with non-SASL builds.
* Fix pthread.h detection.

v 1.1.1
-------

..

   released 2021-09-16



* Fix `gh #67 <https://github.com/awesomized/libmemcached/issues/67>`_\ :
  GET returns ``NOTFOUND`` on ``TIMEOUT``.
* Fix `gh #113 <https://github.com/awesomized/libmemcached/issues/105>`_\ :
  Build failure with Catch2 < 2.13.5.
* Add `gh #114 <https://github.com/awesomized/libmemcached/pull/114>`_\ :
  Add possibility to use libcrypto for encryption.
* Add `gh #115 <https://github.com/awesomized/libmemcached/pull/115>`_\ :
  Add ``LIBMEMCACHED_AWESOME`` CPP define.
* Add test for `gh #75 <https://github.com/awesomized/libmemcached/issues/75>`_\ :
  memcached_clone of SASL connection closes random file descriptor.
* Fix `gh #116 <https://github.com/awesomized/libmemcached/issues/116>`_\ :
  Add libmemcachedpotocol-0-0/configure.h guarding ``ssize_t`` typedef.
* Fix `gh #120 <https://github.com/awesomized/libmemcached/issues/120>`_\ :
  libmemcached.pc is missing a ``Requires`` entry for libsasl2.

v 1.1.0
-------

..

   released 2021-06-23


**Changes from beta3:**


* Add ASCII multi get support to bin/memslap.

See logs from ``beta3``\ , ``beta2``\ , and ``beta1`` for
the full list of changes since the last 1.0 release.

v 1.1.0-beta3
-------------

..

   released 2021-04-15


**Changes from beta2:**


* Fix `gh #108 <https://github.com/awesomized/libmemcached/issues/105>`_\ :
  macOS Big Sur: dtrace does not understand -G switch.
* Add support for IPv6 bracketed syntax in ``memcached_servers_parse``.
* Make ``memcat``\ 's ``--file`` option's argument optional defaulting to ``<key>``.
* Fix libmemcachedprotocol's binary ``STAT`` and ``VERSION`` handlers.
* Fix `gh #105 <https://github.com/awesomized/libmemcached/issues/105>`_\ :
  EINTR handled too defensively when polling.

v 1.1.0-beta2
-------------

..

   released 2020-12-28


**Changes from beta1:**


* Fix `gh #103 <https://github.com/awesomized/libmemcached/issues/103>`_\ :
  Build failure on 32-bit.
* Fix `gh #102 <https://github.com/awesomized/libmemcached/issues/102>`_\ :
  Doc build with old sphinx.
* Fix `gh #100 <https://github.com/awesomized/libmemcached/issues/100>`_\ :
  Revert symbolic rename of public header include directories.
* Fix `gh #98 <https://github.com/awesomized/libmemcached/issues/98>`_\ :
  Library SONAMEs and NAME_LINKs differ from 1.0.18.
* Fix `gh #97 <https://github.com/awesomized/libmemcached/issues/97>`_\ :
  Location of cmake files installation directory.
* Fix `gh #96 <https://github.com/awesomized/libmemcached/issues/96>`_\ :
  LIBXXX_VERSION_HEX constants format.

v 1.1.0-beta1
-------------

..

   released 2020-12-21



* Ported build system to CMake.
* Ported test suite to Catch2.
* Build requires C++11 compiler support.
* Tests require C++17 compiler support.
* Moved to the Semantic Versioning Specification: https://semver.org
* Moved the project from launchpad to github:

  * Source: https://github.com/awesomized/libmemcached
  * Documentation: https://awesomized.github.io/libmemcached
  * Continuous Integration:

    * Github: https://github.com/awesomized/libmemcached/actions (Linux, MacOS, Windows **·** amd64)
    * Sourcehut: https://builds.sr.ht/~m6w6/libmemcached (FreeBSD, 
      OpenBSD **·** amd64)
    * Build artifacts: https://artifacts.m6w6.name/libmemcached/ rsync://m6w6.name::artifacts/libmemcached/


* Fix build failure due to comparison of incompatible types in bin/memflush and bin/memstat.
* Fix wrong type of memcached_instance_st::server_timeout_counter_query_id from uint32_t to uint64_t.
* Fix memcached_dump():
  returned MEMCACHED_CLIENT_ERROR on request to dump illegal slab id.
* Fix bin/memcapable:
  failed with "No hostname was provided" when providing a hostname.
* Fix hashkit/murmur and hashkit/murur3:
  undefined behavior on platforms requiring aligned access.
* Fix Memcache::set():
  possible subscription of empty vector.
* Fix libmemcached_util_version_check().
* Fix ketama/consistent hashing:
  crash on reallocation of continuum.
* Fix `gh #90 <https://github.com/awesomized/libmemcached/issues/90>`_\ :
  Build fails on Darwin.
* Fix `gh #83 <https://github.com/awesomized/libmemcached/issues/83>`_\ :
  memcp waits forever if file no found.
* Fix `gh #80 <https://github.com/awesomized/libmemcached/issues/80>`_\ :
  memparse docs.
* Fix `gh #72 <https://github.com/awesomized/libmemcached/issues/72>`_
  and `gh #47 <https://github.com/awesomized/libmemcached/issues/47>`_\ :
  memcached_return_t docs.
* Fix `gh #62 <https://github.com/awesomized/libmemcached/issues/62>`_\ :
  uint32_t overflow cause busy loop.
* Removed restriction of UDP+IPv6.
* Fix SERVER_ERROR_MEMORY_ALLOCATION_FAILURE:
  recognize more strings returned by the server.
* Fix `gh #13 <https://github.com/awesomized/libmemcached/issues/13>`_\ :
  reset continuum counter after freeing them.
* Fix `gh #14 <https://github.com/awesomized/libmemcached/issues/14>`_
  and `gh #17 <https://github.com/awesomized/libmemcached/issues/17>`_\ :
  SASL: AUTH_CONTINUE was considered a failure and caused IO reset.
* Fix `gh #25 <https://github.com/awesomized/libmemcached/issues/25>`_\ :
  hashkit/murmur3 unavailable.
* Fix missing handling of EAGAIN for non-blocking unix domain socket.
* Fix `gh #35 <https://github.com/awesomized/libmemcached/issues/35>`_\ :
  handling of BEHAVIOR_REMOVE_FAILED_SERVERS.
* Fix `gh #41 <https://github.com/awesomized/libmemcached/issues/41>`_\ :
  ensure stable sort on continuum host key collision.
* Fix `gh #42 <https://github.com/awesomized/libmemcached/issues/42>`_\ :
  MEMCACHED_MAX_BUFFER docs.
* Fix `gh #43 <https://github.com/awesomized/libmemcached/issues/43>`_\ :
  libmemcached_configuration docs.
* Fix `gh #46 <https://github.com/awesomized/libmemcached/issues/46>`_\ :
  clarification on millisecond timeout docs.
* Fix `gh #50 <https://github.com/awesomized/libmemcached/issues/50>`_\ :
  memcached_fetch_result() can return previously returned data.
* Fix `gh #53 <https://github.com/awesomized/libmemcached/issues/53>`_\ :
  stack overflow in memcached_fetch_result().
* Fix `gh #57 <https://github.com/awesomized/libmemcached/issues/57>`_\ :
  include <inttypes.h> vs :raw-html-m2r:`<cinttypes>`
* Fix `gh #58 <https://github.com/awesomized/libmemcached/issues/58>`_\ :
  more specific error messages when connect() fails.
* Fix `gh #59 <https://github.com/awesomized/libmemcached/issues/59>`_\ :
  bin/memcat: typo in "No servers provied".
* Fix `gh #77 <https://github.com/awesomized/libmemcached/issues/77>`_\ :
  undeclared UINT64_C in ketama.cc.
* Fix `gh #12 <https://github.com/awesomized/libmemcached/issues/12>`_\ :
  never reconnects after connection reset (binary protocol).
* Fix `gh #49 <https://github.com/awesomized/libmemcached/issues/49>`_\ :
  assertion memcached_failed(rc) failed in memcached_send_ascii().
* Fix `gh #67 <https://github.com/awesomized/libmemcached/issues/67>`_\ :
  get returns NOTFOUND on timeout.
* Fix `gh #76 <https://github.com/awesomized/libmemcached/issues/76>`_\ :
  memcached_touch() crashes when expiration=-1 (ASCII only).
* Fix `gh #23 <https://github.com/awesomized/libmemcached/issues/23>`_\ :
  build fails with bison 2.3.
* Fix memaslap: build fails with newer compiler versions.
* Fix usage of strerror_r() implementations returning pointer to char.
* Fix pipelining commands with memcached >= 1.6.
* Fix memcached_stat_get_value(): buffer overflow.
* Fix memcached_stat(): undefined behavior due to unintialized memcached_return_t.
* Fix SASL tests: requires SASL_PWDB_CONF.
* Fix bin/memaslap to idnentify itself as memaslap instead of memslap.
* Fix bin/memcapable to work with memcached >= 1.6.
* Fix murmur and murmur3 hashes on big endian platforms.
* Fix `gh #82 <https://github.com/awesomized/libmemcached/issues/82>`_\ ,
  `gh #64 <https://github.com/awesomized/libmemcached/issues/64>`_ and
  `gh #21 <https://github.com/awesomized/libmemcached/issues/21>`_\ :
  clarify documentation on replication.
* Fix `gh #95 <https://github.com/awesomized/libmemcached/issues/95>`_\ :
  MEMCACHED_CALLBACK_GET_FAILURE and MEMCACHED_BEHAVIOR_BUFFER_REQUESTS
* Fix bin/memcat to output flags if requested with ``--flag``.
* Fix `gh #68 <https://github.com/awesomized/libmemcached/issues/68>`_\ :
  Windows support.

----

See :doc:`ChangeLog-1.0 <./ChangeLog-1.0>` for changes prior v1.1.
