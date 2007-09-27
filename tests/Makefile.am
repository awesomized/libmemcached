INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

EXTRA_DIST = output.res output2.res

LIBS = 

noinst_PROGRAMS = testapp

testapp_SOURCES = test.c
testapp_LDADD = $(LDADDS)

record:
	./testapp > output.res

record-extended:
	./testapp extended > output2.res

test: testapp
	./testapp > output.cmp
	diff output.res output.cmp

test-extended: testapp
	./testapp extended > output.cmp
	diff output2.res output.cmp

valgrind:
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  testapp

valgrind-extended:
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  testapp extended
