INCLUDES = include
SUBDIRS = docs include lib src tests support

test: all
	cd tests; make test; cd ..

test-extended: all
	cd tests; make test-extended; cd ..

valgrind:
	cd tests; make valgrind; cd ..

valgrind-extended:
	cd tests; make valgrind-extended; cd ..


rpm: all dist
	cp $(DISTVNAME).tar$(SUFFIX) /usr/src/redhat/SOURCES
	rpm -ba support/libmemcached.spec
	cp /usr/src/redhat/RPMS/i386/$(DISTVNAME)*.i386.rpm .
	cp /usr/src/redhat/SRPMS/$(DISTVNAME)*.src.rpm .
