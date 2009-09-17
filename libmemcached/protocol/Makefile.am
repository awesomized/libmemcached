EXTRA_DIST= libmemcachedprotocol.ver

lib_LTLIBRARIES=libmemcachedprotocol.la

noinst_HEADERS = common.h 

libmemcachedprotocol_la_SOURCES= protocol_handler.c cache.c pedantic.c
libmemcachedprotocol_la_LDFLAGS= -version-info $(MEMCACHEDPROTOCOL_LIBRARY_VERSION) $(LD_PROTOCOL_VERSION_SCRIPT)
