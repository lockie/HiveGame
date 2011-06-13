cmake_minimum_required(VERSION 2.8)

# define the project
project(BeeEngine)

set(HDRS
	include/BeeEngine.hpp
	include/CharacterManager.hpp
	include/World.hpp
	include/GameObject.hpp
	include/DotSceneLoader.hpp
)

set(SRCS
	src/BeeEngine.cpp
	src/CharacterManager.cpp
	src/World.cpp
	src/GameObject.cpp
	src/DotSceneLoader.cpp
)

# build static libs by default
set(BUILD_SHARED_LIBS OFF)

include_directories(${OGRE_INCLUDE_DIRS}
	${OGRE_Terrain_INCLUDE_DIRS}
	${OIS_INCLUDE_DIRS}
	${BULLET_INCLUDE_DIR}
	"include"
	"../OgreBullet/Collisions/include"
	"../OgreBullet/Dynamics/include"
	"../PagedGeometry/include"
	"../rapidxml"
	"../Caelum/main/include"
	"${Caelum_BINARY_DIR}/main/include"
)

add_library(BeeEngine STATIC ${SRCS} ${HDRS})
