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

#include "World.hpp"
#include "CharacterManager.hpp"

#include <OgreSceneManager.h>

using namespace std;
using namespace Ogre;


// TODO : разхардкодить!
#define CHAR_HEIGHT 5          // высота центра масс персонажа
#define CAM_HEIGHT 2           // высота камеры по отношеню к центру масс персонажа
#define RUN_SPEED 17           // скорость бега персонажа, единиц/с
#define TURN_SPEED 500.0f      // скорость поворота персонажа, град/с
#define JUMP_ACCEL 30.0f       // ускорение прыжка, единиц/с^2
#define GRAVITY 90.0f          // гравитация, единиц/с^2
#define ANIM_FADE_SPEED 7.5f   // скорость перехода между анимациями, % от полного веса/с

CharacterManager::CharacterManager(Camera* cam, const String& filename) :
 mCamera(cam), mKeyDirection(Vector3::ZERO), mVerticalVelocity(0)
{
	// создать ось где-то у плеча персонажа
	mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	// здесь скоро будет камера, и она вращается вокруг оси
	mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0, 0, 15));
	// здесь, собственно, и есть камера
	mCameraNode = cam->getSceneManager()->getRootSceneNode()->
		createChildSceneNode();
	mCameraNode->setPosition(mCameraPivot->getPosition() +
		mCameraGoal->getPosition());
	mCameraPivot->setFixedYawAxis(true);
	mCameraGoal->setFixedYawAxis(true);
	mCameraNode->setFixedYawAxis(true);
	mCameraNode->attachObject(cam);
	cam->setNearClipDistance(0.1f);
	cam->setFarClipDistance(100);
	mPivotPitch = 0;

	if(!setModel(filename))
		if(!setModel("Sinbad.mesh"))  // попробовать модель по умолчанию
			OGRE_EXCEPT( Ogre::Exception::ERR_FILE_NOT_FOUND,
				"Не удалось загрузить модель игрока.\nПереустановите игру.",
				"BeeEngine" );

	String animNames[] =
		{"IdleBase", "IdleTop", "RunBase", "RunTop",
			"JumpStart", "JumpLoop", "JumpEnd"};

	// проинициализировать список анимаций
	for (int i = 0; i < NUM_ANIMS; i++)
	{
		mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	// начать с состояния idle
	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);
}

CharacterManager::~CharacterManager()
{

}

bool CharacterManager::setModel(const String& filename)
{
	if(filename.empty())
		return false;
	try
	{
		mBodyNode = mCamera->getSceneManager()->getRootSceneNode()->
			createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
		mBodyEnt = mCamera->getSceneManager()->createEntity("Player",
			filename);
		mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);
		mBodyNode->attachObject(mBodyEnt);
		mBodyNode->rotate(Vector3::UNIT_Y, Degree(180));
		mPlayer = World::getSingletonPtr()->addPlayer(mBodyEnt, mBodyNode);
		return true;
	} catch(Exception&)
	{
		return false;
	}
}

void CharacterManager::update(Real deltaTime)
{
	updateBody(deltaTime);
	updateAnimations(deltaTime);
	updateCamera(deltaTime);
}

void CharacterManager::injectKeyDown(const OIS::KeyEvent& evt)
{
	// следим, куда направляется игрок
	if(evt.key == OIS::KC_W) mKeyDirection.z = -1;
	else if(evt.key == OIS::KC_A) mKeyDirection.x = -1;
	else if(evt.key == OIS::KC_S) mKeyDirection.z = 1;
	else if(evt.key == OIS::KC_D) mKeyDirection.x = 1;
	else if(evt.key == OIS::KC_SPACE &&
		(mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// прыгаем
		setBaseAnimation(ANIM_JUMP_START, true);
		setTopAnimation(ANIM_NONE);
		mTimer = 0;
	}

	if(!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
	{
		// начать бежать, если ещё не движемся и игрок хочет двигаться
		setBaseAnimation(ANIM_RUN_BASE, true);
		if (mTopAnimID == ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
	}
}

void CharacterManager::injectKeyUp(const OIS::KeyEvent& evt)
{
	// следим, куда направляется игрок
	if(evt.key == OIS::KC_W && mKeyDirection.z == -1) mKeyDirection.z = 0;
	else if(evt.key == OIS::KC_A && mKeyDirection.x == -1) mKeyDirection.x = 0;
	else if(evt.key == OIS::KC_S && mKeyDirection.z == 1) mKeyDirection.z = 0;
	else if(evt.key == OIS::KC_D && mKeyDirection.x == 1) mKeyDirection.x = 0;

	if(mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE)
	{
		// перестать бежать, если уже бежим, а игрок не хочет двигаться
		setBaseAnimation(ANIM_IDLE_BASE);
		if (mTopAnimID == ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
	}
}

void CharacterManager::injectMouseMove(const OIS::MouseEvent& evt)
{
	// обновить камеру в соответствии с движением мыши
	updateCameraGoal(-0.05f * evt.state.X.rel, -0.05f * evt.state.Y.rel,
		-0.0005f * evt.state.Z.rel);
}

void CharacterManager::injectMouseDown(const OIS::MouseEvent& evt,
	OIS::MouseButtonID id)
{
	// TODO : стреляли
}

void CharacterManager::updateBody(Real deltaTime)
{
	mGoalDirection = Vector3::ZERO;   // щас посчитаем
	if(mKeyDirection != Vector3::ZERO)
	{
		// посчитать направление цели на основе направления игрока
		mGoalDirection += mKeyDirection.z*mCameraNode->getOrientation().zAxis();
		mGoalDirection += mKeyDirection.x*mCameraNode->getOrientation().xAxis();
		mGoalDirection.y = 0;
		mGoalDirection.normalise();
		Quaternion toGoal = mBodyNode->getOrientation()
			.zAxis().getRotationTo(mGoalDirection);

		// посчитать, насколько персонаж должен повернуться, чтобы увидеть цель
		Real yawToGoal = toGoal.getYaw().valueDegrees();
		// на столько персонаж может повернуться за этот фрейм
		Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) *
			deltaTime * TURN_SPEED;
		// снизить способность к повороту, если персонаж в воздухе
		if(mBaseAnimID == ANIM_JUMP_LOOP)
			yawAtSpeed *= 0.2f;

		// повернуться настолько, насколько можно
		if(yawToGoal < 0)
			yawToGoal = min<Real>(0, max<Real>(yawToGoal, yawAtSpeed));
		else if(yawToGoal > 0) yawToGoal =
			max<Real>(0, min<Real>(yawToGoal, yawAtSpeed));
		mBodyNode->yaw(Degree(yawToGoal));

		// продвинуться в текущем направлении тела
		mBodyNode->translate( 0, 0,
			deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),
			Node::TS_LOCAL );

		// грязнохак для того, чтобы не вылезать за пределы сцены
		Real scene_size = 45.0;
		Vector3 pos = mBodyNode->getPosition();
		if(abs(pos.x) > scene_size)
		{
			pos.x = scene_size * pos.x / abs(pos.x);
			mBodyNode->setPosition(pos);
		}
		if(abs(pos.z) > scene_size)
		{
			pos.z = scene_size * pos.z / abs(pos.z);
			mBodyNode->setPosition(pos);
		}
	}
	if(mBaseAnimID == ANIM_JUMP_LOOP)
	{
		// если мы в прыжке, добавить вертикальное смещение и гравитацию
		mBodyNode->translate(0, mVerticalVelocity * deltaTime, 0, Node::TS_LOCAL);
		mVerticalVelocity -= GRAVITY * deltaTime;
		
		Vector3 pos = mBodyNode->getPosition();
		if (pos.y <= CHAR_HEIGHT)
		{
			// если мы приземлились, сменить состояние анимации
			pos.y = CHAR_HEIGHT;
			mBodyNode->setPosition(pos);
			setBaseAnimation(ANIM_JUMP_END, true);
			mTimer = 0;
		}
	}
}

void CharacterManager::updateAnimations(Real deltaTime)
{
	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;

	mTimer += deltaTime;

	if(mBaseAnimID == ANIM_JUMP_START)
	{
		if(mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			// начать анимацию прыжка
			setBaseAnimation(ANIM_JUMP_LOOP, true);
			// добавить ускорение
			mVerticalVelocity = JUMP_ACCEL;
		}
	}
	else if(mBaseAnimID == ANIM_JUMP_END)
	{
		if(mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			// приземлились, возвращаемся к бегу или idle-состоянию
			if(mKeyDirection == Vector3::ZERO)
			{
				setBaseAnimation(ANIM_IDLE_BASE);
				setTopAnimation(ANIM_IDLE_TOP);
			}
			else
			{
				setBaseAnimation(ANIM_RUN_BASE, true);
				setTopAnimation(ANIM_RUN_TOP, true);
			}
		}
	}

	// увеличить текущее время базовой и верхней анимаций
	if(mBaseAnimID != ANIM_NONE)
		mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if(mTopAnimID != ANIM_NONE)
		mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	// добавить мягкий переход между анимациями
	fadeAnimations(deltaTime);
}

void CharacterManager::updateCamera(Real deltaTime)
{
	// отпозиционировать ось камеры у плеча персонажа
	mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
	// мягко подвинуть камеру к цели
	Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
	mCameraNode->translate(goalOffset * deltaTime * 9.0f);
	// всегда смотреть на ось
	mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Node::TS_WORLD);
}

void CharacterManager::updateCameraGoal(Real deltaYaw, Real deltaPitch,
	Real deltaZoom)
{
	mCameraPivot->yaw(Degree(deltaYaw), Node::TS_WORLD);

	// ограничить pitch
	if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
		!(mPivotPitch + deltaPitch < -60 && deltaPitch < 0))
	{
		mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
		mPivotPitch += deltaPitch;
	}
	
	Real dist = mCameraGoal->_getDerivedPosition().
		distance(mCameraPivot->_getDerivedPosition());
	Real distChange = deltaZoom * dist;

	// ограничить zoom
	if (!(dist + distChange < 8 && distChange < 0) &&
		!(dist + distChange > 25 && distChange > 0))
	{
		mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
	}
}

void CharacterManager::setBaseAnimation(AnimID id, bool reset)
{
	if(mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
	{
		// если есть старая анимация, плавно отключить её
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id;

	if(id != ANIM_NONE)
	{
		// если есть новая анимация, разрешить её и плавно включить
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if(reset)
			mAnims[id]->setTimePosition(0);
	}
}

void CharacterManager::setTopAnimation(AnimID id, bool reset)
{
	if(mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	{
		// если есть старая анимация, плавно отключить её
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if(id != ANIM_NONE)
	{
		// если есть новая анимация, разрешить её и плавно включить
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if(reset)
			mAnims[id]->setTimePosition(0);
	}
}

void CharacterManager::fadeAnimations(Real deltaTime)
{
	for(int i = 0; i < NUM_ANIMS; i++)
	{
		if(mFadingIn[i])
		{
			// медленно включить анимацию, пока она не получит полный вес
			Real newWeight = mAnims[i]->getWeight() + deltaTime*ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if(newWeight >= 1)
				mFadingIn[i] = false;
		}
		else if(mFadingOut[i])
		{
			// медленно отключить анимацию, пока она не получит нулевой вес, после чего отключить
			Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight, 0, 1));
			if(newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

