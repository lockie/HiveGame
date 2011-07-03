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

#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainPagedWorldSection.h>

#include "Hydrax.h"

#include "Terrain.hpp"

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;


void SynchronousGrid2DPageStrategy::notifyCamera(Camera* cam,
	PagedWorldSection* section)
{
	Grid2DPageStrategyData* stratData = 
		static_cast<Grid2DPageStrategyData*>(section->getStrategyData());

	const Vector3& pos = cam->getDerivedPosition();
	Vector2 gridpos;
	stratData->convertWorldToGridSpace(pos, gridpos);
	int32 x, y;
	stratData->determineGridLocation(gridpos, &x, &y);

	Real loadRadius = stratData->getLoadRadiusInCells();
	Real holdRadius = stratData->getHoldRadiusInCells();
	// scan the whole Hold range
	Real fxmin = (Real)x - holdRadius;
	Real fxmax = (Real)x + holdRadius;
	Real fymin = (Real)y - holdRadius;
	Real fymax = (Real)y + holdRadius;

	int32 xmin = stratData->getCellRangeMinX();
	int32 xmax = stratData->getCellRangeMaxX();
	int32 ymin = stratData->getCellRangeMinY();
	int32 ymax = stratData->getCellRangeMaxY();

	// Round UP max, round DOWN min
	xmin = fxmin < xmin ? xmin : (int32)floor(fxmin);
	xmax = fxmax > xmax ? xmax : (int32)ceil(fxmax);
	ymin = fymin < ymin ? ymin : (int32)floor(fymin);
	ymax = fymax > ymax ? ymax : (int32)ceil(fymax);
	// the inner, active load range
	fxmin = (Real)x - loadRadius;
	fxmax = (Real)x + loadRadius;
	fymin = (Real)y - loadRadius;
	fymax = (Real)y + loadRadius;
	// Round UP max, round DOWN min
	int32 loadxmin = fxmin < xmin ? xmin : (int32)floor(fxmin);
	int32 loadxmax = fxmax > xmax ? xmax : (int32)ceil(fxmax);
	int32 loadymin = fymin < ymin ? ymin : (int32)floor(fymin);
	int32 loadymax = fymax > ymax ? ymax : (int32)ceil(fymax);

	for (int32 cy = ymin; cy <= ymax; ++cy)
	{
		for (int32 cx = xmin; cx <= xmax; ++cx)
		{
			PageID pageID = stratData->calculatePageID(cx, cy);
			if (cx >= loadxmin && cx <= loadxmax && cy >= loadymin && cy <= loadymax)
			{
				// in the 'load' range, request it
				// **synchronously**
				section->loadPage(pageID, true);
			}
			else
			{
				// in the outer 'hold' range, keep it but don't actively load
				section->holdPage(pageID);
			}
			// other pages will by inference be marked for unloading
		}
	}
}

TerrainPhysics::~TerrainPhysics()
{
	delete mTerrainShape;
	delete mTerrainBody;
	delete[] mData;
}

TerrainProvider::~TerrainProvider()
{
	for(std::map<PageID, TerrainPhysics*>::iterator it = mTerrainPhysics.begin();
		it != mTerrainPhysics.end(); ++it)
	{
		delete (*it).second;
		(*it).second = NULL;
	}
	mTerrainPhysics.clear();
}

void TerrainProvider::setHydraxMaterialManager(Hydrax::MaterialManager* manager)
{
	mMaterialManager = manager;
}

bool TerrainProvider::prepareProceduralPage(Ogre::Page* page,
	Ogre::PagedWorldSection* section)
{
	Ogre::TerrainGroup* pGroup = 
		((Ogre::TerrainPagedWorldSection*)section)->getTerrainGroup();
	long x, y;
	pGroup->unpackIndex(page->getID(), &x, &y);
	if(!pGroup->getTerrain(x, y))
		return false;

	// добавить технику глубины в материал террейна
	//
	mMaterialManager->addDepthTechnique(
		pGroup->getTerrain(x, y)->getMaterial()->createTechnique());

	if(!pGroup->getTerrain(x, y)->getHeightData())
		return false;

	// загрузить карту высот террейна в физический движок
	//
	uint16 size = pGroup->getTerrain(x, y)->getSize();
	Real* data = new Real[size*size];
	for(uint16 i = 0; i < size; i++)
		for(uint16 j = 0; j < size; j++)
			// flip along Z axis, norm to 1
			data[i*size+j] = 
				pGroup->getTerrain(x, y)->getHeightAtPoint(j, size-i-1) /
				pGroup->getTerrain(x, y)->getMaxHeight();
	Vector3 scale(pGroup->getTerrain(x, y)->getWorldSize() / (size-1),
		pGroup->getTerrain(x, y)->getMaxHeight(),
		pGroup->getTerrain(x, y)->getWorldSize() / (size-1));
	HeightmapCollisionShape* terrainShape = 
		new HeightmapCollisionShape(size, size, scale, data, false); 
	RigidBody* terrainBody = OGRE_NEW RigidBody(section->getName() + "_phys",
		mWorld, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::AllFilter);
	Vector3 terrainShiftPos = Vector3(0, scale.y * scale.y / 2, 0) +
		pGroup->getTerrain(x, y)->getPosition();
	terrainBody->setStaticShape(terrainShape, 0.01f, 5, terrainShiftPos);

	mTerrainPhysics[page->getID()] =
		new TerrainPhysics(terrainShape, terrainBody, data);

	return true;
}

bool TerrainProvider::unprepareProceduralPage(Ogre::Page* page,
	Ogre::PagedWorldSection* section)
{
	std::map<PageID, TerrainPhysics*>::iterator it =
		mTerrainPhysics.find(page->getID());
	if(it != mTerrainPhysics.end())
	{
		delete (*it).second;
		mTerrainPhysics.erase(it);
		return true;
	}
	return false;
}
