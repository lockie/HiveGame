/*  This file is part of HiveGame.
    Copyright(C) 2011 Anonymous <fake0mail0@gmail.com>
    DotSceneLoader is adopted from Ogitor editor, http://ogitor.org
    Ogitor is Copyright (c) 2008-2010 Ismail TARIM and the Ogitor Team

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

#ifndef __DotSceneLoader_h__
#define __DotSceneLoader_h__

// Includes
#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreResourceGroupManager.h>
#include <vector>

#include "rapidxml.hpp"

	// Forward declarations
	namespace Ogre
	{
		class SceneManager;
		class SceneNode;
		class TerrainGroup;
		class TerrainGlobalOptions;
	}

	namespace Forests
	{
		class PagedGeometry;
		class TreeLoader3D;
		class GrassLoader;
		class GrassLayer;
	}

    namespace Caelum
    {
        class CaelumSystem;
    }

	namespace Hydrax
	{
		class Hydrax;
	}

	class nodeProperty
	{
	public:
		Ogre::String nodeName;
		Ogre::String propertyNm;
		Ogre::String valueName;
		Ogre::String typeName;

		nodeProperty(const Ogre::String &node, const Ogre::String &propertyName, const Ogre::String &value, const Ogre::String &type)
			: nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) {}
	};

	class DotSceneLoader
	{
	public:
		DotSceneLoader();
		virtual ~DotSceneLoader();

		void parseDotScene(const Ogre::String &SceneName,
			const Ogre::String &groupName, const Ogre::String& resourcesDir,
			Ogre::SceneManager *yourSceneMgr, Ogre::Viewport* viewport,
			Ogre::TerrainGlobalOptions* terrainOptions,
			Ogre::SceneNode *pAttachNode = NULL, const Ogre::String &sPrependNode = "");
		Ogre::String getProperty(const Ogre::String &ndNm, const Ogre::String &prop);

		Ogre::TerrainGroup* getTerrainGroup() { return mTerrainGroup; }

		std::vector<nodeProperty> nodeProperties;
		std::vector<Ogre::String> staticObjects;
		std::vector<Ogre::String> dynamicObjects;
		std::vector<Forests::PagedGeometry *> mPGHandles;
		std::vector<Forests::TreeLoader3D *> mTreeHandles;
		Forests::GrassLoader* mGrassLoaderHandle;  /** Handle to Forests::GrassLoader object */
		Caelum::CaelumSystem *mCaelum; /* Handle to Caelum object */
		Hydrax::Hydrax *mHydrax; /* Handle to Hydrax object */
		Ogre::Vector3 mOriginalWaterColor;
		bool mHydraxCaelumIntegration;

	protected:
		void processScene(rapidxml::xml_node<>* XMLRoot);

		void processNodes(rapidxml::xml_node<>* XMLNode);
		void processExternals(rapidxml::xml_node<>* XMLNode);
		void processResourceLocations(rapidxml::xml_node<>* XMLNode);
		void processEnvironment(rapidxml::xml_node<>* XMLNode);
		void processTerrain(rapidxml::xml_node<>* XMLNode);
		void processTerrainPage(rapidxml::xml_node<>* XMLNode);
		void processGrassLayers(rapidxml::xml_node<>* XMLNode);
		void processBlendmaps(rapidxml::xml_node<>* XMLNode);
		void processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0);
		void processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::Entity *pEntity);
		void processOctree(rapidxml::xml_node<>* XMLNode);
		void processLight(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0);
		void processCamera(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0);

		void processNode(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent = 0);
		void processLookTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
		void processTrackTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
		void processEntity(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
		void processSubEntity(rapidxml::xml_node<>* XMLNode, Ogre::Entity *pEntity);
		void processParticleSystem(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
		void processBillboardSet(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
		void processPlane(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
		void processPagedGeometry(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);

		void processFog(rapidxml::xml_node<>* XMLNode);
		void processSkyBox(rapidxml::xml_node<>* XMLNode);
		void processSkyDome(rapidxml::xml_node<>* XMLNode);
		void processSkyPlane(rapidxml::xml_node<>* XMLNode);
		void processClipping(rapidxml::xml_node<>* XMLNode);

		void processLightRange(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight);
		void processLightAttenuation(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight);

		void processCaelum(rapidxml::xml_node<>* XMLNode);
		void processHydrax(rapidxml::xml_node<>* XMLNode);

		Ogre::String getAttrib(rapidxml::xml_node<>* XMLNode, const Ogre::String &parameter, const Ogre::String &defaultValue = "");
		Ogre::Real getAttribReal(rapidxml::xml_node<>* XMLNode, const Ogre::String &parameter, Ogre::Real defaultValue = 0);
		bool getAttribBool(rapidxml::xml_node<>* XMLNode, const Ogre::String &parameter, bool defaultValue = false);

		Ogre::Vector3 parseVector3(rapidxml::xml_node<>* XMLNode);
		Ogre::Quaternion parseQuaternion(rapidxml::xml_node<>* XMLNode);
		Ogre::ColourValue parseColour(rapidxml::xml_node<>* XMLNode);

		Ogre::SceneManager *mSceneMgr;
		Ogre::Viewport *mViewPort;
		Ogre::SceneNode *mAttachNode;
		Ogre::String m_sGroupName;
		Ogre::String mResourcesDir;
		Ogre::String m_sPrependNode;
		Ogre::TerrainGroup* mTerrainGroup;
		Ogre::TerrainGlobalOptions* mTerrainGlobalOptions;
		Ogre::Vector3 mTerrainPosition;
		Ogre::Vector3 mLightDirection;

		// paged geometry related values
		int mPGPageSize;
		int mPGDetailDistance;
	};

#endif // __DotSceneLoader_h__
