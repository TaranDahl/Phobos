#include "Commands.h"

#include "ObjectInfo.h"
#include "NextIdleHarvester.h"
#include "QuickSave.h"
#include "DamageDisplay.h"
#include "FrameByFrame.h"
#include "FrameStep.h"
#include "ToggleDigitalDisplay.h"
#include "ToggleDesignatorRange.h"
#include "SaveVariablesToFile.h"
#include "SelectedInfo.h"
#include "HerosInfo.h"
#include "FPSNewCounter.h"
#include "AutoBuilding.h"
#include "DistributionMode.h"
#include "ShowCurrentInfo.h"
#include "ManualReloadAmmo.h"
#include "ToggleSWSidebar.h"
#include "FireTacticalSW.h"

#include <CCINIClass.h>
#include <InputManagerClass.h>
#include <WWMouseClass.h>

#include <Utilities/Macro.h>
#include <Ext/Sidebar/SWSidebar/SWSidebarClass.h>

#include <CCINIClass.h>
#include <InputManagerClass.h>
#include <MouseClass.h>
#include <WWMouseClass.h>

#include <Utilities/Macro.h>

DEFINE_HOOK(0x533066, CommandClassCallback_Register, 0x6)
{
	// Load it after Ares'

	MakeCommand<NextIdleHarvesterCommandClass>();
	MakeCommand<QuickSaveCommandClass>();
	MakeCommand<ToggleDigitalDisplayCommandClass>();
	MakeCommand<ToggleDesignatorRangeCommandClass>();
	MakeCommand<SelectedInfoCommandClass>();
	MakeCommand<HerosInfoCommandClass>();
	MakeCommand<FPSNewCounterCommandClass>();
	MakeCommand<AutoBuildingCommandClass>();

	if (Phobos::Config::AllowDistributionCommand)
	{
		MakeCommand<DistributionMode1CommandClass>();
		MakeCommand<DistributionMode2CommandClass>();
		MakeCommand<DistributionMode3CommandClass>();
	}

	MakeCommand<ManualReloadAmmoCommandClass>();
	MakeCommand<ToggleSWSidebar>();

	SWSidebarClass::Commands[0] = MakeCommand<FireTacticalSWCommandClass<0>>();
	SWSidebarClass::Commands[1] = MakeCommand<FireTacticalSWCommandClass<1>>();
	SWSidebarClass::Commands[2] = MakeCommand<FireTacticalSWCommandClass<2>>();
	SWSidebarClass::Commands[3] = MakeCommand<FireTacticalSWCommandClass<3>>();
	SWSidebarClass::Commands[4] = MakeCommand<FireTacticalSWCommandClass<4>>();
	SWSidebarClass::Commands[5] = MakeCommand<FireTacticalSWCommandClass<5>>();
	SWSidebarClass::Commands[6] = MakeCommand<FireTacticalSWCommandClass<6>>();
	SWSidebarClass::Commands[7] = MakeCommand<FireTacticalSWCommandClass<7>>();
	SWSidebarClass::Commands[8] = MakeCommand<FireTacticalSWCommandClass<8>>();
	SWSidebarClass::Commands[9] = MakeCommand<FireTacticalSWCommandClass<9>>();

	if (Phobos::Config::DevelopmentCommands)
	{
		MakeCommand<DamageDisplayCommandClass>();
		MakeCommand<SaveVariablesToFileCommandClass>();
		MakeCommand<ObjectInfoCommandClass>();
		MakeCommand<ShowCurrentInfoCommandClass>();
		MakeCommand<FrameByFrameCommandClass>();
		MakeCommand<FrameStepCommandClass<1>>(); // Single step in
		MakeCommand<FrameStepCommandClass<5>>(); // Speed 1
		MakeCommand<FrameStepCommandClass<10>>(); // Speed 2
		MakeCommand<FrameStepCommandClass<15>>(); // Speed 3
		MakeCommand<FrameStepCommandClass<30>>(); // Speed 4
		MakeCommand<FrameStepCommandClass<60>>(); // Speed 5
	}

	return 0;
}

static void MouseWheelDownCommand()
{
//	Debug::LogAndMessage("[Frame: %d] Mouse Wheel Down", Unsorted::CurrentFrame());

//	SomeCommand->Execute(WWKey);
}

static void MouseWheelUpCommand()
{
//	Debug::LogAndMessage("[Frame: %d] Mouse Wheel Up", Unsorted::CurrentFrame());

//	SomeCommand->Execute(WWKey);
}

DEFINE_HOOK(0x777998, Game_WndProc_ScrollMouseWheel, 0x6)
{
	GET(const WPARAM, WParam, ECX);

	if (WParam & 0x80000000u)
		MouseWheelDownCommand();
	else
		MouseWheelUpCommand();

	return 0;
}

static inline bool CheckSkipScrollSidebar()
{
	return !Phobos::Config::ScrollSidebarStripWhenHoldAlt && InputManagerClass::Instance->IsForceMoveKeyPressed()
		|| !Phobos::Config::ScrollSidebarStripWhenHoldCtrl && InputManagerClass::Instance->IsForceFireKeyPressed()
		|| !Phobos::Config::ScrollSidebarStripWhenHoldShift && InputManagerClass::Instance->IsForceSelectKeyPressed()
		|| !Phobos::Config::ScrollSidebarStripInTactical && WWMouseClass::Instance->XY1.X < Make_Global<int>(0xB0CE30); // TacticalClass::view_bound.Width
}

DEFINE_HOOK(0x533F50, Game_ScrollSidebar_Skip, 0x5)
{
	enum { SkipScrollSidebar = 0x533FC3 };
	return CheckSkipScrollSidebar() ? SkipScrollSidebar : 0;
}
