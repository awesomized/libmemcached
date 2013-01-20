#!/bin/bash
# 
# Copyright (C) 2012-2013 Brian Aker
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


# Environment Variables that will influence the build:
#   AUTOMAKE
#   AUTORECONF
#   LIBTOOLIZE
#   MAKE
#   PREFIX
#   TESTS_ENVIRONMENT
#   VERBOSE
#   WARNINGS
#

command_not_found_handle ()
{
  warn "$@: command not found"

  #if $DEBUG; then 
    echo ""
    echo "Stack trace:"
    local frame=0
    while caller $frame; do
      ((frame++));
    done
    echo ""
  #fi

  return 127
}

function error ()
{ 
  echo "$BASH_SOURCE:$BASH_LINENO: $@" >&2
}

function die ()
{ 
  echo "$BASH_SOURCE:$BASH_LINENO: $@" >&2
  exit 1; 
}

function warn ()
{ 
  echo "$BASH_SOURCE:$BASH_LINENO: $@"
  #echo "$BASH_SOURCE:$BASH_LINENO: $@" >&1
}

function nassert ()
{
  local param_name=\$"$1"
  local param_value=`eval "expr \"$param_name\" "`

  if [ -n "$param_value" ]; then
    echo "$bash_source:$bash_lineno: assert($param_name) had value of "$param_value"" >&2
    exit 1
  fi
}

function assert ()
{
  local param_name=\$"$1"
  local param_value=`eval "expr \"$param_name\" "`

  if [ -z "$param_value" ]; then
    echo "$bash_source:$bash_lineno: assert($param_name)" >&2
    exit 1
  fi
}

function assert_file ()
{
  if [ ! -f "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) does not exist: $2" >&2
    exit 1; 
  fi
}

function assert_no_file ()
{
  if [ -f "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) file exists: $2" >&2
    exit 1;
  fi
}

function assert_no_directory ()
{
  if [ -d "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) directory exists: $2" >&2
    exit 1;
  fi
}

function assert_exec_file ()
{
  if [ ! -f "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) does not exist: $2" >&2
    exit 1;
  fi

  if [ ! -x "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) exists but is not executable: $2" >&2
    exit 1;
  fi
}

function command_exists ()
{
  type "$1" &> /dev/null ;
}

function rebuild_host_os ()
{
  HOST_OS="${UNAME_MACHINE_ARCH}-${VENDOR}-${VENDOR_DISTRIBUTION}-${VENDOR_RELEASE}-${UNAME_KERNEL}-${UNAME_KERNEL_RELEASE}"
  if [ -z "$1" ]; then
    if $VERBOSE; then
      echo "HOST_OS=$HOST_OS"
    fi
  fi
}

#  Valid values are: darwin,fedora,rhel,ubuntu
function set_VENDOR_DISTRIBUTION ()
{
  local dist=`echo "$1" | tr '[A-Z]' '[a-z]'`
  case "$dist" in
    darwin)
      VENDOR_DISTRIBUTION='darwin'
      ;;
    fedora)
      VENDOR_DISTRIBUTION='fedora'
      ;;
    rhel)
      VENDOR_DISTRIBUTION='rhel'
      ;;
    ubuntu)
      VENDOR_DISTRIBUTION='ubuntu'
      ;;
    opensuse)
      VENDOR_DISTRIBUTION='opensuse'
      ;;
    *)
      die "attempt to set an invalid VENDOR_DISTRIBUTION=$dist"
      ;;
  esac
}

function set_VENDOR_RELEASE ()
{
  local release=`echo "$1" | tr '[A-Z]' '[a-z]'`
  case "$VENDOR_DISTRIBUTION" in
    darwin)
      VENDOR_RELEASE='mountain'
      ;;
    fedora)
      VENDOR_RELEASE="$release"
      ;;
    rhel)
      VENDOR_RELEASE="$release"
      ;;
    ubuntu)
      VENDOR_RELEASE="$release"
      ;;
    opensuse)
      VENDOR_RELEASE="$release"
      ;;
    unknown)
      die "attempt to set VENDOR_RELEASE without setting VENDOR_DISTRIBUTION"
      ;;
    *)
      die "attempt to set with an invalid VENDOR_DISTRIBUTION=$VENDOR_DISTRIBUTION"
      ;;
  esac
}


#  Valid values are: apple, redhat, centos, canonical
function set_VENDOR ()
{
  local vendor=`echo "$1" | tr '[A-Z]' '[a-z]'`

  case $vendor in
    apple)
      VENDOR='apple'
      ;;
    redhat)
      VENDOR='redhat'
      ;;
    centos)
      VENDOR='centos'
      ;;
    canonical)
      VENDOR='canonical'
      ;;
    suse)
      VENDOR='suse'
      ;;
    *)
      die "An attempt was made to set an invalid VENDOR=$_vendor"
      ;;
  esac

  set_VENDOR_DISTRIBUTION $2
  set_VENDOR_RELEASE $3
}

function determine_target_platform ()
{
  UNAME_MACHINE_ARCH=`(uname -m) 2>/dev/null` || UNAME_MACHINE_ARCH=unknown
  UNAME_KERNEL=`(uname -s) 2>/dev/null`  || UNAME_SYSTEM=unknown
  UNAME_KERNEL_RELEASE=`(uname -r) 2>/dev/null` || UNAME_KERNEL_RELEASE=unknown

  if [[ $(uname) == 'Darwin' ]]; then
    set_VENDOR 'apple' 'darwin' 'mountain'
  elif [[ -f '/etc/fedora-release' ]]; then 
    local fedora_version=`cat /etc/fedora-release | awk ' { print $3 } '`
    set_VENDOR 'redhat' 'fedora' $fedora_version
    if [[ "x$VENDOR_RELEASE" == 'x17' ]]; then
      AUTORECONF_REBUILD_HOST=true
    fi
  elif [[ -f '/etc/centos-release' ]]; then
    local centos_version=`cat /etc/centos-release | awk ' { print $7 } '`
    set_VENDOR 'centos' 'rhel' $centos_version
  elif [[ -f '/etc/SuSE-release' ]]; then
    local suse_distribution=`head -1 /etc/SuSE-release | awk ' { print $1 } '`
    local suse_version=`head -1 /etc/SuSE-release | awk ' { print $2 } '`
    set_VENDOR 'suse' $suse_distribution $suse_version
  elif [[ -f '/etc/redhat-release' ]]; then
    local rhel_version=`cat /etc/redhat-release | awk ' { print $7 } '`
    set_VENDOR 'redhat' 'rhel' $rhel_version
  elif [[ -f '/etc/lsb-release' ]]; then 
    local debian_DISTRIB_ID=`cat /etc/lsb-release | grep DISTRIB_ID | awk -F= ' { print $2 } '`
    local debian_version=`cat /etc/lsb-release | grep DISTRIB_CODENAME | awk -F= ' { print $2 } '`
    set_VENDOR 'canonical' $debian_DISTRIB_ID $debian_version
    if [[ "x$VENDOR_RELEASE" == 'xprecise' ]]; then
      AUTORECONF_REBUILD_HOST=true
    fi
  fi

  rebuild_host_os
}

function run_configure ()
{
  # We will run autoreconf if we are required
  run_autoreconf_if_required

  # We always begin at the root of our build
  if [ ! popd ]; then
    die "Programmer error, we entered run_configure with a stacked directory"
  fi

  if ! command_exists "$CONFIGURE"; then
    die "$CONFIGURE does not exist"
  fi

  local BUILD_DIR="$1"
  if [[ -n "$BUILD_DIR" ]]; then
    rm -r -f $BUILD_DIR
    mkdir -p $BUILD_DIR
    safe_pushd $BUILD_DIR
  fi

  # Arguments for configure
  local BUILD_CONFIGURE_ARG= 

  # Set ENV DEBUG in order to enable debugging
  if $DEBUG; then 
    BUILD_CONFIGURE_ARG='--enable-debug'
  fi

  # Set ENV ASSERT in order to enable assert
  if [[ -n "$ASSERT" ]]; then 
    local ASSERT_ARG=
    ASSERT_ARG='--enable-assert'
    BUILD_CONFIGURE_ARG="$ASSERT_ARG $BUILD_CONFIGURE_ARG"
  fi

  if [[ -n "$CONFIGURE_ARG" ]]; then 
    BUILD_CONFIGURE_ARG= "$BUILD_CONFIGURE_ARG $CONFIGURE_ARG"
  fi

  ret=1;
  # If we are executing on OSX use CLANG, otherwise only use it if we find it in the ENV
  case $HOST_OS in
    *-darwin-*)
      CC=clang CXX=clang++ $top_srcdir/configure $BUILD_CONFIGURE_ARG || die "Cannot execute CC=clang CXX=clang++ configure $BUILD_CONFIGURE_ARG $PREFIX_ARG"
      ret=$?
      ;;
    rhel-5*)
      command_exists 'gcc44' || die "Could not locate gcc44"
      CC=gcc44 CXX=gcc44 $top_srcdir/configure $BUILD_CONFIGURE_ARG $PREFIX_ARG || die "Cannot execute CC=gcc44 CXX=gcc44 configure $BUILD_CONFIGURE_ARG $PREFIX_ARG"
      ret=$?
      ;;
    *)
      $CONFIGURE $BUILD_CONFIGURE_ARG $PREFIX_ARG
      ret=$?
      ;;
  esac

  if [ $ret -ne 0 ]; then
    die "Could not execute $CONFIGURE $BUILD_CONFIGURE_ARG $PREFIX_ARG"
  fi

  if [ ! -f 'Makefile' ]; then
    die "Programmer error, configure was run but no Makefile existed after $CONFIGURE was run"
  fi
}

function setup_gdb_command () {
  GDB_TMPFILE=$(mktemp /tmp/gdb.XXXXXXXXXX)
  echo 'set logging overwrite on' > $GDB_TMPFILE
  echo 'set logging on' >> $GDB_TMPFILE
  echo 'set environment LIBTEST_IN_GDB=1' >> $GDB_TMPFILE
  echo 'run' >> $GDB_TMPFILE
  echo 'thread apply all bt' >> $GDB_TMPFILE
  echo 'quit' >> $GDB_TMPFILE
  GDB_COMMAND="gdb -f -batch -x $GDB_TMPFILE"
}

function setup_valgrind_command () {
  VALGRIND_PROGRAM=`type -p valgrind`
  if [[ -n "$VALGRIND_PROGRAM" ]]; then
    VALGRIND_COMMAND="$VALGRIND_PROGRAM --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE"
  fi
}

function save_BUILD ()
{
  if [[ -n "$OLD_CONFIGURE" ]]; then
    die "OLD_CONFIGURE($OLD_CONFIGURE) was set on push, programmer error!"
  fi

  if [[ -n "$OLD_CONFIGURE_ARG" ]]; then
    die "OLD_CONFIGURE_ARG($OLD_CONFIGURE_ARG) was set on push, programmer error!"
  fi

  if [[ -n "$OLD_MAKE" ]]; then
    die "OLD_MAKE($OLD_MAKE) was set on push, programmer error!"
  fi

  if [[ -n "$OLD_TESTS_ENVIRONMENT" ]]; then
    die "OLD_TESTS_ENVIRONMENT($OLD_TESTS_ENVIRONMENT) was set on push, programmer error!"
  fi

  if [[ -n "$CONFIGURE" ]]; then
    OLD_CONFIGURE=$CONFIGURE
  fi

  if [[ -n "$CONFIGURE_ARG" ]]; then
    OLD_CONFIGURE_ARG=$CONFIGURE_ARG
  fi

  if [[ -n "$MAKE" ]]; then
    OLD_MAKE=$MAKE
  fi

  if [[ -n "$TESTS_ENVIRONMENT" ]]; then
    OLD_TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT
  fi
}

function restore_BUILD ()
{
  if [[ -n "$OLD_CONFIGURE" ]]; then
    CONFIGURE=$OLD_CONFIGURE
  fi

  if [[ -n "$OLD_CONFIGURE_ARG" ]]; then
    CONFIGURE_ARG=$OLD_CONFIGURE_ARG
  fi

  if [[ -n "$OLD_MAKE" ]]; then
    MAKE=$OLD_MAKE
  fi

  if [[ -n "$OLD_TESTS_ENVIRONMENT" ]]; then
    TESTS_ENVIRONMENT=$OLD_TESTS_ENVIRONMENT
  fi

  OLD_CONFIGURE=
  OLD_CONFIGURE_ARG=
  OLD_MAKE=
  OLD_TESTS_ENVIRONMENT=

  export -n CC CXX
}

function push_PREFIX_ARG ()
{
  if [[ -n "$OLD_PREFIX_ARG" ]]; then
    die "OLD_PREFIX_ARG was set on push, programmer error!"
  fi

  if [[ -n "$PREFIX_ARG" ]]; then
    OLD_PREFIX_ARG=$PREFIX_ARG
    PREFIX_ARG=
  fi

  if [[ -n "$1" ]]; then
    PREFIX_ARG="--prefix=$1"
  fi
}

function pop_PREFIX_ARG ()
{
  if [[ -n "$OLD_PREFIX_ARG" ]]; then
    PREFIX_ARG=$OLD_PREFIX_ARG
    OLD_PREFIX_ARG=
  else
    PREFIX_ARG=
  fi
}

function push_TESTS_ENVIRONMENT ()
{
  if [[ -n "$OLD_TESTS_ENVIRONMENT" ]]; then
    die "OLD_TESTS_ENVIRONMENT was set on push, programmer error!"
  fi

  if [[ -n "$TESTS_ENVIRONMENT" ]]; then
    OLD_TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT
    TESTS_ENVIRONMENT=
  fi
}

function pop_TESTS_ENVIRONMENT ()
{
  TESTS_ENVIRONMENT=
  if [[ -n "$OLD_TESTS_ENVIRONMENT" ]]; then
    TESTS_ENVIRONMENT=$OLD_TESTS_ENVIRONMENT
    OLD_TESTS_ENVIRONMENT=
  fi
}

function safe_pushd ()
{
  pushd $1 &> /dev/null ;

  if [ -n "$BUILD_DIR" ]; then
    if $VERBOSE; then
      echo "BUILD_DIR=$BUILD_DIR"
    fi
  fi
}

function safe_popd ()
{
  local directory_to_delete=`pwd`
  popd &> /dev/null ;
  if [ $? -eq 0 ]; then
    if [[ "$top_srcdir" == "$directory_to_delete" ]]; then
      die "We almost deleted top_srcdir($top_srcdir), programmer error"
    fi

    rm -r -f "$directory_to_delete"
  fi
}

function make_valgrind ()
{
  if [[ "$VENDOR_DISTRIBUTION" == 'darwin' ]]; then
    make_darwin_malloc
    return
  fi

  # If the env VALGRIND_COMMAND is set then we assume it is valid
  local valgrind_was_set=false
  if [[ -z "$VALGRIND_COMMAND" ]]; then
    setup_valgrind_command
    if [[ -n "$VALGRIND_COMMAND" ]]; then
      valgrind_was_set=true
    fi
  else
    valgrind_was_set=true
  fi

  # If valgrind_was_set is set to no we bail
  if ! $valgrind_was_set; then
    echo 'valgrind was not present'
    return 1
  fi

  # If we are required to run configure, do so now
  run_configure_if_required

  push_TESTS_ENVIRONMENT

  # If we don't have a configure, then most likely we will be missing libtool
  assert_file 'configure'
  if [[ -f 'libtool' ]]; then
    TESTS_ENVIRONMENT="./libtool --mode=execute $VALGRIND_COMMAND"
  else
    TESTS_ENVIRONMENT="$VALGRIND_COMMAND"
  fi

  make_target 'check' || return 1

  pop_TESTS_ENVIRONMENT
}

function make_install_system ()
{
  local INSTALL_LOCATION=$(mktemp -d /tmp/XXXXXXXXXX)
  push_PREFIX_ARG $INSTALL_LOCATION

  if [ ! -d $INSTALL_LOCATION ] ; then
    die "ASSERT temp directory not found '$INSTALL_LOCATION'"
  fi

  run_configure #install_buid_dir

  push_TESTS_ENVIRONMENT

  make_target 'install'

  make_target 'installcheck'

  make_target 'uninstall'

  pop_TESTS_ENVIRONMENT
  pop_PREFIX_ARG

  rm -r -f $INSTALL_LOCATION
  make 'distclean'

  if [ -f 'Makefile' ]; then
    die "ASSERT Makefile should not exist"
  fi

  safe_popd
}

function make_darwin_malloc ()
{
  run_configure_if_required

  old_MallocGuardEdges=$MallocGuardEdges
  MallocGuardEdges=1
  old_MallocErrorAbort=$MallocErrorAbort
  MallocErrorAbort=1
  old_MallocScribble=$MallocScribble
  MallocScribble=1

  make_check

  MallocGuardEdges=$old_MallocGuardEdges
  MallocErrorAbort=$old_MallocErrorAbort
  MallocScribble=$old_MallocScribble
}

function snapshot_check ()
{
  if [ ! -f "$BOOTSTRAP_SNAPSHOT_CHECK" ]; then
    make_for_snapshot
  fi

  if [ -n "$BOOTSTRAP_SNAPSHOT_CHECK" ]; then
    assert_file "$BOOTSTRAP_SNAPSHOT_CHECK" 'snapshot check failed'
  fi
}

# This will reset our environment, and make sure built files are available.
function make_for_snapshot ()
{
  # Make sure it is clean
  make_maintainer_clean

  run_configure
  make_target 'dist'
  make_target 'distclean'

  # We should have a configure, but no Makefile at the end of this exercise
  assert_no_file 'Makefile'
  assert_exec_file 'configure'

  snapshot_check
}

function check_mingw ()
{
  command_exists 'mingw64-configure'
  ret=$?
  if [ "$ret" -ne 0 ]; then
    return 1
  fi

  command_exists 'mingw64-make'
  ret=$?
  if [ "$ret" -ne 0 ]; then
    return 1
  fi

  return 0
}

function check_clang ()
{
  command_exists 'clang'
  ret=$?
  if [ "$ret" -ne 0 ]; then
    return 1
  fi

  return 0
}

function check_clang_analyzer ()
{
  command_exists 'scan-build'
  ret=$?
  if [ "$ret" -ne 0 ]; then
    return 1
  fi

  return 0
}

function make_skeleton ()
{
  run_configure
  ret=$?

  if [ $ret -eq 0 ]; then
    assert_file 'Makefile'

    make_target 'all' 'warn'
    ret=$?
    if [ $ret -ne 0 ]; then
      warn "$MAKE failed"
    else
      if [[ -n "$DISPLAY" ]]; then
        if command_exists 'wine'; then
          TESTS_ENVIRONMENT='wine'
        fi
      fi

      if [[ -n "$TESTS_ENVIRONMENT" ]]; then
        make_target 'check' 'warn' || warn "$MAKE check failed"
        ret=$?
      fi
    fi

    if $jenkins_build_environment; then
      make_target 'clean' 'warn'
    fi
  fi

  return $ret
}

function make_for_mingw ()
{
  if ! check_mingw; then
    return 1
  fi

  # Make sure it is clean
  if [ -f Makefile -o -f configure ]; then
    make_maintainer_clean
  fi

  run_autoreconf

  save_BUILD

  CONFIGURE='mingw64-configure'
  MAKE='mingw64-make'
  CONFIGURE_ARGS='--enable-static --disable-shared'

  make_skeleton
  ret=$?

  restore_BUILD

  return $ret
}

function make_for_clang ()
{
  if ! check_clang; then
    return 1
  fi

  # Make sure it is clean
  if [ -f Makefile -o -f configure ]; then
    make_maintainer_clean
  fi

  run_autoreconf

  save_BUILD

  CC=clang CXX=clang++
  export CC CXX

  make_skeleton
  ret=$?

  make_target 'check'

  restore_BUILD

  return $ret
}

function make_for_clang_analyzer ()
{
  if ! check_clang; then
    return 1
  fi

  if ! check_clang_analyzer; then
    die 'clang-analyzer was not found'
  fi

  # Make sure it is clean
  if [ -f Makefile -o -f configure ]; then
    make_maintainer_clean
  fi

  run_autoreconf

  save_BUILD

  CC=clang CXX=clang++
  export CC CXX
  CONFIGURE_ARGS='--enable-debug'

  make_skeleton
  ret=$?

  make_target 'clean' 'warn'

  scan-build -o clang-html make -j4 -k

  restore_BUILD

  return $ret
}

# If we are locally testing, we should make sure the environment is setup correctly
function check_for_jenkins ()
{
  if ! $jenkins_build_environment; then
    echo "Not inside of jenkins"

    if [ -f 'configure' ]; then
      make_maintainer_clean
    fi

    if $BOOTSTRAP_SNAPSHOT; then
      make_for_snapshot
    fi
  fi
}

function make_universe ()
{
  make_for_snapshot
  make_valgrind
  make_gdb
  make_rpm
  make_for_clang
  make_for_clang_analyzer

  if [ check_mingw -eq 0 ]; then
    make_for_mingw
  fi

  make_distcheck
  make_install_system
}

function make_for_continuus_integration ()
{
  # Setup the environment if we are local
  check_for_jenkins

  # No matter then evironment, we should not have a Makefile at this point
  assert_no_file 'Makefile'

  # Platforms which require bootstrap should have some setup done before we hit this stage.
  # If we are building locally, skip this step, unless we are just testing locally. 
  if $BOOTSTRAP_SNAPSHOT; then
    snapshot_check
  else
    # If we didn't require a snapshot, then we should not have a configure
    assert_no_file 'configure'

    run_autoreconf
  fi

  assert_no_file 'Makefile' 'Programmer error, Makefile existed where build state should have been clean'

  case $HOST_OS in
    *-fedora-*)
      run_configure

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_target 'all'

      # make rpm includes "make distcheck"
      if [[ -f rpm.am ]]; then
        make_rpm
      elif [[ -d rpm ]]; then
        make_rpm
      else
        make_distcheck
      fi

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_install_system
      ;;
    *-precise-*)
      run_configure

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_target 'all'

      make_distcheck

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_valgrind

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_install_system
      ;;
    *)
      make_jenkins_default
      ;;
  esac

  make_maintainer_clean

  safe_popd
}

# The point to this test is to test bootstrap.sh itself
function self_test ()
{
  # We start off with a clean env
  make_maintainer_clean

  eval "./bootstrap.sh jenkins" || die "failed 'jenkins'"
  eval "./bootstrap.sh all" || die "failed 'all'"
  eval "./bootstrap.sh gdb" || die "failed 'gdb'"
  eval "./bootstrap.sh maintainer-clean" || die "failed 'maintainer-clean'"
}

function make_install_html ()
{
  run_configure_if_required
  assert_file 'configure'

  make_target 'install-html'
}

function make_gdb ()
{
  if command_exists 'gdb'; then
    run_configure_if_required

    push_TESTS_ENVIRONMENT

    # Set ENV GDB_COMMAND
    if [[ -z "$GDB_COMMAND" ]]; then
      setup_gdb_command
    fi

    # If we don't have a configure, then most likely we will be missing libtool
    assert_file 'configure'
    if [[ -f 'libtool' ]]; then
      TESTS_ENVIRONMENT="./libtool --mode=execute $GDB_COMMAND"
    else
      TESTS_ENVIRONMENT="$GDB_COMMAND"
    fi

    make_target 'check'

    if [ -f 'gdb.txt' ]; then
      rm 'gdb.txt'
    fi

    pop_TESTS_ENVIRONMENT

    if [ -f '.gdb_history' ]; then
      rm '.gdb_history'
    fi

    if $jenkins_build_environment; then
      make_target 'clean'
    fi
  else
    echo 'gdb was not present'
    return 1
  fi
}

# $1 target to compile
# $2 to die, or not to die, based on contents
function make_target ()
{
  if [ -z "$1" ]; then
    die "Programmer error, no target provided for make"
  fi

  if [ ! -f 'Makefile' ]; then
    die "Programmer error, make was called before configure"
    run_configure
  fi

  if [ -n "$TESTS_ENVIRONMENT" ]; then
    if $VERBOSE; then
      echo "TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT"
    fi
  fi

  if [ -z "$MAKE" ]; then
    die "MAKE was not set"
  fi

  # $2 represents error or warn
  run $MAKE $1
  ret=$?

  if [ $ret -ne 0 ]; then
    if [ -n "$2" ]; then
      warn "Failed to execute $MAKE $1: $ret"
    else
      die "Failed to execute $MAKE $1: $ret"
    fi
  fi

  return $ret
}

function make_distcheck ()
{
  make_target 'distcheck'
}

function make_rpm ()
{
  if command_exists 'rpmbuild'; then
    if [ -f 'rpm.am' -o -d 'rpm' ]; then
      run_configure_if_required
      make_target 'rpm'

      if $jenkins_build_environment; then
        make_target 'clean'
      fi

    fi
  fi
}

function make_maintainer_clean ()
{
  run_configure_if_required
  make_target 'maintainer-clean' 'no_error'

  # Lets make sure we really cleaned up the environment
  assert_no_file 'Makefile'
  assert_no_file 'configure'
  assert_no_directory 'autom4te.cache'
}

function make_check ()
{
  make_target 'check'
}

function make_jenkins_default ()
{
  run_configure
  make_target 'all'
}

function make_default ()
{
  run_configure_if_required
  make_target 'all'
}

function run_configure_if_required () 
{
  run_autoreconf_if_required

  if [ ! -f 'Makefile' ]; then
    run_configure
  fi

  assert_file 'Makefile' 'configure did not produce a Makefile'
}

function run_autoreconf_if_required () 
{
  if [ ! -x 'configure' ]; then
    run_autoreconf
  fi

  assert_exec_file 'configure'
}

function run_autoreconf () 
{
  if [[ -z "$AUTORECONF" ]]; then
    die "Programmer error, tried to call run_autoreconf () but AUTORECONF was not set"
  fi

  if test $use_libtool = 1; then
    assert $BOOTSTRAP_LIBTOOLIZE
    run $BOOTSTRAP_LIBTOOLIZE '--copy' '--install' '--force' || die "Cannot execute $BOOTSTRAP_LIBTOOLIZE"
  fi

  run $AUTORECONF || die "Cannot execute $AUTORECONF"

  eval 'bash -n configure' || die "autoreconf generated a malformed configure"
}

function run ()
{
  if $VERBOSE; then
    echo "\`$@' $ARGS"
  fi

  if [ -z "$1" ]; then
    return 127;
  fi

  eval $@ $ARGS
} 

function parse_command_line_options ()
{
  local SHORTOPTS=':apcmt:dvh'

  nassert MAKE_TARGET

  while getopts "$SHORTOPTS" opt; do
    case $opt in
      a) #--autoreconf
        AUTORECONF_OPTION=true
        MAKE_TARGET='autoreconf'
        ;;
      p) #--print-env
        PRINT_SETUP_OPTION=true
        ;;
      c) # --configure
        CONFIGURE_OPTION=true
        MAKE_TARGET='configure'
        ;;
      m) # maintainer-clean
        CLEAN_OPTION=true
        MAKE_TARGET='clean_op'
        ;;
      t) # target
        TARGET_OPTION=true
        TARGET_OPTION_ARG="$OPTARG"
        MAKE_TARGET="$OPTARG"
        ;;
      d) # debug
        DEBUG_OPTION=true
        enable_debug
        ;;
      h) # help
        echo "bootstrap.sh [options] optional_target ..."
        exit
        ;;
      v) # verbose
        VERBOSE_OPTION=true
        VERBOSE=true
        ;;
      :)
        echo "Option -$OPTARG requires an argument." >&2
        exit 1
        ;;
      *)
        echo "$0: error - unrecognized option $1" 1>&2
        exit 1
        ;;
    esac
  done

  shift $((OPTIND-1))

  if [ -n "$1" ]; then
    MAKE_TARGET="$@"
  fi
}

function determine_vcs ()
{
  if [[ -d '.git' ]]; then
    VCS_CHECKOUT=git
  elif [[ -d '.bzr' ]]; then
    VCS_CHECKOUT=bzr
  elif [[ -d '.svn' ]]; then
    VCS_CHECKOUT=svn
  elif [[ -d '.hg' ]]; then
    VCS_CHECKOUT=hg
  fi

  if [[ -n "$VCS_CHECKOUT" ]]; then
    VERBOSE=true
  fi
}

function require_libtoolise ()
{
  use_libtool=0
  grep '^[         ]*A[CM]_PROG_LIBTOOL' configure.ac >/dev/null \
    && use_libtool=1
  grep '^[         ]*LT_INIT' configure.ac >/dev/null \
    && use_libtool=1
}

function autoreconf_setup ()
{
  # Set ENV MAKE in order to override "make"
  if [[ -z "$MAKE" ]]; then 
    if command_exists 'gmake'; then
      MAKE=`type -p gmake`
    else
      if command_exists 'make'; then
        MAKE=`type -p make`
      fi
    fi
    
    if [ "$VCS_CHECKOUT" ]; then
      if $DEBUG; then
        MAKE="$MAKE --warn-undefined-variables"
      fi
    fi

    if $DEBUG; then
      MAKE="$MAKE -d"
    fi
  fi

  if [[ -z "$GNU_BUILD_FLAGS" ]]; then
    GNU_BUILD_FLAGS="--install --force"
  fi

  if $VERBOSE; then
    GNU_BUILD_FLAGS="$GNU_BUILD_FLAGS --verbose"
  fi

  if [ -z "$ACLOCAL_PATH" ]; then
    ACLOCAL_PATH="/usr/local/share/aclocal $ACLOCAL_PATH"
  fi

  if [[ -z "$WARNINGS" ]]; then
    if [[ -n "$VCS_CHECKOUT" ]]; then
      WARNINGS="all,error"
    else
      WARNINGS="all"
    fi
  fi

  if test $use_libtool = 1; then
    if [[ -n "$LIBTOOLIZE" ]]; then
      BOOTSTRAP_LIBTOOLIZE=`type -p $LIBTOOLIZE`

      if [[ -z "$BOOTSTRAP_LIBTOOLIZE" ]]; then
        echo "Couldn't find user supplied libtoolize, it is required"
      fi
    else
      # If we are using OSX, we first check to see glibtoolize is available
      if [[ "$VENDOR_DISTRIBUTION" == "darwin" ]]; then
        BOOTSTRAP_LIBTOOLIZE=`type -p glibtoolize`

        if [[ -z "$BOOTSTRAP_LIBTOOLIZE" ]]; then
          echo "Couldn't find glibtoolize, it is required on OSX"
        fi
      else
        BOOTSTRAP_LIBTOOLIZE=`type -p libtoolize`

        if [[ -z "$BOOTSTRAP_LIBTOOLIZE" ]]; then
          echo "Couldn't find libtoolize, it is required"
        fi
      fi
    fi
    if $VERBOSE; then
      LIBTOOLIZE_OPTIONS="--verbose $BOOTSTRAP_LIBTOOLIZE_OPTIONS"
    fi
    if $DEBUG; then
      LIBTOOLIZE_OPTIONS="--debug $BOOTSTRAP_LIBTOOLIZE_OPTIONS"
    fi
    LIBTOOLIZE=true
  fi

  # Test the ENV AUTOMAKE if it exists
  if [[ -n "$AUTOMAKE" ]]; then
    run $AUTOMAKE '--help'    &> /dev/null    || die "Failed to run AUTOMAKE:$AUTOMAKE"
  fi

  # Test the ENV AUTOCONF if it exists
  if [[ -n "$AUTOCONF" ]]; then
    run $AUTOCONF '--help'    &> /dev/null    || die "Failed to run AUTOCONF:$AUTOCONF"
  fi

  # Test the ENV AUTOHEADER if it exists
  if [[ -n "$AUTOHEADER" ]]; then
    run $AUTOHEADER '--help'  &> /dev/null    || die "Failed to run AUTOHEADER:$AUTOHEADER"
  fi

  # Test the ENV AUTOM4TE if it exists
  if [[ -n "$AUTOM4TE" ]]; then
    run $AUTOM4TE '--help'    &> /dev/null    || die "Failed to run AUTOM4TE:$AUTOM4TE"
  fi

  # Test the ENV AUTOHEADER if it exists, if not we add one and add --install
  if [[ -z "$ACLOCAL" ]]; then
    ACLOCAL="aclocal --install"
  fi
  run $ACLOCAL '--help'  &> /dev/null    || die "Failed to run ACLOCAL:$ACLOCAL"

  if [[ -z "$AUTORECONF" ]]; then
    AUTORECONF=`type -p autoreconf`

    if [[ -z "$AUTORECONF" ]]; then
      die "Couldn't find autoreconf"
    fi

    if [[ -n "$GNU_BUILD_FLAGS" ]]; then
      AUTORECONF="$AUTORECONF $GNU_BUILD_FLAGS"
    fi
  fi

  run $AUTORECONF '--help'  &> /dev/null    || die "Failed to run AUTORECONF:$AUTORECONF"
}

function print_setup ()
{
  saved_debug_status=$DEBUG
  if $DEBUG; then
    disable_debug
  fi

  echo '----------------------------------------------' 
  echo 'BOOTSTRAP ENV' 
  echo "AUTORECONF=$AUTORECONF"
  echo "HOST_OS=$HOST_OS"

  echo "getopt()"
  if $AUTORECONF_OPTION; then
    echo "--autoreconf"
  fi

  if $CLEAN_OPTION; then
    echo "--clean"
  fi

  if $CONFIGURE_OPTION; then
    echo "--configure"
  fi

  if $DEBUG_OPTION; then
    echo "--debug"
  fi

  if $PRINT_SETUP_OPTION; then
    echo "--print-env"
  fi

  if $TARGET_OPTION; then
    echo "--target=$TARGET_OPTION_ARG"
  fi

  if $VERBOSE_OPTION; then
    echo "--verbose"
  fi

  if [[ -n "$MAKE" ]]; then
    echo "MAKE=$MAKE"
  fi

  if [[ -n "$MAKE_TARGET" ]]; then
    echo "MAKE_TARGET=$MAKE_TARGET"
  fi

  if [[ -n "$PREFIX" ]]; then
    echo "PREFIX=$PREFIX"
  fi

  if [[ -n "$TESTS_ENVIRONMENT" ]]; then
    echo "TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT"
  fi

  if [[ -n "$VCS_CHECKOUT" ]]; then
    echo "VCS_CHECKOUT=$VCS_CHECKOUT"
  fi

  if $VERBOSE; then
    echo "VERBOSE=true"
  fi

  if $DEBUG; then
    echo "DEBUG=true"
  fi

  if [[ -n "$WARNINGS" ]]; then
    echo "WARNINGS=$WARNINGS"
  fi
  echo '----------------------------------------------' 

  if $saved_debug_status; then
    enable_debug
  fi
}

function make_clean_option ()
{
  run_configure_if_required

  make_maintainer_clean

  if [[ "$VCS_CHECKOUT" == 'git' ]]; then
    run "$VCS_CHECKOUT" status --ignored
  elif [[ -n "$VCS_CHECKOUT" ]]; then
    run "$VCS_CHECKOUT" status
  fi
}

function make_for_autoreconf ()
{
  if [ -f 'Makefile' ]; then
    make_maintainer_clean
  fi

  run_autoreconf

  assert_no_file 'Makefile'
}

function check_make_target()
{
  case $1 in
    'self')
      ;;
    'rpm')
      ;;
    'gdb')
      ;;
    'clean_op')
      ;;
    'autoreconf')
      ;;
    'install-system')
      ;;
    'configure')
      ;;
    'distcheck')
      ;;
    'check')
      ;;
    'snapshot')
      ;;
    'mingw')
      ;;
    'universe')
      ;;
    'valgrind')
      ;;
    'jenkins')
      ;;
    'distclean')
      ;;
    'maintainer-clean')
      ;;
    'install')
      ;;
    'all')
      ;;
    'make_default')
      ;;
    'clang')
      ;;
    'clang-analyzer')
      ;;
    'test-*')
      ;;
    'valgrind-*')
      ;;
    'gdb-*')
      ;;
    'dist')
      ;;
    *)
      die "Unknown MAKE_TARGET option: $1"
      ;;
  esac
}

function bootstrap ()
{
  determine_target_platform

  determine_vcs

  # Set up whatever we need to do to use autoreconf later
  require_libtoolise
  autoreconf_setup

  if [ -z "$MAKE_TARGET" ]; then
    MAKE_TARGET="make_default"
  fi

  if $PRINT_SETUP_OPTION -o  $DEBUG; then
    echo 
    print_setup
    echo 

    # Exit if all we were looking for were the currently used options
    if $PRINT_SETUP_OPTION; then
      exit
    fi
  fi

  # Use OLD_TESTS_ENVIRONMENT for tracking the state of the variable
  local OLD_TESTS_ENVIRONMENT=

  # Set ENV PREFIX in order to set --prefix for ./configure
  if [[ -n "$PREFIX" ]]; then 
    push_PREFIX_ARG $PREFIX
  fi

  # We should always have a target by this point
  assert MAKE_TARGET

  local MAKE_TARGET_ARRAY=($MAKE_TARGET)

  for target in "${MAKE_TARGET_ARRAY[@]}"
  do
    # If we are running inside of Jenkins, we want to only run some of the possible tests
    if $jenkins_build_environment; then
      check_make_target $target
    fi

    case $target in
      'self')
        self_test
        ;;
      'gdb')
        make_gdb
        ;;
      'install-html')
        make_install_html
        ;;
      'clean_op')
        make_clean_option
        ;;
      'autoreconf')
        make_for_autoreconf
        ;;
      'install-system')
        make_install_system
        ;;
      'configure')
        run_configure
        ;;
      'make_default')
        make_default
        ;;
      'clang')
        if ! check_clang; then
          die "clang was not found"
        fi

        if ! make_for_clang; then
          die "Failed to build clang: $?"
        fi
        ;;
      'clang-analyzer')
        if ! check_clang_analyzer; then
          die "clang-analyzer was not found"
        fi
        if ! check_clang; then
          die "clang was not found"
        fi

        if ! make_for_clang_analyzer; then
          die "Failed to build clang-analyzer: $?"
        fi
        ;;
      'mingw')
        if ! check_mingw; then
          die "mingw was not found"
        fi

        if ! make_for_mingw; then
          die "Failed to build mingw: $?"
        fi
        ;;
      'snapshot')
        make_for_snapshot
        ;;
      'rpm')
        make_rpm
        ;;
      'valgrind')
        make_valgrind
        ;;
      'universe')
        make_universe
        ;;
      'jenkins')
        make_for_continuus_integration
        ;;
      *)
        run_configure_if_required
        make_target "$target"
        ;;
    esac
  done
}

function main ()
{
  # Variables we export
  declare -x VCS_CHECKOUT=

  # Variables we control globally
  local MAKE_TARGET=
  local CONFIGURE=

  # Options for getopt
  local AUTORECONF_OPTION=false
  local CLEAN_OPTION=false
  local CONFIGURE_OPTION=false
  local DEBUG_OPTION=false
  local PRINT_SETUP_OPTION=false
  local TARGET_OPTION=false
  local TARGET_OPTION_ARG=
  local VERBOSE_OPTION=false

  local OLD_CONFIGURE=
  local OLD_CONFIGURE_ARG=
  local OLD_MAKE=
  local OLD_TESTS_ENVIRONMENT=

  # If we call autoreconf on the platform or not
  local AUTORECONF_REBUILD_HOST=false
  local AUTORECONF_REBUILD=false

  local -r top_srcdir=`pwd`

  # Default configure
  if [ -z "$CONFIGURE" ]; then
    CONFIGURE="$top_srcdir/configure"
  fi


  # Variables for determine_target_platform () and rebuild_host_os ()
  #   UNAME_MACHINE_ARCH= uname -m
  #   VENDOR= apple, redhat, centos, canonical
  #   VENDOR_RELEASE=  
  #                  RHEL{rhel,Tikanga,Santiago}
  #                  Ubuntu{ubuntu,Lucid,Maverick,Natty,Oneiric,Precise,Quantal}
  #                  Fedora{fedora,Verne,Beefy}
  #                  OSX{osx,lion,snow,mountain}
  #   VENDOR_DISTRIBUTION= darwin,fedora,rhel,ubuntu
  #   UNAME_KERNEL= Linux, Darwin,...
  #   UNAME_KERNEL_RELEASE= Linux, Darwin,...
  local UNAME_MACHINE_ARCH=unknown
  local VENDOR=unknown
  local VENDOR_RELEASE=unknown
  local VENDOR_DISTRIBUTION=unknown
  local UNAME_KERNEL=unknown
  local UNAME_KERNEL_RELEASE=unknown
  local HOST_OS=

  rebuild_host_os no_output

  parse_command_line_options $@

  # If we are running under Jenkins we predetermine what tests we will run against
  # This MAKE_TARGET can be overridden by parse_command_line_options based MAKE_TARGET changes.
  # We don't want Jenkins overriding other variables, so we NULL them.
  if [ -z "$MAKE_TARGET" ]; then
    if $jenkins_build_environment; then
      MAKE_TARGET='jenkins'
    fi
  fi

  bootstrap

  jobs -l
  wait

  exit 0
}

function set_branch ()
{
  if [ -z "$BRANCH" ]; then 
    if [ -z "$CI_PROJECT_TEAM" ]; then 
      die "Variable CI_PROJECT_TEAM has not been set"
    fi
    if [ -z "$PROJECT" ]; then 
      die "Variable PROJECT has not been set"
    fi
    if [ -z "$BUILD_TAG" ]; then 
      die "Variable BUILD_TAG has not been set"
    fi

    BRANCH="lp:~$CI_PROJECT_TEAM/$PROJECT/$BUILD_TAG"
    export BRANCH
  fi

  if [ -z "$BRANCH" ]; then 
    die "Missing values required to build BRANCH variable."
  fi
}

function merge ()
{
  if [ -z "$VCS_CHECKOUT" ]; then
    die "Merges require VCS_CHECKOUT."
  fi

  set_branch

  if [[ "$VCS_CHECKOUT" == 'bzr' ]]; then
    if test -n "$BRANCH_TO_MERGE"; then
      bzr merge $BRANCH_TO_MERGE
      bzr commit --message="Merge $BRANCH_TO_MERGE Build: $BUILD_TAG" --unchanged
    fi

    bzr push "$BRANCH"
  elif [[ -n "$VCS_CHECKOUT" ]]; then
    die "Merge attempt occured, current VCS setup does not support this"
  fi
}

function enable_debug ()
{
  if ! $DEBUG; then
    local caller_loc=`caller`
    if [ -n $1 ]; then
      echo "$caller_loc Enabling debug: $1"
    else
      echo "$caller_loc Enabling debug"
    fi
    set -x
    DEBUG=true
  fi
}

function usage ()
{
  cat << EOF
  Usage: $program_name [OPTION]..

  Bootstrap this package from the checked-out sources, and optionally walk through CI run.

  Options:

EOF
}

function disable_debug ()
{
  set +x
  DEBUG=true
}

# Script begins here

program_name=$0

env_debug_enabled=false
if [[ -n "$JENKINS_HOME" ]]; then 
  declare -r jenkins_build_environment=true
else
  declare -r jenkins_build_environment=false
fi

export ACLOCAL
export AUTOCONF
export AUTOHEADER
export AUTOM4TE
export AUTOMAKE
export AUTORECONF
export DEBUG
export GNU_BUILD_FLAGS
export LIBTOOLIZE
export LIBTOOLIZE_OPTIONS
export MAKE
export TESTS_ENVIRONMENT
export VERBOSE
export WARNINGS

case $OSTYPE in
  darwin*)
    export MallocGuardEdges
    export MallocErrorAbort
    export MallocScribble
    ;;
esac

# We check for DEBUG twice, once before we source the config file, and once afterward
env_debug_enabled=false
if [[ -n "$DEBUG" ]]; then
  env_debug_enabled=true
  enable_debug
  print_setup
fi

# Variables which only can be set by .bootstrap
BOOTSTRAP_SNAPSHOT=false
BOOTSTRAP_SNAPSHOT_CHECK=

if [ -f '.bootstrap' ]; then
  source '.bootstrap'
fi

if $env_debug_enabled; then
  enable_debug
else
  if [[ -n "$DEBUG" ]]; then
    enable_debug "Enabling DEBUG from '.bootstrap'"
    print_setup
  fi
fi

# We do this in order to protect the case where DEBUG
if ! $env_debug_enabled; then
  DEBUG=false
fi

main $@
