INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

LIBS = 

noinst_PROGRAMS = testapp

testapp_SOURCES = test.c
testapp_LDADD = $(LDADDS)

record:
	./testapp output.txt > output.res
test: testapp
	./testapp output.txt > output.cmp
	diff output.res output.cmp

valgrind:
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  testapp output.txt
