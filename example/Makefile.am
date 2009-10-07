noinst_PROGRAMS = memcached_light

memcached_light_SOURCES= memcached_light.c \
                         memcached_light.h \
                         storage.h \
                         interface_v0.c \
                         interface_v1.c
memcached_light_LDADD= $(top_builddir)/libmemcached/libmemcachedprotocol.la $(LIBINNODB)
memcached_light_DEPENDENCIES= $(top_builddir)/libmemcached/libmemcachedprotocol.la

if BUILD_BYTEORDER
memcached_light_LDADD+= $(top_builddir)/libmemcached/libbyteorder.la
memcached_light_DEPENDENCIES+= $(top_builddir)/libmemcached/libbyteorder.la
endif

if HAVE_LIBINNODB
memcached_light_SOURCES+= storage_innodb.c
else
memcached_light_SOURCES+= storage.c
endif
