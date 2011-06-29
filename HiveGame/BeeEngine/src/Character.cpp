/*  This file is part of HiveGame.
    Copyright(C) 2011 Anonymous <fake0mail0@gmail.com>

    HiveGame is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HiveGame is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HiveGame.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Utils/OgreBulletConverter.h>

#include "Character.hpp"

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;


Character::Character(Entity* entity, SceneNode* node, CollisionShape* shape,
	btPairCachingGhostObject* ghostObject, DynamicsWorld* world) :
 btKinematicCharacterController(ghostObject,
  static_cast<btConvexShape*>(shape->getBulletShape()),
  entity->getBoundingRadius() / 3),
  mBodyEnt(entity), mBodyNode(node), mShape(shape), mParent(world)
{
	m_jumpSpeed = 15;
	m_ghostObject->setCcdMotionThreshold(0.5f);
}

Character::~Character()
{
	mParent->getBulletDynamicsWorld()->removeCollisionObject(m_ghostObject);
	delete m_ghostObject;
	delete mShape;
}

const Vector3& Character::getPosition() const
{
	return mBodyNode->getPosition();
}

void Character::setPosition(const Vector3& pos)
{
	btTransform trans = m_ghostObject->getWorldTransform();
	trans.setOrigin(OgreBtConverter::to(pos));
	m_ghostObject->setWorldTransform(trans);
}

const Quaternion& Character::getOrientation() const
{
	return mBodyNode->getOrientation();
}

void Character::yaw(const Radian& angle)
{
	btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
	orn *= btMatrix3x3(btQuaternion(btVector3(0,1,0), angle.valueRadians()));
	m_ghostObject->getWorldTransform().setBasis(orn);
}

AnimationState* Character::getAnimationState(const String& name) const
{
	return mBodyEnt->getAnimationState(name);
}

SkeletonInstance* Character::getSkeleton() const
{
	return mBodyEnt->getSkeleton();
}

void Character::update()
{
	mBodyNode->setPosition(BtOgreConverter::to(m_ghostObject->getWorldTransform().getOrigin()));
	mBodyNode->setOrientation(BtOgreConverter::to(m_ghostObject->getWorldTransform().getRotation()));
}
