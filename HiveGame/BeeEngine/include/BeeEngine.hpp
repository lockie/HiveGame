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

#ifndef __BeeEngine_h__
#define __BeeEngine_h__

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>

#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

#include "CharacterManager.hpp"
#include "World.hpp"


class BeeEngine : public Ogre::FrameListener, public Ogre::WindowEventListener,
 public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
{
public:
	BeeEngine();
	virtual ~BeeEngine();

	virtual void run();
	virtual int exec();

protected:
	virtual bool setup();
	virtual bool configure();
	virtual bool loadPlugins(const Ogre::String& plugins_dir);
	virtual void chooseSceneManager();
	virtual void setupInput();
	virtual void setupCharacter();
	virtual void createCamera();
	virtual void createFrameListener();
	virtual void createScene() = 0;
	virtual void destroyScene();
	virtual void createViewports();
	virtual void setupResources();
	virtual void createResourceListener();
	virtual void loadResources();
	virtual void createGUI();

	// Ogre::FrameListener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	Ogre::Root* mRoot;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::Camera* mCamera;
	Ogre::Viewport* mViewport;
	CharacterManager* mCharacterMan;

	Ogre::String mResourcesDir;

	// OgreBites
	OgreBites::SdkTrayManager* mTrayMgr;
	OgreBites::ParamsPanel* mDetailsPanel;
	bool mCursorWasVisible;
	bool mShutDown;

	// Девайсы OIS
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;

	// Копрокубы для проверки физики
	std::vector< Ogre::SharedPtr<GameObject> > boxes;

	// ГУЙ
	MyGUI::Gui* mGUI;
	MyGUI::OgrePlatform* mGUIPlatform;
};

#endif // #ifndef __BeeEngine_h__

