INCLUDES = include
SUBDIRS = docs include lib src tests

test: all
	cd tests; make test; cd ..

test-extended: all
	cd tests; make test-extended; cd ..

valgrind:
	cd tests; make valgrind; cd ..

valgrind-extended:
	cd tests; make valgrind-extended; cd ..


