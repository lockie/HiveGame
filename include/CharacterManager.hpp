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

#ifndef __Character_h__
#define __Character_h__

#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreAnimationState.h>

#include <OISKeyboard.h>
#include <OISMouse.h>


class CharacterManager
{
public:
	static const size_t NUM_ANIMS = 7;
	enum AnimID
	{
		ANIM_IDLE_BASE = 0,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE = NUM_ANIMS
	};

	CharacterManager(Ogre::Camera* camera,
		const Ogre::String& filename = Ogre::StringUtil::BLANK);
	~CharacterManager();

	bool setModel(const Ogre::String& filename);

	void update(Ogre::Real deltaTime);

	void injectKeyDown(const OIS::KeyEvent& evt);
	void injectKeyUp(const OIS::KeyEvent& evt);
	void injectMouseMove(const OIS::MouseEvent& evt);
	void injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id);


private:
	Ogre::Camera* mCamera;
	Ogre::SceneNode* mBodyNode;
	Ogre::Entity* mBodyEnt;
	Ogre::SceneNode* mCameraPivot;
	Ogre::SceneNode* mCameraGoal;
	Ogre::SceneNode* mCameraNode;
	Ogre::Real mPivotPitch;

	Ogre::Vector3 mKeyDirection;      // направление игрока на основе WASD
	Ogre::Real mVerticalVelocity;     // для прыжков; TODO : заменить физикой
	Ogre::Vector3 mGoalDirection;

	Ogre::AnimationState* mAnims[NUM_ANIMS];    // главный список анимаций
	AnimID mBaseAnimID;         // текущая базовая (нижней части тела или всей меши) анмация
	AnimID mTopAnimID;          // текущая верхняя (верхней части тела) анимация
	bool mFadingIn[NUM_ANIMS];  // какие анимации включаются
	bool mFadingOut[NUM_ANIMS]; // какие анимации выключаются
	Ogre::Real mTimer;          // таймер, чтобы смотреть, как долго анимации проигрываются

	void updateBody(Ogre::Real deltaTime);
	void updateAnimations(Ogre::Real deltaTime);
	void updateCamera(Ogre::Real deltaTime);

	void updateCameraGoal(Ogre::Real deltaYaw, Ogre::Real deltaPitch,
		Ogre::Real deltaZoom);

	void setBaseAnimation(AnimID id, bool reset = false);
	void setTopAnimation(AnimID id, bool reset = false);
	void fadeAnimations(Ogre::Real deltaTime);

};

#endif  // #ifndef __Character_h__

