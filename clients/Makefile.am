INCLUDES = -I$(top_builddir)
LDADDS = $(top_builddir)/libmemcached/libmemcached.la

bin_PROGRAMS = memcat memcp memstat memrm memflush memslap memerror

noinst_HEADERS = client_options.h \
		utilities.h \
		generator.h \
		execute.h 

memcat_SOURCES = memcat.c utilities.c
memcat_LDADD = $(LDADDS)
memcat_LDFLAGS = -rpath $(pkglibdir)

memcp_SOURCES = memcp.c utilities.c
memcp_LDADD = $(LDADDS)
memcp_LDFLAGS = -rpath $(pkglibdir)

memstat_SOURCES = memstat.c utilities.c
memstat_LDADD = $(LDADDS)
memstat_LDFLAGS = -rpath $(pkglibdir)

memrm_SOURCES = memrm.c utilities.c
memrm_LDADD = $(LDADDS)
memrm_LDFLAGS = -rpath $(pkglibdir)

memflush_SOURCES = memflush.c utilities.c
memflush_LDADD = $(LDADDS)
memflush_LDFLAGS = -rpath $(pkglibdir)


memerror_SOURCES = memerror.c utilities.c
memerror_LDADD = $(LDADDS)
memerror_LDFLAGS = -rpath $(pkglibdir)

memslap_SOURCES = memslap.c utilities.c generator.c execute.c
memslap_LDADD = $(LDADDS) -lpthread
memslap_LDFLAGS = -rpath $(pkglibdir)
