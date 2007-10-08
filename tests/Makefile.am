INCLUDES = -I$(top_builddir)/include
LDADDS = ../lib/libmemcached.la

EXTRA_DIST = output.res output2.res\
		r/memcat.res\
		r/memcp.res\
		r/memrm.res\
		r/memslap.res\
		r/memstat.res\
		t/memcat.test\
		t/memcp.test\
		t/memrm.test\
		t/memslap.test\
		t/memstat.test

LIBS = 

noinst_PROGRAMS = testapp

testapp_SOURCES = test.c
testapp_LDADD = $(LDADDS)

record:
	./testapp > output.res
	sh t/memcat.test > r/memcat.res
	sh t/memcp.test > r/memcp.res
	sh t/memrm.test > r/memrm.res
	sh t/memslap.test > r/memslap.res
	sh t/memstat.test > r/memstat.res

record-extended:
	./testapp extended > output2.res

test: testapp
	./testapp > output.cmp
	diff output.res output.cmp
	sh t/memcat.test > r/memcat.cmp
	diff r/memcat.res r/memcat.cmp
	sh t/memcp.test > r/memcp.cmp
	diff r/memcp.res r/memcp.cmp
	sh t/memrm.test > r/memrm.cmp
	diff r/memrm.res r/memrm.cmp
	sh t/memslap.test > r/memslap.cmp
	diff r/memslap.res r/memslap.cmp
	sh t/memstat.test > r/memstat.cmp
	diff r/memstat.res r/memstat.cmp

test-extended: testapp
	./testapp extended > output.cmp
	diff output2.res output.cmp

valgrind:
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  testapp

valgrind-extended:
	libtool --mode=execute valgrind --leak-check=yes --show-reachable=yes  testapp extended
