# Author: David Cohen <dacohen@gmail.com>
# Modified: 05/02/2008

AC_PREREQ(2.59)
AC_INIT([OgreBullet], [0.2])
AM_INIT_AUTOMAKE([OgreBullet], [0.2])
AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([foreign dist-bzip2])

dnl Checks for programs
AC_PROG_CXX
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(OGRE, [OGRE >= 1.4.0],,
 AC_MSG_ERROR([OGRE is required to compile OgreBullet]))
AC_SUBST(OGRE_CFLAGS)
AC_SUBST(OGRE_LIBS)

PKG_CHECK_MODULES(bullet, [bullet >= 2.6.0],,
 AC_MSG_ERROR([Bullet is required to compile OgreBullet]))
AC_SUBST(bullet_CFLAGS)
AC_SUBST(bullet_LIBS)

AC_CONFIG_FILES([Makefile
 Collisions/Makefile
 Collisions/src/Makefile
 Collisions/include/Makefile
 Collisions/include/Debug/Makefile
 Collisions/include/Shapes/Makefile
 Collisions/include/Utils/Makefile
 Dynamics/Makefile
 Dynamics/src/Makefile
 Dynamics/include/Makefile
 Dynamics/include/Constraints/Makefile
 Dynamics/include/Prefab/Makefile
 OgreBullet.pc
 ])

AC_OUTPUT

