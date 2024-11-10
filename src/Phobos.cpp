#include "Phobos.h"

#include <Drawing.h>
#include <FPSCounter.h>
#include <GameOptionsClass.h>

#include <Utilities/Debug.h>
#include <Utilities/Patch.h>
#include <Utilities/Macro.h>

#include "Utilities/AresHelper.h"

#ifndef IS_RELEASE_VER
bool HideWarning = false;
#endif

HANDLE Phobos::hInstance = 0;

char Phobos::readBuffer[Phobos::readLength];
wchar_t Phobos::wideBuffer[Phobos::readLength];

const char* Phobos::AppIconPath = nullptr;

bool Phobos::DisplayDamageNumbers = false;
bool Phobos::IsLoadingSaveGame = false;

#ifdef STR_GIT_COMMIT
const wchar_t* Phobos::VersionDescription = L"Phobos nightly build (" STR_GIT_COMMIT L" @ " STR_GIT_BRANCH L"). DO NOT SHIP IN MODS!";
#elif !defined(IS_RELEASE_VER)
const wchar_t* Phobos::VersionDescription = L" Phobos special merge build #" _STR(BUILD_NUMBER) L"+" _STR(MERGE_NUMBER) L"_" _STR(MERGE_PATCH) L". Please test the build before shipping.";
#else
//const wchar_t* Phobos::VersionDescription = L"Phobos release build v" FILE_VERSION_STR L".";
#endif


void Phobos::CmdLineParse(char** ppArgs, int nNumArgs)
{
	bool foundInheritance = false;
	bool foundInclude = false;

	// > 1 because the exe path itself counts as an argument, too!
	for (int i = 1; i < nNumArgs; i++)
	{
		const char* pArg = ppArgs[i];

		if (_stricmp(pArg, "-Icon") == 0)
		{
			Phobos::AppIconPath = ppArgs[++i];
		}
#ifndef IS_RELEASE_VER
		if (_stricmp(pArg, "-b=" _STR(BUILD_NUMBER)) == 0)
		{
			HideWarning = true;
		}
#endif
		if (_stricmp(pArg, "-Inheritance") == 0)
		{
			foundInheritance = true;
		}
		if (_stricmp(pArg, "-Include") == 0)
		{
			foundInclude = true;
		}
	}

	if (foundInclude)
	{
		Patch::Apply_RAW(0x474200, // Apply CCINIClass_ReadCCFile1_DisableAres
			{ 0x8B, 0xF1, 0x8D, 0x54, 0x24, 0x0C }
		);

		Patch::Apply_RAW(0x474314, // Apply CCINIClass_ReadCCFile2_DisableAres
			{ 0x81, 0xC4, 0xA8, 0x00, 0x00, 0x00 }
		);
	}
	else
	{
		Patch::Apply_RAW(0x474230, // Revert CCINIClass_Load_Inheritance
			{ 0x8B, 0xE8, 0x88, 0x5E, 0x40 }
		);
	}

	if (foundInheritance)
	{
		Patch::Apply_RAW(0x528A10, // Apply INIClass_GetString_DisableAres
			{ 0x83, 0xEC, 0x0C, 0x33, 0xC0 }
		);

		Patch::Apply_RAW(0x526CC0, // Apply INIClass_GetKeyName_DisableAres
			{ 0x8B, 0x54, 0x24, 0x04, 0x83, 0xEC, 0x0C }
		);
	}
	else
	{
		Patch::Apply_RAW(0x528BAC, // Revert INIClass_GetString_Inheritance_NoEntry
			{ 0x8B, 0x7C, 0x24, 0x2C, 0x33, 0xC0, 0x8B, 0x4C, 0x24, 0x28 }
		);
	}

	Debug::Log("Initialized version: " PRODUCT_VERSION "\n");
}

void Phobos::ExeRun()
{
	Patch::ApplyStatic();

#ifdef DEBUG

	if (Phobos::DetachFromDebugger())
	{
		MessageBoxW(NULL,
		L"You can now attach a debugger.\n\n"

		L"Press OK to continue YR execution.",
		L"Debugger Notice", MB_OK);
	}
	else
	{
		MessageBoxW(NULL,
		L"You can now attach a debugger.\n\n"

		L"To attach a debugger find the YR process in Process Hacker "
		L"/ Visual Studio processes window and detach debuggers from it, "
		L"then you can attach your own debugger. After this you should "
		L"terminate Syringe.exe because it won't automatically exit when YR is closed.\n\n"

		L"Press OK to continue YR execution.",
		L"Debugger Notice", MB_OK);
	}

	if (!Console::Create())
	{
		MessageBoxW(NULL,
		L"Failed to allocate the debug console!",
		L"Debug Console Notice", MB_OK);
	}

#endif
}

void Phobos::ExeTerminate()
{
	Console::Release();
}

// =============================
// hooks

bool __stdcall DllMain(HANDLE hInstance, DWORD dwReason, LPVOID v)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Phobos::hInstance = hInstance;
	}
	return true;
}

DEFINE_HOOK(0x7CD810, ExeRun, 0x9)
{
	Phobos::ExeRun();
	AresHelper::Init();

	return 0;
}
// Avoid confusing the profiler unless really necessary
#ifdef DEBUG
DEFINE_NAKED_HOOK(0x7CD8EA, _ExeTerminate)
{
	// Call WinMain
	SET_REG32(EAX, 0x6BB9A0);
	CALL(EAX);
	PUSH_REG(EAX);

	__asm {call Phobos::ExeTerminate};

	// Jump back
	POP_REG(EAX);
	SET_REG32(EBX, 0x7CD8EF);
	__asm {jmp ebx};
}
#endif
DEFINE_HOOK(0x52F639, _YR_CmdLineParse, 0x5)
{
	GET(char**, ppArgs, ESI);
	GET(int, nNumArgs, EDI);

	Phobos::CmdLineParse(ppArgs, nNumArgs);
	Debug::LogDeferredFinalize();
	return 0;
}

DEFINE_HOOK(0x67E44D, LoadGame_SetFlag, 0x5)
{
	Phobos::IsLoadingSaveGame = true;
	return 0;
}

DEFINE_HOOK(0x67E68A, LoadGame_UnsetFlag, 0x5)
{
	Phobos::IsLoadingSaveGame = false;
	return 0;
}

#ifndef IS_RELEASE_VER
DEFINE_HOOK(0x4F4583, GScreenClass_DrawText, 0x6)
{
#ifndef STR_GIT_COMMIT
	if (!HideWarning)
#endif // !STR_GIT_COMMIT
	{
		const auto fps = FPSCounter::CurrentFrameRate.get();
		wchar_t fpsBuffer[0x20];
		swprintf_s(fpsBuffer, L"FPS: %-4u", fps);
		RectangleStruct fpsDim = Drawing::GetTextDimensions(fpsBuffer, Point2D::Empty, 0, 2, 0);

		const auto avg = FPSCounter::GetAverageFrameRate();
		wchar_t avgBuffer[0x20];
		swprintf_s(avgBuffer, L"Avg: %.2f", avg);
		RectangleStruct avgDim = Drawing::GetTextDimensions(avgBuffer, Point2D::Empty, 0, 2, 0);

		RectangleStruct verDim = Drawing::GetTextDimensions(Phobos::VersionDescription, Point2D::Empty, 0, 2, 0);

		const auto width = verDim.Width + avgDim.Width + fpsDim.Width + 20;
		RectangleStruct rect
		{
			(DSurface::Composite->GetWidth() - width),
			0,
			width,
			(std::max(verDim.Height, fpsDim.Height) + 10)
		};

		ColorStruct fillColor { 0, 0, 0 };
		DSurface::Composite->FillRectTrans(&rect, &fillColor, 40);
		DSurface::Composite->DrawRect(&rect, 0x061C);

		COLORREF color = 0x67EC;
		const auto gameSpeed = GameOptionsClass::Instance->GameSpeed;

		if (!gameSpeed || fps < (60 / gameSpeed))
		{
			if (gameSpeed < 10)
				color = 0xF986;
			else if (gameSpeed < 20)
				color = 0xFC05;
			else if (gameSpeed < 30)
				color = 0xFCE5;
			else if (gameSpeed < 40)
				color = 0xFFEC;
			else if (gameSpeed < 50)
				color = 0x9FEC;
			else if (gameSpeed < 60)
				color = 0x67EC;
		}

		auto location = Point2D { (rect.X + 5), 5 };
		DSurface::Composite->DrawText(fpsBuffer, &location, color);

		location.X += (fpsDim.Width + 5);
		DSurface::Composite->DrawText(avgBuffer, &location, COLOR_WHITE);

		location.X += (avgDim.Width + 5);
		DSurface::Composite->DrawText(Phobos::VersionDescription, &location, 0x061C);
	}
	return 0;
}
#endif
