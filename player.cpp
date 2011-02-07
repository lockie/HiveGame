
#include "map.h"
#include "player.h"

#include <propclass/actormove.h>
#include <propclass/input.h>
#include <propclass/linmove.h>
#include <propclass/mesh.h>
#include <propclass/zone.h>
#include <physicallayer/propclas.h>
#include <physicallayer/pl.h>
#include <behaviourlayer/bl.h>


Player* player_instance = NULL;

Player::Player(/*const char* model*/)
{
	iObjectRegistry* object_reg = csApplicationFramework::GetObjectRegistry();
	csRef<iCelPlLayer> pl = csQueryRegistry<iCelPlLayer>(object_reg);
	if(!pl)
	{
		csApplicationFramework::ReportError("Failed to load celPl plugin!");
		return;
	}
	csRef<iCelBlLayer> bl = csQueryRegistry<iCelBlLayer>(object_reg);

	entity = pl->CreateEntity("player", bl, "player_behave",
		"pccamera.old",
		"pcobject.mesh",
		"pcmove.linear",
		"pcmove.actor.standard",
		"pcinput.standard",
		"pctools.inventory",
		CEL_PROPCLASS_END );
	if (!entity)
	{
		csApplicationFramework::ReportError("Failed to create player entity!");
		return;
	}
	if(!SetModel("/model/cally.cal3d"))
		return;

	csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(
		entity, iPcLinearMovement);
	pclinmove->InitCD(
		csVector3(0.5, 0.8, 0.5),
		csVector3(0.5, 0.4, 0.5),
		csVector3(0, 0, 0) );

	csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT(
		entity, iPcActorMove );
	pcactormove->SetMovementSpeed (3.0f);
	pcactormove->SetRunningSpeed (5.0f);
	pcactormove->SetRotationSpeed (1.75f);

	csRef<iPcCommandInput> pcinput = CEL_QUERY_PROPCLASS_ENT(
		entity, iPcCommandInput);
	pcinput->Bind ("w", "forward");
	pcinput->Bind ("s", "backward");
	pcinput->Bind ("a", "rotateleft");
	pcinput->Bind ("d", "rotateright");
	pcinput->Bind ("q", "cammode");
	pcinput->Bind ("e", "drop");

	player_instance = this;
}

Player::~Player()
{
}

csPtr<iCelEntity> Player::Entity()
{
	return csPtr<iCelEntity>(entity);
}

Player* Player::GetPlayer()
{
	if(!player_instance)
		player_instance = new Player();
	return player_instance;
}

bool Player::SetModel(const char* filename)
{
	size_t s = strlen(filename);
	char* path = new char[s];
	char* file = new char[s];
	csSplitPath(filename, path, s, file, s);

	csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(
		entity, iPcMesh );
	pcmesh->SetPath(path);
	pcmesh->SetMesh("player", file);
	delete[] file; delete[] path;

	if(!pcmesh->GetMesh())
	{
		csApplicationFramework::ReportError(
			"Failed to load player model \"%s\"!",
			filename );
		return false;
	}
	return true;
}
