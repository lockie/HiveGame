cmake_minimum_required(VERSION 2.8)

# define the project
project(HiveGame)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

if (CMAKE_BUILD_TYPE STREQUAL "")
	# Выставить тип сборки по умолчанию
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
set(CMAKE_DEBUG_POSTFIX "_d")
if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-D_DEBUG)
	else()
		add_definitions(-DNDEBUG)
	endif()
endif()

# Найти Ogre
#
if(WIN32)
	file(TO_CMAKE_PATH "$ENV{OGRE_HOME}" OGRE_HOME)
	set(CMAKE_MODULE_PATH "${OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif(WIN32)
if(UNIX)
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CMAKE_MODULE_PATH "/usr/lib64/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	endif()
endif(UNIX)
find_package(OGRE REQUIRED)
if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
	message(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
endif()

# Найти OIS
#
find_package(OIS REQUIRED)
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

# Найти boost
#
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else()
		# Статическая линковка boost к динамическому Ogre не заработает на 64-битном Linux
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif()
	if (MINGW)
		# Это, скорее всего, баг в CMake: модуль поиска boost пытается искать
		# бустовские бибилиотеки с именем libboost_*, но CMake заранее
		# добавляет префикс "lib". Далее - костылик для исправления.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif()
	set(Boost_ADDITIONAL_VERSIONS "1.46" "1.46.1" "1.44" "1.44.0")
	# Компоненты, которым нужна линковка. NB: сюда не включаются компоненты, состоящие из одних заголовков
	set(OGRE_BOOST_COMPONENTS system filesystem)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	if (NOT Boost_FOUND)
		# Попробуем с другим типом либ
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	find_package(Boost QUIET)
	if(Boost_INCLUDE_DIR)
		include_directories(${Boost_INCLUDE_DIR})
	endif()
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()

# Собрать Bullet
#
add_subdirectory("Bullet")
mark_as_advanced(FORCE BUILD_AMD_OPENCL_DEMOS)
mark_as_advanced(FORCE BUILD_CPU_DEMOS)
mark_as_advanced(FORCE BUILD_DEMOS)
mark_as_advanced(FORCE BUILD_EXTRAS)
mark_as_advanced(FORCE BUILD_MINICL_OPENCL_DEMOS)
mark_as_advanced(FORCE BUILD_NVIDIA_OPENCL_DEMOS)
mark_as_advanced(FORCE BUILD_UNIT_TESTS)
mark_as_advanced(FORCE USE_GRAPHICAL_BENCHMARK)
mark_as_advanced(FORCE USE_GLUT)
mark_as_advanced(FORCE USE_MSVC_RUNTIME_LIBRARY_DLL)
mark_as_advanced(FORCE INSTALL_LIBS)
mark_as_advanced(FORCE INSTALL_EXTRA_LIBS)
set(BULLET_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Bullet/src/" "${CMAKE_SOURCE_DIR}/Bullet/Extras/ConvexDecomposition/")
set(BULLET_LIBRARIES BulletDynamics BulletCollision LinearMath BulletSoftBody ConvexDecomposition)

# Собрать OgreBullet
#
add_subdirectory("OgreBullet")

# Собрать Paged Geomertry
#
add_subdirectory("PagedGeometry")

# Собрать Caelum
#
add_subdirectory("Caelum")

# Собрать Hydrax
#
add_subdirectory("Hydrax")


# BeeEngine
#
add_subdirectory("BeeEngine")

# HiveGame
#
add_subdirectory("HiveGame")
