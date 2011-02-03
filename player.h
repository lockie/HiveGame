
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "config.h"

#include <crystalspace.h>

#include <physicallayer/entity.h>


class Player
{
public:
	Player();
	~Player();

	csPtr<iCelEntity> Entity();

	bool SetModel(const char* filename);

	static Player* GetPlayer();

private:
	csRef<iCelEntity> entity;

};

#endif  // __PLAYER_H__
