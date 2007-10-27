INCLUDES = include
SUBDIRS = docs include lib src tests support
EXTRA_dist = README.FIRST

test: all
	cd tests; make test; cd ..

test-extended: all
	cd tests; make test-extended; cd ..

valgrind:
	cd tests; make valgrind; cd ..

rpm: all dist
	cp libmemcached-$(VERSION).tar.gz /home/brian/rpmbuild/SOURCES/
	rpmbuild -ba support/libmemcached.spec
	cp /home/brian/rpmbuild/RPMS/x86_64/libmemcached-$(VERSION)*.rpm .
	cp /home/brian/rpmbuild/SRPMS/libmemcached-$(VERSION)*.rpm .
