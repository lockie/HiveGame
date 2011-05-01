CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(OgreBullet)


INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIR} ${BULLET_INCLUDE_DIRS}
"${BULLET_INCLUDE_DIRS}/ConvexDecomposition"
"${CMAKE_CURRENT_SOURCE_DIR}/Collisions/include" "${CMAKE_CURRENT_SOURCE_DIR}/Dynamics/include")
LINK_DIRECTORIES(${OGRE_LIB_DIR})

AUX_SOURCE_DIRECTORY(Collisions/src src0)
AUX_SOURCE_DIRECTORY(Collisions/src/Debug src1)
AUX_SOURCE_DIRECTORY(Collisions/src/Shapes src2)
AUX_SOURCE_DIRECTORY(Collisions/src/Utils src3)
ADD_LIBRARY(OgreBulletCol ${src0} ${src1} ${src2} ${src3})

AUX_SOURCE_DIRECTORY(Dynamics/src src0)
AUX_SOURCE_DIRECTORY(Dynamics/src/Constraints src1)
AUX_SOURCE_DIRECTORY(Dynamics/src/Prefab src2)
ADD_LIBRARY(OgreBulletDyn ${src0} ${src1} ${src2})
