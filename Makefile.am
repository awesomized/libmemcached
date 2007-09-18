INCLUDES = include
SUBDIRS = docs include lib src tests

test: all
	cd tests; make test; cd ..

ltest: all 
	./tests/testapp output.txt

valgrind:
	cd tests; make valgrind; cd ..


