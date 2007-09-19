INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

bin_PROGRAMS = memcat memcp memstat

noinst_includes= client_options.h utilities.h

memcat_SOURCES = memcat.c utilities.c
memcat_LDADD = $(LDADDS)

memcp_SOURCES = memcp.c utilities.c
memcp_LDADD = $(LDADDS)

memstat_SOURCES = memstat.c utilities.c
memstat_LDADD = $(LDADDS)
