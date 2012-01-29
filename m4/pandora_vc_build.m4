dnl  Copyright (C) 2009 Sun Microsystems, Inc.
dnl This file is free software; Sun Microsystems, Inc.
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([PANDORA_TEST_VC_DIR],[
  pandora_building_from_vc=no

  if test -d ".bzr" ; then
    pandora_building_from_bzr=yes
    pandora_building_from_vc=yes
  else
    pandora_building_from_bzr=no
  fi

  if test -d ".svn" ; then
    pandora_building_from_svn=yes
    pandora_building_from_vc=yes
  else
    pandora_building_from_svn=no
  fi

  if test -d ".hg" ; then
    pandora_building_from_hg=yes
    pandora_building_from_vc=yes
  else
    pandora_building_from_hg=no
  fi

  if test -d ".git" ; then
    pandora_building_from_git=yes
    pandora_building_from_vc=yes
  else
    pandora_building_from_git=no
  fi
])
