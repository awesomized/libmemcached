EXTRA_DIST= libmemcachedprotocol.ver

lib_LTLIBRARIES=libmemcachedprotocol.la

noinst_HEADERS = common.h

libmemcachedprotocol_la_SOURCES= protocol_handler.c cache.c pedantic.c
libmemcachedprotocol_la_LDFLAGS= -version-info $(MEMCACHEDPROTOCOL_LIBRARY_VERSION) $(LD_PROTOCOL_VERSION_SCRIPT)

if BUILD_BYTEORDER
libmemcachedprotocol_la_LIBADD=$(top_builddir)/libmemcached/libbyteorder.la
libmemcachedprotocol_la_DEPENDENCIES= $(top_builddir)/libmemcached/libbyteorder.la
endif
