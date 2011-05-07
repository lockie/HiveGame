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

#include <OgreBulletDynamicsWorld.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Shapes/OgreBulletCollisionsConvexHullShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>

#include "DotSceneLoader.hpp"
#include "World.hpp"


using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


static const float default_restitution = 0.1f;
static const float default_friction    = 0.1f;
static const float default_mass        = 1.0f;

template<> World* Ogre::Singleton<World>::ms_Singleton = NULL;

World::World(SceneManager* sceneMgr, Vector3& gravityVector,
	AxisAlignedBox& bounds) :
 mSceneMgr(sceneMgr)
{
	// ¬ключить Bullet
	mWorld = new DynamicsWorld(mSceneMgr, bounds, gravityVector);

	// ¬ключить отладочную рисовашку
	debugDrawer = new DebugDrawer();
	debugDrawer->setDrawWireframe(true);
	mWorld->setDebugDrawer(debugDrawer);
	mWorld->setShowDebugShapes(true);
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		"debugDrawer", Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *> (debugDrawer));

	ms_Singleton = this;
}

World::~World()
{
	delete mWorld->getDebugDrawer();
	mWorld->setDebugDrawer(NULL);
	delete mWorld;
}

bool World::Load(const String& filename)
{
	DotSceneLoader loader;
	loader.parseDotScene(filename, "General", mSceneMgr);
	return true;
}

SharedPtr<GameObject> World::addPlane(const Ogre::String& name, const Plane& p,
	Ogre::Real width, Ogre::Real height,
	int xsegments, int ysegments,
	Ogre::Real uTile, Ogre::Real vTile)
{
	MeshManager::getSingleton().createPlane(name,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			p, width, height, xsegments, ysegments, true, 1, uTile, vTile,
			Vector3::UNIT_Z);
	Entity* entity = mSceneMgr->createEntity(name + "_ent", name);
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(name + "_node");
	node->attachObject(entity);

	CollisionShape* shape = new StaticPlaneCollisionShape(p.normal, p.d);
	RigidBody* body = new RigidBody(name + "_phys", mWorld);
	body->setStaticShape(shape, default_restitution, default_friction);

	return SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body));
}

SharedPtr<GameObject> World::addBox(const String& name, Ogre::Real size)
{
	Entity* entity = mSceneMgr->createEntity(name + "_ent", "cube.mesh");
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		name + "_node");
	node->attachObject(entity);
	node->scale(size, size, size);

	CollisionShape* shape = 
		new BoxCollisionShape(size * entity->getBoundingBox().getSize() / 2);
	RigidBody* body = new RigidBody(name + "_phys", mWorld);
	body->setShape(node,
		shape,
		default_restitution,
		default_friction,
		default_mass,
		Vector3::ZERO,
		Quaternion::IDENTITY);

	return SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body));
}

SharedPtr<GameObject> World::addMesh(const Ogre::String& name,
	const Ogre::String& mesh, bool kinematic)
{
	Entity* entity = mSceneMgr->createEntity(name + "_ent", mesh);
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		name + "_node");
	node->attachObject(entity);

	AnimatedMeshToShapeConverter conv(entity);
	CollisionShape* shape = conv.createConvexDecomposition();

	RigidBody* body = new RigidBody(name + "_phys", mWorld);
	body->setShape(node,
		shape,
		default_restitution,
		default_friction,
		default_mass,
		Vector3::ZERO,
		Quaternion::IDENTITY);
	body->setKinematicObject(kinematic);
	if(kinematic)
		body->disableDeactivation();

	return SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body));
}

Ogre::SharedPtr<GameObject> World::addPlayer(Ogre::Entity* entity, Ogre::SceneNode* node)
{
	AnimatedMeshToShapeConverter conv(entity);
	// TODO : видимо, нужно отдельно прогрузить все кости и сделать из них композицию
	CollisionShape* shape = //conv.createAlignedBox(0, Vector3::ZERO, Quaternion::ZERO);
		conv.createConvex();

	Vector3 position = node->getPosition();
	Quaternion orientation = node->getOrientation();
	RigidBody* body = new RigidBody("Player_phys", mWorld);
	body->setShape(node,
		shape,
		default_restitution,
		default_friction,
		default_mass,
		position,
		orientation);
	body->setKinematicObject(true);
	body->disableDeactivation();

	return SharedPtr<GameObject>(new GameObject("Player", mSceneMgr, entity, node, shape, body));
}

bool World::frameStarted(const FrameEvent& evt)
{
	mWorld->stepSimulation(evt.timeSinceLastFrame);
	return true;
}
 
bool World::frameEnded(const FrameEvent& evt)
{
	mWorld->stepSimulation(evt.timeSinceLastFrame);
	return true;
}
