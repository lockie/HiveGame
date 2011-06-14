cmake_minimum_required(VERSION 2.8)

set(HDRS
	include/HiveGame.hpp
)

set(SRCS
	src/HiveGame.cpp
)

include_directories(
	${OGRE_INCLUDE_DIRS}
	${OGRE_Terrain_INCLUDE_DIRS}
	${OIS_INCLUDE_DIRS}
	${BULLET_INCLUDE_DIR}
	"include"
	"../BeeEngine/include"
	"../OgreBullet/Collisions/include"
	"../OgreBullet/Dynamics/include"
	"../PagedGeometry/include"
	"../rapidxml"
	"../Caelum/main/include"
	"${Caelum_BINARY_DIR}/main/include"
	"../Hydrax"
)

if(WIN32)
	add_executable(HiveGame WIN32 ${SRCS} ${HDRS} res/icon.rc)
endif(WIN32)
if(UNIX)
	add_executable(HiveGame ${SRCS} ${HDRS})
endif(UNIX)
if(MSVC_IDE) # грязнохак для студии с её тупыми каталогами Debug и Release и экспортом символов из exe-шников, блеать
	set_target_properties(HiveGame PROPERTIES PREFIX "../")
	set_target_properties(HiveGame PROPERTIES LINK_FLAGS  "/implib:HiveGame.lib")
endif()

target_link_libraries(HiveGame BeeEngine ${OGRE_LIBRARIES} ${OGRE_Terrain_LIBRARIES} ${OIS_LIBRARIES} ${BULLET_LIBRARIES} OgreBulletCol OgreBulletDyn PagedGeometry Caelum Hydrax)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

install(
	TARGETS HiveGame
	COMPONENT Binaries
	RUNTIME DESTINATION bin
)

if(WIN32)
	install(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/OgreTerrain.dll
		${OGRE_PLUGIN_DIR_REL}/OgrePaging.dll
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
		${OGRE_PLUGIN_DIR_DBG}/OgreTerrain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/OgrePaging_d.dll
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
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}\\\\HiveGame\\\\res\\\\installer.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}\\\\HiveGame\\\\res\\\\uninstaller.ico")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\HiveGame\\\\res\\\\banner.bmp")
set(CPACK_NSIS_COMPRESSOR "/FINAL /SOLID lzma")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/fake-human/HiveGame")
set(CPACK_NSIS_CONTACT "fake0mail0@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
SetOutPath '$INSTDIR\\\\bin'
CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\HiveGame.lnk' '$INSTDIR\\\\bin\\\\HiveGame.exe'
File /a '$%VS90COMNTOOLS%\\\\..\\\\..\\\\VC\\\\redist\\\\x86\\\\Microsoft.VC90.CRT\\\\*'
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
