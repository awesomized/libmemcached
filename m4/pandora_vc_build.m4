dnl  Copyright (C) 2009 Sun Microsystems
dnl This file is free software; Sun Microsystems
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([PANDORA_BUILDING_FROM_VC],[

  ac_cv_building_from_vc=no

  AS_IF([test -d "${srcdir}/.bzr"],[
    ac_cv_building_from_bzr=yes
    ac_cv_building_from_vc=yes
    ],[
    ac_cv_building_from_bzr=no
  ])

  AS_IF([test -d "${srcdir}/.svn"],[
    ac_cv_building_from_svn=yes
    ac_cv_building_from_vc=yes
    ],[
    ac_cv_building_from_svn=no
  ])

  AS_IF([test -d "${srcdir}/.hg"],[
    ac_cv_building_from_hg=yes
    ac_cv_building_from_vc=yes
    ],[
    ac_cv_building_from_hg=no
  ])

  AS_IF([test -d "${srcdir}/.git"],[
    ac_cv_building_from_git=yes
    ac_cv_building_from_vc=yes
    ],[
    ac_cv_building_from_git=no
  ])


])
  
dnl Takes one argument which is the prefix to append
AC_DEFUN([PANDORA_EXPORT_BZR_INFO],[
  m4_ifval(m4_normalize([$1]),[
    m4_define([PEBI_PREFIX],[])
  ],[
    m4_define([PEBI_PREFIX],m4_toupper(m4_normalize($1))[_])
  ])

  AC_DEFINE(PEBI_PREFIX[BZR_REVID], ["BZR_REVID"], [bzr revision ID])
  AC_DEFINE(PEBI_PREFIX[BZR_BRANCH], ["BZR_BRANCH"], [bzr branch name])
  AC_DEFINE(PEBI_PREFIX[RELEASE_DATE], ["RELEASE_DATE"], [Release date based on the date of the repo checkout])
  AC_DEFINE(PEBI_PREFIX[RELEASE_VERSION], ["RELEASE_VERSION"], [$1 version number formatted for display])
  AC_DEFINE(PEBI_PREFIX[RELEASE_COMMENT], ["RELEASE_COMMENT"], [Set to trunk if the branch is the main $1 branch])
  AC_DEFINE(PEBI_PREFIX[RELEASE_ID], [RELEASE_ID], [$1 version number formatted for numerical comparison])
])

AC_DEFUN([_PANDORA_READ_FROM_FILE],[
  $1=`grep $1 $2 | cut -f2 -d=`
])

AC_DEFUN([PANDORA_VC_VERSION],[
  AC_REQUIRE([PANDORA_BUILDING_FROM_VC])

  PANDORA_RELEASE_DATE=`date +%Y.%m`
  PANDORA_RELEASE_NODOTS_DATE=`date +%Y%m`

  dnl Set some defaults
  PANDORA_VC_REVNO="0"
  PANDORA_VC_REVID="unknown"
  PANDORA_VC_BRANCH="bzr-export"

  AS_IF([test -f ${srcdir}/config/bzr_revinfo],[
    _PANDORA_READ_FROM_FILE([PANDORA_VC_REVNO],${srcdir}/config/bzr_revinfo)
    _PANDORA_READ_FROM_FILE([PANDORA_VC_REVID],${srcdir}/config/bzr_revinfo)
    _PANDORA_READ_FROM_FILE([PANDORA_VC_BRANCH],
                            ${srcdir}/config/bzr_revinfo)
    _PANDORA_READ_FROM_FILE([PANDORA_RELEASE_DATE],
                            ${srcdir}/config/bzr_revinfo)
    _PANDORA_READ_FROM_FILE([PANDORA_RELEASE_NODOTS_DATE],
                            ${srcdir}/config/bzr_revinfo)
    _PANDORA_READ_FROM_FILE([PANDORA_RELEASE_COMMENT],
                            ${srcdir}/config/bzr_revinfo)
  ])
  AS_IF([test "${ac_cv_building_from_bzr}" = "yes"],[
dnl  echo "Grabbing changelog and version information from bzr"
dnl  bzr log --short > ChangeLog || touch ChangeLog
    PANDORA_BZR_REVNO=`bzr revno`
    AS_IF([test "x$PANDORA_BZR_REVNO" != "${PANDORA_VC_REVNO}"],[
      PANDORA_VC_REVNO="${PANDORA_BZR_REVNO}"
      PANDORA_VC_REVID=`bzr log -r-1 --show-ids | grep revision-id | awk '{print $[]2}' | head -1`
      PANDORA_VC_BRANCH=`bzr nick`
      AS_IF([test "x${PANDORA_VC_BRANCH}" != "${PACKAGE}"],[
        PANDORA_RELEASE_COMMENT="${PANDORA_VC_BRANCH}"
      ],[
        PANDORA_RELEASE_COMMENT="trunk"
      ])
    ])
  ])
  AS_IF([! test -d config],[mkdir -p config])
  cat > "config/bzr_revinfo" <<EOF
PANDORA_VC_REVNO=${PANDORA_VC_REVNO}
PANDORA_VC_REVID=${PANDORA_VC_REVID}
PANDORA_VC_BRANCH=${PANDORA_VC_BRANCH}
PANDORA_RELEASE_DATE=${PANDORA_RELEASE_DATE}
PANDORA_RELEASE_NODOTS_DATE=${PANDORA_RELEASE_NODOTS_DATE}
PANDORA_RELEASE_COMMENT=${PANDORA_RELEASE_COMMENT}
EOF
    
  PANDORA_RELEASE_VERSION="${PANDORA_RELEASE_DATE}.${PANDORA_VC_REVNO}"
  PANDORA_RELEASE_ID="${PANDORA_RELEASE_NODOTS_DATE}${PANDORA_VC_REVNO}"

  VERSION="${PANDORA_RELEASE_VERSION}"
  AC_DEFINE_UNQUOTED([PANDORA_RELEASE_VERSION],["${PANDORA_RELEASE_VERSION}"],
                     [The real version of the software])
  AC_SUBST(PANDORA_VC_REVNO)
  AC_SUBST(PANDORA_VC_REVID)
  AC_SUBST(PANDORA_VC_BRANCH)
  AC_SUBST(PANDORA_RELEASE_DATE)
  AC_SUBST(PANDORA_RELEASE_NODOTS_DATE)
  AC_SUBST(PANDORA_RELEASE_COMMENT)
  AC_SUBST(PANDORA_RELEASE_VERSION)
  AC_SUBST(PANDORA_RELEASE_ID)
])
