
PROJECT(OgreOggSound)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE (CheckIncludeFileCXX)

# Option.
SET(OGGSOUND_THREADED YES CACHE BOOL "Enable multi-threaded streamed sounds")
SET(USE_POCO OFF)

IF(CMAKE_BUILD_TYPE STREQUAL Debug)

    # Enable all compiler warnings and debug support.
    ADD_DEFINITIONS(-D_DEBUG -g -Wall)
	
ENDIF(CMAKE_BUILD_TYPE STREQUAL Debug)


IF(OGGSOUND_THREADED)
	IF(USE_POCO)
		CHECK_INCLUDE_FILE_CXX(Poco/ScopedLock.h HAVE_POCO_H)
		IF(HAVE_POCO_H)
			ADD_DEFINITIONS(-DOGGSOUND_THREADED=1)
			ADD_DEFINITIONS(-DPOCO_THREAD=1)
		ENDIF()
	ELSE()
		SET(Boost_ADDITIONAL_VERSIONS "1.44" "1.43" "1.42")
		FIND_PACKAGE(Boost 1.42.0 COMPONENTS thread)
		IF (Boost_FOUND)
			INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
			LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
			LINK_LIBRARIES(${Boost_LIBRARIES})
			ADD_DEFINITIONS(-DOGGSOUND_THREADED=1)
		ENDIF()
	ENDIF()	 
ELSE()
    ADD_DEFINITIONS(-DOGGSOUND_THREADED=0)
ENDIF()

IF (APPLE)
   FIND_PACKAGE(OPENAL REQUIRED)
   FIND_LIBRARY(OGRE OGRE REQUIRED)
   FIND_LIBRARY(OGG Ogg REQUIRED)
   FIND_LIBRARY(VORBIS Vorbis REQUIRED)

   set(OGG_LIBRARIES ${OGG})
   set(VORBISFILE_LIBRARIES ${VORBIS})
   set(VORBISFILE_INCLUDE_DIRS ${VORBIS_INCLUDE_DIRS})
ELSE (APPLE) 
    #FIND_PACKAGE(PkgConfig REQUIRED)
    #PKG_CHECK_MODULES(OGRE REQUIRED OGRE)
    #PKG_CHECK_MODULES(OPENAL REQUIRED openal)
    #PKG_CHECK_MODULES(OGG REQUIRED ogg)
    #PKG_CHECK_MODULES(VORBISFILE REQUIRED vorbisfile)
ENDIF (APPLE)

# TODO : efx.h на самом деле есть
CHECK_INCLUDE_FILE_CXX(efx.h HAVE_EFX)

AUX_SOURCE_DIRECTORY(src OGREOGGSOUND_SRCS)

INCLUDE_DIRECTORIES(include
                    ${OGRE_INCLUDE_DIRS}
                    ${OPENAL_INCLUDE_DIR}
                    ${OGG_INCLUDE_DIRS}
                    ${VORBISFILE_INCLUDE_DIRS})

LINK_DIRECTORIES(${OGRE_LIBRARY_DIRS}
                 ${OPENAL_LIBRARY_DIRS})

LINK_LIBRARIES(${OGRE_LIBRARIES}
               ${OPENAL_LIBRARY}
               ${OGG_LIBRARIES}
               ${VORBISFILE_LIBRARIES})

ADD_LIBRARY(${PROJECT_NAME} SHARED ${OGREOGGSOUND_SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES
	PREFIX "")
#set_target_properties(${PROJECT_NAME} PROPERTIES
#	DEBUG_POSTFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR})


if(MSVC_IDE) # грязнохаки для студии
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "../")
endif()

#INSTALL(FILES include/LocklessQueue.h
#              include/OgreOggISound.h
#              include/OgreOggListener.h
#              include/OgreOggSoundCallback.h
#              include/OgreOggSoundFactory.h
#              include/OgreOggSound.h
#              include/OgreOggSoundManager.h
#              include/OgreOggSoundPlugin.h
#              include/OgreOggSoundPrereqs.h
#              include/OgreOggSoundRecord.h
#              include/OgreOggStaticSound.h
#              include/OgreOggStaticWavSound.h
#              include/OgreOggStreamSound.h
#              include/OgreOggStreamWavSound.h
#        DESTINATION include/OgreOggSound)

#INSTALL(TARGETS ${PROJECT_NAME}
#		LIBRARY DESTINATION lib${LIB_SUFFIX}
#		ARCHIVE DESTINATION lib${LIB_SUFFIX}
#		RUNTIME DESTINATION bin)
