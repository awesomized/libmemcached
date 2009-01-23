EXTRA_DIST = libmemcached_probes.d memcached/README.txt

BUILT_SOURCES= @DTRACE_HEADER@

INCLUDES = -I$(top_builddir)
LIBS = @LIBS@

EXTRA_HEADERS = 
DTRACE =                @DTRACE@
DTRACEFLAGS =           @DTRACEFLAGS@

noinst_HEADERS = libmemcached_probes.h \
		 memcached_io.h \
                 common.h \
                 memcached/protocol_binary.h @DTRACE_HEADER@

pkginclude_HEADERS= memcached.h \
		    memcached.hh \
		    libmemcached_config.h \
		    memcached_constants.h \
		    memcached_get.h \
		    memcached_result.h \
		    memcached_server.h \
		    memcached_storage.h \
		    memcached_string.h \
		    memcached_types.h \
		    memcached_watchpoint.h

lib_LTLIBRARIES = libmemcached.la

libmemcached_la_SOURCES = crc.c \
			  hsieh_hash.c \
			  memcached.c \
			  memcached_auto.c \
			  memcached_analyze.c \
			  memcached_behavior.c \
			  memcached_callback.c \
			  memcached_connect.c \
			  memcached_delete.c \
			  memcached_do.c \
			  memcached_fetch.c \
			  memcached_fetch_execute.c \
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
                          byteorder.c

libmemcached_la_DEPENDENCIES = @DTRACE_OBJ@
libmemcached_la_LIBADD = @DTRACE_OBJ@
libmemcached_la_LDFLAGS = -version-info $(MEMCACHED_LIBRARY_VERSION)

dtrace_probes.h: libmemcached_probes.d
	$(DTRACE) $(DTRACEFLAGS) -o dtrace_probes.tmp -h -s libmemcached_probes.d
	sed "s/#include <unistd.h>//g" dtrace_probes.tmp > dtrace_probes.h
	rm dtrace_probes.tmp


# So libtool doesn't support dtrace, but just copy one of the existing
# lo-file and replace the file name ;-) 
libmemcached_probes.lo: libmemcached_probes.o
	sed "s,jenkins_hash,libmemcached_probes,g" jenkins_hash.lo > libmemcached_probes.lo

libmemcached_probes.o: $(libmemcached_la_OBJECTS)
	$(DTRACE) $(DTRACEFLAGS) -o .libs/libmemcached_probes.o -G -s libmemcached_probes.d `grep pic_object *.lo | cut -f 2 -d\' | grep -v non_pic_object`
	$(DTRACE) $(DTRACEFLAGS) -o libmemcached_probes.o -G -s libmemcached_probes.d `grep non_pic_object *.lo | cut -f 2 -d\' `

