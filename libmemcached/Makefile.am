EXTRA_DIST = libmemcached.ver \
             libmemcached_probes.d \
             memcached/README.txt \
             memcached_configure.h.in \
             protocol/libmemcachedprotocol.ver \
             util/libmemcachedutil.ver


EXTRA_HEADERS =
BUILT_SOURCES=

noinst_HEADERS = libmemcached_probes.h \
                 memcached_io.h \
                 memcached_internal.h \
                 common.h \
                 memcached/protocol_binary.h \
                 protocol/common.h \
                 protocol/ascii_handler.h \
                 protocol/binary_handler.h

pkginclude_HEADERS= memcached.h \
		    memcached.hpp \
		    exception.hpp \
                    memcached_configure.h \
		    memcached_constants.h \
		    memcached_get.h \
		    memcached_result.h \
		    memcached_server.h \
		    memcached_storage.h \
		    memcached_string.h \
		    memcached_types.h \
		    memcached_watchpoint.h \
                    protocol_handler.h \
		    visibility.h

nobase_pkginclude_HEADERS=protocol/cache.h \
                          protocol/callback.h


libmemcachedprotocol_la_SOURCES= protocol/ascii_handler.c \
                                 protocol/binary_handler.c \
                                 protocol/cache.c \
                                 protocol/pedantic.c \
                                 protocol/protocol_handler.c

libmemcachedprotocol_la_LDFLAGS= -version-info $(MEMCACHEDPROTOCOL_LIBRARY_VERSION) $(LD_PROTOCOL_VERSION_SCRIPT)

lib_LTLIBRARIES = libmemcached.la libmemcachedprotocol.la
noinst_LTLIBRARIES = libmemcachedcallbacks.la
libmemcachedcallbacks_la_CFLAGS = ${AM_CFLAGS} ${NO_STRICT_ALIASING}
libmemcachedcallbacks_la_SOURCES = memcached_callback.c

libmemcached_la_CFLAGS= ${AM_CFLAGS} ${NO_CONVERSION}
libmemcached_la_SOURCES = crc.c \
			  memcached.c \
			  memcached_auto.c \
			  memcached_analyze.c \
			  memcached_behavior.c \
			  memcached_connect.c \
			  memcached_delete.c \
			  memcached_do.c \
			  memcached_dump.c \
			  memcached_fetch.c \
			  memcached_flush.c \
			  memcached_get.c \
			  memcached_hash.c \
			  memcached_hosts.c \
			  memcached_io.c \
			  memcached_purge.c \
                          memcached_flush_buffers.c \
			  md5.c \
			  memcached_key.c \
			  memcached_quit.c \
			  memcached_parse.c \
			  memcached_response.c \
			  memcached_result.c \
			  memcached_server.c \
			  memcached_storage.c \
			  memcached_string.c \
			  memcached_stats.c \
			  memcached_strerror.c \
			  memcached_verbosity.c \
			  memcached_version.c \
			  murmur_hash.c \
			  jenkins_hash.c \
                          memcached_allocators.c


if INCLUDE_HSIEH_SRC
libmemcached_la_SOURCES += hsieh_hash.c
endif

libmemcached_la_DEPENDENCIES= libmemcachedcallbacks.la
libmemcached_la_LIBADD= $(LIBM) libmemcachedcallbacks.la
libmemcached_la_LDFLAGS = -version-info $(MEMCACHED_LIBRARY_VERSION) $(LD_VERSION_SCRIPT) $(LIBM)

if BUILD_LIBMEMCACHEDUTIL
pkginclude_HEADERS+= memcached_util.h memcached_pool.h
lib_LTLIBRARIES+= libmemcachedutil.la
endif

libmemcachedutil_la_SOURCES= util/memcached_pool.c
libmemcachedutil_la_LDFLAGS= -version-info $(MEMCACHEDUTIL_LIBRARY_VERSION) $(LD_UTIL_VERSION_SCRIPT)
libmemcachedutil_la_LIBADD= libmemcached.la
libmemcachedutil_la_DEPENDENCIES=libmemcached.la

if BUILD_BYTEORDER
noinst_LTLIBRARIES += libbyteorder.la
libbyteorder_la_SOURCES= byteorder.c byteorder.h
libmemcached_la_LIBADD += libbyteorder.la
libmemcached_la_DEPENDENCIES+= libbyteorder.la
libmemcachedprotocol_la_LIBADD=libbyteorder.la
libmemcachedprotocol_la_DEPENDENCIES=libbyteorder.la
endif

if HAVE_DTRACE
BUILT_SOURCES+= dtrace_probes.h
libmemcached_la_SOURCES += libmemcached_probes.d
endif

if DTRACE_NEEDS_OBJECTS
libmemcached_la_DEPENDENCIES += libmemcached_probes.o
endif

SUFFIXES= .d

dtrace_probes.h: libmemcached_probes.d
	$(DTRACE) $(DTRACEFLAGS) -h -o dtrace_probes.h -s libmemcached_probes.d

libmemcached_probes.o: libmemcached_probes.d $(libmemcached_la_OBJECTS)
	$(DTRACE) $(DTRACEFLAGS) -o .libs/libmemcached_probes.o -G -s libmemcached_probes.d `grep '^pic_object' *.lo | cut -f 2 -d\'`
	$(DTRACE) $(DTRACEFLAGS) -o libmemcached_probes.o -G -s libmemcached_probes.d `grep non_pic_object *.lo | cut -f 2 -d\' `

