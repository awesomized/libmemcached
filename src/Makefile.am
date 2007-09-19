INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

bin_PROGRAMS = memcat memcp memrm

memcat_SOURCES = memcat.c
memcat_LDADD = $(LDADDS)

memcp_SOURCES = memcp.c
memcp_LDADD = $(LDADDS)

memrm_SOURCES = memrm.c
memrm_LDADD = $(LDADDS)
