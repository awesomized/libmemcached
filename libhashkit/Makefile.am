# HashKit
# Copyright (C) 2009 Brian Aker
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in the parent directory for full text.

lib_LTLIBRARIES= libhashkit.la

libhashkitincludedir= ${includedir}/libhashkit

dist_libhashkitinclude_HEADERS= \
	algorithm.h \
	behavior.h \
        hashkit.h \
	strerror.h \
        types.h \
        visibility.h

noinst_HEADERS= \
		common.h

libhashkit_la_SOURCES= \
        crc32.c \
        behavior.c \
        default.c \
        fnv.c \
        hashkit.c \
        jenkins.c \
        ketama.c \
        md5.c \
        murmur.c \
	strerror.c

if INCLUDE_HSIEH_SRC
libhashkit_la_SOURCES+= hsieh.c
endif

libhashkit_la_CFLAGS= \
        ${AM_CFLAGS} \
        -DBUILDING_HASHKIT

libhashkit_la_LDFLAGS= \
	$(LIBM) \
        -version-info $(HASHKIT_LIBRARY_VERSION)
