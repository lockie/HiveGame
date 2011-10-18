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
	set(PLUGINS_DIR ".\\\\")
endif(WIN32)
if(UNIX)
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
		set(PLUGINS_DIR "/usr/lib/OGRE/")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CMAKE_MODULE_PATH "/usr/lib64/OGRE/cmake/;${CMAKE_MODULE_PATH}")
		set(PLUGINS_DIR "/usr/lib64/OGRE/")
	endif()
	if(EXISTS "/usr/local/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
		set(PLUGINS_DIR "/usr/local/lib/OGRE/")
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

# Найти OpenAL
#
find_package(OpenAL REQUIRED)
if(NOT OPENAL_FOUND)
	message(SEND_ERROR "Failed to find OpenAL.")
endif()

# Собрать libogg
#
add_subdirectory("libogg")

# Собрать libvorbis
#
add_subdirectory("libvorbis")

# Собрать OgreOggSound
#
add_subdirectory("OgreOggSound")

# Собрать Bullet
#
add_subdirectory("Bullet")
mark_as_advanced(FORCE BUILD_AMD_OPENCL_DEMOS BUILD_CPU_DEMOS BUILD_DEMOS BUILD_EXTRAS BUILD_MINICL_OPENCL_DEMOS BUILD_NVIDIA_OPENCL_DEMOS BUILD_UNIT_TESTS USE_GRAPHICAL_BENCHMARK USE_GLUT USE_MSVC_RUNTIME_LIBRARY_DLL INSTALL_LIBS INSTALL_EXTRA_LIBS)
mark_as_advanced(FORCE INTERNAL_CREATE_DISTRIBUTABLE_MSVC_PROJECTFILES INTERNAL_CREATE_MSVC_RELATIVE_PATH_PROJECTFILES INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES INTERNAL_UPDATE_SERIALIZATION_STRUCTURES)
mark_as_advanced(FORCE USE_DOUBLE_PRECISION USE_GRAPHICAL_BENCHMARK USE_MULTITHREADED_BENCHMARK USE_MSVC_RUNTIME_LIBRARY_DLL USE_MSVC_INCREMENTAL_LINKING USE_CUSTOM_VECTOR_MATH USE_DX11 USE_MSVC_SSE USE_MSVC_FAST_FLOATINGPOINT)
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

# Собрать MyGUI
#
if(WIN32)
	add_subdirectory("freetype")
endif()
set(FREETYPE_FT2BUILD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/freetype/include")
set(FREETYPE_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/freetype/include/freetype")
set(FREETYPE_LIBRARIES "freetype")
set(FREETYPE_FOUND TRUE)
add_subdirectory("MyGUI")


# BeeEngine
#
add_subdirectory("BeeEngine")

# HiveGame
#
add_subdirectory("HiveGame")
