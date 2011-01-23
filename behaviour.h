
#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__

#include "config.h"

#include <crystalspace.h>

#include <physicallayer/pl.h>
#include <behaviourlayer/bl.h>
#include <behaviourlayer/behave.h>
#include <propclass/actormove.h>
#include <propclass/meshsel.h>
#include <propclass/mesh.h>
#include <propclass/camera.h>
#include <propclass/inv.h>


class BehaviourLayer : public iCelBlLayer
{
public:
	BehaviourLayer(iCelPlLayer* pl);
	virtual ~BehaviourLayer();

	SCF_DECLARE_IBASE;

	virtual const char* GetName() const;
	virtual iCelBehaviour* CreateBehaviour(iCelEntity* entity,
		const char* name);

private:
	iCelPlLayer* pl;
};

class CommonBehaviour : public iCelBehaviour
{
public:
	CommonBehaviour(iCelEntity* entity, BehaviourLayer* bl, iCelPlLayer* pl);
	virtual ~CommonBehaviour();

	virtual bool SendMessage (csStringID msg_id, iCelPropertyClass* pc,
		celData& ret, iCelParameterBlock* params, va_list arg);

	SCF_DECLARE_IBASE;

	virtual iCelBlLayer* GetBehaviourLayer() const;
	virtual bool SendMessage (const char* msg_id, iCelPropertyClass* pc,
		celData& ret, iCelParameterBlock* params, ...);
	virtual bool SendMessageV (const char* msg_id, iCelPropertyClass* pc,
		celData& ret, iCelParameterBlock* params, va_list arg);
	virtual void* GetInternalObject();

protected:
	iCelEntity* entity;
	BehaviourLayer* bl;
	iCelPlLayer* pl;
};

class LevelBehaviour : public CommonBehaviour
{
public:
	LevelBehaviour(iCelEntity* entity, BehaviourLayer* bl, iCelPlLayer* pl)
    : CommonBehaviour (entity, bl, pl) { }
	virtual ~LevelBehaviour() { }

	virtual const char* GetName () const;

	virtual bool SendMessage (csStringID msg_id, iCelPropertyClass* pc,
		celData& ret, iCelParameterBlock* params, va_list arg);
};

class PlayerBehaviour : public CommonBehaviour
{
public:
	PlayerBehaviour(iCelEntity* entity, BehaviourLayer* bl, iCelPlLayer* pl);
	virtual ~PlayerBehaviour() { }

	virtual const char* GetName () const;
	virtual bool SendMessage (csStringID msg_id, iCelPropertyClass* pc,
		celData& ret, iCelParameterBlock* params, va_list arg);

private:
	csStringID id_pccommandinput_forward1;
	csStringID id_pccommandinput_forward0;
	csStringID id_pccommandinput_backward1;
	csStringID id_pccommandinput_backward0;
	csStringID id_pccommandinput_rotateleft1;
	csStringID id_pccommandinput_rotateleft0;
	csStringID id_pccommandinput_rotateright1;
	csStringID id_pccommandinput_rotateright0;
	csStringID id_pccommandinput_cammode1;
	csStringID id_pccommandinput_drop1;

	csStringID id_pcinventory_addchild;
	csStringID id_pcinventory_removechild;

	void GetActorMove();
	csWeakRef<iPcActorMove> pcactormove;

	void GetInventory();
	csWeakRef<iPcInventory> pcinventory;

	void GetMesh();
	csWeakRef<iPcMesh> pcmesh;

	void ShowInventory();
	void Drop();
};

#endif  // __BEHAVIOUR_H__

