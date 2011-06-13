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

#ifndef __GameObject_h__
#define __GameObject_h__

#include <Ogre.h>

#include <OgreBulletDynamics.h>


class GameObject
{
friend class World;
public:
	virtual ~GameObject();

	// Геометрические свойства
	//
	const Ogre::Vector3& getPosition() const;
	void setPosition(const Ogre::Vector3&);
	const Ogre::Quaternion& getOrientation() const;
	void setOrientation(const Ogre::Quaternion&);
	const Ogre::Vector3& getScale() const;
	void setScale(const Ogre::Vector3&);
	void setMaterial(const Ogre::String&);

	// Физические свойства
	//
	// прыгучесть
	void setRestitution(Ogre::Real);
	// трение
	void setFriction(Ogre::Real);
	// масса
	void setMass(Ogre::Real);
	// скорость
	void setVelocity(const Ogre::Vector3&);

protected:
	GameObject(const Ogre::String& name, Ogre::SceneManager* parent,
		Ogre::Entity* entity, Ogre::SceneNode* sceneNode,
		OgreBulletCollisions::CollisionShape*,
		OgreBulletDynamics::RigidBody*);
	GameObject(const GameObject&);  // copy ctor disabled
	GameObject& operator= (const GameObject&);  // operator= disabled

	const Ogre::String mName;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Entity* mEntity;
	Ogre::SceneNode* mNode;
	OgreBulletCollisions::CollisionShape* mShape;
	OgreBulletDynamics::RigidBody* mBody;
};


#endif  // __GameObject_h__

