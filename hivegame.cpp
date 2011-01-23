
#include "behaviour.h"
#include "hivegame.h"


CS_IMPLEMENT_APPLICATION

HiveGame::HiveGame()
{
	SetApplicationName("HiveGame");
}

HiveGame::~HiveGame()
{
}

bool HiveGame::Application()
{
	if(!OpenApplication(object_reg))
		return ReportError ("Failed to initialize CrystalSpace!");
	if(!Setup())
		return false;
	Run();
	return true;
}

bool HiveGame::OnInitialize(int /*argc*/, char* /*argv*/ [])
{
	if(!csInitializer::SetupConfigManager(object_reg, "/hivegame/hivegame.cfg"))
		ReportWarning("Failed to read config file hivegame.cfg!");

	if(!celInitializer::RequestPlugins(object_reg,
		CS_REQUEST_VFS,
		CS_REQUEST_OPENGL3D,
		CS_REQUEST_ENGINE,
		CS_REQUEST_FONTSERVER,
		CS_REQUEST_IMAGELOADER,
		CS_REQUEST_LEVELLOADER,
		CS_REQUEST_REPORTER,
		CS_REQUEST_REPORTERLISTENER,
		CS_REQUEST_PLUGIN("crystalspace.collisiondetection.opcode",
			iCollideSystem),
		CS_REQUEST_PLUGIN("crystalspace.console.output.standard",
			iConsoleOutput),
		CS_REQUEST_PLUGIN("cel.physicallayer",
			iCelPlLayer),
		CS_REQUEST_END))
		return ReportError("Failed to initialize plugins!");

	csBaseEventHandler::Initialize(object_reg);

	if(!RegisterQueue(object_reg, csevAllEvents(object_reg)))
		return ReportError("Failed to initialize events!");

	return true;
}

void HiveGame::OnExit()
{
}

void HiveGame::DecRef()
{
}

void HiveGame::IncRef()
{
}

bool HiveGame::Setup()
{
	g2d = csQueryRegistry<iGraphics2D>(object_reg);
	if(!g2d) return ReportError("Failed to load Graphics!");

	g3d = csQueryRegistry<iGraphics3D>(object_reg);
	if (!g3d) return ReportError("Failed to load Renderer!");

	engine = csQueryRegistry<iEngine>(object_reg);
	if(!engine) return ReportError("Failed to load Engine!");

	vc = csQueryRegistry<iVirtualClock>(object_reg);
	if(!vc) return ReportError("Failed to load Clock!");

	kbd = csQueryRegistry<iKeyboardDriver>(object_reg);
	if(!kbd) return ReportError("Failed to load Keyboard!");

	reporter = csQueryRegistry<iReporter>(object_reg);
	if(!reporter) return ReportError("Failed to load Reporter!");

	pl = csQueryRegistry<iCelPlLayer> (object_reg);
	if(!pl) return ReportError("Failed to load CEL physical layer!");

	bl.AttachNew(new BehaviourLayer(pl));
	if (!object_reg->Register(bl, "iCelBlLayer"))
		return ReportError("Fial to register behaviour layer!");
	pl->RegisterBehaviourLayer(bl);

	LoadConfig();

	// TODO : опцию в конфиге на предмет наличия кэша света
	engine->SetLightingCacheMode (0);

	if(use_console)
	{
		console.AttachNew(new Console);
		if(!console->Initialize(object_reg))
			ReportError("Failed to initialize console!");
	}
	if(enable_logging)
	{
		listener = csQueryRegistry<iStandardReporterListener>(object_reg);
		listener->SetMessageDestination(CS_REPORTER_SEVERITY_ERROR, true, false, true, false, true, false);
		listener->SetMessageDestination(CS_REPORTER_SEVERITY_WARNING, true, false, true, false, true, false);
		listener->SetMessageDestination(CS_REPORTER_SEVERITY_NOTIFY, true, false, true, false, true, false);
		listener->SetMessageDestination(CS_REPORTER_SEVERITY_DEBUG, true, false, false, false, true, false);
		listener->SetDebugFile(log_filename,append_logfile);

		time_t curr_time;
		time(&curr_time);
		csString time_str = "HiveGame started at ";
		time_str += ctime(&curr_time);
		csReporterHelper::Report(object_reg, CS_REPORTER_SEVERITY_NOTIFY, "HiveGame", time_str);
	}

	return true;
}

void HiveGame::LoadConfig()
{
	csRef<iConfigManager> confman = csQueryRegistry<iConfigManager>(object_reg);
	use_console = confman->GetBool("HiveGame.Settings.EnableConsole", 
#ifdef _DEBUG
		true
#else  // _DEBUG
		false
#endif  // _DEBUG
	);
	enable_logging = confman->GetBool("HiveGame.Settings.EnableLogging", false);
	log_filename = confman->GetStr("HiveGame.Settings.DebugFileName", "/hivegame/hivegame.log");
	append_logfile = confman->GetBool("HiveGame.Settings.AppendLogFile", false);
}



void HiveGame::ProcessFrame()
{
//		collider_actor.Move(float(elapsed_time) / 1000.0f, 1.0f,
//    		obj_move, obj_rotate);

/*	if (!g3d->BeginDraw(engine->GetBeginDrawFlags() | CSDRAW_3DGRAPHICS))
		return; */

//	view->Draw();
}

void HiveGame::FinishFrame ()
{
	g3d->FinishDraw();
	g3d->Print(0);
}

bool HiveGame::OnKeyboard(iEvent& ev)
{
	if(console && console->IsVisible())
		return false;

	csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
	if (eventtype == csKeyEventTypeDown)
	{
		utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
		if (code == CSKEY_ESC)
		{
			csRef<iEventQueue> q = csQueryRegistry<iEventQueue>(object_reg);
			if (q.IsValid())
				q->GetEventOutlet()->Broadcast(csevQuit(object_reg));
		}
	}
	return false;
}


int main (int argc, char* argv[])
{
	return csApplicationRunner<HiveGame>::Run(argc, argv);
}

