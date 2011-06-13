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


	World::getSingletonPtr()->Load("test.scene");

	// Пострендер-эффект
	//CompositorManager::getSingleton().addCompositor(mViewport, "HDR");
	//CompositorManager::getSingleton().setCompositorEnabled(mViewport,
	//	"HDR", true);
	//CompositorManager::getSingleton().addCompositor(mViewport, "Bloom");
	//CompositorManager::getSingleton().setCompositorEnabled(mViewport,
	//	"Bloom", true);

}

void HiveGame::setupCharacter()
{
	BeeEngine::setupCharacter();

	mCharacterMan->setPosition(Vector3(-100, 195, -155));
}


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
 int main(int argc, char *argv[])
#endif
{
	HiveGame app;
	return app.exec();
}

