
hivegamedir=./
hivegame_PROGRAMS=hivegame
hivegame_SOURCES=hivegame.cpp console.cpp behaviour.cpp
hivegame_CXXFLAGS=
ACLOCAL_AMFLAGS = -I m4

# swig

#pyhivegamedir=./
pkgpython_PYTHON = pyhivegame.py
#pkgpyexec_LTLIBRARIES = _hivegame.la

_hivegame_sodir=./
#_hivegame_so_LTLIBRARIES=_hivegame.so
pkgpyexec_LTLIBRARIES=_pyhivegame.la
_pyhivegame_la_SOURCES = pyhivegame_wrap.cpp util.cpp map.cpp player.cpp
_pyhivegame_la_CPPFLAGS = $(cs_cv_python_cflags) -I.
_pyhivegame_la_LDFLAGS = -module $(cs_cv_python_lflags) $(cs_cv_python_libs)
#_hivegame_la_LIBADD = libhivegame.la
CLEANFILES=pyhivegame_wrap.cpp pyhivegame.py

pyhivegame_wrap.cpp : pyhivegame.i
	$(SWIG) -c++ -python -I. -o $@ $<

