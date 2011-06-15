CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(OgreBullet)


INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR} ${BULLET_INCLUDE_DIRS}
"${CMAKE_CURRENT_SOURCE_DIR}/Collisions/include" "${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/include")
LINK_DIRECTORIES(${OGRE_LIB_DIR})

AUX_SOURCE_DIRECTORY(Collisions/src col_src0)
AUX_SOURCE_DIRECTORY(Collisions/src/Debug col_src1)
AUX_SOURCE_DIRECTORY(Collisions/src/Shapes col_src2)
AUX_SOURCE_DIRECTORY(Collisions/src/Utils col_src3)
ADD_LIBRARY(OgreBulletCol "Collisions/src/OgreBulletCollisionsPrecompiled.cpp" ${col_src0} ${col_src1} ${col_src2} ${col_src3})
if (MSVC)
	set_target_properties(OgreBulletCol PROPERTIES COMPILE_FLAGS "/YuOgreBulletCollisions.h")
	set_source_files_properties(Collisions/src/OgreBulletCollisionsPrecompiled.cpp PROPERTIES COMPILE_FLAGS "/YcOgreBulletCollisions.h")
endif(MSVC)

AUX_SOURCE_DIRECTORY(Dynamics/src dyn_src0)
AUX_SOURCE_DIRECTORY(Dynamics/src/Constraints dyn_src1)
AUX_SOURCE_DIRECTORY(Dynamics/src/Prefab dyn_src2)
ADD_LIBRARY(OgreBulletDyn "Dynamics/src/OgreBulletDynamicsPrecompiled.cpp" ${dyn_src0} ${dyn_src1} ${dyn_src2})
if (MSVC)
	set_target_properties(OgreBulletDyn PROPERTIES COMPILE_FLAGS "/YuOgreBulletDynamics.h")
	set_source_files_properties(Dynamics/src/OgreBulletDynamicsPrecompiled.cpp PROPERTIES COMPILE_FLAGS "/YcOgreBulletDynamics.h")
endif(MSVC)
