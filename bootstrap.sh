#!/bin/bash

if test -f configure; then make clean; make merge-clean; make distclean; fi;

rm -r -f autom4te.cache
./config/autorun.sh
./configure
make
