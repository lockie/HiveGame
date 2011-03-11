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

#include "HiveGame.hpp"

using namespace std;
using namespace Ogre;


HiveGame::HiveGame()
{
}

HiveGame::~HiveGame()
{
}

void HiveGame::createScene()
{
	// Свет и тень
	mSceneMgr->setAmbientLight(ColourValue(0, 0, 0));
	Light* directionalLight = mSceneMgr->createLight("sunLight");
	directionalLight->setType(Light::LT_DIRECTIONAL);
	directionalLight->setDiffuseColour(ColourValue(.8f, .58f, .26f));
	directionalLight->setSpecularColour(ColourValue(.21f, .06f, .22f));
	directionalLight->setDirection(Vector3(0, -10, -10));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

	// Модель
	Entity* entNinja = mSceneMgr->createEntity("Ninja", "ninja.mesh");
	entNinja->setCastShadows(true);
	SceneNode* nodeNinja =
		mSceneMgr->getRootSceneNode()->createChildSceneNode();
	nodeNinja->attachObject(entNinja);
	nodeNinja->setScale(0.06f, 0.06f, 0.06f);
	nodeNinja->setPosition( -20 * Vector3::UNIT_Z +
		5 * Vector3::UNIT_X);
	nodeNinja->rotate(Vector3::UNIT_Y, Degree(180));

	// Пол
	MeshManager::getSingleton().createPlane( "floor",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, 0), 100, 100, 10, 10, true, 1, 10, 10,
		Vector3::UNIT_Z );
	Entity* entGround = mSceneMgr->createEntity("FloorEntity", "floor");
	entGround->setMaterialName("Ground/Rockwall");
	entGround->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->
		attachObject(entGround);

	// Небо
	mSceneMgr->setSkyBox(true, "SkyBox/EarlyMorningSkyBox");
	
	// Океан
	Plane oceanSurface(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane( "OceanSurface",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		oceanSurface, 10000, 10000, 50, 50, true, 1, 1, 1,
		Vector3::UNIT_Z );
	Entity* oceanEnt = mSceneMgr->createEntity("OceanSurface", "OceanSurface");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, -5, 0))->
		attachObject(oceanEnt);
	oceanEnt->setMaterialName("Ocean2_HLSL_GLSL");

	// Пострендер-эффект
	//CompositorManager::getSingleton().addCompositor(mViewport, "HDR");
	//CompositorManager::getSingleton().setCompositorEnabled(mViewport,
	//	"HDR", true);
	//CompositorManager::getSingleton().addCompositor(mViewport, "Bloom");
	//CompositorManager::getSingleton().setCompositorEnabled(mViewport,
	//	"Bloom", true);

}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 static const int wbuffer_size = 4096;
 static wchar_t wbuffer[wbuffer_size];

 INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
 int main(int argc, char *argv[])
#endif
	{
		HiveGame app;
		try
		{
			app.run();
		} catch(Exception& e)
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			if(MultiByteToWideChar(CP_UTF8, 0, e.getFullDescription().c_str(),
			 -1, wbuffer, wbuffer_size) != 0)
				MessageBoxW(NULL, wbuffer,
					L"ERROR", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			else
				MessageBoxW(NULL, L"An error occured.\nSee BeeEngine.log",
					L"ERROR", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			cerr << e.getFullDescription().c_str() << endl;
#endif
		}
		return 0;
}

