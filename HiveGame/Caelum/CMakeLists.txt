######################################################################
# Caelum BUILD SYSTEM
# Welcome to the CMake build system for Caelum
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################
# cmake system for Caelum updated on the 22th of March 2011 by thomas{at}thomasfischer{DOT}biz

cmake_minimum_required(VERSION 2.4)
# loose if - else constructs
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


# add some functions we use that are shipped with cmake
INCLUDE(CheckLibraryExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCSourceCompiles)


# define the project
project(Caelum)
set(LIBNAME "Caelum")

# build static libs by default
SET(BUILD_SHARED_LIBS OFF)

# some optimization flags
if(MSVC)
  # add multi processor compilation flags
  set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        /MP /GL /Ox /Ob2 /Oi /Ot /Oy /fp:fast /GS- /MP /Zi")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}     /MP /Zi")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP /Od /Zi")
  set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /MP /Od /Zi")
  # some general flags
  add_definitions("-D_CRT_SECURE_NO_WARNINGS -DCAELUM_LIB -DCAELUM_SCRIPT_SUPPORT -D_CAELUM_TYPE_DESCRIPTORS")
  include_directories(${Boost_INCLUDE_DIRS} ${OGRE_INCLUDE_DIRS} ${DirectX_INCLUDE_DIR})

  set(CMAKE_EXE_LINKER_FLAGS_DEBUG          "${CMAKE_EXE_LINKER_FLAGS_DEBUG}          /DEBUG /SUBSYSTEM:WINDOWS")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE        "${CMAKE_EXE_LINKER_FLAGS_RELEASE}        /DEBUG /SUBSYSTEM:WINDOWS /LTCG /OPT:REF")
  set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL     "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}     /DEBUG /SUBSYSTEM:WINDOWS /LTCG /OPT:REF")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /DEBUG /SUBSYSTEM:WINDOWS /LTCG /OPT:REF")
ELSE(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE      "${CMAKE_EXE_LINKER_FLAGS_RELEASE}      -mfpmath=sse -msse2 -mmmx -msse -msse3 -m3dnow -O2 -fomit-frame-pointer -fstrict-aliasing -ffast-math -ftracer")
  set(CMAKE_EXE_LINKER_FLAGS_RelWithDebug "${CMAKE_EXE_LINKER_FLAGS_RelWithDebug} -O0")
endif(MSVC)

# some PG build options
#set(Caelum_ALTERNATE_COORDSYSTEM "FALSE" CACHE BOOL "alternate coordinate system, do not use unless you are very sure about it")
#set(Caelum_USE_OGRE_RANDOM       "FALSE" CACHE BOOL "fallback to Ogre's PRNG instead of using our own (not recommended)")
set(Caelum_USER_DATA             "TRUE" CACHE INTERNAL BOOL "ability to attach user data to entities")

# some versioning things
SET(LIB_MAJOR_VERSION "0")
SET(LIB_MINOR_VERSION "6")
SET(LIB_BUILD_VERSION "0")
SET(LIB_VERSION "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_BUILD_VERSION}")

# configuration of the config.h and PkgConfig
CONFIGURE_FILE(
    "${Caelum_SOURCE_DIR}/main/include/CaelumConfig.h.in"
    "${Caelum_BINARY_DIR}/main/include/CaelumConfig.h")

# some additional compiler flags
IF(NOT MSVC)
	ADD_DEFINITIONS(-Wall)
	CHECK_C_COMPILER_FLAG(-Wextra HAVE_W_EXTRA)
	IF(HAVE_W_EXTRA)
		ADD_DEFINITIONS(-Wextra)
	ENDIF()
endif()

# Set visibility options if available
IF(NOT WIN32)
	CHECK_C_SOURCE_COMPILES("int foo() __attribute__((destructor));
								int main() {return 0;}" HAVE_GCC_DESTRUCTOR)

	CHECK_C_COMPILER_FLAG(-fvisibility=hidden HAVE_VISIBILITY_SWITCH)
	IF(HAVE_VISIBILITY_SWITCH)
		CHECK_C_SOURCE_COMPILES("int foo() __attribute__((visibility(\"default\")));
									int main() {return 0;}" HAVE_GCC_VISIBILITY)
		IF(HAVE_GCC_VISIBILITY)
			ADD_DEFINITIONS(-fvisibility=hidden -DHAVE_GCC_VISIBILITY)
		ENDIF()
	ENDIF()
ENDIF()

link_directories   (${Ogre_LIB_DIRS} ${Boost_LIBRARY_DIRS})
include_directories(${Caelum_SOURCE_DIR}/main/include/)
include_directories(${Caelum_BINARY_DIR}/main/include/)

FILE(GLOB sources ${Caelum_SOURCE_DIR}/main/src/*.cpp ${Caelum_SOURCE_DIR}/main/include/*.h)

ADD_LIBRARY(${LIBNAME} STATIC ${Caelum_SOURCE_DIR}/main/src/CaelumPrecompiled.cpp ${sources})

target_link_libraries(${LIBNAME} ${Ogre_LIBRARIES})

# precompiled header
if (MSVC)
	set_target_properties(Caelum PROPERTIES COMPILE_FLAGS "/YuCaelumPrecompiled.h")
	set_source_files_properties(main/src/CaelumPrecompiled.cpp PROPERTIES COMPILE_FLAGS "/YcCaelumPrecompiled.h")
endif(MSVC)
