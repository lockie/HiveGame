
#include "config.h"

#include <crystalspace.h>

#include <physicallayer/entity.h>
#include <physicallayer/propclas.h>
#include <propclass/linmove.h>
#include <propclass/prop.h>

#include "behaviour.h"


SCF_IMPLEMENT_IBASE (BehaviourLayer)
	SCF_IMPLEMENTS_INTERFACE (iCelBlLayer)
SCF_IMPLEMENT_IBASE_END

BehaviourLayer::BehaviourLayer(iCelPlLayer* pl)
{
	SCF_CONSTRUCT_IBASE(NULL);
	BehaviourLayer::pl = pl;
}

BehaviourLayer::~BehaviourLayer()
{
	SCF_DESTRUCT_IBASE ();
}

const char* BehaviourLayer::GetName() const
{
	return "behaviourlayer";
}

iCelBehaviour* BehaviourLayer::CreateBehaviour (iCelEntity* entity,
	const char* name)
{
	iCelBehaviour* behave = NULL;
	if(!strcmp(name, "level_behave"))
		behave = new LevelBehaviour(entity, this, pl);
	else if(!strcmp(name, "player_behave"))
		behave = new PlayerBehaviour(entity, this, pl);

	if(behave)
	{
		entity->SetBehaviour(behave);
		behave->DecRef ();
	}
	return behave;
}

SCF_IMPLEMENT_IBASE (CommonBehaviour)
	SCF_IMPLEMENTS_INTERFACE (iCelBehaviour)
SCF_IMPLEMENT_IBASE_END

CommonBehaviour::CommonBehaviour(iCelEntity* _entity, BehaviourLayer* _bl,
	iCelPlLayer* _pl)
{
	SCF_CONSTRUCT_IBASE(NULL);
	entity = _entity;
	bl = _bl;
	pl = _pl;
}

CommonBehaviour::~CommonBehaviour()
{
	SCF_DESTRUCT_IBASE();
}

iCelBlLayer* CommonBehaviour::GetBehaviourLayer() const
{
	return bl;
}

void* CommonBehaviour::GetInternalObject()
{
	return 0;
}

bool CommonBehaviour::SendMessage(const char* msg_id, iCelPropertyClass* pc,
	celData& ret, iCelParameterBlock* params, ...)
{
	va_list arg;
	va_start (arg, params);
	bool rc = SendMessageV(msg_id, pc, ret, params, arg);
	va_end (arg);
	return rc;
}

bool CommonBehaviour::SendMessageV (const char* msg_id, iCelPropertyClass* pc,
	celData& ret, iCelParameterBlock* params, va_list arg)
{
	csStringID id = pl->FetchStringID (msg_id);
	return SendMessage (id, pc, ret, params, arg);
}

bool CommonBehaviour::SendMessage (csStringID, iCelPropertyClass*,
	celData&, iCelParameterBlock*, va_list)
{
	return false;
}

bool LevelBehaviour::SendMessage (csStringID msg_id, iCelPropertyClass* pc,
	celData& ret, iCelParameterBlock* params, va_list arg)
{
	return CommonBehaviour::SendMessage (msg_id, pc, ret, params, arg);
}

const char* LevelBehaviour::GetName () const
{
	return "level_behave";
}

PlayerBehaviour::PlayerBehaviour(iCelEntity* entity, BehaviourLayer* bl,
	iCelPlLayer* pl) : CommonBehaviour(entity, bl, pl)
{
	id_pccommandinput_forward1 = pl->FetchStringID("pccommandinput_forward1");
	id_pccommandinput_forward0 = pl->FetchStringID("pccommandinput_forward0");
	id_pccommandinput_backward1 = pl->FetchStringID("pccommandinput_backward1");
	id_pccommandinput_backward0 = pl->FetchStringID("pccommandinput_backward0");
	id_pccommandinput_rotateleft1 = pl->FetchStringID("pccommandinput_rotateleft1");
	id_pccommandinput_rotateleft0 = pl->FetchStringID("pccommandinput_rotateleft0");
	id_pccommandinput_rotateright1 = pl->FetchStringID("pccommandinput_rotateright1");
	id_pccommandinput_rotateright0 = pl->FetchStringID("pccommandinput_rotateright0");
	id_pccommandinput_cammode1 = pl->FetchStringID("pccommandinput_cammode1");
	id_pccommandinput_drop1 = pl->FetchStringID("pccommandinput_drop1");

	id_pcinventory_addchild = pl->FetchStringID("pcinventory_addchild");
	id_pcinventory_removechild = pl->FetchStringID("pcinventory_removechild");
}

const char* PlayerBehaviour::GetName() const
{
	return "player_behave";
}

void PlayerBehaviour::GetActorMove()
{
	if(!pcactormove)
	{
		pcactormove = CEL_QUERY_PROPCLASS_ENT(entity, iPcActorMove);
	}
}

void PlayerBehaviour::GetInventory()
{
	if(!pcinventory)
	{
	pcinventory = CEL_QUERY_PROPCLASS_ENT(entity, iPcInventory);
	}
}

void PlayerBehaviour::GetMesh()
{
	if(!pcmesh)
	{
		pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
	}
}

void PlayerBehaviour::ShowInventory()
{
	size_t count = pcinventory->GetEntityCount();
	size_t i;
	for(i = 0 ; i < count ; i++)
	{
		iCelEntity* child = pcinventory->GetEntity(i);
		printf ("  child %d is '%s'\n", i, child->GetName());
	}
}

void PlayerBehaviour::Drop()
{
	GetInventory();
	size_t count = pcinventory->GetEntityCount();
	if(count <= 0)
	{
		printf("Inventory is empty!\n");
		return;
	}
	iCelEntity* child = pcinventory->GetEntity(0);
	pcinventory->RemoveEntity(child);
	csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(
		child, iPcLinearMovement);
	if(pclinmove)
	{
		GetMesh ();
		csVector3 pos = pcmesh->GetMesh()->GetMovable()->GetTransform()
			.This2Other(csVector3 (0, 2, -2));
		iSector* sector = pcmesh->GetMesh()->GetMovable()->GetSectors()->Get(0);
		pclinmove->SetPosition(pos, 0, sector);
		pclinmove->SetVelocity(csVector3 (0, .1f, 0));
		csRef<iPcMesh> pcmesh_child = CEL_QUERY_PROPCLASS_ENT(child, iPcMesh);
		if(pcmesh_child) pcmesh_child->Show();
	}
}

bool PlayerBehaviour::SendMessage (csStringID msg_id, iCelPropertyClass* pc,
	celData& ret, iCelParameterBlock* params, va_list arg)
{
	GetActorMove ();

	if(msg_id == id_pccommandinput_forward1)
		pcactormove->Forward(true);
	else if(msg_id == id_pccommandinput_forward0)
		pcactormove->Forward(false);
	else if(msg_id == id_pccommandinput_backward1)
		pcactormove->Backward(true);
	else if(msg_id == id_pccommandinput_backward0)
		pcactormove->Backward(false);
	else if(msg_id == id_pccommandinput_rotateleft1)
		pcactormove->RotateLeft(true);
	else if(msg_id == id_pccommandinput_rotateleft0)
    pcactormove->RotateLeft(false);
	else if(msg_id == id_pccommandinput_rotateright1)
		pcactormove->RotateRight(true);
	else if(msg_id == id_pccommandinput_rotateright0)
		pcactormove->RotateRight(false);
	else if(msg_id == id_pccommandinput_cammode1)
		pcactormove->ToggleCameraMode ();
	else if(msg_id == id_pccommandinput_drop1)
		Drop();
	else if(msg_id == id_pcinventory_addchild)
	{
		GetInventory();
		printf("Got a new object! Objects in inventory:\n");
		ShowInventory();
	}
	else if(msg_id == id_pcinventory_removechild)
	{
		GetInventory();
		printf("Object removed from inventory! Objects in inventory:\n");
		ShowInventory();
	}
	else
		return CommonBehaviour::SendMessage(msg_id, pc, ret, params, arg);

	return true;
}

