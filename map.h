
#ifndef __MAP_H__
#define __MAP_H__

#include "config.h"

#include <crystalspace.h>

#include <celtool/initapp.h>
#include <propclass/zone.h>
#include <propclass/camera.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>
#include <propclass/actormove.h>
#include <propclass/input.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <physicallayer/pl.h>
#include <behaviourlayer/bl.h>


class Map
{
public:
	Map(const char* name);
	~Map();

	const char* Name() const;

	static Map* CurrentMap();
	static void LoadMap(const char* name);

private:
	csRef<iEngine> engine;
	csRef<iLoader> loader;
	csRef<iCollideSystem> cdsys;
	csRef<iView> view;

	csRef<iCelPlLayer> pl;
	csRef<iCelBlLayer> bl;

	csRef<iCelEntity> level_entity;
	csRef<iCelEntity> player_entity;

	csString name;
};

#endif  // __MAP_H__

