INCLUDES = 
SUBDIRS = docs libmemcached tests support clients
EXTRA_dist = README.FIRST

test: all
	@(cd tests; ${MAKE} test)

test-extended: all
	@(cd tests; ${MAKE} test-extended)

valgrind:
	@(cd tests; ${MAKE} valgrind)

test-no-outputdiff:
	@(cd tests; ${MAKE} test-no-outputdiff)

rpm: all dist
	cp libmemcached-$(VERSION).tar.gz /home/brian/rpm/SOURCES/
	rpmbuild -ba support/libmemcached.spec
	cp /home/brian/rpm/RPMS/x86_64/libmemcached-$(VERSION)*.rpm .
	cp /home/brian/rpm/SRPMS/libmemcached-$(VERSION)*.rpm .
