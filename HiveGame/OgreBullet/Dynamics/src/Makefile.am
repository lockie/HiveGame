INCLUDES = \
 ${OGRE_CFLAGS} \
 ${bullet_CFLAGS} \
 -I../../Collisions/include \
 -I../include

lib_LTLIBRARIES = libOgreBulletDyn.la

libOgreBulletDyn_la_SOURCES = \
 OgreBulletDynamicsConstraint.cpp \
 OgreBulletDynamicsObjectState.cpp \
 OgreBulletDynamicsPrecompiled.cpp \
 OgreBulletDynamicsRigidBody.cpp \
 OgreBulletDynamicsWorld.cpp \
 Constraints/OgreBulletDynamics6DofConstraint.cpp \
 Constraints/OgreBulletDynamics6DofSpringConstraint.cpp \
 Constraints/OgreBulletDynamicsConeTwistConstraint.cpp \
 Constraints/OgreBulletDynamicsHingeConstraint.cpp \
 Constraints/OgreBulletDynamicsPoint2pointConstraint.cpp \
 Constraints/OgreBulletDynamicsRaycastVehicle.cpp \
 Prefab/OgreBulletDynamicsRagDoll.cpp

libOgreBulletDyn_la_LIBADD = \
 $(OGRE_LIBS) \
 $(bullet_LIBS) \
 -L../../Collisions/src \
 -lOgreBulletCol

