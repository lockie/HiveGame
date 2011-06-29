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

#ifndef __Character_h__
#define __Character_h__

#include <Ogre.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

#include <OgreBulletCollisionsShape.h>
#include <OgreBulletDynamicsWorld.h>


class Character	: public btKinematicCharacterController
{
friend class World;
public:
	~Character();

	const Ogre::Vector3& getPosition() const;
	void setPosition(const Ogre::Vector3&);
	const Ogre::Quaternion& getOrientation() const;
	void yaw(const Ogre::Radian& angle);
	Ogre::AnimationState* getAnimationState(const Ogre::String& name) const;
	Ogre::SkeletonInstance* getSkeleton() const;

	void update();

private:
	Character(Ogre::Entity* entity, Ogre::SceneNode* node,
		OgreBulletCollisions::CollisionShape* shape,
		btPairCachingGhostObject* ghostObject,
		OgreBulletDynamics::DynamicsWorld* parent);

	Ogre::SceneNode* mBodyNode;
	Ogre::Entity* mBodyEnt;
	OgreBulletCollisions::CollisionShape* mShape;
	OgreBulletDynamics::DynamicsWorld* mParent;

};

#endif  // #ifndef __Character_h__
