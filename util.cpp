
#include "config.h"

#include <crystalspace.h>

#include "map.h"

CS_IMPLEMENT_FOREIGN_DLL


void echo(const char* message)
{
	csReporterHelper::Report(
		csApplicationFramework::GetObjectRegistry(),
		CS_REPORTER_SEVERITY_WARNING,
		"HiveGame.Console",
		message );
}

void quit()
{
	csApplicationFramework::Quit();
}

void ls(const char* directory)
{
	csRef<iVFS> vfs =
		csQueryRegistry<iVFS>(csApplicationFramework::GetObjectRegistry());
	if(!vfs)
	{
		csApplicationFramework::ReportError("Unable to query filesystem");
		return;
	}
	csRef<iStringArray> files = vfs->FindFiles(directory);
	for(size_t i = 0; i < files->GetSize(); i++)
		csApplicationFramework::ReportInfo("%s", files->Get(i));
}

void dir(const char* directory)
{
	ls(directory);
}

void path()
{
	csRef<iVFS> vfs =
		csQueryRegistry<iVFS>(csApplicationFramework::GetObjectRegistry());
	if(!vfs)
	{
		csApplicationFramework::ReportError("Unable to query filesystem");
		return;
	}
	csRef<iStringArray> mounts = vfs->GetMounts();
	for(size_t i = 0; i < mounts->GetSize(); i++)
	{
		csApplicationFramework::ReportInfo("%s on %s",
			vfs->GetRealMountPaths(mounts->Get(i))->Get(0),
			mounts->Get(i) );
	}
}

void map(const char* name)
{
	if(!name)
		csApplicationFramework::ReportInfo(Map::CurrentMap()->Name());
	else
		Map::LoadMap(name);
}

