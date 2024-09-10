#include <Utilities/Macro.h>
#include <Utilities/TemplateDef.h>
#include <WWMouseClass.h>

#include <MessageListClass.h>
#include <GameStrings.h>

// Test function for extra pressable buttons above tactical map
namespace MousePressHelper
{
	Point2D LastPosition = Point2D::Empty;
	int ButtonIndex = -1;
	bool PressedInButton = false;
	int CheckMouseOverButtons(const Point2D* pMousePosition, int mode);
	void PressDesignatedButton(int buttonIndex);
	void DrawSuperWeaponButtonCameo();
}

// Note 1:
// After long pressing the right button to move the in-game perspective,
// both the mouse lift action and the next press action will be invalid.
// Therefore, it is recommended to trigger the function when the button
// is lifted, just like in the vanilla game. - CrimRecya

// Note 2:
// If some of the functions are intended for some multiplayer gamemodes,
// I think they should be triggered through the EventClass. But in fact,
// there are too few existing events can be used that I don't know what
// universal functionality can be achieved. - CrimRecya

// TODO ActiveButtonClass::ButtonGroup::ButtonShape

// Functions
int MousePressHelper::CheckMouseOverButtons(const Point2D* pMousePosition, int mode)
{
	// mode == 0 -> Check Only

	if (!mode || mode == 1) // Left Press
	{
		// Button index 0-5
		if (pMousePosition->X < 60 && pMousePosition->X >= 0)
		{
			const int currentCounts = 6; // Link to house
			const int height = DSurface::Composite->GetHeight();
			Point2D position { 0, (height - 32 - 48 * currentCounts - 2 * (currentCounts - 1)) / 2 };

			for (int i = 0; i < currentCounts; ++i)
			{
				if (pMousePosition->Y < position.Y)
					break;

				position.Y += 48;

				if (pMousePosition->Y < position.Y)
					return i;

				position.Y += 2;
			}
		}

		// TODO New buttons (Start from index = 6)
	}
/* // Temporary have not use
	if (!mode || mode == 2) // Left Release
	{
		; // TODO New buttons (Start from index = 6)
	}

	if (!mode || mode == 3) // Right Press
	{
		; // TODO New buttons (Start from index = 6)
	}

	if (!mode || mode == 4) // Right Release
	{
		; // TODO New buttons (Start from index = 6)
	}
*/
	return -1;
}

void MousePressHelper::PressDesignatedButton(int buttonIndex)
{
	switch (buttonIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	{


		break;
	}
	default:
	{
		break;
	}
	}
}

void MousePressHelper::DrawSuperWeaponButtonCameo()
{
	const int currentCounts = 6; // Link to house
	const int height = DSurface::Composite->GetHeight();
	Point2D position { 0, (height - 32 - 48 * currentCounts - 2 * (currentCounts - 1)) / 2 };
	RectangleStruct rect { 0, 0, 60, position.Y + 48 };
	int recordHeight = -1;

	for (int i = 0; i < currentCounts; ++i, position.Y += 50, rect.Height += 50) // Button index 0-5
	{
		DSurface::Composite->DrawSHP(FileSystem::SIDEBAR_PAL, Make_Global<SHPStruct*>(0xB07BC0), 0, &position, &rect,
			BlitterFlags(0x401), 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);

		if (i == MousePressHelper::ButtonIndex)
			recordHeight = position.Y;
	}

	if (recordHeight >= 0)
	{
		position.Y = recordHeight;
		rect.Height = recordHeight + 48;
		RectangleStruct drawRect { 0, position.Y, 60, 48 };
		DSurface::Composite->DrawRectEx( &rect, &drawRect, Drawing::RGB_To_Int(Drawing::TooltipColor));
	}
}

// Hooks for trigger
DEFINE_HOOK(0x6931A5, ScrollClass_WindowsProcedure_LeftMouseButtonDown, 0x6)
{
	enum { SkipGameCode = 0x6931B4 };

	const Point2D mousePosition = WWMouseClass::Instance->XY1;

	if (Phobos::Config::DevelopmentCommands)
	{
		wchar_t text[0x80];
		swprintf_s(text, L"Left mouse button down. Position: %d , %d ", mousePosition.X, mousePosition.Y);
		MessageListClass::Instance->PrintMessage(text, 600);
	}

	const int buttonIndex = MousePressHelper::CheckMouseOverButtons(&mousePosition, 1);

	if (buttonIndex >= 0) // TODO Fire super weapons
	{
		MousePressHelper::PressedInButton = true;

		// Functions (Recommended)

		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x693268, ScrollClass_WindowsProcedure_LeftMouseButtonUp, 0x5)
{
	enum { SkipGameCode = 0x693276 };

	const Point2D mousePosition = WWMouseClass::Instance->XY1;

	if (Phobos::Config::DevelopmentCommands)
	{
		wchar_t text[0x80];
		swprintf_s(text, L"Left mouse button up. Position: %d , %d ", mousePosition.X, mousePosition.Y);
		MessageListClass::Instance->PrintMessage(text, 600);
	}

	if (MousePressHelper::PressedInButton)
	{
		MousePressHelper::PressedInButton = false;
		const int buttonIndex = MousePressHelper::CheckMouseOverButtons(&mousePosition, 2);

		if (buttonIndex >= 0)
		{
			; // Functions (Not recommended)
		}

		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x69330E, ScrollClass_WindowsProcedure_RightMouseButtonDown, 0x6)
{
	enum { SkipGameCode = 0x69334A };

	const Point2D mousePosition = WWMouseClass::Instance->XY1;

	if (Phobos::Config::DevelopmentCommands)
	{
		wchar_t text[0x80];
		swprintf_s(text, L"Right mouse button down. Position: %d , %d ", mousePosition.X, mousePosition.Y);
		MessageListClass::Instance->PrintMessage(text, 600);
	}

	const int buttonIndex = MousePressHelper::CheckMouseOverButtons(&mousePosition, 3);

	if (buttonIndex >= 0)
	{
		MousePressHelper::PressedInButton = true;

		// Functions (Recommended)

		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x693397, ScrollClass_WindowsProcedure_RightMouseButtonUp, 0x6)
{
	enum { SkipGameCode = 0x6933CB };

	const Point2D mousePosition = WWMouseClass::Instance->XY1;

	if (Phobos::Config::DevelopmentCommands)
	{
		wchar_t text[0x80];
		swprintf_s(text, L"Right mouse button up. Position: %d , %d ", mousePosition.X, mousePosition.Y);
		MessageListClass::Instance->PrintMessage(text, 600);
	}

	if (MousePressHelper::PressedInButton)
	{
		MousePressHelper::PressedInButton = false;
		const int buttonIndex = MousePressHelper::CheckMouseOverButtons(&mousePosition, 4);

		if (buttonIndex >= 0)
		{
			; // Functions (Not recommended)
		}

		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x692F85, ScrollClass_MouseUpdate_MouseLongPress, 0x7)
{
	enum { ContinueGameCode = 0x692F8E, SkipGameCode = 0x692FDC };

	GET(ScrollClass*, pThis, EBX);

	// 555A: AnyMouseButtonDown
	return (pThis->unknown_byte_554A && !MousePressHelper::PressedInButton) ? ContinueGameCode : SkipGameCode;
}

// Hooks for suspend
DEFINE_HOOK(0x69300B, ScrollClass_MouseUpdate_MouseMoving, 0x6)
{
	enum { SkipGameCode = 0x69301A };

	const Point2D mousePosition = WWMouseClass::Instance->XY1;

	if (MousePressHelper::LastPosition != mousePosition)
	{
		MousePressHelper::LastPosition = mousePosition;
		const int buttonIndex = MousePressHelper::CheckMouseOverButtons(&mousePosition, 0);

		if (buttonIndex >= 0)
		{
			MousePressHelper::ButtonIndex = buttonIndex;
			R->EAX(Action::None);
			return SkipGameCode;
		}

		MousePressHelper::ButtonIndex = -1;
	}

	return 0;
}

// Hooks for display
DEFINE_HOOK(0x6D4941, TacticalClass_Render_DrawButtonCameo, 0x6)
{
	MousePressHelper::DrawSuperWeaponButtonCameo();
	// TODO New buttons (Start from index = 6)

	return 0;
}
