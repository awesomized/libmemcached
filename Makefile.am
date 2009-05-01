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

fedora:
	cp libmemcached-$(VERSION).tar.gz /home/brian/rpmbuild/SOURCES/
	rpmbuild -ba support/libmemcached.spec
	cp /home/brian/rpmbuild/RPMS/x86_64/libmemcached-$(VERSION)*.rpm .
	cp /home/brian/rpmbuild/SRPMS/libmemcached-$(VERSION)*.rpm .

generic:
	cp libmemcached-$(VERSION).tar.gz /home/brian/rpmbuild/SOURCES/
	rpmbuild -ba support/libmemcached.spec
	cp /home/brian/rpmbuild/RPMS/x86_64/libmemcached-$(VERSION)*.rpm .
	cp /home/brian/rpmbuild/SRPMS/libmemcached-$(VERSION)*.rpm .

rpm: all dist generic fedora
