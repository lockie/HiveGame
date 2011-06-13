INCLUDES = \
 ${OGRE_CFLAGS} \
 ${bullet_CFLAGS} \
 -I../include

lib_LTLIBRARIES = libOgreBulletCol.la

libOgreBulletCol_la_SOURCES = \
 OgreBulletCollisionsObject.cpp \
 OgreBulletCollisionsObjectState.cpp \
 OgreBulletCollisionsPrecompiled.cpp \
 OgreBulletCollisionsRay.cpp \
 OgreBulletCollisionsShape.cpp \
 OgreBulletCollisionsWorld.cpp \
 Debug/OgreBulletCollisionsDebugContact.cpp \
 Debug/OgreBulletCollisionsDebugDrawer.cpp \
 Debug/OgreBulletCollisionsDebugLines.cpp \
 Debug/OgreBulletCollisionsDebugShape.cpp \
 Shapes/OgreBulletCollisionsBoxShape.cpp \
 Shapes/OgreBulletCollisionsCapsuleShape.cpp \
 Shapes/OgreBulletCollisionsCompoundShape.cpp \
 Shapes/OgreBulletCollisionsConeShape.cpp \
 Shapes/OgreBulletCollisionsConvexHullShape.cpp \
 Shapes/OgreBulletCollisionsCylinderShape.cpp \
 Shapes/OgreBulletCollisionsGImpactShape.cpp \
 Shapes/OgreBulletCollisionsMinkowskiSumShape.cpp \
 Shapes/OgreBulletCollisionsMultiSphereShape.cpp \
 Shapes/OgreBulletCollisionsSphereShape.cpp \
 Shapes/OgreBulletCollisionsStaticPlaneShape.cpp \
 Shapes/OgreBulletCollisionsTriangleShape.cpp \
 Shapes/OgreBulletCollisionsTrimeshShape.cpp \
 Utils/OgreBulletCollisionsMeshToShapeConverter.cpp

libOgreBulletCol_la_LIBADD = \
 $(OGRE_LIBS) \
 $(bullet_LIBS)

