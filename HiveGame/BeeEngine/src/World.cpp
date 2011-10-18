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
#include <Shapes/OgreBulletCollisionsSphereShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Shapes/OgreBulletCollisionsConvexHullShape.h>
#include <Shapes/OgreBulletCollisionsCapsuleShape.h>
#include <Shapes/OgreBulletCollisionsTerrainShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>
#include <Debug/OgreBulletCollisionsDebugShape.h>
#include "Shapes/OgreBulletCollisionsMultiSphereShape.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include <LinearMath/btQuickprof.h>

#include "PagedGeometry.h"

#include "DotSceneLoader.hpp"
#include "World.hpp"

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;
using namespace Forests;
using namespace OgreOggSound;


static const float default_restitution = 0.1f;
static const float default_friction    = 0.1f;
static const float default_mass        = 1.0f;

template<> World* Ogre::Singleton<World>::ms_Singleton = NULL;

World::World(SceneManager* sceneMgr, Viewport* viewPort, const String& resourcesDir, 
	Vector3& gravityVector, AxisAlignedBox& bounds) :
mSceneMgr(sceneMgr), mViewPort(viewPort), mResourcesDir(resourcesDir),
mTerrainGlobalOptions(NULL), mTerrainGroup(NULL),
mTerrainPageManager(NULL), mTerrainPaging(NULL)
{
	// Включить Bullet
	mWorld = new DynamicsWorld(mSceneMgr, bounds, gravityVector);
	// http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6773
	mWorld->getBulletCollisionWorld()->getDispatchInfo().
		m_allowedCcdPenetration = 0.0001f;

	// Включить отладочную рисовашку
	debugDrawer = new DebugDrawer();
	debugDrawer->setDrawWireframe(true);
	mWorld->setDebugDrawer(debugDrawer);
	mWorld->setShowDebugShapes(true);
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		"debugDrawer", Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *> (debugDrawer));

	mTerrainProvider = new TerrainProvider(mWorld);

	// Включить звук
	mSoundManager = OgreOggSoundManager::getSingletonPtr();
	mSoundManager->init();
	mSoundManager->setDistanceModel(AL_LINEAR_DISTANCE);

	ms_Singleton = this;
}

World::~World()
{
	OGRE_DELETE mTerrainPaging;
	if(mTerrainPageManager)
		mTerrainPageManager->removeCamera(mSceneMgr->getCamera("PlayerCamera"));
	OGRE_DELETE mTerrainPageManager;
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobalOptions;

	delete mTerrainProvider;

	objects.clear();

	delete mWorld->getDebugDrawer();
	mWorld->setDebugDrawer(NULL);
	delete mWorld;
}

bool World::Load(const String& filename)
{
	OGRE_DELETE mTerrainPaging;
	OGRE_DELETE mTerrainPageManager;
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobalOptions;
	objects.clear();

	mTerrainGlobalOptions = OGRE_NEW Ogre::TerrainGlobalOptions;
	DotSceneLoader loader;
	loader.parseDotScene(filename, "Scene", mResourcesDir,
		mSceneMgr, mViewPort, mTerrainGlobalOptions, mTerrainProvider);
	mTerrainGroup = loader.getTerrainGroup();
	mTerrainPaging = loader.mTerrainPaging;
	mTerrainPageManager = loader.mPageManager;

	mPagedGeometryHandles = loader.mPGHandles;
	mCaelum = loader.mCaelum;
	mHydrax = loader.mHydrax;
	mOriginalWaterColor = loader.mOriginalWaterColor;
	mHydraxCaelumIntegration = loader.mHydraxCaelumIntegration;
	for(std::vector<PagedGeometry*>::iterator it = mPagedGeometryHandles.begin();
		it != mPagedGeometryHandles.end(); ++it)
	{
		// если здесь рушится, см. BeeEngine::createCamera()
		(*it)->setCamera(mSceneMgr->getCamera("PlayerCamera"));
		(*it)->update();
	}
	for(std::vector< SharedPtr<GameObject> >::iterator it = objects.begin();
		it != objects.end(); ++it)
	{
		(*it)->startSound();
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

	objects.push_back(SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body, NULL)));
	return objects.back();
}

SharedPtr<GameObject> World::addBox(const String& name, Real size,
	Entity* entity, SceneNode* node, const String& soundfile)
{
	if(!entity)
		entity = mSceneMgr->createEntity(name + "_ent", "cube.mesh");
	if(!node)
	{
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			name + "_node");
		node->attachObject(entity);
		node->scale(size, size, size);  // TODO : is it scale?
	}

	CollisionShape* shape = 
		new BoxCollisionShape(entity->getBoundingBox().getSize() / 2);
	shape->getBulletShape()->setLocalScaling(OgreBtConverter::to(node->getScale()));
	RigidBody* body = new RigidBody(name + "_phys", mWorld);
	body->setShape(node,
		shape,
		default_restitution,
		default_friction,
		default_mass,
		Vector3::ZERO,
		Quaternion::IDENTITY);

	OgreOggISound* sound = NULL;
	if(!soundfile.empty())
	{
		sound = mSoundManager->createSound(name + "_sound",
			soundfile, false, true);
		node->attachObject(sound);
	}

	objects.push_back(SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body, sound)));
	return objects.back();
}

SharedPtr<GameObject> World::addSphere(const String& name, Real radius,
	Entity* entity, SceneNode* node, const String& soundfile)
{
	if(!entity)
		entity = mSceneMgr->createEntity(name + "_ent", SceneManager::PT_SPHERE);
	if(!node)
	{
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			name + "_node");
		node->attachObject(entity);
	}

	CollisionShape* shape = new SphereCollisionShape(radius);
	shape->getBulletShape()->setLocalScaling(OgreBtConverter::to(node->getScale()));
	RigidBody* body = new RigidBody(name + "_phys", mWorld);
	body->setShape(node,
		shape,
		default_restitution,
		default_friction,
		default_mass,
		Vector3::ZERO,
		Quaternion::IDENTITY);

	OgreOggISound* sound = NULL;
	if(!soundfile.empty())
	{
		sound = mSoundManager->createSound(name + "_sound",
			soundfile, false, true);
		node->attachObject(sound);
	}

	objects.push_back(SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body, sound)));
	return objects.back();
}

SharedPtr<GameObject> World::addMesh(const String& name,
	const String& mesh, bool kinematic, Entity* entity, SceneNode* node, const String& soundfile)
{
	if(!entity)
		entity = mSceneMgr->createEntity(name + "_ent", mesh);
	if(!node)
	{
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			name + "_node");
		node->attachObject(entity);
	}

	// TODO : анимированные меши в физике
	StaticMeshToShapeConverter conv(entity);
	CollisionShape* shape = conv.createConvexDecomposition();
	shape->getBulletShape()->setLocalScaling(OgreBtConverter::to(node->getScale()));

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

	OgreOggISound* sound = NULL;
	if(!soundfile.empty())
	{
		sound = mSoundManager->createSound(name + "_sound",
			soundfile, false, true);
		node->attachObject(sound);
	}

	objects.push_back(SharedPtr<GameObject>(new GameObject(name, mSceneMgr, entity, node, shape, body, sound)));
	return objects.back();
}

SharedPtr<Character> World::createPlayer(const String& mesh, SceneNode* camNode)
{
	Entity* entity = mSceneMgr->createEntity("Player",
		mesh);
	SceneNode* node = mSceneMgr->getRootSceneNode()->
		createChildSceneNode(Vector3::UNIT_Y * entity->getBoundingRadius());
	node->attachObject(entity);
	node->attachObject(mSoundManager->getListener());
	//camNode->attachObject(mSoundManager->getListener());
	// TODO : так начинает звучать ещё до того, как начался рендер. Придумать
	//  что-нибудь.

	Vector3 size = entity->getBoundingBox().getSize();
	Real factor = 1 - Ogre::MeshManager::getSingleton().getBoundsPaddingFactor();
	// из документации к Bullet:
	//  The total height is height+2*radius, so the height is just the height
	//  between the center of each 'sphere' of the capsule caps.
	Real radius = factor * std::max(size.x, size.z) / 2;
	Real height = factor * size.y - 2 * radius;
	if(height < 0)
		height = 0;

	CollisionShape* shape = new CapsuleCollisionShape(
		radius, height, Vector3::UNIT_Y);

	btPairCachingGhostObject* ghost = new btPairCachingGhostObject;
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(node->getPosition()));
	ghost->setWorldTransform(startTransform);
	ghost->setCollisionShape(shape->getBulletShape());
	ghost->setFriction(0.01f);
	ghost->setRestitution(default_restitution);
	ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT | btCollisionObject::CF_KINEMATIC_OBJECT);
	mWorld->getBulletDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback);

	mWorld->getBulletCollisionWorld()->addCollisionObject(ghost, btBroadphaseProxy::CharacterFilter);
	Character* ch = new Character(entity, node, shape, ghost, mWorld);
	mWorld->getBulletDynamicsWorld()->addAction(ch);
	return SharedPtr<Character>(ch);
}

void World::getTime(int& hour, int& minute, int& second)
{
	hour = minute = second = 0;

	if(!mCaelum)
		return;

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

			Hydrax::HydraxComponent c = mHydrax->getComponents();
			if(height < 0)
			{
				if(mHydrax->isComponent(Hydrax::HYDRAX_COMPONENT_CAUSTICS))
					mHydrax->setComponents(Hydrax::HydraxComponent(c ^ Hydrax::HYDRAX_COMPONENT_CAUSTICS));
			} else
			{
				if(!mHydrax->isComponent(Hydrax::HYDRAX_COMPONENT_CAUSTICS))
					mHydrax->setComponents(Hydrax::HydraxComponent(c | Hydrax::HYDRAX_COMPONENT_CAUSTICS));
			}

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
	mTerrainGroup->update();
	for(std::vector<PagedGeometry*>::iterator it = mPagedGeometryHandles.begin();
		it != mPagedGeometryHandles.end(); ++it)
		(*it)->update();
	mSoundManager->update(evt.timeSinceLastFrame);
	return true;
}
