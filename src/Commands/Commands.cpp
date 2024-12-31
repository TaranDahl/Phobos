#include <CCINIClass.h>
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
#include "SWShortcuts.h"
#include "SelectedInfo.h"
#include "HerosInfo.h"
#include "FPSNewCounter.h"
#include "DistributionMode.h"
#include "ShowCurrentInfo.h"

DEFINE_HOOK(0x533066, CommandClassCallback_Register, 0x6)
{
	// Load it after Ares'

	MakeCommand<NextIdleHarvesterCommandClass>();
	MakeCommand<QuickSaveCommandClass>();
	MakeCommand<ToggleDigitalDisplayCommandClass>();
	MakeCommand<ToggleDesignatorRangeCommandClass>();
	MakeCommand<SWShortcutsCommandClass<0>>();
	MakeCommand<SWShortcutsCommandClass<1>>();
	MakeCommand<SWShortcutsCommandClass<2>>();
	MakeCommand<SWShortcutsCommandClass<3>>();
	MakeCommand<SWShortcutsCommandClass<4>>();
	MakeCommand<SWShortcutsCommandClass<5>>();
	MakeCommand<SWShortcutsCommandClass<6>>();
	MakeCommand<SWShortcutsCommandClass<7>>();
	MakeCommand<SWShortcutsCommandClass<8>>();
	MakeCommand<SWShortcutsCommandClass<9>>();
	MakeCommand<SWShortcutsCommandClass<10>>();
	MakeCommand<SelectedInfoCommandClass>();
	MakeCommand<HerosInfoCommandClass>();
	MakeCommand<FPSNewCounterCommandClass>();
	MakeCommand<DistributionMode1CommandClass>();
	MakeCommand<DistributionMode2CommandClass>();

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
