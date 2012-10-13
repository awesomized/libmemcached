# vim:ft=automake

ACLOCAL_AMFLAGS = -I m4 -I libtest/m4 --install -Wall -Werror

# includes append to these:
SUFFIXES =
.PHONY =
TESTS =
CLEANFILES =
DISTCLEANFILES =
bin_PROGRAMS =
noinst_HEADERS =
lib_LTLIBRARIES =
man_MANS =
noinst_LTLIBRARIES =
noinst_PROGRAMS =
include_HEADERS =
nobase_include_HEADERS =
check_PROGRAMS =
EXTRA_HEADERS =
BUILT_SOURCES=
EXTRA_DIST=

#includes additional rules from aminclude.am
@INC_AMINCLUDE@

DISTCLEANFILES+= aminclude.am

EXTRA_DIST+= ${srcdir}/m4/pandora_*.m4
EXTRA_DIST+= .quickly
EXTRA_DIST+= README.FIRST
EXTRA_DIST+= README.win32

aclocaldir= $(datadir)/aclocal
dist_aclocal_DATA=

include libtest/include.am

include libhashkit-1.0/include.am
include libmemcached-1.0/include.am
include libmemcachedprotocol-0.0/include.am
include libmemcachedutil-1.0/include.am

include clients/include.am
include docs/include.am
include man/include.am
include example/include.am
include libhashkit/include.am
include libmemcached/include.am
include libmemcachedutil/include.am
include libmemcachedprotocol/include.am
include libmemcachedinternal/include.am
include libmemcachedinternal/util/include.am
include poll/include.am
include rpm/include.am
include support/include.am
include tests/include.am
include util/include.am
include win32/include.am

include m4/include.am

TESTS += ${check_PROGRAMS}

EXTRA_DIST+= ${man_MANS}

merge-clean:
	@find ./ | $(GREP) \.gcda | xargs rm -f
	@find ./ | $(GREP) \.gcno | xargs rm -f
	@find ./ | $(GREP) \.gz | xargs rm -f
	@find ./ | $(GREP) \.moved | xargs rm -r -f
	@find ./ | $(GREP) \\.orig | xargs rm -f
	@find ./ | $(GREP) \.rej | xargs rm -f
	@rm -f *\.rpm
	@find ./ | $(GREP) \.THIS | xargs rm -f
	@find ./ | $(GREP) \.OTHER | xargs rm -f
	@find ./ | $(GREP) \.BASE | xargs rm -f
	@find ./ | $(GREP) \~$$ | xargs rm -f
	@echo "Files that need to be either removed or checked in:"
	@bzr unknowns

clean-local:
	-rm -rf docs/_build docs/doctrees

lcov: lcov-clean check
	@echo
	@echo "	------------------------------------------------------"
	@echo "Make sure ./configure was run with '--enable-coverage'"
	@echo "------------------------------------------------------"
	@echo
	@cd libmemcached && lcov --capture --directory . --base-directory .. --output-file lcov.out
	@genhtml -o lcov --function-coverage -t libmemcached libmemcached/lcov.out

lcov-clean: clean
	@rm -rf lcov */lcov.out
	@find . -name '*.gcno' | xargs rm -f
	@find . -name '*.gcda' | xargs rm -f

DISTCLEANFILES+= config/top.h

install-website: html-local
	@rm -r -f /var/www/libmemcached/*
	@cp -r docs/html /var/www/libmemcached
