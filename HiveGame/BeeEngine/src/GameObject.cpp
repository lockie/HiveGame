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

#include <OgreBulletDynamicsRigidBody.h>
#include <OgreBulletCollisionsShape.h>

#include "World.hpp"
#include "GameObject.hpp"

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


GameObject::~GameObject()
{
	delete mBody;
	delete mShape;
	mNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(mNode);
	mSceneMgr->destroyEntity(mEntity);
}

const Ogre::Vector3& GameObject::getPosition() const
{
	return mNode->getPosition();
}

void GameObject::setPosition(const Ogre::Vector3& pos)
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(OgreBtConverter::to(pos));
	mBody->getBulletRigidBody()->setWorldTransform(transform);
}

const Ogre::Quaternion& GameObject::getOrientation() const
{
	return mNode->getOrientation();
}

void GameObject::setOrientation(const Quaternion& q)
{
	btTransform transform = mBody->getBulletRigidBody()->getCenterOfMassTransform();
	transform.setRotation(OgreBtConverter::to(q));
	mBody->getBulletRigidBody()->setWorldTransform(transform);
}

const Ogre::Vector3& GameObject::getScale() const
{
	return mNode->getScale();
}

void GameObject::setScale(const Ogre::Vector3& scale)
{
	mNode->setScale(scale);
	mShape->getBulletShape()->setLocalScaling(
		OgreBtConverter::to(scale));
	
	// мы сменили форму физического тела, поэтому нужно пересоздать тело
	btScalar restitution = mBody->getBulletRigidBody()->getRestitution(),
		friction = mBody->getBulletRigidBody()->getFriction(),
		mass = mBody->getBulletRigidBody()->getInvMass();
	DynamicsWorld* world = mBody->getDynamicsWorld();
	bool kinematic = mBody->isKinematicObject();

	delete mBody;
	mBody = new RigidBody(mName + "_phys", world);

	// ... и установить новую форму
	mBody->setShape(mNode, mShape, restitution, friction, mass, 
		mNode->getPosition(), mNode->getOrientation());
	mBody->setKinematicObject(kinematic);
	if(kinematic)
		mBody->disableDeactivation();
}

void GameObject::setMaterial(const String& material)
{
	mEntity->setMaterialName(material);
}

void GameObject::setRestitution(Real rest)
{
	mBody->getBulletRigidBody()->setRestitution(rest);
}

void GameObject::setFriction(Real frict)
{
	mBody->getBulletRigidBody()->setFriction(frict);
}

void GameObject::setMass(Real mass)
{
	mShape->getBulletShape()->calculateLocalInertia(mass, btVector3(0, 0, 0));
}

void GameObject::setVelocity(const Vector3& vel)
{
	mBody->setLinearVelocity(vel);
}

GameObject::GameObject(const String& name, Ogre::SceneManager* parent,
	Entity* entity, SceneNode* node,
	CollisionShape* shape, RigidBody* body) : 
 mSceneMgr(parent), mName(name), mEntity(entity), mNode(node), mShape(shape), mBody(body)
{
}
