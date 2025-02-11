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
#include "SelectCaptured.h"
#include "SelectedInfo.h"
#include "HerosInfo.h"
#include "FPSNewCounter.h"
#include "AutoBuilding.h"
#include "DistributionMode.h"
#include "ShowCurrentInfo.h"
#include "ManualReloadAmmo.h"
#include "ToggleSWSidebar.h"
#include "FireTacticalSW.h"
#include "AggressiveStance.h"
#include "AggressiveStance.h"

#include <CCINIClass.h>
#include <InputManagerClass.h>
#include <WWMouseClass.h>

#include <Utilities/Macro.h>
#include <Ext/Sidebar/SWSidebar/SWSidebarClass.h>

DEFINE_HOOK(0x533066, CommandClassCallback_Register, 0x6)
{
	// Load it after Ares'

	MakeCommand<NextIdleHarvesterCommandClass>();
	MakeCommand<QuickSaveCommandClass>();
	MakeCommand<ToggleDigitalDisplayCommandClass>();
	MakeCommand<ToggleDesignatorRangeCommandClass>();
	MakeCommand<SelectCapturedCommandClass>();
	MakeCommand<SelectedInfoCommandClass>();
	MakeCommand<HerosInfoCommandClass>();
	MakeCommand<FPSNewCounterCommandClass>();
	MakeCommand<AutoBuildingCommandClass>();
	MakeCommand<DistributionMode1CommandClass>();
	MakeCommand<DistributionMode2CommandClass>();
	MakeCommand<DistributionMode3CommandClass>();
	MakeCommand<ManualReloadAmmoCommandClass>();
	MakeCommand<AggressiveStanceClass>();
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

DEFINE_HOOK(0x777998, Game_WndProc_ScrollMouseWheel, 0x6)
{
	GET(const WPARAM, WParam, ECX);
/*
	if (WParam & 0x80000000u)
		Debug::LogAndMessage("[Frame: %d] Mouse Wheel Down", Unsorted::CurrentFrame());
	else
		Debug::LogAndMessage("[Frame: %d] Mouse Wheel Up", Unsorted::CurrentFrame());
*/
	return 0;
}

DEFINE_HOOK(0x533F50, Game_ScrollSidebar_Skip, 0x5)
{
	enum { SkipScrollSidebar = 0x533FC3 };

	if (!Phobos::Config::ScrollSidebarStripWhenHoldKey)
	{
		const auto pInput = InputManagerClass::Instance();

		if (pInput->IsForceFireKeyPressed() || pInput->IsForceMoveKeyPressed() || pInput->IsForceSelectKeyPressed())
			return SkipScrollSidebar;
	}

	if (!Phobos::Config::ScrollSidebarStripInTactical)
	{
		const auto pMouse = WWMouseClass::Instance();

		if (pMouse->XY1.X < Make_Global<int>(0xB0CE30)) // TacticalClass::view_bound.Width
			return SkipScrollSidebar;
	}

	return 0;
}
