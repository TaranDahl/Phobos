#include <CCINIClass.h>
#include <Helpers/Macro.h>
#include "FootClass.h"
#include "Commands.h"

#include <Ext/Rules/Body.h>

#include "ObjectInfo.h"
#include "NextIdleHarvester.h"
#include "QuickSave.h"
#include "DamageDisplay.h"
#include "FrameByFrame.h"
#include "FrameStep.h"
#include "ToggleDigitalDisplay.h"
#include "ToggleDesignatorRange.h"
#include "SaveVariablesToFile.h"

DEFINE_HOOK(0x533066, CommandClassCallback_Register, 0x6)
{
	// Load it after Ares'

	MakeCommand<NextIdleHarvesterCommandClass>();
	MakeCommand<QuickSaveCommandClass>();
	MakeCommand<ToggleDigitalDisplayCommandClass>();
	MakeCommand<ToggleDesignatorRangeCommandClass>();

	if (Phobos::Config::DevelopmentCommands)
	{
		MakeCommand<DamageDisplayCommandClass>();
		MakeCommand<SaveVariablesToFileCommandClass>();
		MakeCommand<ObjectInfoCommandClass>();
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

// Causing crash. No idea about the reason for now.
/*
DEFINE_HOOK(0x730DE6, GuardCommandClass_Execute_InvertGuardCommand, 0x5)
{
	GET(FootClass*, pFoot, ESI);
	GET(int, pCell, EAX);

	if (RulesExt::Global()->InvertGuardCommand)
	{
		bool result = false;

		if (pFoot->GetTechnoType()->DefaultToGuardArea)
		{
			result = pFoot->ClickedMission(Mission::Guard, (ObjectClass*)pCell, nullptr, nullptr);
		}
		else
		{
			result = pFoot->ClickedMission(Mission::Area_Guard, (ObjectClass*)pCell, nullptr, nullptr);
		}

		R->AL(result);
		return 0x730DF1;
	}

	return 0;
}*/
