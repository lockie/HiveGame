######################################################################
# OGITOR BUILD SYSTEM
# Welcome to the CMake build system for OGITOR.
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Works around warnings libraries linked against that don't
# have absolute paths (e.g. -lOgreMain)
cmake_policy(SET CMP0003 NEW)

# Works around warnings about escaped quotes in ADD_DEFINITIONS
# statements.
cmake_policy(SET CMP0005 NEW)

project(Ogitor)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/RunPath")

option(OGITOR_DIST "Ogitor Installer Installation" FALSE)

option(OGITOR_EXPERIMENTAL_INTERFACE "Use experimental irrelevant stuff" FALSE)

set(OGITOR_PREPROCESSOR "OGITOR_EXPORT")

set(OGITOR_QT_PREPROCESSOR "")


# Find Boost
# Prefer static linking in all cases
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	set(Boost_ADDITIONAL_VERSIONS "1.38.0" "1.38" "1.39.0" "1.39" "1.40.0" "1.40" "1.44.0" "1.44" "1.45.0" "1.45" "1.46.0" "1.46.1" "1.46")
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time regex)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	if (NOT Boost_FOUND)
		# Try again with the other type of libs
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	find_package(Boost QUIET)
endif(NOT OGRE_BUILD_PLATFORM_IPHONE)

# Somehow, relative paths doesn't work on Linux when installing files..
IF(UNIX)
	SET(PREFIX ${CMAKE_INSTALL_PREFIX})
ELSE()
	SET(PREFIX "./")
ENDIF()

set(OGITOR_WORK_DIR ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
SET(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

#IF(UNIX)
#   SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#   SET(CMAKE_INSTALL_RPATH ../lib ../Plugins)
#ENDIF(UNIX)

find_package(OGRE REQUIRED)

if(APPLE)
    find_package(CG REQUIRED)
endif(APPLE)

IF ((NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha") AND (NOT  "${OGRE_VERSION_NAME}" STREQUAL "Byatis"))
  MESSAGE(SEND_ERROR "You need Ogre 1.7 Cthugha or 1.8.0 Byatis to build this.")
ENDIF()

if(UNIX)
    get_filename_component(OGRE_PLUGIN_DIR_LINUX ${OGRE_Plugin_OctreeSceneManager_LIBRARY_REL} PATH)
    set (OGRE_PLUGIN_DIR_LINUX ${OGRE_PLUGIN_DIR_LINUX} CACHE STRING "Ogre plugin dir (release)")
endif(UNIX)

include_directories(${OGRE_INCLUDE_DIRS})
include_directories(${OGRE_Terrain_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIR})
#library_directories(${Boost_LIBRARY_DIR})

# Ogitor
SET(OGITOR_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/Ogitor/include")
SET(OGITOR_LIBPATH  "${CMAKE_CURRENT_SOURCE_DIR}/Ogitor/lib")

# Dependencies
SET(DEPENDENCIES_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/include")
SET(DEPENDENCIES_LIBPATH "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/lib")

# Resources (Only used by OSX)
SET(MAC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Mac")
SET(RUNPATH "${CMAKE_CURRENT_SOURCE_DIR}/RunPath")

#set_target_properties(ALL_BUILD PROPERTIES SOLUTION_FOLDER CMake) 
#set_target_properties(ZERO_CHECK PROPERTIES SOLUTION_FOLDER CMake) 
#set_target_properties(INSTALL PROPERTIES SOLUTION_FOLDER CMake) 
#set_target_properties(PACKAGE PROPERTIES SOLUTION_FOLDER CMake) 

# Fix the plugins configuration file for Linux
if(UNIX)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/plugins.cfg.in ${OGITOR_WORK_DIR}/RunPath/bin/plugins.cfg)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/plugins.cfg.in ${OGITOR_WORK_DIR}/RunPath/plugins.cfg.backup_copy)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/Templates/ogitor.desktop.in ${OGITOR_WORK_DIR}/RunPath/bin/qtOgitor.desktop)
endif(UNIX)

file(MAKE_DIRECTORY ${OGITOR_WORK_DIR}/RunPath/Plugins/debug)
file(MAKE_DIRECTORY ${OGITOR_WORK_DIR}/RunPath/bin)

add_subdirectory(Ogitor)

# Setup Dependencies
add_subdirectory(Dependencies/HYDRAX/Hydrax/src/Hydrax)
add_subdirectory(Dependencies/Caelum-0.5.0/main)
add_subdirectory(Dependencies/PagedGeometry)
add_subdirectory(Dependencies/SkyX)
IF(OGITOR_EXPERIMENTAL_INTERFACE)
add_subdirectory(Dependencies/MeshMagick)
ENDIF(OGITOR_EXPERIMENTAL_INTERFACE)


# plugins
add_subdirectory(Plugins/OgCaelum)
add_subdirectory(Plugins/OgHydrax)
add_subdirectory(Plugins/OgSkyx)
add_subdirectory(Plugins/DotSceneSerializer)
add_subdirectory(Plugins/OgModularZone)

add_subdirectory(qtOgitor/qtpropertybrowser-2.5)

add_subdirectory(qtOgitor)

add_subdirectory(RunPath)

add_subdirectory(Help)

IF(WIN32)
install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
    ${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
	${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
	${OGRE_PLUGIN_DIR_REL}/Plugin_OctreeSceneManager.dll
	${OGRE_PLUGIN_DIR_REL}/cg.dll
	${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
	${OGRE_PLUGIN_DIR_REL}/Plugin_ParticleFX.dll
	${OGRE_PLUGIN_DIR_REL}/OgreTerrain.dll
	${OGRE_PLUGIN_DIR_REL}/OgrePaging.dll
    DESTINATION bin
	CONFIGURATIONS Release)

install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
	${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
	${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
	${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeSceneManager_d.dll
	${OGRE_PLUGIN_DIR_DBG}/cg.dll
	${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.dll
	${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.dll
	${OGRE_PLUGIN_DIR_DBG}/OgreTerrain_d.dll
	${OGRE_PLUGIN_DIR_DBG}/OgrePaging_d.dll
	#${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.pdb
    #${OGRE_PLUGIN_DIR_DBG}/Plugin_OctreeSceneManager_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/Plugin_BSPSceneManager_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/OgreTerrain_d.pdb
	#${OGRE_PLUGIN_DIR_DBG}/OgrePaging_d.pdb
	DESTINATION bin
	CONFIGURATIONS Debug)

IF(OGITOR_DIST)
    install(FILES redist/dxwebsetup.exe
        DESTINATION redist
        CONFIGURATIONS Release)
    install(FILES redist/msvcr90.dll
		redist/msvcp90.dll
        DESTINATION bin
        CONFIGURATIONS Release)
ENDIF(OGITOR_DIST)

ENDIF(WIN32)

# no package support for Linux or Mac, yet.
IF(WIN32 AND NOT UNIX)

    #include (InstallRequiredSystemLibraries)

	set(CPACK_INSTALL_CMAKE_PROJECTS "${OGITOR_WORK_DIR};Ogitor;ALL;/")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ogre Editing Framework")
	set(CPACK_PACKAGE_INSTALL_DIRECTORY "Ogitor")
	set(CPACK_PACKAGE_NAME "Ogitor")
	set(CPACK_PACKAGE_VENDOR "The Ogitor Team")
	set(CPACK_PACKAGE_VERSION "0.4.4")
	set(CPACK_PACKAGE_VERSION_MAJOR "0")
	set(CPACK_PACKAGE_VERSION_MINOR "4")
	set(CPACK_PACKAGE_VERSION_PATCH "4")
	set(CPACK_NSIS_COMPRESSOR "/FINAL /SOLID lzma")

	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
	set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\Resources\\\\OgitorInstall.bmp")
    SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\qtOgitor.exe")
	SET(CPACK_PACKAGE_EXECUTABLES "qtOgitor" "qtOgitor")
    SET(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$DESKTOP\\\\qtOgitor.lnk\\\" \\\"$INSTDIR\\\\bin\\\\qtOgitor.exe\\\"")
	
    # Use custom version of NSIS.InstallOptions.ini
	set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/packaging" ${CMAKE_MODULE_PATH})

    # File types association:
    SET(CPACK_NSIS_DEFINES "!include ${PROJECT_SOURCE_DIR}/CMakeModules\\\\packaging\\\\FileAssociation.nsh")
	
	getenv_path(USERPROFILE)

	SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
		ExecWait '$SYSDIR\\\\cmd.exe /Q /C xcopy \\\"$INSTDIR\\\\Media\\\" \\\"$PROFILE\\\\OgitorProjects\\\\Media\\\\\\\" /E /Y'
		CreateDirectory \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\Ogre*.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\Plugin*.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\RenderSystem*.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\cg.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\OIS.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\Hydrax.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\msvcr90.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\msvcp90.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\Caelum.dll\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\plugins.cfg\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\resources.cfg\\\" \\\"$PROFILE\\\\OgitorProjects\\\\bin\\\"
		ExecWait  \\\"$INSTDIR\\\\redist\\\\dxwebsetup.exe /Q\\\"
		WriteRegStr HKCU \\\"Environment\\\" \\\"OGITOR_HOME\\\" $INSTDIR
	")

    SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
      \\\${registerExtension} \\\"Ogitor Scene File\\\" \\\".ogscene\\\" \\\"\\\$INSTDIR\\\\bin\\\\qtOgitor.exe\\\"
    ")

	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} 'WriteRegStr HKCU \\\"Environment\\\" \\\"OGITOR_HOME\\\" $INSTDIR\\\\bin\\\"' )

    SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
      \\\${unregisterExtension} \\\".ogscene\\\" \\\"Ogitor Scene File\\\"
      DeleteRegKey HKCU \\\"Software\\\\Ogitor\\\"
      DeleteRegKey HKCU \\\"Environment\\\\OGITOR_HOME\\\"
	  Delete \\\"$DESKTOP\\\\qtOgitor.lnk\\\"
    ")

include(CPack)

#add_custom_target(lupdate
#   COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > ${PROJECT_SOURCE_DIR}/qtOgitor/languages/translate.pro
#   COMMAND lupdate -noobsolete ${PROJECT_BINARY_DIR}/mscore.pro
#   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#   )

#add_custom_target(lrelease
#   COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > mscore.pro
#   COMMAND lrelease ${PROJECT_BINARY_DIR}/mscore.pro
#   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
#   )

ENDIF(WIN32 AND NOT UNIX)


