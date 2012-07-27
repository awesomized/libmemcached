#!/bin/bash

if test -f configure; then make clean; make merge-clean; make distclean; fi;

rm -r -f autom4te.cache/ config.h config.log config.status 
./config/autorun.sh
if [ $(uname) = "Darwin" ];
then
  ./configure CC=clang CXX=clang++ --enable-assert
else
  ./configure --enable-assert
fi
make
