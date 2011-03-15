cmake_minimum_required(VERSION 2.8)

project(HiveGame)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR};${CMAKE_MODULE_PATH}")

if (CMAKE_BUILD_TYPE STREQUAL "")
	# Выставить тип сборки по умолчанию
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
set(CMAKE_DEBUG_POSTFIX "_d")

# Найти Ogre
#
if(WIN32)
	file(TO_CMAKE_PATH "$ENV{OGRE_HOME}" OGRE_HOME)
	set(CMAKE_MODULE_PATH "${OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
endif(WIN32)
if(UNIX)
	set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
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
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		# Статическая линковка boost к динамическому Ogre не заработает на 64-битном Linux
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif()
	if (MINGW)
		# Это, скорее всего, баг в CMake: модуль поиска boost пытается искать
		# бустовские бибилиотеки с именем libboost_*, но CMake заранее
		# добавляет префикс "lib". Далее - костылик для исправления.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif()
	set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0")
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

include_directories(${OGRE_INCLUDE_DIRS}
	${OGRE_Terrain_INCLUDE_DIRS}
	${OIS_INCLUDE_DIRS}
	"include"
)


set(HDRS
	include/BeeEngine.hpp
	include/HiveGame.hpp
	include/CharacterManager.hpp
)

set(SRCS
	src/BeeEngine.cpp
	src/HiveGame.cpp
	src/CharacterManager.cpp
)

if(WIN32)
	add_executable(HiveGame WIN32 ${SRCS} ${HDRS} res/icon.rc)
endif(WIN32)
if(UNIX)
	add_executable(HiveGame ${SRCS} ${HDRS})
endif(UNIX)
if(MSVC_IDE) # грязнохак для студии с её тупыми каталогами Debug и Release
	set_target_properties(HiveGame PROPERTIES PREFIX "../")
endif()

target_link_libraries(HiveGame ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OIS_LIBRARIES})

install(
	TARGETS HiveGame
	COMPONENT Binaries
	RUNTIME DESTINATION bin
)

if(WIN32)
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		${OGRE_PLUGIN_DIR_REL}/cg.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
		DESTINATION bin
		COMPONENT Binaries
		CONFIGURATIONS Release RelWithDebInfo
	)

	install(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		${OGRE_PLUGIN_DIR_DBG}/cg.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.dll
		DESTINATION bin
		COMPONENT Binaries
		CONFIGURATIONS Debug
	)

	install(FILES ${Boost_DATE_TIME_LIBRARY_RELEASE}
		${Boost_THREAD_LIBRARY_RELEASE}
		DESTINATION bin
		COMPONENT Binaries
		CONFIGURATIONS Release RelWithDebInfo
	)

	install(FILES ${Boost_DATE_TIME_LIBRARY_DEBUG}
		${Boost_THREAD_LIBRARY_DEBUG}
		DESTINATION bin
		COMPONENT Binaries
		CONFIGURATIONS Debug
	)
endif(WIN32)


#######################################################
# Параметры CPack (системы сборки архивов/установщиков)
#######################################################

if(WIN32)
	set(CPACK_GENERATOR NSIS)
endif(WIN32)
if(UNIX)
	set(CPACK_GENERATOR TBZ2)
endif(UNIX)

# Получить номер версии из git
#
find_package(Git REQUIRED)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty=+ --abbrev=5 --always
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_STRIP_TRAILING_WHITESPACE
	OUTPUT_VARIABLE GIT_REVISION
)
message(STATUS "Found git revision ${GIT_REVISION}")

set(CPACK_NSIS_PACKAGE_NAME "HiveGame")
set(CPACK_PACKAGE_DESCRIPTION "I'm too lazy to edit this field.")
set(CPACK_PACKAGE_VENDOR "HiveMind")
set(CPACK_PACKAGE_VERSION "${GIT_REVISION}")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\res\\\\installer.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\res\\\\uninstaller.ico")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\res\\\\banner.bmp")
set(CPACK_NSIS_COMPRESSOR "/FINAL /SOLID bzip2")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/fake-human/HiveGame")
set(CPACK_NSIS_CONTACT "fake0mail0@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
SetOutPath '$INSTDIR\\\\bin'
CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\HiveGame.lnk' '$INSTDIR\\\\bin\\\\HiveGame.exe'
File /a '$%VS80COMNTOOLS%\\\\..\\\\..\\\\VC\\\\redist\\\\x86\\\\Microsoft.VC80.CRT\\\\*'
IfFileExists $INSTDIR\\\\* +2
CreateDirectory $INSTDIR
IfFileExists $INSTDIR\\\\data.zip  success
try_again:
	inetc::get 'https://github.com/downloads/fake-human/HiveGame/data.zip' '$INSTDIR\\\\data.zip'
	Pop $1
	StrCmp $1 'OK' success
	MessageBox MB_YESNO 'Data downloading failed: $1 Try again?' IDYES try_again
	Quit
success:
	nsisunz::Unzip '$INSTDIR\\\\data.zip' '$INSTDIR'
	Delete '$INSTDIR\\\\data.zip'
")
set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
RMDir /r '$INSTDIR\\\\bin\\\\*'
RMDir /r '$INSTDIR\\\\data\\\\*'
RMDir '$INSTDIR'
")

include(CPack)
