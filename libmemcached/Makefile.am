EXTRA_DIST = \
	    libmemcached.ver \
	    libmemcached_probes.d \
	    memcached/README.txt \
	    memcached_configure.h.in \
	    protocol/libmemcachedprotocol.ver \
	    util/libmemcachedutil.ver


EXTRA_HEADERS =
BUILT_SOURCES=

noinst_HEADERS = libmemcached_probes.h \
                 io.h \
                 internal.h \
                 common.h \
                 memcached/protocol_binary.h \
                 protocol/common.h \
                 protocol/ascii_handler.h \
                 protocol/binary_handler.h

pkginclude_HEADERS= \
		    behavior.h \
		    callback.h \
		    constants.h \
		    exception.hpp \
		    get.h \
		    memcached.h \
		    memcached.hpp \
		    memcached_configure.h \
		    protocol_handler.h \
		    result.h \
		    server.h \
		    storage.h \
		    string.h \
		    types.h \
		    visibility.h \
		    watchpoint.h

nobase_pkginclude_HEADERS = \
			    protocol/cache.h \
			    protocol/callback.h


libmemcachedprotocol_la_SOURCES =  \
				    protocol/ascii_handler.c \
				    protocol/binary_handler.c \
				    protocol/cache.c \
				    protocol/pedantic.c \
				    protocol/protocol_handler.c

libmemcachedprotocol_la_LDFLAGS= ${AM_LDFLAGS} -version-info 0:0:0 

lib_LTLIBRARIES = libmemcached.la libmemcachedprotocol.la
noinst_LTLIBRARIES = libmemcachedcallbacks.la
libmemcachedcallbacks_la_CFLAGS = ${AM_CFLAGS} ${NO_STRICT_ALIASING}
libmemcachedcallbacks_la_SOURCES = callback.c

libmemcached_la_CFLAGS= ${AM_CFLAGS} ${NO_CONVERSION}
libmemcached_la_SOURCES = \
			  allocators.c \
			  analyze.c \
			  auto.c \
			  behavior.c \
			  connect.c \
			  crc.c \
			  delete.c \
			  do.c \
			  dump.c \
			  fetch.c \
			  flush.c \
			  flush_buffers.c \
			  get.c \
			  hash.c \
			  hosts.c \
			  io.c \
			  jenkins_hash.c \
			  key.c \
			  md5.c \
			  memcached.c \
			  murmur_hash.c \
			  parse.c \
			  purge.c \
			  quit.c \
			  response.c \
			  result.c \
			  server.c \
			  stats.c \
			  storage.c \
			  strerror.c \
			  string.c \
			  verbosity.c \
			  version.c


if INCLUDE_HSIEH_SRC
libmemcached_la_SOURCES += hsieh_hash.c
endif

libmemcached_la_DEPENDENCIES= libmemcachedcallbacks.la
libmemcached_la_LIBADD= $(LIBM) libmemcachedcallbacks.la
libmemcached_la_LDFLAGS= ${AM_LDFLAGS} -version-info 3:0:0

if BUILD_LIBMEMCACHEDUTIL
pkginclude_HEADERS+= memcached_util.h pool.h
lib_LTLIBRARIES+= libmemcachedutil.la
endif

libmemcachedutil_la_SOURCES= util/pool.c
libmemcachedutil_la_LIBADD= libmemcached.la
libmemcachedutil_la_LDFLAGS= ${AM_LDFLAGS} -version-info 0:0:0
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

