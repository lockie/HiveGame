
#ifndef __MAP_H__
#define __MAP_H__

#include "config.h"

#include <crystalspace.h>

#include <celtool/initapp.h>
#include <propclass/zone.h>
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
	csRef<iView> view;

	csRef<iCelPlLayer> pl;
	csRef<iCelBlLayer> bl;

	csRef<iPcZoneManager> zonemgr;
	csRef<iCelEntity> entity;

	csString name;
};

#endif  // __MAP_H__

