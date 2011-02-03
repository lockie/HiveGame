
#include "player.h"
#include "map.h"

#include <propclass/camera.h>


Map* instance = NULL;

Map::Map(const char* _name)
{
	name = "<none>";

	iObjectRegistry* object_reg = csApplicationFramework::GetObjectRegistry();

	engine = csQueryRegistry<iEngine>(object_reg);
	if(!engine)
	{
		csApplicationFramework::ReportError("Failed to load Engine!");
		return;
	}
	pl = csQueryRegistry<iCelPlLayer>(object_reg);
	if(!pl)
	{
		csApplicationFramework::ReportError("Failed to load celPl plugin!");
		return;
	}
	bl = csQueryRegistry<iCelBlLayer>(object_reg);

	csApplicationFramework::ReportInfo("Loading map %s", _name);

	entity = pl->CreateEntity("level", bl, "level_behave",
		"pcworld.zonemanager", CEL_PROPCLASS_END );
	if(!entity)
	{
		csApplicationFramework::ReportError("Failed to create level entity!");
		return;
	}

	zonemgr = CEL_QUERY_PROPCLASS_ENT(entity, iPcZoneManager);
	iCelZone* zone = zonemgr->CreateZone("main");
	iCelRegion* region = zonemgr->CreateRegion("main");
	zone->LinkRegion(region);

	iCelMapFile* mapfile = region->CreateMapFile();
	mapfile->SetFile(_name);

	csRef<iCelEntity> player_entity = Player::GetPlayer()->Entity();
	csRef<iPcCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(
		player_entity, iPcCamera);
	pccamera->SetZoneManager(zonemgr, true, "main", "Camera");

	int res = zonemgr->PointMesh("player", "main", "Camera");
	if(res == CEL_ZONEERROR_LOAD)
	{
		csApplicationFramework::ReportError("Failed to load map \"%s\"!", _name);
		return;
	}

	if(res == CEL_ZONEERROR_BADREGION || res == CEL_ZONEERROR_BADSTART)
	{
		csApplicationFramework::ReportWarning("Failed to find starting position!");
	}

	name = _name;
	instance = this;
}

Map::~Map()
{
	engine->DeleteAll();
}

const char* Map::Name() const
{
	return name;
}

Map* Map::CurrentMap()
{
	if(!instance)
		LoadMap("");
	return instance;
}

void Map::LoadMap(const char* name)
{
	delete instance;
	instance = new Map(name);
}

