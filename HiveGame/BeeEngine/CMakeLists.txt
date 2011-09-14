cmake_minimum_required(VERSION 2.8)

# define the project
project(BeeEngine)

configure_file(include/config.h.in config.h)

set(HDRS
	include/BeeEngine.hpp
	include/CharacterManager.hpp
	include/Character.hpp
	include/Terrain.hpp
	include/World.hpp
	include/GameObject.hpp
	include/DotSceneLoader.hpp
)

set(SRCS
	src/BeeEngine.cpp
	src/CharacterManager.cpp
	src/Character.cpp
	src/Terrain.cpp
	src/World.cpp
	src/GameObject.cpp
	src/DotSceneLoader.cpp
)

# build static libs by default
set(BUILD_SHARED_LIBS OFF)

include_directories("${CMAKE_BINARY_DIR}/BeeEngine"
	${OGRE_INCLUDE_DIRS}
	${OGRE_Terrain_INCLUDE_DIRS}
	${OGRE_Paging_INCLUDE_DIRS}
	${OIS_INCLUDE_DIRS}
	${OPENAL_INCLUDE_DIR}
	${OGG_INCLUDE_DIRS}
	${VORBISFILE_INCLUDE_DIRS}
	"../OgreOggSound/include"
	${BULLET_INCLUDE_DIRS}
	"include"
	"../OgreBullet/Collisions/include"
	"../OgreBullet/Dynamics/include"
	"../PagedGeometry/include"
	"../rapidxml"
	"../Caelum/main/include"
	"${Caelum_BINARY_DIR}/main/include"
	"../Hydrax"
)

add_library(BeeEngine STATIC ${SRCS} ${HDRS})
