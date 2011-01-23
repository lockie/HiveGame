
#include "map.h"


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
	loader = csQueryRegistry<iLoader>(object_reg);
	if(!loader)
	{
		csApplicationFramework::ReportError("Yo dawg, Failed to load Loader!");
		return;
	}
	cdsys = csQueryRegistry<iCollideSystem>(object_reg);
	if(!cdsys)
	{
		csApplicationFramework::ReportError("Failed to load Physics!");
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

	level_entity = pl->CreateEntity("level", bl, "level_behave",
		"pcworld.zonemanager", CEL_PROPCLASS_END );
	if(!level_entity)
	{
		csApplicationFramework::ReportError ("Failed to create level entity!");
		return;
	}

	csRef<iPcZoneManager> zonemgr = CEL_QUERY_PROPCLASS_ENT( level_entity,
		iPcZoneManager );
	iCelZone* zone = zonemgr->CreateZone("main");
	iCelRegion* region = zonemgr->CreateRegion("main");
	zone->LinkRegion(region);

	iCelMapFile* mapfile = region->CreateMapFile();
	mapfile->SetFile(_name);

	player_entity = pl->CreateEntity("player", bl, "player_behave",
		"pccamera.old",
		"pcobject.mesh",
		"pcmove.linear",
		"pcmove.actor.standard",
		"pcinput.standard",
		"pctools.inventory",
		CEL_PROPCLASS_END );
	if (!player_entity)
	{
		csApplicationFramework::ReportError("Failed to create player entity!");
		return;
	}

	csRef<iPcCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(
		player_entity, iPcCamera );
	pccamera->SetZoneManager(zonemgr, true, "main", "Camera");

	csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(
		player_entity, iPcMesh );
	pcmesh->SetPath("/hivegame/data/model");
	pcmesh->SetMesh("test", "cally.cal3d");
	if(!pcmesh->GetMesh())
	{
		csApplicationFramework::ReportError(
			"Failed to load player model \"%s\"!",
			"cally.cal3d" );
		return;
	}

	int res = zonemgr->PointMesh("player", "main", "Camera");
	if(res == CEL_ZONEERROR_LOAD)
	{
		csApplicationFramework::ReportError("Failed to load map \"%s\"!", _name);
		return;
	}

	if(res == CEL_ZONEERROR_BADREGION || res == CEL_ZONEERROR_BADSTART)
	{
		csApplicationFramework::ReportError("Failed to find starting position!");
	}

	csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(
		player_entity, iPcLinearMovement);
	pclinmove->InitCD(
		csVector3 (0.5,0.8,0.5),
		csVector3 (0.5,0.4,0.5),
		csVector3 (0,0,0) );

	csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(
		player_entity, iPcActorMove );
	pcactormove->SetMovementSpeed (3.0f);
	pcactormove->SetRunningSpeed (5.0f);
	pcactormove->SetRotationSpeed (1.75f);

	csRef<iPcCommandInput> pcinput = CEL_QUERY_PROPCLASS_ENT(
		player_entity, iPcCommandInput);
	pcinput->Bind ("w", "forward");
	pcinput->Bind ("s", "backward");
	pcinput->Bind ("a", "rotateleft");
	pcinput->Bind ("d", "rotateright");
	pcinput->Bind ("q", "cammode");
	pcinput->Bind ("e", "drop");

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

