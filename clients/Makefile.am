LDADDS = libutilities.la $(top_builddir)/libmemcached/libmemcached.la

bin_PROGRAMS = memcat memdump memcp memstat memrm memflush memerror memcapable

if HAVE_LIBEVENT
  bin_PROGRAMS += memslap
endif

noinst_HEADERS = \
		client_options.h \
		execute.h \
		generator.h \
		ms_atomic.h \
		ms_conn.h \
		ms_memslap.h \
		ms_setting.h \
		ms_sigsegv.h \
		ms_stats.h \
		ms_task.h \
		ms_thread.h \
		utilities.h

noinst_LTLIBRARIES= libutilities.la libgenexec.la

libutilities_la_SOURCES= utilities.c
libgenexec_la_SOURCES= generator.c execute.c

memcat_SOURCES = memcat.c
memcat_LDADD = $(LDADDS)

memcp_SOURCES = memcp.c
memcp_LDADD = $(LDADDS)

memdump_SOURCES = memdump.c
memdump_LDADD = $(LDADDS)

memstat_SOURCES = memstat.c
memstat_LDADD = $(LDADDS)

memrm_SOURCES = memrm.c
memrm_LDADD = $(LDADDS)

memflush_SOURCES = memflush.c
memflush_LDADD = $(LDADDS)

memerror_SOURCES = memerror.c
memerror_LDADD = $(LDADDS)

memslap_SOURCES = \
		memslap.c \
		ms_conn.c \
		ms_setting.c \
		ms_sigsegv.c \
		ms_stats.c \
		ms_task.c \
		ms_thread.c
memslap_LDADD = $(LTLIBEVENT) libgenexec.la $(LDADDS)

memcapable_SOURCES = memcapable.c
if BUILD_BYTEORDER
memcapable_LDADD=$(top_builddir)/libmemcached/libbyteorder.la
endif

test-start-server:
	memflush --servers=localhost
	memcp --servers=localhost /etc/services
	memcat --servers=localhost /etc/services
	memrm --servers=localhost /etc/services
	memstat --servers=localhost
	memslap --servers=localhost
	memslap --servers=localhost --concurrency=10
	memslap --servers=localhost --concurrency=10 --initial-load=1000
	memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10
	memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10 --test=get
	memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10 --test=set
	memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10 --test=set --non-blocking

valgrind:
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost --concurrency=10
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost --concurrency=10 --initial-load=1000
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10 --test=get
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10 --test=set
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  memslap --servers=localhost --concurrency=10 --initial-load=1000 --execute-number=10 --test=set --non-blocking
