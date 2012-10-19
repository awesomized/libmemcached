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
  echo "Command not found: '$@'"
  exit 127
}

die ()
{ 
  echo "$BASH_SOURCE:$BASH_LINENO: $@" >&2
  exit 1; 
}

function assert ()
{
  local param_name=\$"$1"
  local param_value=`eval "expr \"$param_name\" "`

  if [ -z "$param_value" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($param_name)" >&2
    exit 1
  fi
}

assert_file ()
{
  if [ ! -f "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) does not exist: $2" >&2
    exit 1; 
  fi
}

assert_no_file ()
{
  if [ -f "$1" ]; then
    echo "$BASH_SOURCE:$BASH_LINENO: assert($1) file exists: $2" >&2
    exit 1;
  fi
}

assert_exec_file ()
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

command_exists ()
{
  type "$1" &> /dev/null ;
}

rebuild_host_os ()
{
  HOST_OS="${UNAME_MACHINE_ARCH}-${VENDOR}-${VENDOR_DISTRIBUTION}-${VENDOR_RELEASE}-${UNAME_KERNEL}-${UNAME_KERNEL_RELEASE}"
  if [ -z "$1" ]; then
    if $VERBOSE; then
      echo "HOST_OS=$HOST_OS"
    fi
  fi
}

#  Valid values are: darwin,fedora,rhel,ubuntu
set_VENDOR_DISTRIBUTION ()
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

set_VENDOR_RELEASE ()
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
set_VENDOR ()
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

determine_target_platform ()
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

run_configure ()
{
  # We will run autoreconf if we are required
  run_autoreconf_if_required

  # We always begin at the root of our build
  if [ ! popd ]; then
    die "Programmer error, we entered run_configure with a stacked directory"
  fi

  local BUILD_DIR="$1"
  if [[ -n "$BUILD_DIR" ]]; then
    rm -r -f $BUILD_DIR
    mkdir -p $BUILD_DIR
    safe_pushd $BUILD_DIR
  fi

  # Arguments for configure
  local CONFIGURE_ARG= 

  # Set ENV DEBUG in order to enable debugging
  if $DEBUG; then 
    CONFIGURE_ARG='--enable-debug'
  fi

  # Set ENV ASSERT in order to enable assert
  if [[ -n "$ASSERT" ]]; then 
    local ASSERT_ARG=
    ASSERT_ARG='--enable-assert'
    CONFIGURE_ARG="$ASSERT_ARG $CONFIGURE_ARG"
  fi

  # If we are executing on OSX use CLANG, otherwise only use it if we find it in the ENV
  case $HOST_OS in
    *-darwin-*)
      CC=clang CXX=clang++ $top_srcdir/configure $CONFIGURE_ARG || die "Cannot execute CC=clang CXX=clang++ configure $CONFIGURE_ARG $PREFIX_ARG"
      ;;
    rhel-5*)
      command_exists gcc44 || die "Could not locate gcc44"
      CC=gcc44 CXX=gcc44 $top_srcdir/configure $CONFIGURE_ARG $PREFIX_ARG || die "Cannot execute CC=gcc44 CXX=gcc44 configure $CONFIGURE_ARG $PREFIX_ARG"
      ;;
    *)
      $top_srcdir/configure $CONFIGURE_ARG $PREFIX_ARG || die "Cannot execute configure $CONFIGURE_ARG $PREFIX_ARG"
      ;;
  esac

  if [ ! -f 'Makefile' ]; then
    die "Programmer error, configure was run but no Makefile existed afterward"
  fi
}

setup_gdb_command () {
  GDB_TMPFILE=$(mktemp /tmp/gdb.XXXXXXXXXX)
  echo 'set logging overwrite on' > $GDB_TMPFILE
  echo 'set logging on' >> $GDB_TMPFILE
  echo 'set environment LIBTEST_IN_GDB=1' >> $GDB_TMPFILE
  echo 'run' >> $GDB_TMPFILE
  echo 'thread apply all bt' >> $GDB_TMPFILE
  echo 'quit' >> $GDB_TMPFILE
  GDB_COMMAND="gdb -f -batch -x $GDB_TMPFILE"
}

setup_valgrind_command () {
  VALGRIND_PROGRAM=`type -p valgrind`
  if [[ -n "$VALGRIND_PROGRAM" ]]; then
    VALGRIND_COMMAND="$VALGRIND_PROGRAM --error-exitcode=1 --leak-check=yes --show-reachable=yes --track-fds=yes --malloc-fill=A5 --free-fill=DE"
  fi
}

push_PREFIX_ARG ()
{
  if [[ -n "$PREFIX_ARG" ]]; then
    OLD_PREFIX_ARG=$PREFIX_ARG
    PREFIX_ARG=
  fi

  if [[ -n "$1" ]]; then
    PREFIX_ARG="--prefix=$1"
  fi
}

pop_PREFIX_ARG ()
{
  if [[ -n "$OLD_PREFIX_ARG" ]]; then
    PREFIX_ARG=$OLD_TESTS_ENVIRONMENT
    OLD_PREFIX_ARG=
  else
    PREFIX_ARG=
  fi
}

push_TESTS_ENVIRONMENT ()
{
  if [[ -n "$OLD_TESTS_ENVIRONMENT" ]]; then
    die "OLD_TESTS_ENVIRONMENT was set on push, programmer error!"
  fi

  if [[ -n "$TESTS_ENVIRONMENT" ]]; then
    OLD_TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT
    TESTS_ENVIRONMENT=
  fi
}

pop_TESTS_ENVIRONMENT ()
{
  TESTS_ENVIRONMENT=
  if [[ -n "$OLD_TESTS_ENVIRONMENT" ]]; then
    TESTS_ENVIRONMENT=$OLD_TESTS_ENVIRONMENT
    OLD_TESTS_ENVIRONMENT=
  fi
}

safe_pushd ()
{
  pushd $1 &> /dev/null ;

  if $VERBOSE -a test -n "$BUILD_DIR"; then
    echo "BUILD_DIR=$BUILD_DIR"
  fi
}

safe_popd ()
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

make_valgrind ()
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

  if [[ -f 'libtool' ]]; then
    TESTS_ENVIRONMENT="$LIBTOOL_COMMAND $VALGRIND_COMMAND"
  else
    TESTS_ENVIRONMENT="$VALGRIND_COMMAND"
  fi

  make_target 'check' || return 1

  pop_TESTS_ENVIRONMENT
}

make_install_system ()
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

make_darwin_malloc ()
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

snapshot_check ()
{
  if [ -n "$BOOTSTRAP_SNAPSHOT_CHECK" ]; then
    assert_file "$BOOTSTRAP_SNAPSHOT_CHECK" 'snapshot check failed'
  fi
}

# This will reset our environment, and make sure built files are available.
make_for_snapshot ()
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

# If we are locally testing, we should make sure the environment is setup correctly
check_for_jenkins ()
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

make_for_continuus_integration ()
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

      make_distcheck

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_valgrind

      assert_exec_file 'configure'
      assert_file 'Makefile'

      make_install_system
      ;;
    *)
      run_configure
      make_all
      ;;
  esac

  make_maintainer_clean

  safe_popd
}

make_gdb ()
{
  run_configure_if_required

  if command_exists gdb; then

    push_TESTS_ENVIRONMENT

    # Set ENV GDB_COMMAND
    if [[ -z "$GDB_COMMAND" ]]; then
      setup_gdb_command
    fi

    if [ -f 'libtool' ]; then
      TESTS_ENVIRONMENT="$LIBTOOL_COMMAND $GDB_COMMAND"
    else
      TESTS_ENVIRONMENT="$GDB_COMMAND"
    fi

    make_target check

    if [ -f 'gdb.txt' ]; then
      rm 'gdb.txt'
    fi

    pop_TESTS_ENVIRONMENT

    if [ -f '.gdb_history' ]; then
      rm '.gdb_history'
    fi
  else
    echo 'gdb was not present'
    return 1
  fi
}

# $1 target to compile
# $2 to die, or not to die, based on contents
make_target ()
{
  if [[ -z "$1" ]]; then
    die "Programmer error, no target provided for make"
  fi

  if [ ! -f 'Makefile' ]; then
    die "Programmer error, make was called before configure"
    run_configure
  fi

  if test -n "$TESTS_ENVIRONMENT" -a $VERBOSE; then
    echo "TESTS_ENVIRONMENT=$TESTS_ENVIRONMENT"
  fi

  if [[ -z "$MAKE" ]]; then
    die "MAKE was not set"
  fi

  if [[ -n "$2" ]]; then
    run $MAKE $1 || return 1
  else
    run $MAKE $1 || die "Cannot execute $MAKE $1"
  fi
}

make_distcheck ()
{
  make_target 'distcheck'
}

make_rpm ()
{
  make_target 'rpm'
}

make_maintainer_clean ()
{
  run_configure_if_required
  make_target 'maintainer-clean' 'no_error'
}

make_check ()
{
  make_target 'check'
}

make_all ()
{
  make_target 'all'
}

run_configure_if_required () 
{
  run_autoreconf_if_required

  if [ ! -f 'Makefile' ]; then
    run_configure
  fi

  assert_file 'Makefile' 'configure did not produce a Makefile'
}

run_autoreconf_if_required () 
{
  if [ ! -x 'configure' ]; then
    run_autoreconf
  fi

  assert_exec_file 'configure'
}

run_autoreconf () 
{
  if [[ -z "$AUTORECONF" ]]; then
    die "Programmer error, tried to call run_autoreconf () but AUTORECONF was not set"
  fi

  run $AUTORECONF || die "Cannot execute $AUTORECONF"

  eval 'bash -n configure' || die "autoreconf generated a malformed configure"
}

run ()
{
  if $VERBOSE; then
    echo "\`$@' $ARGS"
  fi

  ($@ $ARGS)
} 

parse_command_line_options ()
{
  local SHORTOPTS=':apcmt:dv'

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

  if [ -n "$1" ]; then
    MAKE_TARGET="$1"
  fi
}

determine_vcs ()
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

autoreconf_setup ()
{
  # Set ENV MAKE in order to override "make"
  if [[ -z "$MAKE" ]]; then 
    if command_exists gmake; then
      MAKE=`type -p gmake`
    fi

    if command_exists make; then
      MAKE=`type -p make`
    fi
  fi

  if [[ -z "$GNU_BUILD_FLAGS" ]]; then
    GNU_BUILD_FLAGS="--install --force"
  fi

  if $VERBOSE; then
    GNU_BUILD_FLAGS="$GNU_BUILD_FLAGS --verbose"
  fi

  if [[ -z "$WARNINGS" ]]; then
    if [[ -n "$VCS_CHECKOUT" ]]; then
      WARNINGS="all,error"
    else
      WARNINGS="all"
    fi
  fi

  if [[ -z "$LIBTOOLIZE" ]]; then
    # If we are using OSX, we first check to see glibtoolize is available
    if [[ "$VENDOR_DISTRIBUTION" == "darwin" ]]; then
      LIBTOOLIZE=`type -p glibtoolize`

      if [[ -z "$LIBTOOLIZE" ]]; then
        echo "Couldn't find glibtoolize, it is required on OSX"
      fi
    fi
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

print_setup ()
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

make_clean_option ()
{
  run_configure_if_required

  make_maintainer_clean

  if [[ "$VCS_CHECKOUT" == 'git' ]]; then
    run "$VCS_CHECKOUT" status --ignored
  elif [[ -n "$VCS_CHECKOUT" ]]; then
    run "$VCS_CHECKOUT" status
  fi
}

make_for_autoreconf ()
{
  if [ -f 'Makefile' ]; then
    make_maintainer_clean
  fi

  run_autoreconf

  assert_no_file 'Makefile'
}

check_make_target()
{
  case $1 in
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
    'snapshot')
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
    'dist')
      ;;
    *)
      die "Unknown MAKE_TARGET option: $1"
      ;;
  esac
}

bootstrap ()
{
  determine_target_platform

  determine_vcs

  # Set up whatever we need to do to use autoreconf later
  autoreconf_setup

  if $PRINT_SETUP_OPTION -o  $DEBUG; then
    echo 
    print_setup
    echo 

    # Exit if all we were looking for were the currently used options
    if $PRINT_SETUP_OPTION; then
      exit
    fi
  fi

  # Setup LIBTOOL_COMMAND if we need it
  if [ -f 'libtool' ]; then
    LIBTOOL_COMMAND='./libtool --mode=execute'
  fi

  # Use OLD_TESTS_ENVIRONMENT for tracking the state of the variable
  local OLD_TESTS_ENVIRONMENT=

  # Set ENV PREFIX in order to set --prefix for ./configure
  if [[ -n "$PREFIX" ]]; then 
    push_PREFIX_ARG $PREFIX
  fi

  # Either we run a known target (or error), or we default to "all"
  if [[ -n "$MAKE_TARGET" ]]; then

    # If we are running inside of Jenkins, we want to only run some of the possible tests
    if $jenkins_build_environment; then
      check_make_target $MAKE_TARGET
    fi

    case $MAKE_TARGET in
      'gdb')
        make_gdb
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
      'snapshot')
        make_for_snapshot
        ;;
      'valgrind')
        make_valgrind
        ;;
      'jenkins')
        make_for_continuus_integration
        ;;
      *)
        run_configure_if_required
        make_target $MAKE_TARGET
        ;;
    esac
  else
    run_configure_if_required
    make_all
  fi
}

main ()
{
  # Variables we export
  declare -x VCS_CHECKOUT=

  # Variables we control globally
  local MAKE_TARGET=

  # Options for getopt
  local AUTORECONF_OPTION=false
  local CLEAN_OPTION=false
  local CONFIGURE_OPTION=false
  local DEBUG_OPTION=false
  local PRINT_SETUP_OPTION=false
  local TARGET_OPTION=false
  local TARGET_OPTION_ARG=
  local VERBOSE_OPTION=false

  # If we call autoreconf on the platform or not
  local AUTORECONF_REBUILD_HOST=false
  local AUTORECONF_REBUILD=false

  local -r top_srcdir=`pwd`

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

  # If we are running under Jenkins we predetermine what tests we will run against
  # This MAKE_TARGET can be overridden by parse_command_line_options based MAKE_TARGET changes.
  # We don't want Jenkins overriding other variables, so we NULL them.
  if $jenkins_build_environment; then
    MAKE_TARGET='jenkins'
  fi

  parse_command_line_options $@

  bootstrap

  jobs -l
  wait

  exit 0
}

enable_debug ()
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

disable_debug ()
{
  set +x
  DEBUG=true
}

# Script begins here

env_debug_enabled=false
if [[ -n "$JENKINS_HOME" ]]; then 
  declare -r jenkins_build_environment=true
else
  declare -r jenkins_build_environment=false
fi

export AUTOCONF
export AUTOHEADER
export AUTOM4TE
export AUTOMAKE
export AUTORECONF
export DEBUG
export GNU_BUILD_FLAGS
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
