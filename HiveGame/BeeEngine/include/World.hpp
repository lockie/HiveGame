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

#ifndef __World_h__
#define __World_h__

#include <Ogre.h>
#include <OgreSharedPtr.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include "PagedGeometry.h"

#include "Caelum.h"

#include "Hydrax.h"

#include "GameObject.hpp"


class World: public Ogre::FrameListener, public Ogre::Singleton<World>
{
public:
	World(Ogre::SceneManager* sceneMgr, Ogre::Viewport* viewPort,
		const Ogre::String& resourcesDir,
		Ogre::Vector3& gravityVector, Ogre::AxisAlignedBox& bounds);
	~World();

	bool Load(const Ogre::String& filename);

	Ogre::SharedPtr<GameObject> addPlane(const Ogre::String& name,
		const Ogre::Plane& reference,
		Ogre::Real width, Ogre::Real height,
		int xsegments = 1, int ysegments = 1,
		Ogre::Real uTile = 1.0f, Ogre::Real vTile = 1.0f);
	Ogre::SharedPtr<GameObject> addBox(const Ogre::String& name,
		Ogre::Real size);
	Ogre::SharedPtr<GameObject> addMesh(const Ogre::String& name,
		const Ogre::String& mesh, bool kinematic = false);
	Ogre::SharedPtr<GameObject> addPlayer(Ogre::Entity*, Ogre::SceneNode*);

 	bool frameStarted(const Ogre::FrameEvent& evt);
 	bool frameEnded(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

private:
	Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewPort;
	Ogre::String mResourcesDir;
	Ogre::TerrainGlobalOptions* mTerrainGlobalOptions;
	Ogre::TerrainGroup* mTerrainGroup;
	std::vector<Forests::PagedGeometry*> mPagedGeometryHandles;
	OgreBulletDynamics::DynamicsWorld* mWorld;
	OgreBulletCollisions::DebugDrawer* debugDrawer;
	Caelum::CaelumSystem *mCaelum;
	Hydrax::Hydrax *mHydrax;
	Ogre::Vector3 mOriginalWaterColor;
	bool mHydraxCaelumIntegration;

};

#endif  // __World_h__

