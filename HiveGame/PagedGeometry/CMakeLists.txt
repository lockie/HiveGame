cmake_minimum_required(VERSION 2.8)

# define the project
project(PagedGeometry)

# find all dependencies
#include(CMakeDependenciesConfig.txt)
INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR} ${BULLET_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include")
LINK_DIRECTORIES(${OGRE_LIB_DIR})

# build static libs by default
SET(BUILD_SHARED_LIBS OFF)

# some additional compiler flags
INCLUDE(CheckCCompilerFlag)
IF(NOT WIN32)
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

# now add the directories
add_subdirectory(source)
