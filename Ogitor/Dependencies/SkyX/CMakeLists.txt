cmake_minimum_required (VERSION 2.6)

project (Skyx)

set (HEADER_FILES
    AtmosphereManager.h
    CloudsManager.h
    ColorGradient.h
    GPUManager.h
    MeshManager.h
    MoonManager.h
    Prerequisites.h
    SkyX.h
    VCloudsManager.h
    VClouds/DataManager.h
    VClouds/Ellipsoid.h
    VClouds/FastFakeRandom.h
    VClouds/GeometryBlock.h
    VClouds/GeometryManager.h
    VClouds/VClouds.h
)

set (SOURCE_FILES
    AtmosphereManager.cpp
    CloudsManager.cpp
    ColorGradient.cpp
    GPUManager.cpp
    MeshManager.cpp
    MoonManager.cpp
    Prerequisites.cpp
    SkyX.cpp
    VCloudsManager.cpp
    VClouds/DataManager.cpp
    VClouds/Ellipsoid.cpp
    VClouds/FastFakeRandom.cpp
    VClouds/GeometryBlock.cpp
    VClouds/GeometryManager.cpp
    VClouds/VClouds.cpp
)

set (SKYX_VERSION_MAJOR "0")
set (SKYX_VERSION_MINOR "1")
set (SKYX_VERSION_PATCH "0")
set (SKYX_VERSION "${SKYX_VERSION_MAJOR}.${SKYX_VERSION_MINOR}.${SKYX_VERSION_PATCH}")

add_library(Skyx SHARED ${HEADER_FILES} ${SOURCE_FILES}  )
SET(LIBRARY_OUTPUT_PATH  ../lib)

set_target_properties(PROPERTIES
        INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE  )

set_target_properties(Skyx PROPERTIES VERSION ${SKYX_VERSION} SOVERSION ${SKYX_VERSION_MAJOR})

set_target_properties(Skyx PROPERTIES COMPILE_DEFINITIONS SKYX_LIB)

set_target_properties(Skyx PROPERTIES SOLUTION_FOLDER Dependencies) 

target_link_libraries(Skyx ${OGRE_LIBRARIES} ${Boost_LIBRARIES})

IF(WIN32)
	add_custom_command( TARGET Skyx PRE_BUILD
		COMMAND if not exist ..\\..\\RunPath\\bin mkdir ..\\..\\RunPath\\bin )
	add_custom_command( TARGET Skyx POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\RunPath\\bin )
ENDIF(WIN32)


IF(WIN32)
install(TARGETS Skyx
	RUNTIME DESTINATION bin
	CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
ENDIF(WIN32)

#IF(UNIX AND NOT APPLE)
#install(TARGETS Skyx
#	LIBRARY DESTINATION Plugins/dependencies
#	CONFIGURATIONS Release)
#install(TARGETS Skyx
#	LIBRARY DESTINATION Plugins/dependencies
#	CONFIGURATIONS Debug)
#ENDIF(UNIX AND NOT APPLE)

