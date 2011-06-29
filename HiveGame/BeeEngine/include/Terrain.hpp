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

#ifndef __Terrain_hpp__
#define __Terrain_hpp__

#include <map>

#include <Ogre.h>
#include <Paging/OgrePage.h>
#include <Paging/OgrePageManager.h>
#include <Paging/OgreGrid2DPageStrategy.h>

#include <Shapes/OgreBulletCollisionsTerrainShape.h>
#include <OgreBulletDynamicsRigidBody.h>


class SynchronousGrid2DPageStrategy : public Ogre::Grid2DPageStrategy
{
public:
	SynchronousGrid2DPageStrategy(Ogre::PageManager* mgr) :
	  Ogre::Grid2DPageStrategy(mgr) { }

	void notifyCamera(Ogre::Camera* cam, Ogre::PagedWorldSection* section);
};

class TerrainPhysics
{
public:
	TerrainPhysics(OgreBulletCollisions::HeightmapCollisionShape* terrainShape,
		OgreBulletDynamics::RigidBody* terrainBody, Ogre::Real* data) :
		 mTerrainShape(terrainShape), mTerrainBody(terrainBody), mData(data) { }
	~TerrainPhysics();

private:
	TerrainPhysics();  // default ctor disabled
	TerrainPhysics(const TerrainPhysics&);  // copy ctor disabled
	TerrainPhysics& operator=(const TerrainPhysics&);  // = disabled

	OgreBulletCollisions::HeightmapCollisionShape* mTerrainShape;
	OgreBulletDynamics::RigidBody* mTerrainBody;
	Ogre::Real* mData;
};

class TerrainPhysicsProvider : public Ogre::PageProvider
{
public:
	TerrainPhysicsProvider(OgreBulletDynamics::DynamicsWorld* bulletWorld) :
	  mWorld(bulletWorld) {}
	~TerrainPhysicsProvider();

	bool prepareProceduralPage(Ogre::Page* page,
		Ogre::PagedWorldSection* section);
	bool unprepareProceduralPage(Ogre::Page* page,
		Ogre::PagedWorldSection* section);

private:
	std::map<Ogre::PageID, TerrainPhysics*> mTerrainPhysics;
	OgreBulletDynamics::DynamicsWorld* mWorld;

};

#endif  // __Terrain_hpp__
