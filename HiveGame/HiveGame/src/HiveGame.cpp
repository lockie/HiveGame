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
	World::getSingletonPtr()->Load("test.scene");
}

void HiveGame::setupCharacter()
{
	BeeEngine::setupCharacter();

	mCharacterMan->setPosition(Vector3(-80, 195, -155));
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

