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

#include "DotSceneLoader.hpp"
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>

#include "PagedGeometry.h"
#include "GrassLoader.h" 
#include "BatchPage.h"
#include "ImpostorPage.h"
#include "TreeLoader3D.h"

#include "Caelum.h"

#include "Hydrax.h"
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"

#ifdef _MSC_VER
# pragma warning(disable:4390)
#endif  // _MSC_VER


using namespace Forests;

Ogre::TerrainGroup *StaticGroupPtr = 0;

Ogre::Real OgitorTerrainGroupHeightFunction(Ogre::Real x, Ogre::Real z, void *userData)
{
	return StaticGroupPtr->getHeightAtWorldPosition(x,0,z);
}

DotSceneLoader::DotSceneLoader() : mSceneMgr(0), mTerrainGroup(0), mGrassLoaderHandle(0), mCaelum(0), mHydrax(0)
{
	Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Initializing");
}


DotSceneLoader::~DotSceneLoader()
{
/*	if(mGrassLoaderHandle)
		delete mGrassLoaderHandle;

	std::vector<Forests::PagedGeometry *>::iterator it = mPGHandles.begin();
	while(it != mPGHandles.end())
	{
		delete it[0];
		it++;
	}
	mPGHandles.clear();*/

	/*if(mTerrainGroup)
	{
		OGRE_DELETE mTerrainGroup;
	}

	OGRE_DELETE mTerrainGlobalOptions;*/
}

void ParseStringVector(Ogre::String &str, Ogre::StringVector &list)
{
	list.clear();
	Ogre::StringUtil::trim(str,true,true);
	if(str == "")
		return;

	int pos = str.find(";");
	while(pos != -1)
	{
		list.push_back(str.substr(0,pos));
		str.erase(0,pos + 1);
		pos = str.find(";");
	}

	if(str != "")
		list.push_back(str);
}

void DotSceneLoader::parseDotScene(const Ogre::String &SceneName,
	const Ogre::String &groupName, const Ogre::String& resourcesDir,
	Ogre::SceneManager *yourSceneMgr, Ogre::Viewport* viewport,
	Ogre::TerrainGlobalOptions* terrainOptions,
	Ogre::SceneNode *pAttachNode, const Ogre::String &sPrependNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Started scene parsing in resource group "+groupName);

	// set up shared object values
	m_sGroupName = groupName;
	mSceneMgr = yourSceneMgr;
	mViewPort = viewport;
	mResourcesDir = resourcesDir;
	mTerrainGlobalOptions = terrainOptions;
	m_sPrependNode = sPrependNode;
	staticObjects.clear();
	dynamicObjects.clear();

	rapidxml::xml_document<> XMLDoc;  // character type defaults to char

	rapidxml::xml_node<>* XMLRoot;

    // if the resource group doesn't exists create it
    if(!Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(m_sGroupName))
        Ogre::ResourceGroupManager::getSingleton().createResourceGroup(m_sGroupName);

	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(SceneName, groupName );
	Ogre::String scene = stream->getAsString();
	XMLDoc.parse<0>(const_cast<char*>(scene.c_str()));

	// Grab the scene node
	XMLRoot = XMLDoc.first_node("scene");

	// Validate the File
	if( getAttrib(XMLRoot, "formatVersion", "") == "")
	{
		Ogre::LogManager::getSingleton().logMessage(
			"[DotSceneLoader] Error: Invalid .scene File. Missing <scene>");
		return;
	}

	// figure out where to attach any nodes we create
	mAttachNode = pAttachNode;
	if(!mAttachNode)
		mAttachNode = mSceneMgr->getRootSceneNode();

	// Process the scene
	processScene(XMLRoot);

	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Scene parsing finished");
}

void DotSceneLoader::processScene(rapidxml::xml_node<>* XMLRoot)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing scene");

	// Process the scene parameters
	Ogre::String version = getAttrib(XMLRoot, "formatVersion", "unknown");

	Ogre::String message = "[DotSceneLoader] Parsing dotScene file with version " + version;
	if(XMLRoot->first_attribute("ID"))
		message += ", id " + Ogre::String(XMLRoot->first_attribute("ID")->value());
	if(XMLRoot->first_attribute("sceneManager"))
		message += ", scene manager " + Ogre::String(XMLRoot->first_attribute("sceneManager")->value());
	if(XMLRoot->first_attribute("minOgreVersion"))
		message += ", min. Ogre version " + Ogre::String(XMLRoot->first_attribute("minOgreVersion")->value());
	if(XMLRoot->first_attribute("author"))
		message += ", author " + Ogre::String(XMLRoot->first_attribute("author")->value());

	Ogre::LogManager::getSingleton().logMessage(message);

	rapidxml::xml_node<>* pElement;

	// Process resources (?)
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		mResourcesDir + "/maps/Terrain", "FileSystem", m_sGroupName);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		mResourcesDir + "/maps/Caelum", "FileSystem", Caelum::RESOURCE_GROUP_NAME);
	Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup(
		Caelum::RESOURCE_GROUP_NAME);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
		mResourcesDir + "/maps/Hydrax", "FileSystem", HYDRAX_RESOURCE_GROUP);
	Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup(
		HYDRAX_RESOURCE_GROUP);
	pElement = XMLRoot->first_node("resourceLocations");
	if(pElement)
		processResourceLocations(pElement);

	// Process environment (?)
	pElement = XMLRoot->first_node("environment");
	if(pElement)
		processEnvironment(pElement);

	// Process nodes (?)
	pElement = XMLRoot->first_node("nodes");
	if(pElement)
		processNodes(pElement);

	// Process externals (?)
	pElement = XMLRoot->first_node("externals");
	if(pElement)
		processExternals(pElement);

	// Process userDataReference (?)
	pElement = XMLRoot->first_node("userDataReference");
	if(pElement)
		processUserDataReference(pElement);

	// Process octree (?)
	pElement = XMLRoot->first_node("octree");
	if(pElement)
		processOctree(pElement);

	// Process light (?)
	pElement = XMLRoot->first_node("light");
	while(pElement)
	{
		processLight(pElement);
		pElement = pElement->next_sibling("light");
	}

	// Process camera (?)
	pElement = XMLRoot->first_node("camera");
	while(pElement)
	{
		processCamera(pElement);
		pElement = pElement->next_sibling("camera");
	}

	// Process terrain (?)
	pElement = XMLRoot->first_node("terrain");
	if(pElement)
		processTerrain(pElement);

	// Process Hydrax
	pElement = XMLRoot->first_node("hydrax");
	if(pElement)
		processHydrax(pElement);

	// Process Caelum
	pElement = XMLRoot->first_node("caelum");
	if(pElement)
		processCaelum(pElement);
}

void DotSceneLoader::processNodes(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing nodes");

	rapidxml::xml_node<>* pElement;

	// Process node (*)
	pElement = XMLNode->first_node("node");
	while(pElement)
	{
		processNode(pElement);
		pElement = pElement->next_sibling("node");
	}

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if(pElement)
	{
		mAttachNode->setPosition(parseVector3(pElement));
		mAttachNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->first_node("rotation");
	if(pElement)
	{
		mAttachNode->setOrientation(parseQuaternion(pElement));
		mAttachNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->first_node("scale");
	if(pElement)
	{
		mAttachNode->setScale(parseVector3(pElement));
		mAttachNode->setInitialState();
	}
}

void DotSceneLoader::processExternals(rapidxml::xml_node<>* XMLNode)
{
	//! @todo Implement this
}

void DotSceneLoader::processResourceLocations(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing resource locations");

	rapidxml::xml_node<>* pElement;

	// Process resources (?)
	pElement = XMLNode->first_node("resourceLocation");
	if(pElement)
	{
		// remove the particle templates what are in this resource group 
		// (error happens if an already loaded particle is loaded again)
		Ogre::ParticleSystemManager::getSingletonPtr()->removeTemplatesByResourceGroup(m_sGroupName);
		// and empty the resource group. The previously declared resource locations are not deleted!
		Ogre::ResourceGroupManager::getSingleton().clearResourceGroup(m_sGroupName);

		// add the resource locations what were in the .scene file
		while(pElement)
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation( mResourcesDir + "/maps/" + getAttrib(pElement, "name"),
                                                                        getAttrib(pElement, "type"),
                                                                        m_sGroupName );
			pElement = pElement->next_sibling("resourceLocation");
		}
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(m_sGroupName);
	}
}

void DotSceneLoader::processEnvironment(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing environment");

	rapidxml::xml_node<>* pElement;

	// Process camera (?)
	pElement = XMLNode->first_node("camera");
	if(pElement)
		processCamera(pElement);

	// Process fog (?)
	pElement = XMLNode->first_node("fog");
	if(pElement)
		processFog(pElement);

	// Process skyBox (?)
	pElement = XMLNode->first_node("skyBox");
	if(pElement)
		processSkyBox(pElement);

	// Process skyDome (?)
	pElement = XMLNode->first_node("skyDome");
	if(pElement)
		processSkyDome(pElement);

	// Process skyPlane (?)
	pElement = XMLNode->first_node("skyPlane");
	if(pElement)
		processSkyPlane(pElement);

	// Process clipping (?)
	pElement = XMLNode->first_node("clipping");
	if(pElement)
		processClipping(pElement);

	// Process colourAmbient (?)
	pElement = XMLNode->first_node("colourAmbient");
	if(pElement)
		mSceneMgr->setAmbientLight(parseColour(pElement));

	// Process colourBackground (?)
	pElement = XMLNode->first_node("colourBackground");
	if(pElement)
		mViewPort->setBackgroundColour(parseColour(pElement));

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if(pElement)
		processUserDataReference(pElement);
}

void DotSceneLoader::processTerrain(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing terrain");

	Ogre::Real worldSize = getAttribReal(XMLNode, "worldSize");
	int mapSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("mapSize")->value());
	bool colourmapEnabled = getAttribBool(XMLNode, "colourmapEnabled");
	int colourMapTextureSize = Ogre::StringConverter::parseInt(XMLNode->first_attribute("colourMapTextureSize")->value());
	int compositeMapDistance = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningCompositeMapDistance")->value());
	int maxPixelError = Ogre::StringConverter::parseInt(XMLNode->first_attribute("tuningMaxPixelError")->value());

	// TODO : штоа
	Ogre::Vector3 lightdir(0, -0.3f, 0.75f);
	lightdir.normalise();
	Ogre::Light* l = mSceneMgr->createLight("tstLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	l->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));

	mTerrainGlobalOptions->setMaxPixelError((Ogre::Real)maxPixelError);
	mTerrainGlobalOptions->setCompositeMapDistance((Ogre::Real)compositeMapDistance);
	mTerrainGlobalOptions->setLightMapDirection(lightdir);
	mTerrainGlobalOptions->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobalOptions->setCompositeMapDiffuse(l->getDiffuseColour());

	mSceneMgr->destroyLight("tstLight");

	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(mSceneMgr, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize);
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	mTerrainGroup->setResourceGroup(m_sGroupName);

	rapidxml::xml_node<>* pElement;
	rapidxml::xml_node<>* pPageElement;

	// Process terrain pages (*)
	pElement = XMLNode->first_node("terrainPages");
	if(pElement)
	{
		pPageElement = pElement->first_node("terrainPage");
		while(pPageElement)
		{
			processTerrainPage(pPageElement);
			pPageElement = pPageElement->next_sibling("terrainPage");
		}
	}
	mTerrainGroup->loadAllTerrains(true);

	mTerrainGroup->freeTemporaryResources();
	//mTerrain->setPosition(mTerrainPosition);
}

void DotSceneLoader::processTerrainPage(rapidxml::xml_node<>* XMLNode)
{
	Ogre::String name = getAttrib(XMLNode, "name");
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing terrain page \"" + name + "\"");

	int pageX = Ogre::StringConverter::parseInt(XMLNode->first_attribute("pageX")->value());
	int pageY = Ogre::StringConverter::parseInt(XMLNode->first_attribute("pageY")->value());
	mPGPageSize = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "pagedGeometryPageSize", "10"));
	mPGDetailDistance = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "pagedGeometryDetailDistance", "100"));
	// error checking
	if(mPGPageSize < 10){
		Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] pagedGeometryPageSize value error!", Ogre::LML_CRITICAL);
		mPGPageSize = 10;
	}
	if(mPGDetailDistance < 100){
		Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] pagedGeometryDetailDistance value error!", Ogre::LML_CRITICAL);
		mPGDetailDistance = 100;
	}

	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), name))
	{
		mTerrainGroup->defineTerrain(pageX, pageY, name);
	}
	
	// grass layers
	rapidxml::xml_node<>* pElement = XMLNode->first_node("grassLayers");

	if(pElement)
	{
		processGrassLayers(pElement);
	}
}

void DotSceneLoader::processGrassLayers(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing grass layers");

	Ogre::String dMapName = getAttrib(XMLNode, "densityMap");
	mTerrainGlobalOptions->setVisibilityFlags(Ogre::StringConverter::parseUnsignedInt(XMLNode->first_attribute("visibilityFlags")->value()));

	// create a temporary camera
	Ogre::Camera* tempCam = mSceneMgr->createCamera("ThIsNamEShoUlDnOtExisT");

	// create paged geometry what the grass will use
	Forests::PagedGeometry * mPGHandle = new PagedGeometry(tempCam, mPGPageSize);
	mPGHandle->addDetailLevel<GrassPage>(mPGDetailDistance);

	//Create a GrassLoader object
	mGrassLoaderHandle = new GrassLoader(mPGHandle);
	mGrassLoaderHandle->setVisibilityFlags(mTerrainGlobalOptions->getVisibilityFlags());

	//Assign the "grassLoader" to be used to load geometry for the PagedGrass instance
	mPGHandle->setPageLoader(mGrassLoaderHandle);

	// set the terrain group pointer
	StaticGroupPtr = mTerrainGroup;

	//Supply a height function to GrassLoader so it can calculate grass Y values
	mGrassLoaderHandle->setHeightFunction(OgitorTerrainGroupHeightFunction);
	
	// push the page geometry handle into the PGHandles array
	mPGHandles.push_back(mPGHandle);

	// create the layers and load the options for them
	rapidxml::xml_node<>* pElement = XMLNode->first_node("grassLayer");
	rapidxml::xml_node<>* pSubElement;
	Forests::GrassLayer* gLayer;
	Ogre::String tempStr;
	while(pElement)
	{
		// grassLayer
		gLayer = mGrassLoaderHandle->addLayer(pElement->first_attribute("material")->value());
		gLayer->setId(Ogre::StringConverter::parseInt(pElement->first_attribute("id")->value()));
		gLayer->setEnabled(Ogre::StringConverter::parseBool(pElement->first_attribute("enabled")->value()));
		gLayer->setMaxSlope(Ogre::StringConverter::parseReal(pElement->first_attribute("maxSlope")->value()));
		gLayer->setLightingEnabled(Ogre::StringConverter::parseBool(pElement->first_attribute("lighting")->value()));

		Ogre::LogManager::getSingleton().logMessage(
			"[DotSceneLoader] Processing grass layer \""
			+Ogre::StringConverter::toString(gLayer->getId()) + "\"" );

		// densityMapProps
		pSubElement = pElement->first_node("densityMapProps");
		tempStr = pSubElement->first_attribute("channel")->value();
		MapChannel mapCh;
		if(!tempStr.compare("ALPHA")) mapCh = CHANNEL_ALPHA; else
		if(!tempStr.compare("BLUE"))  mapCh = CHANNEL_BLUE;  else
		if(!tempStr.compare("COLOR")) mapCh = CHANNEL_COLOR; else
		if(!tempStr.compare("GREEN")) mapCh = CHANNEL_GREEN; else
		if(!tempStr.compare("RED"))   mapCh = CHANNEL_RED;
		
		gLayer->setDensityMap(dMapName, mapCh);
		gLayer->setDensity(Ogre::StringConverter::parseReal(pSubElement->first_attribute("density")->value()));

		// mapBounds
		pSubElement = pElement->first_node("mapBounds");
		gLayer->setMapBounds( TBounds( 
						Ogre::StringConverter::parseReal(pSubElement->first_attribute("left")->value()),  // left
						Ogre::StringConverter::parseReal(pSubElement->first_attribute("top")->value()),   // top
						Ogre::StringConverter::parseReal(pSubElement->first_attribute("right")->value()), // right
						Ogre::StringConverter::parseReal(pSubElement->first_attribute("bottom")->value()) // bottom
								)
							);

		// grassSizes
		pSubElement = pElement->first_node("grassSizes");
		gLayer->setMinimumSize( Ogre::StringConverter::parseReal(pSubElement->first_attribute("minWidth")->value()),   // width
								Ogre::StringConverter::parseReal(pSubElement->first_attribute("minHeight")->value()) );// height
		gLayer->setMaximumSize( Ogre::StringConverter::parseReal(pSubElement->first_attribute("maxWidth")->value()),   // width
								Ogre::StringConverter::parseReal(pSubElement->first_attribute("maxHeight")->value()) );// height

		// techniques
		pSubElement = pElement->first_node("techniques");
		tempStr = pSubElement->first_attribute("renderTechnique")->value();
		GrassTechnique rendTech;
		
		if(!tempStr.compare("QUAD"))	   rendTech = GRASSTECH_QUAD;	   else
		if(!tempStr.compare("CROSSQUADS")) rendTech = GRASSTECH_CROSSQUADS; else
		if(!tempStr.compare("SPRITE"))	 rendTech = GRASSTECH_SPRITE;
		gLayer->setRenderTechnique( rendTech,
									Ogre::StringConverter::parseBool(pSubElement->first_attribute("blend")->value()) );

		tempStr = pSubElement->first_attribute("fadeTechnique")->value();
		FadeTechnique fadeTech;
		if(!tempStr.compare("ALPHA")) fadeTech = FADETECH_ALPHA; else
		if(!tempStr.compare("GROW"))  fadeTech = FADETECH_GROW; else
		if(!tempStr.compare("ALPHAGROW")) fadeTech = FADETECH_ALPHAGROW;
		gLayer->setFadeTechnique(fadeTech);
		
		// animation
		pSubElement = pElement->first_node("animation");
		gLayer->setAnimationEnabled(Ogre::StringConverter::parseBool(pSubElement->first_attribute("animate")->value()));
		gLayer->setSwayLength(Ogre::StringConverter::parseReal(pSubElement->first_attribute("swayLength")->value()));
		gLayer->setSwaySpeed(Ogre::StringConverter::parseReal(pSubElement->first_attribute("swaySpeed")->value()));
		gLayer->setSwayDistribution(Ogre::StringConverter::parseReal(pSubElement->first_attribute("swayDistribution")->value()));

		// next layer
		pElement = pElement->next_sibling("grassLayer");
	}

	mSceneMgr->destroyCamera(tempCam);
}

void DotSceneLoader::processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	//! @todo Implement this
}

void DotSceneLoader::processOctree(rapidxml::xml_node<>* XMLNode)
{
	//! @todo Implement this
}

void DotSceneLoader::processLight(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::String name = getAttrib(XMLNode, "name");
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing light \"" + name + "\"");

	// Process attributes
	Ogre::String id = getAttrib(XMLNode, "id");

	// Create the light
	Ogre::Light *pLight = mSceneMgr->createLight(name);
	if(pParent)
		pParent->attachObject(pLight);

	Ogre::String sValue = getAttrib(XMLNode, "type");
	if(sValue == "point")
		pLight->setType(Ogre::Light::LT_POINT);
	else if(sValue == "directional")
		pLight->setType(Ogre::Light::LT_DIRECTIONAL);
	else if(sValue == "spot")
		pLight->setType(Ogre::Light::LT_SPOTLIGHT);
	else if(sValue == "radPoint")
		pLight->setType(Ogre::Light::LT_POINT);

	pLight->setVisible(getAttribBool(XMLNode, "visible", true));
	pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));

	rapidxml::xml_node<>* pElement;

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if(pElement)
		pLight->setPosition(parseVector3(pElement));

	// Process normal (?)
	pElement = XMLNode->first_node("normal");
	if(pElement)
		pLight->setDirection(parseVector3(pElement));

	pElement = XMLNode->first_node("directionVector");
	if(pElement)
	{
		pLight->setDirection(parseVector3(pElement));
		mLightDirection = parseVector3(pElement);
	}

	// Process colourDiffuse (?)
	pElement = XMLNode->first_node("colourDiffuse");
	if(pElement)
		pLight->setDiffuseColour(parseColour(pElement));

	// Process colourSpecular (?)
	pElement = XMLNode->first_node("colourSpecular");
	if(pElement)
		pLight->setSpecularColour(parseColour(pElement));

	if(sValue != "directional")
	{
		// Process lightRange (?)
		pElement = XMLNode->first_node("lightRange");
		if(pElement)
			processLightRange(pElement, pLight);

		// Process lightAttenuation (?)
		pElement = XMLNode->first_node("lightAttenuation");
		if(pElement)
			processLightAttenuation(pElement, pLight);
	}
	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if(pElement)
		;//processUserDataReference(pElement, pLight);
}

void DotSceneLoader::processCamera(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	return;

	Ogre::String name = getAttrib(XMLNode, "name");
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing camera \"" + name + "\"");

	// Process attributes
	Ogre::String id = getAttrib(XMLNode, "id");
	Ogre::Real fov = getAttribReal(XMLNode, "fov", 45);
	Ogre::Real aspectRatio = getAttribReal(XMLNode, "aspectRatio", 1.3333f);
	Ogre::String projectionType = getAttrib(XMLNode, "projectionType", "perspective");

	// Create the camera
	Ogre::Camera *pCamera = mSceneMgr->createCamera(name);

	if(pParent)
		pParent->attachObject(pCamera);

	// Set the field-of-view
	//! @todo Is this always in degrees?
	//pCamera->setFOVy(Ogre::Degree(fov));

	// Set the aspect ratio
	pCamera->setAutoAspectRatio(true);
	//pCamera->setAspectRatio(aspectRatio);
	
	// Set the projection type
	if(projectionType == "perspective")
		pCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
	else if(projectionType == "orthographic")
		pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);

	rapidxml::xml_node<>* pElement;

	// Process clipping (?)
	pElement = XMLNode->first_node("clipping");
	if(pElement)
	{
		Ogre::Real nearDist = getAttribReal(pElement, "near");
		pCamera->setNearClipDistance(nearDist);

		Ogre::Real farDist =  getAttribReal(pElement, "far");
		pCamera->setFarClipDistance(farDist);
	}

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if(pElement)
		pCamera->setPosition(parseVector3(pElement));

	// Process rotation (?)
	pElement = XMLNode->first_node("rotation");
	if(pElement)
		pCamera->setOrientation(parseQuaternion(pElement));

	// Process normal (?)
	pElement = XMLNode->first_node("normal");
	if(pElement)
		;//!< @todo What to do with this element?

	// Process lookTarget (?)
	pElement = XMLNode->first_node("lookTarget");
	if(pElement)
		;//!< @todo Implement the camera look target

	// Process trackTarget (?)
	pElement = XMLNode->first_node("trackTarget");
	if(pElement)
		;//!< @todo Implement the camera track target

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if(pElement)
		;//!< @todo Implement the camera user data reference
/*
	// construct a scenenode is no parent
	if(!pParent)
	{
		Ogre::SceneNode* pNode = mAttachNode->createChildSceneNode(name);
		pNode->setPosition(pCamera->getPosition());
		pNode->setOrientation(pCamera->getOrientation());
		pNode->scale(1,1,1);
	}
*/
}

void DotSceneLoader::processNode(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	// Construct the node's name
	Ogre::String name = m_sPrependNode + getAttrib(XMLNode, "name");

	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing node \"" + name + "\"");

	// Create the scene node
	Ogre::SceneNode *pNode;
	if(name.empty())
	{
		// Let Ogre choose the name
		if(pParent)
			pNode = pParent->createChildSceneNode();
		else
			pNode = mAttachNode->createChildSceneNode();
	}
	else
	{
		// Provide the name
		if(pParent)
			pNode = pParent->createChildSceneNode(name);
		else
			pNode = mAttachNode->createChildSceneNode(name);
	}

	// Process other attributes
	Ogre::String id = getAttrib(XMLNode, "id");
	bool isTarget = getAttribBool(XMLNode, "isTarget");

	rapidxml::xml_node<>* pElement;

	// Process position (?)
	pElement = XMLNode->first_node("position");
	if(pElement)
	{
		pNode->setPosition(parseVector3(pElement));
		pNode->setInitialState();
	}

	// Process rotation (?)
	pElement = XMLNode->first_node("rotation");
	if(pElement)
	{
		pNode->setOrientation(parseQuaternion(pElement));
		pNode->setInitialState();
	}

	// Process scale (?)
	pElement = XMLNode->first_node("scale");
	if(pElement)
	{
		pNode->setScale(parseVector3(pElement));
		pNode->setInitialState();
	}

	// Process lookTarget (?)
	pElement = XMLNode->first_node("lookTarget");
	if(pElement)
		processLookTarget(pElement, pNode);

	// Process trackTarget (?)
	pElement = XMLNode->first_node("trackTarget");
	if(pElement)
		processTrackTarget(pElement, pNode);

	// Process node (*)
	pElement = XMLNode->first_node("node");
	while(pElement)
	{
		processNode(pElement, pNode);
		pElement = pElement->next_sibling("node");
	}

	// Process entity (*)
	pElement = XMLNode->first_node("entity");
	while(pElement)
	{
		processEntity(pElement, pNode);
		pElement = pElement->next_sibling("entity");
	}

	// Process light (*)
	pElement = XMLNode->first_node("light");
	while(pElement)
	{
		processLight(pElement, pNode);
		pElement = pElement->next_sibling("light");
	}

	// Process camera (*)
	pElement = XMLNode->first_node("camera");
	while(pElement)
	{
		processCamera(pElement, pNode);
		pElement = pElement->next_sibling("camera");
	}

	// Process particleSystem (*)
	pElement = XMLNode->first_node("particleSystem");
	while(pElement)
	{
		processParticleSystem(pElement, pNode);
		pElement = pElement->next_sibling("particleSystem");
	}

	// Process billboardSet (*)
	pElement = XMLNode->first_node("billboardSet");
	while(pElement)
	{
		processBillboardSet(pElement, pNode);
		pElement = pElement->next_sibling("billboardSet");
	}

	// Process plane (*)
	pElement = XMLNode->first_node("plane");
	while(pElement)
	{
		processPlane(pElement, pNode);
		pElement = pElement->next_sibling("plane");
	}

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if(pElement)
		processUserDataReference(pElement, pNode);

	// Process entity (*)
	pElement = XMLNode->first_node("pagedgeometry");
	while(pElement)
	{
		processPagedGeometry(pElement, pNode);
		pElement = pElement->next_sibling("pagedgeometry");
	}
}

void DotSceneLoader::processLookTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing look target");

	//! @todo Is this correct? Cause I don't have a clue actually

	// Process attributes
	Ogre::String nodeName = getAttrib(XMLNode, "nodeName");

	Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
	Ogre::String sValue = getAttrib(XMLNode, "relativeTo");
	if(sValue == "local")
		relativeTo = Ogre::Node::TS_LOCAL;
	else if(sValue == "parent")
		relativeTo = Ogre::Node::TS_PARENT;
	else if(sValue == "world")
		relativeTo = Ogre::Node::TS_WORLD;

	rapidxml::xml_node<>* pElement;

	// Process position (?)
	Ogre::Vector3 position;
	pElement = XMLNode->first_node("position");
	if(pElement)
		position = parseVector3(pElement);

	// Process localDirection (?)
	Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->first_node("localDirection");
	if(pElement)
		localDirection = parseVector3(pElement);

	// Setup the look target
	try
	{
		if(!nodeName.empty())
		{
			Ogre::SceneNode *pLookNode = mSceneMgr->getSceneNode(nodeName);
			position = pLookNode->_getDerivedPosition();
		}

		pParent->lookAt(position, relativeTo, localDirection);
	}
	catch(Ogre::Exception &/*e*/)
	{
		Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
	}
}

void DotSceneLoader::processTrackTarget(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing track target");

	// Process attributes
	Ogre::String nodeName = getAttrib(XMLNode, "nodeName");

	rapidxml::xml_node<>* pElement;

	// Process localDirection (?)
	Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;
	pElement = XMLNode->first_node("localDirection");
	if(pElement)
		localDirection = parseVector3(pElement);

	// Process offset (?)
	Ogre::Vector3 offset = Ogre::Vector3::ZERO;
	pElement = XMLNode->first_node("offset");
	if(pElement)
		offset = parseVector3(pElement);

	// Setup the track target
	try
	{
		Ogre::SceneNode *pTrackNode = mSceneMgr->getSceneNode(nodeName);
		pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
	}
	catch(Ogre::Exception &/*e*/)
	{
		Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
	}
}

void DotSceneLoader::processEntity(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::String name = getAttrib(XMLNode, "name");
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing entity \"" + name + "\"");

	// Process attributes
	Ogre::String id = getAttrib(XMLNode, "id");
	Ogre::String meshFile = getAttrib(XMLNode, "meshFile");
	Ogre::String materialFile = getAttrib(XMLNode, "materialFile");
	bool isStatic = getAttribBool(XMLNode, "static", false);;
	bool castShadows = getAttribBool(XMLNode, "castShadows", true);

	// TEMP: Maintain a list of static and dynamic objects
	if(isStatic)
		staticObjects.push_back(name);
	else
		dynamicObjects.push_back(name);

	rapidxml::xml_node<>* pElement;

	// Process vertexBuffer (?)
	pElement = XMLNode->first_node("vertexBuffer");
	if(pElement)
		;//processVertexBuffer(pElement);

	// Process indexBuffer (?)
	pElement = XMLNode->first_node("indexBuffer");
	if(pElement)
		;//processIndexBuffer(pElement);

	// Create the entity
	Ogre::Entity *pEntity = 0;
	try
	{
		Ogre::MeshManager::getSingleton().load(meshFile, m_sGroupName);
		pEntity = mSceneMgr->createEntity(name, meshFile);
		pEntity->setCastShadows(castShadows);
		pParent->attachObject(pEntity);

		if(!materialFile.empty())
			pEntity->setMaterialName(materialFile);
		
		// Process subentity (*)
		/* if materials defined within subentities, those
		   materials will be used instead of the materialFile */
		pElement = XMLNode->first_node("subentities");

		if(pElement)
		{
			processSubEntity(pElement, pEntity);
		}else{
			// if the .scene file contains the subentites without
			// the <subentities> </subentities>
			processSubEntity(XMLNode, pEntity);
		}
	}
	catch(Ogre::Exception &/*e*/)
	{
		Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
	}

	// Process userDataReference (?)
	pElement = XMLNode->first_node("userDataReference");
	if(pElement)
		processUserDataReference(pElement, pEntity);

	
}

void DotSceneLoader::processSubEntity(rapidxml::xml_node<>* XMLNode, Ogre::Entity *pEntity)
{
	rapidxml::xml_node<>* pElement;
	int index = 0;
	Ogre::String materialName;
	Ogre::String sIndex;

	// Process subentity
	pElement = XMLNode->first_node("subentity");
	
	while(pElement){
		
		sIndex.clear();
		materialName.clear();

		sIndex = getAttrib(pElement, "index");  // submesh index
		materialName = getAttrib(pElement, "materialName"); // new material for submesh
		
		if(!sIndex.empty() && !materialName.empty()){
			
			index = Ogre::StringConverter::parseInt(sIndex);
			try{
				pEntity->getSubEntity(index)->setMaterialName(materialName);
			} catch (...){
				Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Subentity material index is invalid!");
			}
		}
		pElement = pElement->next_sibling("subentity");
	}
}

void DotSceneLoader::processParticleSystem(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::String name = getAttrib(XMLNode, "name");
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing particle system \"" + name + "\"");

	// Process attributes
	Ogre::String id = getAttrib(XMLNode, "id");
	Ogre::String file = getAttrib(XMLNode, "file");

	// Create the particle system
	try
	{
		Ogre::ParticleSystem *pParticles = mSceneMgr->createParticleSystem(name, file);
		pParent->attachObject(pParticles);
		// there is a bug with particles and paged geometry if particle's
		// renderQueue is value is smaller than the grass's renderQueue
		if(mGrassLoaderHandle)
			pParticles->setRenderQueueGroup(mGrassLoaderHandle->getRenderQueueGroup());
	}
	catch(Ogre::Exception &/*e*/)
	{
		Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
	}
}

void DotSceneLoader::processBillboardSet(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	//! @todo Implement this
}

void DotSceneLoader::processPlane(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::String name = getAttrib(XMLNode, "name");
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing plane \"" + name + "\"");

	Ogre::Real distance = getAttribReal(XMLNode, "distance");
	Ogre::Real width = getAttribReal(XMLNode, "width");
	Ogre::Real height = getAttribReal(XMLNode, "height");
	int xSegments = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "xSegments"));
	int ySegments = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "ySegments"));
	int numTexCoordSets = Ogre::StringConverter::parseInt(getAttrib(XMLNode, "numTexCoordSets"));
	Ogre::Real uTile = getAttribReal(XMLNode, "uTile");
	Ogre::Real vTile = getAttribReal(XMLNode, "vTile");
	Ogre::String material = getAttrib(XMLNode, "material");
	bool hasNormals = getAttribBool(XMLNode, "hasNormals");
	Ogre::Vector3 normal = parseVector3(XMLNode->first_node("normal"));
	Ogre::Vector3 up = parseVector3(XMLNode->first_node("upVector"));

	Ogre::Plane plane(normal, distance);
	Ogre::MeshPtr res = Ogre::MeshManager::getSingletonPtr()->createPlane(
						name + "mesh", m_sGroupName, plane, width, height, xSegments, ySegments, hasNormals,
	numTexCoordSets, uTile, vTile, up);
	Ogre::Entity* ent = mSceneMgr->createEntity(name, name + "mesh");

	ent->setMaterialName(material);

	pParent->attachObject(ent);
}

struct PGInstanceInfo
{
	Ogre::Vector3 pos;
	Ogre::Real	scale;
	Ogre::Real	yaw;
};

typedef std::vector<PGInstanceInfo> PGInstanceList;

void DotSceneLoader::processPagedGeometry(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing paged geometry");

	Ogre::String filename = mResourcesDir + "/maps/" + getAttrib(XMLNode, "fileName");
	Ogre::String model = getAttrib(XMLNode, "model");
	Ogre::Real pagesize = getAttribReal(XMLNode, "pageSize");
	Ogre::Real batchdistance = getAttribReal(XMLNode, "batchDistance");
	Ogre::Real impostordistance = getAttribReal(XMLNode, "impostorDistance");
	Ogre::Vector4 bounds = Ogre::StringConverter::parseVector4(getAttrib(XMLNode, "bounds"));
	PagedGeometry *mPGHandle = new PagedGeometry();
	mPGHandle->setCamera(mSceneMgr->getCameraIterator().begin()->second);
	mPGHandle->setPageSize(pagesize);
	mPGHandle->setInfinite();

	mPGHandle->addDetailLevel<BatchPage>(batchdistance,0);
	mPGHandle->addDetailLevel<ImpostorPage>(impostordistance,0);

	TreeLoader3D *mHandle = new TreeLoader3D(mPGHandle, Forests::TBounds(bounds.x, bounds.y, bounds.z, bounds.w));
	mPGHandle->setPageLoader(mHandle);

	mPGHandles.push_back(mPGHandle);
	mTreeHandles.push_back(mHandle);

	std::ifstream stream(filename.c_str());

	if(!stream.is_open())
		return;

	Ogre::StringVector list;

	char res[128];

	PGInstanceList mInstanceList;

	while(!stream.eof())
	{
		stream.getline(res, 128);
		Ogre::String resStr(res);

		ParseStringVector(resStr, list);

		if(list.size() == 3)
		{
			PGInstanceInfo info;
			
			info.pos = Ogre::StringConverter::parseVector3(list[0]);
			info.scale = Ogre::StringConverter::parseReal(list[1]);
			info.yaw = Ogre::StringConverter::parseReal(list[2]);

			mInstanceList.push_back(info);
		}
		else if(list.size() == 4)
		{
			PGInstanceInfo info;
			
			info.pos = Ogre::StringConverter::parseVector3(list[1]);
			info.scale = Ogre::StringConverter::parseReal(list[2]);
			info.yaw = Ogre::StringConverter::parseReal(list[3]);

			mInstanceList.push_back(info);
		}
	}

	stream.close();

	if(model != "")
	{
		Ogre::Entity *mEntityHandle = mSceneMgr->createEntity(model + ".mesh");

		PGInstanceList::iterator it = mInstanceList.begin();

		while(it != mInstanceList.end())
		{
			mHandle->addTree(mEntityHandle, it->pos, Ogre::Degree(it->yaw), it->scale);

			it++;
		}
	}
}

void DotSceneLoader::processFog(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Processing fog");

	// Process attributes
	Ogre::Real expDensity = getAttribReal(XMLNode, "density", 0.001f);
	Ogre::Real linearStart = getAttribReal(XMLNode, "start", 0.0);
	Ogre::Real linearEnd = getAttribReal(XMLNode, "end", 1.0);

	Ogre::FogMode mode = Ogre::FOG_NONE;
	Ogre::String sMode = getAttrib(XMLNode, "mode");
	if(sMode == "none")
		mode = Ogre::FOG_NONE;
	else if(sMode == "exp")
		mode = Ogre::FOG_EXP;
	else if(sMode == "exp2")
		mode = Ogre::FOG_EXP2;
	else if(sMode == "linear")
		mode = Ogre::FOG_LINEAR;
	else
		mode = (Ogre::FogMode)Ogre::StringConverter::parseInt(sMode);

	rapidxml::xml_node<>* pElement;

	// Process colourDiffuse (?)
	Ogre::ColourValue colourDiffuse = Ogre::ColourValue::White;
	pElement = XMLNode->first_node("colour"); // BUG : "colourDiffuse" in OgreMax
	if(pElement)
		colourDiffuse = parseColour(pElement);

	// Setup the fog
	mSceneMgr->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void DotSceneLoader::processSkyBox(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing skybox");

	// Process attributes
	Ogre::String material = getAttrib(XMLNode, "material", "BaseWhite");
	Ogre::Real distance = getAttribReal(XMLNode, "distance", 5000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
	bool active = getAttribBool(XMLNode, "active", false);
	if(!active)
		return;

	rapidxml::xml_node<>* pElement;

	// Process rotation (?)
	Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
	pElement = XMLNode->first_node("rotation");
	if(pElement)
		rotation = parseQuaternion(pElement);

	// Setup the sky box
	mSceneMgr->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
}

void DotSceneLoader::processSkyDome(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing skydome");

	// Process attributes
	Ogre::String material = XMLNode->first_attribute("material")->value();
	Ogre::Real curvature = getAttribReal(XMLNode, "curvature", 10);
	Ogre::Real tiling = getAttribReal(XMLNode, "tiling", 8);
	Ogre::Real distance = getAttribReal(XMLNode, "distance", 4000);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);
	bool active = getAttribBool(XMLNode, "active", false);
	if(!active)
		return;

	rapidxml::xml_node<>* pElement;

	// Process rotation (?)
	Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;
	pElement = XMLNode->first_node("rotation");
	if(pElement)
		rotation = parseQuaternion(pElement);

	// Setup the sky dome
	mSceneMgr->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
}

void DotSceneLoader::processSkyPlane(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Processing skyplane");

	// Process attributes
	Ogre::String material = getAttrib(XMLNode, "material");
	Ogre::Real planeX = getAttribReal(XMLNode, "planeX", 0);
	Ogre::Real planeY = getAttribReal(XMLNode, "planeY", -1);
	Ogre::Real planeZ = getAttribReal(XMLNode, "planeX", 0);
	Ogre::Real planeD = getAttribReal(XMLNode, "planeD", 5000);
	Ogre::Real scale = getAttribReal(XMLNode, "scale", 1000);
	Ogre::Real bow = getAttribReal(XMLNode, "bow", 0);
	Ogre::Real tiling = getAttribReal(XMLNode, "tiling", 10);
	bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

	// Setup the sky plane
	Ogre::Plane plane;
	plane.normal = Ogre::Vector3(planeX, planeY, planeZ);
	plane.d = planeD;
	mSceneMgr->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
}

void DotSceneLoader::processClipping(rapidxml::xml_node<>* XMLNode)
{
	//! @todo Implement this

	// Process attributes
	Ogre::Real fNear = getAttribReal(XMLNode, "near", 0);
	Ogre::Real fFar = getAttribReal(XMLNode, "far", 1);
}

void DotSceneLoader::processLightRange(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing light range for light \""
		+ pLight->getName() + "\"");

	// Process attributes
	Ogre::Real inner = getAttribReal(XMLNode, "inner");
	Ogre::Real outer = getAttribReal(XMLNode, "outer");
	Ogre::Real falloff = getAttribReal(XMLNode, "falloff", 1.0);

	// Setup the light range
	pLight->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
}

void DotSceneLoader::processLightAttenuation(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight)
{
	Ogre::LogManager::getSingleton().logMessage(
		"[DotSceneLoader] Processing light attenuation for light \""
		+ pLight->getName() + "\"");

	// Process attributes
	Ogre::Real range = getAttribReal(XMLNode, "range");
	Ogre::Real constant = getAttribReal(XMLNode, "constant");
	Ogre::Real linear = getAttribReal(XMLNode, "linear");
	Ogre::Real quadratic = getAttribReal(XMLNode, "quadratic");

	// Setup the light attenuation
	pLight->setAttenuation(range, constant, linear, quadratic);
}

void DotSceneLoader::processCaelum(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage( "[DotSceneLoader] Processing Caelum" );

	int componentMask = Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
		Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS;

	rapidxml::xml_node<>* pSun = XMLNode->first_node("sun");
	if(pSun && getAttribBool(pSun, "enable"))
		componentMask |= Caelum::CaelumSystem::CAELUM_COMPONENT_SUN;
	rapidxml::xml_node<>* pMoon = XMLNode->first_node("moon");
	if(pMoon && getAttribBool(pMoon, "enable"))
		componentMask |= Caelum::CaelumSystem::CAELUM_COMPONENT_MOON;
	rapidxml::xml_node<>* pStars = XMLNode->first_node("stars");
	if(pStars && getAttribBool(pStars, "enable"))
		componentMask |= Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD;

	mCaelum = new Caelum::CaelumSystem(Ogre::Root::getSingletonPtr(), mSceneMgr,
		static_cast<Caelum::CaelumSystem::CaelumComponent>(componentMask));

	rapidxml::xml_attribute<>* attr;

	if((componentMask & Caelum::CaelumSystem::CAELUM_COMPONENT_SUN) == Caelum::CaelumSystem::CAELUM_COMPONENT_SUN)
	{
		Caelum::BaseSkyLight *sun = mCaelum->getSun();

		sun->setAutoDisable(getAttribBool(pSun, "autoDisable", sun->getAutoDisable()));

		attr = pSun->first_attribute("ambientMultiplier");
		if(attr)
			sun->setAmbientMultiplier(Ogre::StringConverter::parseColourValue(attr->value(), sun->getAmbientMultiplier()));
		attr = pSun->first_attribute("diffuseMultiplier");
		if(attr)
			sun->setDiffuseMultiplier(Ogre::StringConverter::parseColourValue(attr->value(), sun->getDiffuseMultiplier()));
		attr = pSun->first_attribute("specularMultiplier");
		if(attr)
			sun->setSpecularMultiplier(Ogre::StringConverter::parseColourValue(attr->value(), sun->getSpecularMultiplier()));

		sun->getMainLight()->setCastShadows(getAttribBool(pSun, "castShadow", sun->getMainLight()->getCastShadows()));

		attr = pSun->first_attribute("position");
		if(attr)
			sun->getSceneNode()->setPosition(Ogre::StringConverter::parseVector3(attr->value(), sun->getSceneNode()->getPosition()));

		attr = pSun->first_attribute("colour");
		if(attr)
			sun->setBodyColour(Ogre::StringConverter::parseColourValue(attr->value(), sun->getBodyColour()));

		// TODO : how do I set sunlight colour?

		sun->getMainLight()->setAttenuation(
			getAttribReal(pSun, "distance", sun->getMainLight()->getAttenuationRange()),
			getAttribReal(pSun, "constantMultiplier", sun->getMainLight()->getAttenuationConstant()),
			getAttribReal(pSun, "linearMultiplier", sun->getMainLight()->getAttenuationLinear()),
			getAttribReal(pSun, "quadricMultiplier", sun->getMainLight()->getAttenuationQuadric())
		);
	}

	if((componentMask & Caelum::CaelumSystem::CAELUM_COMPONENT_MOON) == Caelum::CaelumSystem::CAELUM_COMPONENT_MOON)
	{
		Caelum::Moon *moon = mCaelum->getMoon();

		moon->setAutoDisable(getAttribBool(pMoon, "autoDisable", moon->getAutoDisable()));

		attr = pMoon->first_attribute("ambientMultiplier");
		if(attr)
			moon->setAmbientMultiplier(Ogre::StringConverter::parseColourValue(attr->value(), moon->getAmbientMultiplier()));
		attr = pMoon->first_attribute("diffuseMultiplier");
		if(attr)
			moon->setDiffuseMultiplier(Ogre::StringConverter::parseColourValue(attr->value(), moon->getDiffuseMultiplier()));
		attr = pMoon->first_attribute("specularMultiplier");
		if(attr)
			moon->setSpecularMultiplier(Ogre::StringConverter::parseColourValue(attr->value(), moon->getSpecularMultiplier()));

		moon->getMainLight()->setCastShadows(getAttribBool(pMoon, "castShadow", moon->getMainLight()->getCastShadows()));

		attr = pMoon->first_attribute("position");
		if(attr)
			moon->getSceneNode()->setPosition(Ogre::StringConverter::parseVector3(attr->value(), moon->getSceneNode()->getPosition()));

		attr = pMoon->first_attribute("colour");
		if(attr)
			moon->setBodyColour(Ogre::StringConverter::parseColourValue(attr->value(), moon->getBodyColour()));

		// TODO : how do I set moonlight colour?

		moon->getMainLight()->setAttenuation(
			getAttribReal(pMoon, "distance", moon->getMainLight()->getAttenuationRange()),
			getAttribReal(pMoon, "constantMultiplier", moon->getMainLight()->getAttenuationConstant()),
			getAttribReal(pMoon, "linearMultiplier", moon->getMainLight()->getAttenuationLinear()),
			getAttribReal(pMoon, "quadricMultiplier", moon->getMainLight()->getAttenuationQuadric())
		);
	}

	rapidxml::xml_node<>* pClock = XMLNode->first_node("clock");
	if(pClock)
	{
		Caelum::UniversalClock *clock = mCaelum->getUniversalClock();
		int year = 2000, month = 1, day = 1, hour = 12, minute = 0;
		double second = getAttribReal(pClock, "second");
		attr = pClock->first_attribute("year");
		if(attr)
			year = Ogre::StringConverter::parseInt(attr->value(), 2000);
		attr = pClock->first_attribute("month");
		if(attr)
			month = Ogre::StringConverter::parseInt(attr->value(), 1);
		attr = pClock->first_attribute("day");
		if(attr)
			day = Ogre::StringConverter::parseInt(attr->value(), 1);
		attr = pClock->first_attribute("hour");
		if(attr)
			hour = Ogre::StringConverter::parseInt(attr->value(), 12);
		attr = pClock->first_attribute("minute");
		if(attr)
			minute = Ogre::StringConverter::parseInt(attr->value());
		clock->setGregorianDateTime(year, month, day, hour, minute, second);
		clock->setTimeScale(getAttribReal(pClock, "speed", mCaelum->getUniversalClock()->getTimeScale()));
	}

	rapidxml::xml_node<>* pObserver = XMLNode->first_node("observer");
	if(pObserver)
	{
		mCaelum->setObserverLongitude(Ogre::Degree(getAttribReal(pObserver, "longitude", mCaelum->getObserverLongitude().valueDegrees())));
		mCaelum->setObserverLatitude (Ogre::Degree(getAttribReal(pObserver, "latitude",  mCaelum->getObserverLatitude().valueDegrees())));
	}

	rapidxml::xml_node<>* pLighting = XMLNode->first_node("lighting");
	if(pLighting)
	{
		mCaelum->setEnsureSingleLightSource(getAttribBool(pLighting, "singleLightsource", mCaelum->getEnsureSingleLightSource()));
		mCaelum->setEnsureSingleShadowSource(getAttribBool(pLighting, "singleShadowsource", mCaelum->getEnsureSingleShadowSource()));
		mCaelum->setManageAmbientLight(getAttribBool(pLighting, "manageAmbientLight", mCaelum->getManageAmbientLight()));

		attr = pLighting->first_attribute("minimumAmbientLight");
		if(attr)
			mCaelum->setMinimumAmbientLight(Ogre::StringConverter::parseColourValue(attr->value(), mCaelum->getMinimumAmbientLight()));
	}

	rapidxml::xml_node<>* pFog = XMLNode->first_node("fog");
	if(pFog)
	{
		mCaelum->setManageSceneFog(getAttribBool(pFog, "manage", mCaelum->getManageSceneFog() != Ogre::FOG_NONE) ? Ogre::FOG_EXP2 : Ogre::FOG_NONE);
		mCaelum->setGlobalFogDensityMultiplier(getAttribReal(pFog, "densityMultiplier", mCaelum->getGlobalFogDensityMultiplier()));
	}

	if((componentMask & Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD) == Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD)
	{
		Caelum::PointStarfield *stars = mCaelum->getPointStarfield();
		stars->setMagnitudeScale(getAttribReal(pStars, "magnitudeScale", stars->getMagnitudeScale()));
		stars->setMag0PixelSize(getAttribReal(pStars, "mag0PixelSize", stars->getMag0PixelSize()));
		stars->setMinPixelSize(getAttribReal(pStars, "minPixelSize", stars->getMinPixelSize()));
		stars->setMaxPixelSize(getAttribReal(pStars, "maxPixelSize", stars->getMaxPixelSize()));
	}

	Caelum::CloudSystem *clouds = mCaelum->getCloudSystem();
	if(!clouds)
	{
		clouds = new Caelum::CloudSystem(mSceneMgr, mCaelum->getCaelumCameraNode());
		mCaelum->setCloudSystem(clouds);
	}
	clouds->clearLayers();
	clouds->getLayerVector().clear();
	rapidxml::xml_node<>* pClouds = XMLNode->first_node("clouds");
	if(pClouds)
	{
		rapidxml::xml_node<>* pCloudLayer = pClouds->first_node("layer");
		while(pCloudLayer)
		{
			Caelum::FlatCloudLayer *layer = clouds->createLayer();
			layer->setVisibilityFlags(getAttribBool(pCloudLayer, "enable") == true ? 0xFFFFFFFF : 0);
			layer->setCloudCover(getAttribReal(pCloudLayer, "coverage", layer->getCloudCover()));
			layer->setHeight(getAttribReal(pCloudLayer, "height", layer->getHeight()));
			attr = pCloudLayer->first_attribute("speed");
			if(attr)
				layer->setCloudSpeed(Ogre::StringConverter::parseVector2(attr->value(), layer->getCloudSpeed()));

			pCloudLayer = pCloudLayer->next_sibling("layer");
		}
	}
}

void DotSceneLoader::processHydrax(rapidxml::xml_node<>* XMLNode)
{
	Ogre::LogManager::getSingleton().logMessage( "[DotSceneLoader] Processing Hydrax" );

	mHydrax = new Hydrax::Hydrax(mSceneMgr, mViewPort->getCamera(), mViewPort);
	Hydrax::Module::ProjectedGrid *mModule =
		new Hydrax::Module::ProjectedGrid(
			// Hydrax parent pointer
			mHydrax,
			// Noise module
			new Hydrax::Noise::Perlin(/* Generic one */),
			// Base plane
			Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
			// Normal mode
			Hydrax::MaterialManager::NM_VERTEX,
			// Projected grid options
			Hydrax::Module::ProjectedGrid::Options(64));
	mHydrax->setModule(mModule);

	mHydraxCaelumIntegration = getAttribBool(XMLNode, "caelumIntegration");
	Ogre::String configFile = getAttrib(XMLNode, "configFile");
	if(!configFile.empty())
		mHydrax->loadCfg(configFile);

	mOriginalWaterColor = mHydrax->getWaterColor();

	mHydrax->create();

	if(mTerrainGroup)
	{
		Ogre::TerrainGroup::TerrainIterator it =
			mTerrainGroup->getTerrainIterator();
		while(it.hasMoreElements())
			mHydrax->getMaterialManager()->addDepthTechnique(
				it.getNext()->instance->getMaterial()->createTechnique());
	}
}

Ogre::String DotSceneLoader::getAttrib(rapidxml::xml_node<>* XMLNode, const Ogre::String &attrib, const Ogre::String &defaultValue)
{
	if(XMLNode->first_attribute(attrib.c_str()))
		return XMLNode->first_attribute(attrib.c_str())->value();
	else
		return defaultValue;
}

Ogre::Real DotSceneLoader::getAttribReal(rapidxml::xml_node<>* XMLNode, const Ogre::String &attrib, Ogre::Real defaultValue)
{
	if(XMLNode->first_attribute(attrib.c_str()))
		return Ogre::StringConverter::parseReal(XMLNode->first_attribute(attrib.c_str())->value());
	else
		return defaultValue;
}

bool DotSceneLoader::getAttribBool(rapidxml::xml_node<>* XMLNode, const Ogre::String &attrib, bool defaultValue)
{
	if(!XMLNode->first_attribute(attrib.c_str()))
		return defaultValue;

	if(Ogre::String(XMLNode->first_attribute(attrib.c_str())->value()) == "true")
		return true;

	return false;
}

Ogre::Vector3 DotSceneLoader::parseVector3(rapidxml::xml_node<>* XMLNode)
{
	return Ogre::Vector3(
		Ogre::StringConverter::parseReal(XMLNode->first_attribute("x")->value()),
		Ogre::StringConverter::parseReal(XMLNode->first_attribute("y")->value()),
		Ogre::StringConverter::parseReal(XMLNode->first_attribute("z")->value())
	);
}

Ogre::Quaternion DotSceneLoader::parseQuaternion(rapidxml::xml_node<>* XMLNode)
{
	//! @todo Fix this crap!

	Ogre::Quaternion orientation;

	if(XMLNode->first_attribute("qx"))
	{
		orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
		orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
		orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
		orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
	}
	if(XMLNode->first_attribute("qw"))
	{
		orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
		orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
		orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
		orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
	}
	else if(XMLNode->first_attribute("axisX"))
	{
		Ogre::Vector3 axis;
		axis.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("axisX")->value());
		axis.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("axisY")->value());
		axis.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("axisZ")->value());
		Ogre::Real angle = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angle")->value());;
		orientation.FromAngleAxis(Ogre::Angle(angle), axis);
	}
	else if(XMLNode->first_attribute("angleX"))
	{
		Ogre::Vector3 axis;
		axis.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angleX")->value());
		axis.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angleY")->value());
		axis.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("angleZ")->value());
		//orientation.FromAxes(&axis);
		//orientation.F
	}
	else if(XMLNode->first_attribute("x"))
	{
		orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("x")->value());
		orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("y")->value());
		orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("z")->value());
		orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("w")->value());
	}
	else if(XMLNode->first_attribute("w"))
	{
		orientation.w = Ogre::StringConverter::parseReal(XMLNode->first_attribute("w")->value());
		orientation.x = Ogre::StringConverter::parseReal(XMLNode->first_attribute("x")->value());
		orientation.y = Ogre::StringConverter::parseReal(XMLNode->first_attribute("y")->value());
		orientation.z = Ogre::StringConverter::parseReal(XMLNode->first_attribute("z")->value());
	}

	return orientation;
}

Ogre::ColourValue DotSceneLoader::parseColour(rapidxml::xml_node<>* XMLNode)
{
	return Ogre::ColourValue(
		Ogre::StringConverter::parseReal(XMLNode->first_attribute("r")->value()),
		Ogre::StringConverter::parseReal(XMLNode->first_attribute("g")->value()),
		Ogre::StringConverter::parseReal(XMLNode->first_attribute("b")->value()),
		XMLNode->first_attribute("a") != NULL ? Ogre::StringConverter::parseReal(XMLNode->first_attribute("a")->value()) : 1
	);
}

Ogre::String DotSceneLoader::getProperty(const Ogre::String &ndNm, const Ogre::String &prop)
{
	for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ )
	{
		if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop )
		{
			return nodeProperties[i].valueName;
		}
	}

	return "";
}

void DotSceneLoader::processUserDataReference(rapidxml::xml_node<>* XMLNode, Ogre::Entity *pEntity)
{
	Ogre::String str = XMLNode->first_attribute("id")->value();
	pEntity->setUserAny(Ogre::Any(str));
}
