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

#include "PagedGeometry.h"

#include "DotSceneLoader.hpp"
#include "World.hpp"

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;
using namespace Forests;


static const float default_restitution = 0.1f;
static const float default_friction    = 0.1f;
static const float default_mass        = 1.0f;

template<> World* Ogre::Singleton<World>::ms_Singleton = NULL;

World::World(SceneManager* sceneMgr, Viewport* viewPort, const String& resourcesDir, 
	Vector3& gravityVector, AxisAlignedBox& bounds) :
mSceneMgr(sceneMgr), mViewPort(viewPort), mResourcesDir(resourcesDir),
mTerrainGlobalOptions(NULL), mTerrainGroup(NULL)
{
	// ¬ключить Bullet
	mWorld = new DynamicsWorld(mSceneMgr, bounds, gravityVector);

	// ¬ключить отладочную рисовашку
	debugDrawer = new DebugDrawer();
	debugDrawer->setDrawWireframe(true);
	//mWorld->setDebugDrawer(debugDrawer);
	//mWorld->setShowDebugShapes(true);
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		"debugDrawer", Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *> (debugDrawer));

	ms_Singleton = this;
}

World::~World()
{
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobalOptions;

	delete mWorld->getDebugDrawer();
	mWorld->setDebugDrawer(NULL);
	delete mWorld;
}

bool World::Load(const String& filename)
{
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobalOptions;

	mTerrainGlobalOptions = OGRE_NEW Ogre::TerrainGlobalOptions;
	DotSceneLoader loader;
	loader.parseDotScene(filename, "Scene", mResourcesDir,
		mSceneMgr, mViewPort, mTerrainGlobalOptions);
	mTerrainGroup = loader.getTerrainGroup();
	mPagedGeometryHandles = loader.mPGHandles;
	mCaelum = loader.mCaelum;
	mHydrax = loader.mHydrax;
	mOriginalWaterColor = loader.mOriginalWaterColor;
	mHydraxCaelumIntegration = loader.mHydraxCaelumIntegration;
	for(std::vector<PagedGeometry*>::iterator it = mPagedGeometryHandles.begin();
		it != mPagedGeometryHandles.end(); ++it)
	{
		// если здесь рушитс€, см. BeeEngine::createCamera()
		(*it)->setCamera(mSceneMgr->getCamera("PlayerCamera"));
		(*it)->update();
	}

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

void World::getTime(int& hour, int& minute, int& second)
{
	unsigned long long sec = mCaelum->getUniversalClock()->getJulianSecond();
	sec %= (60 * 60 * 24);
	hour = sec / (60 * 60);
	minute = (sec - hour * 60 * 60) / 60;
	second = sec - minute * 60 - hour * 60 * 60;

	hour += 12; hour %= 24;
}

bool World::frameStarted(const FrameEvent& evt)
{
	mWorld->stepSimulation(evt.timeSinceLastFrame);
	return true;
}
 
bool World::frameEnded(const FrameEvent& evt)
{
	mWorld->stepSimulation(evt.timeSinceLastFrame);
	if(mCaelum)
	{
		mCaelum->updateSubcomponents(evt.timeSinceLastFrame);
		mCaelum->notifyCameraChanged(mViewPort->getCamera());
	}
	if(mHydrax)
	{
		if(mHydraxCaelumIntegration && mCaelum)
		{
			Ogre::Vector3 value = mCaelum->getSun()->getSceneNode()->getPosition();
			Ogre::ColourValue cval = mCaelum->getSun()->getBodyColour();
			mHydrax->setSunPosition(value);
			mHydrax->setSunColor(Ogre::Vector3(cval.r,cval.g,cval.b));

			Caelum::LongReal mJulian = mCaelum->getUniversalClock()->getJulianDay();
			cval = mCaelum->getSunLightColour(mJulian, mCaelum->getSunDirection(mJulian));
			mHydrax->setWaterColor(Ogre::Vector3(cval.r - 0.3, cval.g - 0.2, cval.b));

			Ogre::Vector3 col = mHydrax->getWaterColor();
			float height = mHydrax->getSunPosition().y / 10.0f;

			if(height < -99.0f)
			{
				col = mOriginalWaterColor * 0.1f;
				height = 9999.0f;
			}
			else if(height < 1.0f)
			{
				col = mOriginalWaterColor * (0.1f + (0.009f * (height + 99.0f)));
				height = 100.0f / (height + 99.001f);
			}
			else if(height < 2.0f)
			{
				col += mOriginalWaterColor;
				col /= 2.0f;
				float percent = (height - 1.0f);
				col = (col * percent) + (mOriginalWaterColor * (1.0f - percent));
			}
			else
			{
				col += mOriginalWaterColor;
				col	/= 2.0f;
			}
			mHydrax->setWaterColor(col);
			mHydrax->setSunArea(height * 10);
		}
		mHydrax->update(evt.timeSinceLastFrame);
	}
	return true;
}

bool World::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	for(std::vector<PagedGeometry*>::iterator it = mPagedGeometryHandles.begin();
		it != mPagedGeometryHandles.end(); ++it)
		(*it)->update();
	return true;
}
