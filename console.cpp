
#include "console.h"


void Console::ExecCallback::Execute(const char* cmd)
{
	if(strlen(cmd) == 0)
		return;
	// FORCED INDENTATION OF CODE lol
	csString wrap_cmd = csString("try:\n\texec\"\"\"") + csString(cmd) + 
		csString("\n\"\"\"\nexcept Exception as e:\n\tprint 'Error:', e");
	python->RunText(wrap_cmd);
}

bool Console::ExecCallback::InitPython(iObjectRegistry *obj_reg)
{
	python = csQueryRegistryOrLoad<iScript>(obj_reg,
		"crystalspace.script.python");
	if(!python)
	{
		csReport(obj_reg,
			CS_REPORTER_SEVERITY_ERROR, "HiveGame.Console",
			"Unable to load python interpreter!");
		return false;
	}
	// TODO : LT_OBJDIR
	if(!python->RunText("import sys; sys.path.append('.libs')") ||
		!python->RunText("from cspace import *") ||
		!python->LoadModule("cshelper"))
	{
		csReport(obj_reg,
			CS_REPORTER_SEVERITY_ERROR, "HiveGame.Console",
			"Unable to prepare python interpreter!");
		return false;
	}
	if(!python->RunText("from pyhivegame import *"))
	{
		csReport(obj_reg,
			CS_REPORTER_SEVERITY_ERROR, "HiveGame.Console",
			"Unable to load pyhivegame module!");
		return false;
	}
	return true;
}

Console::~Console()
{
}

bool Console::Initialize(iObjectRegistry* obj_reg)
{
	object_reg = obj_reg;

	conout = csQueryRegistryOrLoad<iConsoleOutput> (object_reg,
		"crystalspace.console.output.standard");
	if(!conout)
	{
		csReport (object_reg,
			CS_REPORTER_SEVERITY_ERROR, "HiveGame.Console",
			"Can't load the output console!");
		return false;
	}

	conin = csQueryRegistryOrLoad<iConsoleInput> (object_reg,
		"crystalspace.console.input.standard");
	if(!conin)
	{
		csReport (object_reg,
			CS_REPORTER_SEVERITY_ERROR, "HiveGame.Console",
			"Can't load the input console!");
		return false;
	}

	conin->Bind (conout);
	conin->SetPrompt("] ");
	ExecCallback* cb = new ExecCallback ();
	conin->SetExecuteCallback (cb);
	if(!cb->InitPython(object_reg))
		return false;

	cb->DecRef();
	Hide();

	eventHandler.AttachNew(new EventHandler(this));
	csRef<iEventQueue> queue = csQueryRegistry<iEventQueue>(object_reg);
	if(queue.IsValid())
	{
		csEventID events[] =
		{
			csevKeyboardEvent(object_reg),
			csevPostProcess(object_reg),
			CS_EVENTLIST_END
		};
		queue->RegisterListener(eventHandler, events);
	}
	FinalProcess = csevPostProcess(object_reg);

	return true;
}

bool Console::HandleEvent(iEvent &ev)
{
	if(CS_IS_KEYBOARD_EVENT(object_reg, ev))
	{
		csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
		if(eventtype == csKeyEventTypeDown)
		{
			utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
			if (code == 0x60)  // ~
			{
				Toggle();
				return true;
			}
			if(code == CSKEY_ESC && visible)
			{
				Hide();
				return true;
			}
		}
	}
	if (ev.Name == FinalProcess)
	{
		csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D>(object_reg);
		csRef<iGraphics2D> g2d = csQueryRegistry<iGraphics2D>(object_reg);

		if(conout->GetVisible())
		{
			g3d->BeginDraw(CSDRAW_2DGRAPHICS);
			conout->Draw2D(0);
			g3d->BeginDraw(CSDRAW_3DGRAPHICS);
			conout->Draw3D(0);
		}
		return false;
	}
	if(visible)
		return conin->HandleEvent(ev);
	else
		return false;
}

void Console::Hide()
{
	visible = false;
	conout->SetVisible(false);
}

void Console::Show()
{
	visible = true;
	conout->SetVisible(true);
}

void Console::Toggle()
{
	visible = !visible;
	conout->SetVisible(visible);
}

bool Console::IsVisible()
{
	return visible;
}

