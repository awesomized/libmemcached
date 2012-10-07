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

determine_target_platform () {
  if [ $(uname) = "Darwin" ]; then
    PLATFORM="darwin"
  elif [[ -f "/etc/fedora-release" ]]; then 
    PLATFORM="fedora"
  elif [[ -f "/etc/lsb-release" ]]; then 
    debian_version=`cat /etc/lsb-release | grep DISTRIB_CODENAME | awk -F= ' { print $2 } '`
    case $debian_version in
      precise)
        PLATFORM="precise"
        ;;
      *)
        ;;
    esac
  fi

  PLATFORM_VERSION=`uname -r`

  if [ "$PLATFORM" == "unknown" ]; then 
    PLATFORM=`uname -s`
  fi

  TARGET_PLATFORM="$PLATFORM-$PLATFORM_VERSION"
}

configure_target_platform () {
  # If we are executing on OSX use CLANG, otherwise only use it if we find it in the ENV
  case $TARGET_PLATFORM in
    darwin-*)
      CC=clang CXX=clang++ ./configure $DEBUG_ARG $ASSERT_ARG $PREFIX_ARG || die "Cannot execute CC=clang CXX=clang++ configure $DEBUG_ARG $ASSERT_ARG $PREFIX_ARG"
      ;;
    *)
      ./configure $DEBUG_ARG $ASSERT_ARG $PREFIX_ARG || die "Cannot execute configure $DEBUG_ARG $ASSERT_ARG $PREFIX_ARG"
      ;;
  esac
}

setup_gdb_command () {
  GDB_TMPFILE=$(mktemp /tmp/gdb.XXXXXXXXXX)
  echo "set logging on" > $GDB_TMPFILE
  echo "set logging overwrite on" >> $GDB_TMPFILE
  echo "set environment LIBTEST_IN_GDB=1" >> $GDB_TMPFILE
  echo "run" >> $GDB_TMPFILE
  echo "thread apply all bt" >> $GDB_TMPFILE
  echo "quit" >> $GDB_TMPFILE
  GDB_COMMAND="gdb -f -batch -x $GDB_TMPFILE"
}

setup_valgrind_command () {
  VALGRIND_COMMAND="valgrind --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE"
}

make_valgrind () {
  if [ "$PLATFORM" = "darwin" ]; then
    make_darwin_malloc
  else
    if command_exists valgrind; then

      if [ -n "$TESTS_ENVIRONMENT" ]; then
        OLD_TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT
        export -n TESTS_ENVIRONMENT
      fi

      # Set ENV VALGRIND_COMMAND
      if [ -z "$VALGRIND_COMMAND" ]; then
        setup_valgrind_command
      fi

      if [[ -f libtool ]]; then
        TESTS_ENVIRONMENT="$LIBTOOL_COMMAND $VALGRIND_COMMAND"
      else
        TESTS_ENVIRONMENT="$VALGRIND_COMMAND"
      fi
      export TESTS_ENVIRONMENT

      export TESTS_ENVIRONMENT
      make_target check
      export -n TESTS_ENVIRONMENT

      if [ -n "$OLD_TESTS_ENVIRONMENT" ]; then
        TESTS_ENVIRONMENT=$OLD_TESTS_ENVIRONMENT
        export TESTS_ENVIRONMENT
      fi

    fi
  fi
}

make_install_system () {
  make_distclean

  INSTALL_LOCATION=$(mktemp -d /tmp/XXXXXXXXXX)
  PREFIX_ARG="--prefix=$INSTALL_LOCATION"

  configure_target_platform

  if [ -n "$TESTS_ENVIRONMENT" ]; then
    OLD_TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT
    export -n TESTS_ENVIRONMENT
  fi

  make_target all

  make_target "install"

  make_target "installcheck"

  make_target "uninstall"

  export -n TESTS_ENVIRONMENT

  if [ -n "$OLD_TESTS_ENVIRONMENT" ]; then
    TESTS_ENVIRONMENT=$OLD_TESTS_ENVIRONMENT
    export TESTS_ENVIRONMENT
  fi

  rm -r -f $INSTALL_LOCATION
}

make_darwin_malloc () {
  MallocGuardEdges=1
  MallocErrorAbort=1
  MallocScribble=1
  export MallocGuardEdges MallocErrorAbort MallocScribble
  make_check
  export -n MallocGuardEdges MallocErrorAbort MallocScribble
}

make_local () {
  case $TARGET_PLATFORM in
    darwin-*)
      make_distcheck
      make_valgrind
      ;;
    *)
      make_target_platform
      ;;
  esac
}

make_target_platform () {
  case $TARGET_PLATFORM in
    fedora-*)
      # make rpm includes "make distcheck"
      if [ -f rpm.am ]; then
        make_rpm
      else
        make_distcheck
      fi
      ;;
    precise-*)
      make_distcheck
      make_valgrind
      make_gdb
      ;;
    unknown-*)
      make_all
      ;;
    *)
      make_all
      ;;
  esac

  make_install_system

  make_distclean
}

make_gdb () {
  if command_exists gdb; then

    if [ -n "$TESTS_ENVIRONMENT" ]; then
      OLD_TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT
      export -n TESTS_ENVIRONMENT
    fi

    # Set ENV GDB_COMMAND
    if [ -z "$GDB_COMMAND" ]; then
      setup_gdb_command
    fi

    if [[ -f libtool ]]; then
      TESTS_ENVIRONMENT="$LIBTOOL_COMMAND $GDB_COMMAND"
    else
      TESTS_ENVIRONMENT="$GDB_COMMAND"
    fi

    export TESTS_ENVIRONMENT
    make_target check
    export -n TESTS_ENVIRONMENT

    MAKE_TARGET=
    if [[ -f gdb.txt ]]; then
      rm -f gdb.txt
    fi

    if [ -n "$OLD_TESTS_ENVIRONMENT" ]; then
      TESTS_ENVIRONMENT=$OLD_TESTS_ENVIRONMENT
      export TESTS_ENVIRONMENT
    fi
  fi
}

make_target () {
  if [ -n "$MAKE_TARGET" ]; then
    OLD_MAKE_TARGET=$MAKE_TARGET
  fi

  MAKE_TARGET=$1
  run $MAKE $MAKE_J $MAKE_TARGET || die "Cannot execute $MAKE $MAKE_TARGET"

  if [ -n "$MAKE_TARGET" ]; then
    MAKE_TARGET=$OLD_MAKE_TARGET
  fi
}

make_distcheck () {
  make_target distcheck
}

make_rpm () {
  make_target "rpm"
}

make_distclean () {
  make_target distclean
}

make_check () {
  make_target check
}

make_all () {
  make_target all
}

run() {
  if [ -n "$TESTS_ENVIRONMENT" ]; then
    echo "TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT"
  fi
  echo "\`$@' $ARGS"
  $@ $ARGS
} 

parse_command_line_options() {

  if ! options=$(getopt -o c --long configure -n 'bootstrap' -- "$@"); then
    exit 1
  fi

  eval set -- "$options"

  while [ $# -gt 0 ]; do
    case $1 in
      -c | --configure )
        CONFIGURE_OPTION="yes" ; shift;;
      -- )
        shift; break;;
      -* )
        echo "$0: error - unrecognized option $1" 1>&2; exit 1;;
      *)
        break;;
    esac
  done
}



bootstrap() {
  parse_command_line_options $@
  determine_target_platform

  DEFAULT_DEV_AUTORECONF_FLAGS="--install --force --verbose -Wall -Werror"
  DEFAULT_AUTORECONF_FLAGS="--install --force --verbose -Wall"

  if [ -d .git ]; then
    AUTORECONF_FLAGS=$DEFAULT_DEV_AUTORECONF_FLAGS
    VCS_CHECKOUT=git
  elif [ -d .bzr ]; then
    AUTORECONF_FLAGS=$DEFAULT_DEV_AUTORECONF_FLAGS
    VCS_CHECKOUT=bzr
  elif [ -d .svn ]; then
    AUTORECONF_FLAGS=$DEFAULT_DEV_AUTORECONF_FLAGS
    VCS_CHECKOUT=svn
  elif [ -d .hg ]; then
    AUTORECONF_FLAGS=$DEFAULT_DEV_AUTORECONF_FLAGS
    VCS_CHECKOUT=hg
  else
    AUTORECONF_FLAGS=$DEFAULT_AUTORECONF_FLAGS
  fi

  if [ -z "$LIBTOOLIZE_FLAGS" ]; then
    LIBTOOLIZE_FLAGS="--force --verbose --install"
  fi

  if [ "$PLATFORM" = "darwin" ]; then
    LIBTOOLIZE=glibtoolize
  elif [ -z "$LIBTOOLIZE" ]; then 
    LIBTOOLIZE=libtoolize
  fi

  AUTORECONF=autoreconf

  # Set ENV DEBUG in order to enable debugging
  if [ -n "$DEBUG" ]; then 
    DEBUG_ARG="--enable-debug"
  fi

  # Set ENV ASSERT in order to enable assert
  if [ -n "$ASSERT" ]; then 
    ASSERT_ARG="--enable-assert"
  fi

  # Set ENV MAKE in order to override "make"
  if [ -z "$MAKE" ]; then 
    MAKE="make"
  fi

  # Set ENV MAKE_J in order to override "-j2"
  if [ -z "$MAKE_J" ]; then
    MAKE_J="-j2"
  fi

  # Set ENV PREFIX in order to set --prefix for ./configure
  if [ -n "$PREFIX" ]; then 
    PREFIX_ARG="--prefix=$PREFIX"
  fi

  if [ -f Makefile ]; then
    $MAKE $MAKE_J maintainer-clean
  fi

  run $LIBTOOLIZE $LIBTOOLIZE_FLAGS || die "Cannot execute $LIBTOOLIZE $LIBTOOLIZE_FLAGS"
  run $AUTORECONF $AUTORECONF_FLAGS || die "Cannot execute $AUTORECONF $AUTORECONF_FLAGS"

  configure_target_platform
  
  if [ "$CONFIGURE_OPTION" == "yes" ]; then
    exit
  fi

  # Backwards compatibility
  if [ -n "$VALGRIND" ]; then
    MAKE_TARGET="valgrind"
  fi

  # Setup LIBTOOL_COMMAND if we need it
  if [[ -f libtool ]]; then
    LIBTOOL_COMMAND="./libtool --mode=execute"
  fi

  if [ -f docs/conf.py ]; then 
    if command_exists sphinx-build; then
      make_target "man"
    fi
  fi

  # If we are running under Jenkins we predetermine what tests we will run against
  if [[ -n "$JENKINS_HOME" ]]; then 
    make_target_platform
  elif [ "$MAKE_TARGET" == "gdb" ]; then
    make_gdb
  elif [ "$MAKE_TARGET" == "valgrind" ]; then
    make_valgrind
  elif [ "$MAKE_TARGET" == "jenkins" ]; then 
    # Set ENV MAKE_TARGET in order to override default of "all"
    make_target_platform
  elif [ -z "$MAKE_TARGET" ]; then 
    make_local
  else
    make_target $MAKE_TARGET
  fi
}

export -n VCS_CHECKOUT
export -n PLATFORM
export -n TARGET_PLATFORM
CONFIGURE_OPTION=no
VCS_CHECKOUT=
PLATFORM=unknown
TARGET_PLATFORM=unknown

bootstrap $@
