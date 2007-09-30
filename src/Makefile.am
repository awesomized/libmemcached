INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

bin_PROGRAMS = memcat memcp memstat memrm memflush memslap

noinst_HEADERS = client_options.h utilities.h

memcat_SOURCES = memcat.c utilities.c
memcat_LDADD = $(LDADDS)

memcp_SOURCES = memcp.c utilities.c
memcp_LDADD = $(LDADDS)

memstat_SOURCES = memstat.c utilities.c
memstat_LDADD = $(LDADDS)

memrm_SOURCES = memrm.c utilities.c
memrm_LDADD = $(LDADDS)

memflush_SOURCES = memflush.c utilities.c
memflush_LDADD = $(LDADDS)

memslap_SOURCES = memslap.c utilities.c generator.c
memslap_LDADD = $(LDADDS) -lpthread
