
#ifndef __PYHIVEGAME_H__
#define __PYHIVEGAME_H__

#include "config.h"

#include <crystalspace.h>

#ifdef SWIG
%module pyhivegame
#define CS_EXPORT_SYM_DLL
#endif  // SWIG


CS_EXPORT_SYM_DLL void echo(const char* message);
CS_EXPORT_SYM_DLL void quit();
CS_EXPORT_SYM_DLL void ls(const char* directory);
CS_EXPORT_SYM_DLL void dir(const char* directory);
CS_EXPORT_SYM_DLL void path();
CS_EXPORT_SYM_DLL void map(const char* name = NULL);

#endif  // __PYHIVEGAME_H__

