
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "config.h"

#include <crystalspace.h>


class Console : public csRefCount
{
public:
	~Console();

	bool Initialize(iObjectRegistry* obj_reg);
	bool HandleEvent(iEvent& ev);
	void Hide();
	void Show();
	void Toggle();
	bool IsVisible();

	csRef<iConsoleInput> conin;
	csRef<iConsoleOutput> conout;

private:
	struct EventHandler : public scfImplementation1<EventHandler, iEventHandler>
	{
		EventHandler(Console* _parent) : scfImplementationType(this), 
			parent(_parent) { }
		virtual ~EventHandler() { }

		virtual bool HandleEvent(iEvent& e) { return parent->HandleEvent(e); }

		Console* parent;

	CS_EVENTHANDLER_NAMES("hivegame.console")
	CS_EVENTHANDLER_NIL_CONSTRAINTS
	};

	struct ExecCallback : public scfImplementation1<ExecCallback,iConsoleExecCallback>
	{
		ExecCallback() : scfImplementationType(this) { }
		virtual ~ExecCallback() { }
	
		virtual void Execute(const char* cmd);
		bool InitPython(iObjectRegistry *obj_reg);

		csRef<iScript> python;
	};

	iObjectRegistry* object_reg; 
	csRef<EventHandler> eventHandler;
	csEventID FinalProcess;
	bool visible;
};

#endif // _CONSOLE_H_

