AC_PREREQ([2.56])

# ёбаный фарш
m4_include([m4/checkbinutils.m4])
m4_include([m4/config.sub])
m4_include([m4/split.m4])
m4_include([m4/checkjava.m4])
m4_include([m4/qualify.m4])
m4_include([m4/trim.m4])
m4_include([m4/emit.m4])
m4_include([m4/checkpic.m4])
m4_include([m4/cs_check_host.m4])
m4_include([m4/checkcppunit.m4])
m4_include([m4/visibility.m4])
m4_include([m4/checklib.m4])
m4_include([m4/path.m4])
m4_include([m4/progver.m4])
m4_include([m4/checkcswin32libs.m4])
m4_include([m4/checkpthread.m4])
m4_include([m4/headercache.m4])
m4_include([m4/textcache.m4])
m4_include([m4/compiler.m4])
m4_include([m4/installdirs.m4])
m4_include([m4/diagnose.m4])
m4_include([m4/checkcommon.m4])
m4_include([m4/packageinfo.m4])
m4_include([m4/jamcache.m4])
m4_include([m4/checkstl.m4])
m4_include([m4/config.guess])
m4_include([m4/checkpython.m4])
m4_include([m4/warnings.m4])
m4_include([m4/checktt2.m4])
m4_include([m4/checkbuild.m4])
m4_include([m4/checklibtool.m4])
m4_include([m4/checkprog.m4])
m4_include([m4/makecache.m4])
m4_include([m4/embed.m4])
m4_include([m4/crystal.m4])
m4_include([m4/language.m4])
m4_include([m4/mkdir.m4])
m4_include([m4/listutil.m4])
m4_include([m4/checkopengl.m4])
m4_include([m4/cel.m4])


AC_INIT(hivegame, 0.1, fake0mail0@gmail.com)
AM_INIT_AUTOMAKE
AC_DISABLE_STATIC
AC_CONFIG_SRCDIR([hivegame.cpp])
AC_CONFIG_HEADERS([config.h])

#Проверка программ
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
LT_INIT([disable-static])
AM_PATH_PYTHON(2.6)

#Проверка библиотек
CS_PATH_CRYSTAL_CHECK([1.4.0], , AC_MSG_ERROR([CrystalSpace not found]))
CS_PATH_CEL_CHECK([1.4.0], , AC_MSG_ERROR([CrystalSpace Entity Layer not found]))
CS_CHECK_PYTHON
AC_SUBST(cs_cv_python_cflags)
AC_SUBST(cs_cv_python_lflags)
AC_SUBST(cs_cv_python_libs)
AX_PKG_SWIG(, , AC_MSG_ERROR([SWIG not found]))

#Проверка заголовочных файлов
CPPFLAGS="$CPPFLAGS $CRYSTAL_CFLAGS $CEL_CFLAGS"

#Проверка определений типов

#Проверка структур

#Проверка характеристик компилятора
AC_HEADER_STDBOOL

#Проверка библиотечных функций
#LDFLAGS="$LDFLAGS $CRYSTAL_LIBS $CEL_LIBS"
LDFLAGS="$LDFLAGS $CEL_LIBS $CRYSTAL_LIBS $SWIG_LIB"


AC_CONFIG_FILES([Makefile])
AC_OUTPUT

