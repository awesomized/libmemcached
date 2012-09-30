#!/bin/bash
# 
# Copyright (C) 2012 Brian Aker
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# 
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# 
#     * The names of its contributors may not be used to endorse or
# promote products derived from this software without specific prior
# written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

die() { echo "$@"; exit 1; }

command_exists () {
  type "$1" &> /dev/null ;
}

run() {
  echo $TESTS_ENVIRONMENT
  echo "\`$@' $ARGS"
  $@ $ARGS
} 

bootstrap() {
  if [ -d .git ]
  then
    AUTORECONF_FLAGS="--install --force --verbose -Wall -Werror"
  elif [ -d .bzr ]
  then
    AUTORECONF_FLAGS="--install --force --verbose -Wall -Werror"
  elif [ -d .svn ]
  then
    AUTORECONF_FLAGS="--install --force --verbose -Wall -Werror"
  elif [ -d .hg ]
  then
    AUTORECONF_FLAGS="--install --force --verbose -Wall -Werror"
  else
    AUTORECONF_FLAGS="--install --force --verbose -Wall"
  fi

  LIBTOOLIZE_FLAGS="--force --verbose"

  if [ $(uname) = "Darwin" ]
  then
    LIBTOOLIZE=glibtoolize
  elif [ -z "$LIBTOOLIZE" ]
  then 
    LIBTOOLIZE=libtoolize
  fi

  AUTOMAKE=automake
  AUTORECONF=autoreconf

  AUTOMAKE_FLAGS=--add-missing


  # Set ENV DEBUG in order to enable debugging
  if [ -n "$DEBUG" ]
  then 
    DEBUG="--enable-debug"
  fi

  # Set ENV ASSERT in order to enable assert
  if [ -n "$ASSERT" ]
  then 
    ASSERT="--enable-assert"
  fi

  # Set ENV VALGRIND in order to enable assert
  if [ -n "$VALGRIND" ] && command_exists valgrind
  then 
    VALGRIND="valgrind --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE"
    TESTS_ENVIRONMENT="$VALGRIND"
  fi

  # Set ENV MAKE in order to override "make"
  if [ -z "$MAKE" ]
  then 
    MAKE="make"
  fi

  # Set ENV MAKE_J in order to override "-j2"
  if [ -z "$MAKE_J" ]
  then
    MAKE_J="-j2"
  fi

  # Set ENV PREFIX in order to set --prefix for ./configure
  if [ -n "$PREFIX" ]
  then 
    PREFIX="--prefix=$PREFIX"
  fi

  if [ -f Makefile ]
  then
    $MAKE $MAKE_J maintainer-clean
  fi

  run $LIBTOOLIZE $LIBTOOLIZE_FLAGS || die "Can't execute $LIBTOOLIZE"
  run $AUTOMAKE $AUTOMAKE_FLAGS || die "Can't execute $AUTORECONF"
  run $AUTORECONF $AUTORECONF_FLAGS || die "Can't execute $AUTORECONF"

  # If we are executing on OSX use CLANG, otherwise only use it if we find it in the ENV
  if [ $(uname) = "Darwin" ]
  then
    CC=clang CXX=clang++ ./configure $DEBUG $ASSERT $PREFIX || die "configure failed to run"
  else
    ./configure $DEBUG $ASSERT $PREFIX || die "configure failed to run"
  fi

  if [[ -n "$TESTS_ENVIRONMENT" ]] && [[ -f libtool ]]
  then
    TESTS_ENVIRONMENT="./libtool --mode=execute $TESTS_ENVIRONMENT"
    export TESTS_ENVIRONMENT
  fi

  if [ -f docs/conf.py ]
  then 
    run $MAKE $MAKE_J man || die "Can't execute make"
  fi

  # Set ENV MAKE_TARGET in order to override default of "all"
  if [ -z "$MAKE_TARGET" ]
  then 
    MAKE_TARGET="all"
  fi

  run $MAKE $MAKE_J $MAKE_TARGET || die "Can't execute make"
}

bootstrap
