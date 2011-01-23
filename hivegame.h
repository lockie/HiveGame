
#ifndef __HIVE_H__
#define __HIVE_H__

#include "config.h"

#include <crystalspace.h>
#include <cel.h>

#include "console.h"


class HiveGame : public csApplicationFramework, public csBaseEventHandler
{
public:
	HiveGame();
	~HiveGame();

	bool Application();

	bool OnInitialize(int argc, char* argv[]);
	void OnExit();

	void DecRef();
	void IncRef();

CS_EVENTHANDLER_NAMES("crystalspace.hivegame");
CS_EVENTHANDLER_NIL_CONSTRAINTS
SCF_INTERFACE(csApplicationFramework, 0, 1, 0);

private:
	bool Setup();
	void LoadConfig();
	void ProcessFrame();
	void FinishFrame();
	bool OnKeyboard(iEvent&);

	csRef<iEngine> engine;
	csRef<iGraphics2D> g2d;
	csRef<iGraphics3D> g3d;
	csRef<iKeyboardDriver> kbd;
	csRef<iVirtualClock> vc;

	csRef<iConfigManager> confman;
	csRef<iReporter> reporter;
	csRef<Console> console;
	csRef<iStandardReporterListener> listener;

	csRef<iCelPlLayer> pl;
	csRef<iCelBlLayer> bl;

	bool use_console;
	bool enable_logging;
	csString log_filename;
	bool append_logfile;
};

#endif  // __HIVE_H__

