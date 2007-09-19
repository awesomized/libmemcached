INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

bin_PROGRAMS = memcat memcp

memcat_SOURCES = memcat.c
memcat_LDADD = $(LDADDS)

memcp_SOURCES = memcp.c
memcp_LDADD = $(LDADDS)
