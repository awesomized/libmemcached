noinst_PROGRAMS = memcached_light

memcached_light_SOURCES= memcached_light.c \
                         storage.c storage.h \
                         interface_v0.c \
                         interface_v1.c
memcached_light_LDADD= $(top_builddir)/libmemcached/protocol/libmemcachedprotocol.la
memcached_light_DEPENDENCIES= $(top_builddir)/libmemcached/protocol/libmemcachedprotocol.la

if BUILD_BYTEORDER
memcached_light_LDADD+=$(top_builddir)/libmemcached/libbyteorder.la
memcached_light_DEPENDENCIES+= $(top_builddir)/libmemcached/libbyteorder.la
endif
