#include "TacticalButtons.h"
#include "PhobosToolTip.h"

#include <GameOptionsClass.h>
#include <EventClass.h>
#include <SuperClass.h>
#include <AircraftClass.h>
#include <MessageListClass.h>
#include <TacticalClass.h>
#include <WWMouseClass.h>
#include <CCToolTip.h>
#include <InputManagerClass.h>
#include <FPSCounter.h>

#include <Ext/Side/Body.h>
#include <Ext/House/Body.h>
#include <Ext/SWType/Body.h>
#include <Ext/Scenario/Body.h>
#include <Utilities/TemplateDef.h>

#include <sstream>
#include <iomanip>

TacticalButtonsClass TacticalButtonsClass::Instance;

/*
// Keyboard code dictionary
static PhobosMap<int , const wchar_t*> CreateKeyboardCodeTextMap()
{
	PhobosMap<int , const wchar_t*> Code2Text;

	Code2Text[0x00] = L"  ";
	Code2Text[0x01] = L"MouseLeft";
	Code2Text[0x02] = L"MouseRight";
	Code2Text[0x03] = L"Cancel";
	Code2Text[0x04] = L"MouseCenter";

	Code2Text[0x08] = L"Back";
	Code2Text[0x09] = L"Tab";

	Code2Text[0x0C] = L"Clear";
	Code2Text[0x0D] = L"Enter";

	Code2Text[0x10] = L"Shift";
	Code2Text[0x11] = L"Ctrl";
	Code2Text[0x12] = L"Alt";
	Code2Text[0x13] = L"Pause";
	Code2Text[0x14] = L"CapsLock";

	Code2Text[0x1B] = L"Esc";

	Code2Text[0x20] = L"Space";
	Code2Text[0x21] = L"PageUp";
	Code2Text[0x22] = L"PageDown";
	Code2Text[0x23] = L"End";
	Code2Text[0x24] = L"Home";
	Code2Text[0x25] = L"Left";
	Code2Text[0x26] = L"Up";
	Code2Text[0x27] = L"Right";
	Code2Text[0x28] = L"Down";
	Code2Text[0x29] = L"Select";
	Code2Text[0x2A] = L"Print";
	Code2Text[0x2B] = L"Execute";
	Code2Text[0x2C] = L"PrintScreen";
	Code2Text[0x2D] = L"Insert";
	Code2Text[0x2E] = L"Delete";
	Code2Text[0x2F] = L"Help";
	Code2Text[0x30] = L"0";
	Code2Text[0x31] = L"1";
	Code2Text[0x32] = L"2";
	Code2Text[0x33] = L"3";
	Code2Text[0x34] = L"4";
	Code2Text[0x35] = L"5";
	Code2Text[0x36] = L"6";
	Code2Text[0x37] = L"7";
	Code2Text[0x38] = L"8";
	Code2Text[0x39] = L"9";

	Code2Text[0x41] = L"A";
	Code2Text[0x42] = L"B";
	Code2Text[0x43] = L"C";
	Code2Text[0x44] = L"D";
	Code2Text[0x45] = L"E";
	Code2Text[0x46] = L"F";
	Code2Text[0x47] = L"G";
	Code2Text[0x48] = L"H";
	Code2Text[0x49] = L"I";
	Code2Text[0x4A] = L"J";
	Code2Text[0x4B] = L"K";
	Code2Text[0x4C] = L"L";
	Code2Text[0x4D] = L"M";
	Code2Text[0x4E] = L"N";
	Code2Text[0x4F] = L"O";
	Code2Text[0x50] = L"P";
	Code2Text[0x51] = L"Q";
	Code2Text[0x52] = L"R";
	Code2Text[0x53] = L"S";
	Code2Text[0x54] = L"T";
	Code2Text[0x55] = L"U";
	Code2Text[0x56] = L"V";
	Code2Text[0x57] = L"W";
	Code2Text[0x58] = L"X";
	Code2Text[0x59] = L"Y";
	Code2Text[0x5A] = L"Z";
	Code2Text[0x5B] = L"LWin";
	Code2Text[0x5C] = L"RWin";
	Code2Text[0x5D] = L"Menu";

	Code2Text[0x60] = L"Num0";
	Code2Text[0x61] = L"Num1";
	Code2Text[0x62] = L"Num2";
	Code2Text[0x63] = L"Num3";
	Code2Text[0x64] = L"Num4";
	Code2Text[0x65] = L"Num5";
	Code2Text[0x66] = L"Num6";
	Code2Text[0x67] = L"Num7";
	Code2Text[0x68] = L"Num8";
	Code2Text[0x69] = L"Num9";
	Code2Text[0x6A] = L"Num*";
	Code2Text[0x6B] = L"Num+";
	Code2Text[0x6C] = L"Separator";
	Code2Text[0x6D] = L"Num-";
	Code2Text[0x6E] = L"Num.";
	Code2Text[0x6F] = L"Num/";
	Code2Text[0x70] = L"F1";
	Code2Text[0x71] = L"F2";
	Code2Text[0x72] = L"F3";
	Code2Text[0x73] = L"F4";
	Code2Text[0x74] = L"F5";
	Code2Text[0x75] = L"F6";
	Code2Text[0x76] = L"F7";
	Code2Text[0x77] = L"F8";
	Code2Text[0x78] = L"F9";
	Code2Text[0x79] = L"F10";
	Code2Text[0x7A] = L"F11";
	Code2Text[0x7B] = L"F12";

	Code2Text[0x90] = L"NumLock";
	Code2Text[0x91] = L"ScrollLock";

	Code2Text[0xBA] = L";";
	Code2Text[0xBB] = L"=";
	Code2Text[0xBC] = L",";
	Code2Text[0xBD] = L"-";
	Code2Text[0xBE] = L".";
	Code2Text[0xBF] = L"/";
	Code2Text[0xC0] = L"`";

	Code2Text[0xDB] = L"[";
	Code2Text[0xDC] = L"\\";
	Code2Text[0xDD] = L"]";
	Code2Text[0xDE] = L"'";

	Code2Text[static_cast<int>(WWKey::Shift)] = L"Shift";
	Code2Text[static_cast<int>(WWKey::Ctrl)] = L"Ctrl";
	Code2Text[static_cast<int>(WWKey::Alt)] = L"Alt";

	return Code2Text;
}

PhobosMap<int , const wchar_t*> TacticalButtonsClass::KeyboardCodeTextMap = CreateKeyboardCodeTextMap();
*/

// Functions

// Private functions
int TacticalButtonsClass::CheckMouseOverButtons(const Point2D* pMousePosition)
{
	if (Phobos::Config::MessageDisplayInCenter)
	{
		if (this->MouseIsOverMessageLists(pMousePosition))
		{
			if (!Make_Global<int>(0xABCD40)) // Frame index in this second
			{
				for (auto pText = MessageListClass::Instance->MessageList; pText; pText = static_cast<TextLabelClass*>(pText->GetNext()))
					pText->UserData1 = reinterpret_cast<void*>(reinterpret_cast<int>(pText->UserData1) + 60);
			}

			this->OnMessages = true;
		}
		else
		{
			this->OnMessages = false;
		}
	}

	if (const int currentCounts = ScenarioExt::Global()->SWButtonData.size()) // Button index 1-11
	{
		const int height = DSurface::Composite->GetHeight() - 32;

		if (this->SuperVisible && pMousePosition->X < 65 && pMousePosition->X >= 5) // Button index 1-10 : Super weapons buttons
		{
			int checkHight = (height - 48 * currentCounts - 2 * (currentCounts - 1)) / 2;

			for (int i = 0; i < currentCounts; ++i)
			{
				if (pMousePosition->Y < checkHight)
					break;

				checkHight += 48;

				if (pMousePosition->Y < checkHight)
					return i + 1;

				checkHight += 2;
			}
		}

		if (RulesExt::Global()->SWSidebarBackground) // Button index 11 : SW sidebar switch
		{
			if (this->SuperVisible ? (pMousePosition->X < 90 && pMousePosition->X >= 80) : (pMousePosition->X < 10 && pMousePosition->X >= 0))
			{
				const int checkHight = height / 2;

				if (pMousePosition->Y < checkHight + 25 && pMousePosition->Y >= checkHight - 25)
					return 11;
			}
		}
	}

	// TODO New buttons

	if (this->HeroVisible) // Button index 61-68 : Heros buttons
	{
		auto& vec = ScenarioExt::Global()->OwnedHeros;

		if (const int counts = Math::min(8, static_cast<int>(vec.size())))
		{
			const int width = DSurface::Composite->GetWidth() - 65;

			if (pMousePosition->X >= width && pMousePosition->X < (width + 60))
			{
				int checkHight = 35;

				for (int i = 0; i < counts; ++i)
				{
					if (pMousePosition->Y < checkHight)
						break;

					checkHight += 48;

					if (pMousePosition->Y < checkHight)
						return i + 61;

					checkHight += 2;
				}
			}
		}
	}

	if (Phobos::Config::SelectedDisplay_Enable) // Button index 71-100 : Select buttons
	{
		if (const int currentCounts = Math::min(30, static_cast<int>(this->CurrentSelectCameo.size())))
		{
			const int height = DSurface::Composite->GetHeight() - 80;

			if (pMousePosition->Y >= (this->IndexInSelectButtons() ? (height - 10) : height) && pMousePosition->Y < (height + 48))
			{
				const int gap = Math::min(60, ((DSurface::Composite->GetWidth() << 1) / (5 * currentCounts)));

				if (pMousePosition->X < (gap * (currentCounts - 1) + 60))
				{
					const int index = currentCounts + 71;
					int checkWidth = 0;

					for (int i = 71; i < index; ++i)
					{
						if (this->RecordIndex == i)
						{
							checkWidth += 60;

							if (this->ButtonIndex == this->RecordIndex)
							{
								if (pMousePosition->X < checkWidth)
									return i;

								break;
							}
							else if (pMousePosition->X < checkWidth && pMousePosition->Y >= height)
							{
								return i;
							}
						}
						else
						{
							checkWidth += gap;

							if (pMousePosition->X < checkWidth)
							{
								if (pMousePosition->Y >= height)
									return i;
								else
									break;
							}
						}
					}
				}
			}
		}
	}

	if (this->CheckMouseOverBackground(pMousePosition))
		return 0; // Button index 0 : Background

	return -1;
}

bool TacticalButtonsClass::CheckMouseOverBackground(const Point2D* pMousePosition)
{
	if (RulesExt::Global()->SWSidebarBackground && this->SuperVisible)
	{
		if (const int currentCounts = ScenarioExt::Global()->SWButtonData.size())
		{
			if (pMousePosition->X < 80 && pMousePosition->X >= 0)
			{
				const int height = DSurface::Composite->GetHeight() - 32;
				const int checkHight = (height - 48 * currentCounts - 2 * (currentCounts - 1)) / 2 - 21;

				if (pMousePosition->Y >= checkHight && pMousePosition->Y < (checkHight + currentCounts * 50 + 40))
					return true;
			}
		}
	}

	// TODO New button backgrounds

	if (Phobos::Config::SelectedDisplay_Enable)
	{
		const int currentCounts = this->CurrentSelectCameo.size();

		if (currentCounts == 1 && this->CurrentSelectCameo[0].Count == 1)
		{
			if (pMousePosition->X < 180 && pMousePosition->X >= 0)
			{
				const int height = DSurface::Composite->GetHeight() - 80;

				if (pMousePosition->Y >= height && pMousePosition->Y < (height + 48))
					return true;
			}
		}
	}

	return false;
}

// Inline functions
inline bool TacticalButtonsClass::MouseIsOverButtons()
{
	return this->ButtonIndex > 0;
}

inline bool TacticalButtonsClass::MouseIsOverTactical()
{
	return this->ButtonIndex < 0;
}

// Cite functions
int TacticalButtonsClass::GetButtonIndex()
{
	return this->ButtonIndex;
}

// General functions
void TacticalButtonsClass::SetMouseButtonIndex(const Point2D* pMousePosition)
{
	this->ButtonIndex = this->CheckMouseOverButtons(pMousePosition);

	const auto pToolTips = CCToolTip::Instance();

	// SW ToolTip
	if (this->IndexInSWButtons()) // Button index 1-10 : Super weapons buttons
	{
		const int superIndex = ScenarioExt::Global()->SWButtonData[this->ButtonIndex - 1];

		if (superIndex != -1 && superIndex != this->RecordSuperIndex)
		{
			PhobosToolTip::Instance.HelpText_Super(superIndex);
			this->RecordSuperIndex = superIndex;

			if (pToolTips->ToolTipDelay)
				pToolTips->LastToolTipDelay = pToolTips->ToolTipDelay;

			pToolTips->ToolTipDelay = 0;
		}
	}
	else if (this->RecordSuperIndex != -1)
	{
		this->RecordSuperIndex = -1;
		pToolTips->ToolTipDelay = pToolTips->LastToolTipDelay;
	}

	// TODO New buttons

	// Select ToolTip And Button Record
	if (this->IndexInSelectButtons()) // Button index 71-100 : Select buttons
	{
		this->RecordIndex = this->ButtonIndex;
		const int currentCounts = this->CurrentSelectCameo.size();

		if (currentCounts > 1 || (currentCounts && this->CurrentSelectCameo[0].Count > 1))
		{
			if (TechnoTypeClass* const pType = this->CurrentSelectCameo[this->ButtonIndex - 71].TypeExt->OwnerObject())
			{
				const wchar_t* name = pType->UIName;

				if (name && name != this->HoveredSelected)
					this->HoveredSelected = name;
			}
		}
		else if (this->HoveredSelected)
		{
			this->HoveredSelected = nullptr;
		}
	}
	else if (this->HoveredSelected)
	{
		this->HoveredSelected = nullptr;
	}
}

void TacticalButtonsClass::PressDesignatedButton(int triggerIndex)
{
	if (!this->MouseIsOverButtons()) // In buttons background
		return;

	if (this->IndexInSWButtons()) // Button index 1-10 : Super weapons buttons
	{
		if (!triggerIndex)
			this->SWSidebarTrigger(this->ButtonIndex);
		else if (triggerIndex == 2)
			DisplayClass::Instance->CurrentSWTypeIndex = -1;

		const auto pToolTips = CCToolTip::Instance();
		this->RecordSuperIndex = -1;
		pToolTips->ToolTipDelay = pToolTips->LastToolTipDelay;
	}
	else if (this->IndexIsSWSwitch())
	{
		if (!triggerIndex)
			this->SWSidebarSwitch();
	}
/*	else if (?) // TODO New buttons
	{
		;
	}*/
	else if (this->IndexInHerosButtons())
	{
		if (!triggerIndex)
			this->HeroSelect(this->ButtonIndex);
	}
	else if (this->IndexInSelectButtons())
	{
		if (!triggerIndex)
			this->SelectedTrigger(this->ButtonIndex, true);
		else if (triggerIndex == 2)
			this->SelectedTrigger(this->ButtonIndex, false);
	}
}

// Button index N/A : Message Lists
bool TacticalButtonsClass::MouseIsOverMessageLists(const Point2D* pMousePosition)
{
	const auto pMessages = &MessageListClass::Instance();

	if (TextLabelClass* pText = pMessages->MessageList)
	{
		if (pMousePosition->Y >= pMessages->MessagePos.Y && pMousePosition->X >= pMessages->MessagePos.X && pMousePosition->X <= pMessages->MessagePos.X + pMessages->Width)
		{
			const int textHeight = pMessages->Height;
			int height = pMessages->MessagePos.Y;

			for (; pText; pText = static_cast<TextLabelClass*>(pText->GetNext()))
				height += textHeight;

			if (pMousePosition->Y < (height + 2))
				return true;
		}
	}

	return false;
}

// Button index N/A : FPS Counter
void TacticalButtonsClass::FPSCounterDraw()
{
	if (!Phobos::Config::FPSCounter_Enable)
		return;

	const auto fps = FPSCounter::CurrentFrameRate();
	wchar_t fpsBuffer[0x20];
	swprintf_s(fpsBuffer, L"FPS: %u", fps);
	RectangleStruct fpsDim = Drawing::GetTextDimensions(fpsBuffer, Point2D::Empty, 0);
	wchar_t avgBuffer[0x20];
	swprintf_s(avgBuffer, L"Avg: %.2lf", FPSCounter::GetAverageFrameRate());
	RectangleStruct avgDim = Drawing::GetTextDimensions(avgBuffer, Point2D::Empty, 0);

	const auto gameSpeed = GameOptionsClass::Instance->GameSpeed;
	COLORREF color = 0x67EC;

	if (!gameSpeed || fps < static_cast<unsigned int>(60 / gameSpeed))
	{
		if (fps < 10)
			color = 0xF986;
		else if (fps < 20)
			color = 0xFC05;
		else if (fps < 30)
			color = 0xFCE5;
		else if (fps < 45)
			color = 0xFFEC;
		else if (fps < 60)
			color = 0x9FEC;
	}

	const int trans = InputManagerClass::Instance->IsForceMoveKeyPressed() ? 80 : 40;
	const auto height = DSurface::Composite->GetHeight() - ((Phobos::Config::SelectedDisplay_Enable && this->CurrentSelectCameo.size()) ? 80 : 32);
	ColorStruct fillColor { 0, 0, 0 };
	RectangleStruct rect { 0, (height - avgDim.Height), avgDim.Width + 4, avgDim.Height };
	DSurface::Composite->FillRectTrans(&rect, &fillColor, trans);
	auto location = Point2D { 2, rect.Y };
	DSurface::Composite->DrawText(avgBuffer, &location, color);

	rect.Y -= fpsDim.Height;
	rect.Width = fpsDim.Width + 4;
	rect.Height = fpsDim.Height;
	DSurface::Composite->FillRectTrans(&rect, &fillColor, trans);
	location.Y = rect.Y;
	DSurface::Composite->DrawText(fpsBuffer, &location, color);
}

// Button index 1-10 : Super weapons buttons
inline bool TacticalButtonsClass::IndexInSWButtons()
{
	return this->ButtonIndex > 0 && this->ButtonIndex <= 10;
}

void TacticalButtonsClass::SWSidebarDraw()
{
	auto& data = ScenarioExt::Global()->SWButtonData;
	const int currentCounts = data.size();

	if (!currentCounts)
		return;

	const auto pHouse = HouseClass::CurrentPlayer();
	const auto pSideExt = SideExt::ExtMap.Find(SideClass::Array->GetItem(pHouse->SideIndex));
	const bool drawSWSidebarBackground = RulesExt::Global()->SWSidebarBackground && pSideExt;
	const int height = DSurface::Composite->GetHeight() - 32;
	const int color = Drawing::RGB_To_Int(Drawing::TooltipColor);

	// Draw switch
	if (this->SuperVisible)
	{
		if (drawSWSidebarBackground)
		{
			RectangleStruct drawRect { 80, (height / 2 - 25), 10, 50 };

			if (const auto CameoPCX = pSideExt->SWSidebarBackground_OnPCX.GetSurface())
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			else
				DSurface::Composite->FillRect(&drawRect, COLOR_BLUE);

			if (this->IndexIsSWSwitch())
			{
				RectangleStruct rect { 0, 0, 90, drawRect.Y + 50 };
				DSurface::Composite->DrawRectEx(&rect, &drawRect, color);
			}
		}
	}
	else
	{
		if (drawSWSidebarBackground)
		{
			RectangleStruct drawRect { 0, (height / 2 - 25), 10, 50 };

			if (const auto CameoPCX = pSideExt->SWSidebarBackground_OffPCX.GetSurface())
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			else
				DSurface::Composite->FillRect(&drawRect, COLOR_BLUE);

			if (this->IndexIsSWSwitch())
			{
				RectangleStruct rect { 0, 0, 10, drawRect.Y + 50 };
				DSurface::Composite->DrawRectEx(&rect, &drawRect, color);
			}
		}

		return;
	}

	Point2D position { 5, (height - 48 * currentCounts - 2 * (currentCounts - 1)) / 2 };
	RectangleStruct rect { 0, 0, 65, position.Y + 48 };
	int recordHeight = -1;

	// Draw top background (80 * 20)
	Point2D backPosition { 0, position.Y - 21 };

	if (drawSWSidebarBackground)
	{
		RectangleStruct drawRect { 0, backPosition.Y, 80, 20 };

		if (const auto CameoPCX = pSideExt->SWSidebarBackground_TopPCX.GetSurface())
			PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
		else
			DSurface::Composite->FillRect(&drawRect, COLOR_BLACK);
	}

	// Draw each buttons
	for (int i = 0; i < currentCounts; position.Y += 50, rect.Height += 50) // Button index 1-10
	{
		// Draw center background (80 * 50)
		if (drawSWSidebarBackground)
		{
			backPosition.Y = position.Y - 1;
			RectangleStruct drawRect { 0, backPosition.Y, 80, 50 };

			if (const auto CameoPCX = pSideExt->SWSidebarBackground_CenterPCX.GetSurface())
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			else
				DSurface::Composite->FillRect(&drawRect, COLOR_BLACK);
		}

		// Get SW data
		const auto pSuper = pHouse->Supers.Items[data[i]];
		const auto pSWType = pSuper->Type;
		bool canAfford = true;

		// Draw cameo
		if (const auto pTypeExt = SWTypeExt::ExtMap.Find(pSWType))
		{
			if (const auto CameoPCX = pTypeExt->SidebarPCX.GetSurface())
			{
				RectangleStruct drawRect { position.X, position.Y, 60, 48 };
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			}
			else if (const auto pSHP = pSWType->SidebarImage)
			{
				const auto pCameoRef = pSHP->AsReference();

				char pFilename[0x20];
				strcpy_s(pFilename, RulesExt::Global()->MissingCameo.data());
				_strlwr_s(pFilename);

				if (!_stricmp(pCameoRef->Filename, GameStrings::XXICON_SHP) && strstr(pFilename, ".pcx"))
				{
					PCX::Instance->LoadFile(pFilename);
					RectangleStruct drawRect { position.X, position.Y, 60, 48 };

					if (const auto MissingCameoPCX = PCX::Instance->GetSurface(pFilename))
						PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
				}
				else
				{
					DSurface::Composite->DrawSHP(pTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL), pSHP, 0, &position, &rect,
						BlitterFlags::bf_400, 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
				}
			}

			if (!pHouse->CanTransactMoney(pTypeExt->Money_Amount))
				canAfford = false;
		}

		const bool ready = !pSuper->IsSuspended && (pSWType->UseChargeDrain ? pSuper->ChargeDrainState == ChargeDrainState::Ready : pSuper->IsReady);

		// Flash or darken cameo
		if (ready && canAfford)
		{
			const int delay = pSWType->FlashSidebarTabFrames;

			if (delay > 0 && ((Unsorted::CurrentFrame - pSuper->ReadyFrame) % (delay << 1)) > delay)
			{
				DSurface::Composite->DrawSHP(FileSystem::SIDEBAR_PAL, Make_Global<SHPStruct*>(0xB07BC0), 0, &position, &rect,
					(BlitterFlags::bf_400 | BlitterFlags::TransLucent75), 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
			}
		}
		else if (!canAfford)
		{
			DSurface::Composite->DrawSHP(FileSystem::SIDEBAR_PAL, Make_Global<SHPStruct*>(0xB07BC0), 0, &position, &rect,
				(BlitterFlags::bf_400 | BlitterFlags::Darken), 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
		}

		// SW charge progress
		if (pSuper->ShouldDrawProgress())
		{
			const int frame = pSuper->AnimStage() + 1;

			DSurface::Composite->DrawSHP(FileSystem::SIDEBAR_PAL, Make_Global<SHPStruct*>(0xB0B484), frame, &position, &rect,
				BlitterFlags(0x404), 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
		}

		// SW status
		if (ready && !this->KeyCodeText[i].empty())
		{
			const auto pKey = this->KeyCodeText[i].c_str();
			Point2D textLocation { 35, position.Y + 1 };
			constexpr TextPrintType printType = TextPrintType::Center | TextPrintType::FullShadow | TextPrintType::Point8;
			auto textRect = Drawing::GetTextDimensions(pKey, textLocation, static_cast<WORD>(printType), 2, 1);

			// Text black background
			reinterpret_cast<void(__fastcall*)(RectangleStruct*, DSurface*, unsigned short, unsigned char)>(0x621B80)(&textRect, DSurface::Composite, 0, 0xAFu);
			DSurface::Composite->DrawTextA(pKey, &rect, &textLocation, static_cast<COLORREF>(color), COLOR_BLACK, printType);
		}
		else if (const wchar_t* pName = pSuper->NameReadiness())
		{
			Point2D textLocation { 35, position.Y + 1 };
			constexpr TextPrintType printType = TextPrintType::Center | TextPrintType::FullShadow | TextPrintType::Point8;
			auto textRect = Drawing::GetTextDimensions(pName, textLocation, static_cast<WORD>(printType), 2, 1);

			// Text black background
			reinterpret_cast<void(__fastcall*)(RectangleStruct*, DSurface*, unsigned short, unsigned char)>(0x621B80)(&textRect, DSurface::Composite, 0, 0xAFu);
			DSurface::Composite->DrawTextA(pName, &rect, &textLocation, static_cast<COLORREF>(color), COLOR_BLACK, printType);
		}

		if (++i == this->ButtonIndex)
			recordHeight = position.Y;
	}

	// Draw bottom background (80 * 20)
	if (drawSWSidebarBackground)
	{
		backPosition.Y = position.Y - 1;
		RectangleStruct drawRect { 0, backPosition.Y, 80, 20};

		if (const auto CameoPCX = pSideExt->SWSidebarBackground_BottomPCX.GetSurface())
			PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
		else
			DSurface::Composite->FillRect(&drawRect, COLOR_BLACK);
	}

	// Draw mouse hover rectangle
	if (!ScenarioClass::Instance->UserInputLocked && recordHeight >= 0)
	{
		position.Y = recordHeight;
		rect.Height = recordHeight + 48;

		RectangleStruct drawRect { 5, position.Y, 60, 48 };
		DSurface::Composite->DrawRectEx(&rect, &drawRect, color);
	}
}

void TacticalButtonsClass::SWSidebarRecheck()
{
	const auto pHouse = HouseClass::CurrentPlayer();
	auto& data = ScenarioExt::Global()->SWButtonData;

	for (auto it = data.begin(); it != data.end();)
	{
		const int superIndex = *it;

		if (superIndex >= pHouse->Supers.Count || !pHouse->Supers.Items[superIndex]->IsPresent)
			it = data.erase(it);
		else
			++it;
	}
}

bool TacticalButtonsClass::SWSidebarAdd(int& superIndex)
{
	const auto pType = SuperWeaponTypeClass::Array->Items[superIndex];
	const auto pTypeExt = SWTypeExt::ExtMap.Find(pType);
	bool overflow = true;

	if (pTypeExt && pTypeExt->SW_InScreen_Show)
	{
		const unsigned int ownerBits = 1u << HouseClass::CurrentPlayer->Type->ArrayIndex;

		if (pTypeExt->SW_InScreen_RequiredHouses & ownerBits)
		{
			auto& data = ScenarioExt::Global()->SWButtonData;
			const int currentCounts = data.size();
			bool move = false;

			for (int i = 0; i < 10; ++i) // 10 buttons at max
			{
				if (move)
				{
					if (i < currentCounts)
					{
						int buffer = data[i];
						data[i] = superIndex;
						superIndex = buffer;
					}
					else
					{
						data.push_back(superIndex);
						overflow = false;
						break;
					}
				}
				else if (i < currentCounts)
				{
					if (this->SWSidebarSort(SuperWeaponTypeClass::Array->Items[data[i]], pType, pTypeExt, ownerBits))
					{
						move = true;
						int buffer = data[i];
						data[i] = superIndex;
						superIndex = buffer;
					}
				}
				else
				{
					data.push_back(superIndex);
					overflow = false;
					break;
				}
			}
		}
	}

	return overflow;
}

bool TacticalButtonsClass::SWSidebarSort(SuperWeaponTypeClass* pDataType, SuperWeaponTypeClass* pAddType, SWTypeExt::ExtData* pAddTypeExt, unsigned int ownerBits)
{
	const auto pDataTypeExt = SWTypeExt::ExtMap.Find(pDataType);

	if (pDataTypeExt && pAddTypeExt)
	{
		if ((pDataTypeExt->SW_InScreen_PriorityHouses & ownerBits) && !(pAddTypeExt->SW_InScreen_PriorityHouses & ownerBits))
			return false;

		if (!(pDataTypeExt->SW_InScreen_PriorityHouses & ownerBits) && (pAddTypeExt->SW_InScreen_PriorityHouses & ownerBits))
			return true;

		if (pDataTypeExt->CameoPriority > pAddTypeExt->CameoPriority)
			return false;

		if (pDataTypeExt->CameoPriority < pAddTypeExt->CameoPriority)
			return true;
	}

	if (pDataType->RechargeTime < pAddType->RechargeTime)
		return false;

	if (pDataType->RechargeTime > pAddType->RechargeTime)
		return true;

	return wcscmp(pDataType->UIName, pAddType->UIName) > 0;
}

void TacticalButtonsClass::SWSidebarTrigger(int buttonIndex)
{
	if (ScenarioClass::Instance->UserInputLocked || !this->SuperVisible || HouseClass::CurrentPlayer->IsObserver())
		return;

	auto& data = ScenarioExt::Global()->SWButtonData;

	if (static_cast<size_t>(buttonIndex) > data.size())
		return;

	const auto pSidebar = SidebarClass::Instance();
	this->DummyAction = true;

	DummySelectClass pButton;
	pButton.LinkTo = &pSidebar->Tabs[pSidebar->ActiveTabIndex];
	pButton.unknown_int_30 = 0x7FFFFFFF - (2 * pButton.LinkTo->TopRowIndex);
	pButton.SWIndex = data[buttonIndex - 1];

	DWORD KeyNum = 0;
	reinterpret_cast<bool(__thiscall*)(DummySelectClass*, GadgetFlag, DWORD*, KeyModifier)>(0x6AAD00)(&pButton, GadgetFlag::LeftPress, &KeyNum, KeyModifier::None); // SelectClass_Action
}

void TacticalButtonsClass::SWSidebarRecord(int buttonIndex, int key)
{
	const int index = buttonIndex - 1;

	if (this->KeyCodeData[index] == key)
		return;

	this->KeyCodeData[index] = key;

	wchar_t text[0x40];
	reinterpret_cast<void(__fastcall*)(short, wchar_t*)>(0x61EF70)(static_cast<short>(key), text);

	this->KeyCodeText[index] = text;

/*	std::wostringstream oss;

	if (key & static_cast<int>(WWKey::Shift))
		oss << KeyboardCodeTextMap[static_cast<int>(WWKey::Shift)] << L"+";

	if (key & static_cast<int>(WWKey::Ctrl))
		oss << KeyboardCodeTextMap[static_cast<int>(WWKey::Ctrl)] << L"+";

	if (key & static_cast<int>(WWKey::Alt))
		oss << KeyboardCodeTextMap[static_cast<int>(WWKey::Alt)] << L"+";

	const int pureKey = key & 0xFF;

	if (KeyboardCodeTextMap.contains(pureKey))
		oss << KeyboardCodeTextMap[pureKey];
	else
		oss << L"Unknown";

	this->KeyCodeText[index] = oss.str();*/
}

// Button index 11 : SW sidebar switch
inline bool TacticalButtonsClass::IndexIsSWSwitch()
{
	return this->ButtonIndex == 11;
}

void TacticalButtonsClass::SWSidebarSwitch()
{
	if (ScenarioClass::Instance->UserInputLocked)
		return;

	const auto pHouse = HouseClass::CurrentPlayer();

	if (pHouse->IsObserver())
		return;

	this->SuperVisible = !this->SuperVisible;

	MessageListClass::Instance->PrintMessage
	(
		(this->SuperVisible ?
			GeneralUtils::LoadStringUnlessMissing("TXT_EX_SW_BAR_VISIBLE", L"Set exclusive SW sidebar visible.") :
			GeneralUtils::LoadStringUnlessMissing("TXT_EX_SW_BAR_INVISIBLE", L"Set exclusive SW sidebar invisible.")),
		RulesClass::Instance->MessageDelay,
		pHouse->ColorSchemeIndex,
		true
	);
}

// Extra functions for SW
bool TacticalButtonsClass::SWQuickLaunch(int superIndex)
{
	bool keyboardCall = false;

	if (this->KeyboardCall)
	{
		this->KeyboardCall = false;
		keyboardCall = true;
	}

	const auto pType = SuperWeaponTypeClass::Array->Items[superIndex];

	if (const auto pTypeExt = SWTypeExt::ExtMap.Find(pType))
	{
		const int index = HouseClass::CurrentPlayer->ArrayIndex;
		const auto pTactical = TacticalClass::Instance();

		if (pTypeExt->SW_QuickFireAtMouse && keyboardCall)
		{
			const auto mouseCoords = pTactical->ClientToCoords(WWMouseClass::Instance->XY1);

			if (mouseCoords != CoordStruct::Empty)
			{
				const EventClass event
				(
					index,
					EventType::SpecialPlace,
					pType->ArrayIndex,
					CellClass::Coord2Cell(mouseCoords)
				);
				EventClass::AddEvent(event);

				return true;
			}
		}
		else if (!pTypeExt->SW_QuickFireInScreen)
		{
			return false;
		}

		const EventClass event
		(
			index,
			EventType::SpecialPlace,
			pType->ArrayIndex,
			CellClass::Coord2Cell(pTactical->ClientToCoords(Point2D{ (DSurface::Composite->Width >> 1), (DSurface::Composite->Height >> 1) }))
		);
		EventClass::AddEvent(event);

		return true;
	}

	return false;
}

// Button index 61-68 : Heros buttons
inline bool TacticalButtonsClass::IndexInHerosButtons()
{
	return this->ButtonIndex > 60 && this->ButtonIndex <= 68;
}

void TacticalButtonsClass::HerosDraw()
{
	if (!this->HeroVisible || HouseClass::CurrentPlayer->IsObserver())
		return;

	auto& vec = ScenarioExt::Global()->OwnedHeros;
	const int size = vec.size();

	if (!size)
		return;

	Point2D position { DSurface::Composite->GetWidth() - 65, 35 };
	RectangleStruct drawRect { position.X, position.Y, 60, 48 };
	const int recordIndex = this->ButtonIndex - 61;
	int counts = Math::min(8, size);

	for (int i = 0; i < counts; ++i, position.Y += 50, drawRect.Y = position.Y)
	{
		const auto pExt = vec[i];
		const auto pTechno = pExt->OwnerObject();
		const auto pTypeExt = pExt->TypeExtData;
		const auto pType = pTypeExt->OwnerObject();

		if (const auto CameoPCX = pTypeExt->CameoPCX.GetSurface())
		{
			PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
		}
		else if (const auto pSHP = pType->GetCameo())
		{
			const auto pCameoRef = pSHP->AsReference();

			char pFilename[0x20];
			strcpy_s(pFilename, RulesExt::Global()->MissingCameo.data());
			_strlwr_s(pFilename);

			if (!_stricmp(pCameoRef->Filename, GameStrings::XXICON_SHP) && strstr(pFilename, ".pcx"))
			{
				PCX::Instance->LoadFile(pFilename);

				if (const auto MissingCameoPCX = PCX::Instance->GetSurface(pFilename))
					PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
			}
			else
			{
				RectangleStruct rect { 0, 0, position.X + 60, position.Y + 48 };
				DSurface::Composite->DrawSHP(pTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL), pSHP, 0, &position, &rect,
					BlitterFlags::bf_400, 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
			}
		}

		if (!pTechno->InLimbo)
		{
			const auto pRules = RulesClass::Instance();
			auto ratio = pTechno->GetHealthPercentage();

			if (pTechno->IsIronCurtained())
			{
				ColorStruct fillColor { 50, 50, 50 };
				DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 20);
			}
			else
			{
				int time = Unsorted::CurrentFrame - pExt->LastHurtFrame;

				if (ratio < pRules->ConditionRed)
				{
					ColorStruct fillColor { 255, 0, 0 };
					int trans = 40 - time;

					if (trans < 0)
					{
						const int round = time % 60;
						trans = ((round <= 20) ? 0 : ((round <= 40) ? (round - 20) : (60 - round)));
					}

					if (trans > 0)
						DSurface::Composite->FillRectTrans(&drawRect, &fillColor, trans);
				}
				else if (ratio < pRules->ConditionYellow)
				{
					ColorStruct fillColor { 255, 0, 0 };
					int trans = 30 - time;

					if (trans < 0)
					{
						const int round = time % 160;
						trans = ((round <= 140) ? 0 : ((round <= 150) ? (round - 140) : (160 - round)));
					}

					if (trans > 0)
						DSurface::Composite->FillRectTrans(&drawRect, &fillColor, trans);
				}
				else if (time < 20)
				{
					ColorStruct fillColor { 255, 0, 0 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, (20 - time));
				}

				time = Unsorted::CurrentFrame - pTechno->unknown_int_120;

				if (time < 20)
				{
					ColorStruct fillColor { 255, 255, 0 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 20 - time);
				}

				if (pTechno->TemporalTargetingMe)
				{
					ColorStruct fillColor { 100, 100, 255 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
				else if (pTechno->AirstrikeTintStage)
				{
					ColorStruct fillColor { 255, 50, 0 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
				else if (pTechno->DrainingMe || pTechno->LocomotorSource)
				{
					ColorStruct fillColor { 200, 0, 255 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
				else if (pTechno->IsUnderEMP())
				{
					ColorStruct fillColor { 128, 128, 128 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
			}

			if (pTechno->BunkerLinkedItem && pTechno->WhatAmI() != AbstractType::Building)
			{
				RectangleStruct rect { (position.X + 3), (position.Y + 1), 54, 7 };
				DSurface::Composite->DrawRect(&rect, 0x781F);
			}

			RectangleStruct rect { (position.X + 4), (position.Y + 2), 52, 5 };
			DSurface::Composite->FillRect(&rect, 0);

			++rect.X;
			++rect.Y;
			rect.Width = static_cast<int>(50 * ratio + 0.5);
			rect.Height = 3;

			const int color = (ratio > pRules->ConditionYellow) ? 0x67EC : (ratio > pRules->ConditionRed ? 0xFFEC : 0xF986);
			DSurface::Composite->FillRect(&rect, color);

			const auto pShield = pExt->Shield.get();

			if (pShield && !pShield->IsBrokenAndNonRespawning())
			{
				ratio = (static_cast<double>(pShield->GetHP()) / pShield->GetType()->Strength.Get());
				rect.Width = static_cast<int>(50 * ratio + 0.5);
				ColorStruct fillColor { 153, 153, 255 };
				DSurface::Composite->FillRectTrans(&rect, &fillColor, 80);
			}

			if (pTechno->IsIronCurtained())
			{
				const auto timer = &pTechno->IronCurtainTimer;
				ratio = static_cast<double>(timer->GetTimeLeft()) / timer->TimeLeft;
				rect.Width = static_cast<int>(50 * ratio + 0.5);
				ColorStruct fillColor { 200, 50, 50 };
				DSurface::Composite->FillRectTrans(&rect, &fillColor, 80);
			}
		}
		else if (auto pSelect = pTechno->Transporter)
		{
			for (auto pTrans = pSelect; pTrans; pTrans = pTrans->Transporter)
				pSelect = pTrans;

			const auto pSelectExt = TechnoExt::ExtMap.Find(pSelect);
			const auto pRules = RulesClass::Instance();
			auto ratio = pTechno->GetHealthPercentage();

			if (pSelect->IsIronCurtained())
			{
				ColorStruct fillColor { 50, 50, 50 };
				DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 20);
			}
			else
			{
				int time = Unsorted::CurrentFrame - pSelectExt->LastHurtFrame;

				if (ratio < pRules->ConditionRed)
				{
					ColorStruct fillColor { 255, 0, 0 };
					int trans = 40 - time;

					if (trans < 0)
					{
						const int round = time % 60;
						trans = ((round <= 20) ? 0 : ((round <= 40) ? (round - 20) : (60 - round)));
					}

					if (trans > 0)
						DSurface::Composite->FillRectTrans(&drawRect, &fillColor, trans);
				}
				else if (ratio < pRules->ConditionYellow)
				{
					ColorStruct fillColor { 255, 0, 0 };
					int trans = 30 - time;

					if (trans < 0)
					{
						const int round = time % 160;
						trans = ((round <= 140) ? 0 : ((round <= 150) ? (round - 140) : (160 - round)));
					}

					if (trans > 0)
						DSurface::Composite->FillRectTrans(&drawRect, &fillColor, trans);
				}
				else if (time < 20)
				{
					ColorStruct fillColor { 255, 0, 0 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, (20 - time));
				}

				time = Unsorted::CurrentFrame - pSelect->unknown_int_120;

				if (time < 20)
				{
					ColorStruct fillColor { 255, 255, 0 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 20 - time);
				}

				if (pSelect->TemporalTargetingMe)
				{
					ColorStruct fillColor { 100, 100, 255 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
				else if (pSelect->LocomotorSource)
				{
					ColorStruct fillColor { 200, 0, 255 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
				else if (pSelect->IsUnderEMP())
				{
					ColorStruct fillColor { 128, 128, 128 };
					DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 25);
				}
			}

			RectangleStruct rect { (position.X + 3), (position.Y + 1), 0, 7 };

			if (pSelect->BunkerLinkedItem && pSelect->WhatAmI() != AbstractType::Building)
			{
				rect.Width = 54;
				DSurface::Composite->DrawRect(&rect, 0x781F);
			}
			else
			{
				rect.Width = static_cast<int>(54 * pSelect->GetHealthPercentage() + 0.5);
				DSurface::Composite->DrawRect(&rect, 0xFB20);
			}

			++rect.X;
			++rect.Y;
			rect.Width = 52;
			rect.Height = 5;

			DSurface::Composite->FillRect(&rect, 0);

			++rect.X;
			++rect.Y;
			rect.Width = static_cast<int>(50 * ratio + 0.5);
			rect.Height = 3;

			const int color = (ratio > pRules->ConditionYellow) ? 0x67EC : (ratio > pRules->ConditionRed ? 0xFFEC : 0xF986);
			DSurface::Composite->FillRect(&rect, color);

			const auto pShield = pSelectExt->Shield.get();

			if (pShield && !pShield->IsBrokenAndNonRespawning())
			{
				ratio = (static_cast<double>(pShield->GetHP()) / pShield->GetType()->Strength.Get());
				rect.Width = static_cast<int>(50 * ratio + 0.5);
				ColorStruct fillColor { 153, 153, 255 };
				DSurface::Composite->FillRectTrans(&rect, &fillColor, 80);
			}

			if (pSelect->IsIronCurtained())
			{
				const auto timer = &pSelect->IronCurtainTimer;
				ratio = static_cast<double>(timer->GetTimeLeft()) / timer->TimeLeft;
				rect.Width = static_cast<int>(50 * ratio + 0.5);
				ColorStruct fillColor { 200, 50, 50 };
				DSurface::Composite->FillRectTrans(&rect, &fillColor, 80);
			}
		}
		else
		{
			const auto absType = pTechno->WhatAmI();
			const auto buildCat = (absType == AbstractType::Building) ? static_cast<BuildingClass*>(pTechno)->Type->BuildCat : BuildCat::DontCare;
			const auto pFactory = pTechno->Owner->GetPrimaryFactory(absType, pType->Naval, buildCat);

			if (pFactory && pFactory->Object == pTechno)
			{
				ColorStruct fillColor { 0, 0, 0 };
				DSurface::Composite->FillRectTrans(&drawRect, &fillColor, 30);

				RectangleStruct rect { (position.X + 4), (position.Y + 2), 52, 5 };
				DSurface::Composite->FillRect(&rect, 0);

				const auto ratio = static_cast<double>(pFactory->GetProgress()) / 54;
				rect = RectangleStruct { (position.X + 5), (position.Y + 3), static_cast<int>(50 * ratio), 3 };
				DSurface::Composite->FillRect(&rect, 0xFFFF);
			}
			else
			{
				RectangleStruct rect { (position.X + 3), (position.Y + 1), 54, 7 };
				DSurface::Composite->DrawRect(&rect, 0x781F);

				++rect.X;
				++rect.Y;
				rect.Width = 52;
				rect.Height = 5;

				DSurface::Composite->FillRect(&rect, 0);

				const auto ratio = pTechno->GetHealthPercentage();

				++rect.X;
				++rect.Y;
				rect.Width = static_cast<int>(50 * ratio + 0.5);
				rect.Height = 3;

				const auto pRules = RulesClass::Instance();
				const int color = (ratio > pRules->ConditionYellow) ? 0x67EC : (ratio > pRules->ConditionRed ? 0xFFEC : 0xF986);
				DSurface::Composite->FillRect(&rect, color);
			}
		}

		if (i == recordIndex && !ScenarioClass::Instance->UserInputLocked)
		{
			RectangleStruct rect { 0, 0, position.X + 60, position.Y + 48 };
			DSurface::Composite->DrawRectEx(&rect, &drawRect, Drawing::RGB_To_Int(Drawing::TooltipColor));
		}
	}
}

void TacticalButtonsClass::HeroSelect(int buttonIndex)
{
	if (ScenarioClass::Instance->UserInputLocked || !this->HeroVisible)
		return;

	auto& vec = ScenarioExt::Global()->OwnedHeros;
	const int index = buttonIndex - 61;

	if (index >= static_cast<int>(vec.size()))
		return;

	const auto pTechno = vec[index]->OwnerObject();

	if (pTechno->IsAlive)
	{
		auto pSelect = pTechno;

		for (auto pTrans = pSelect->Transporter; pTrans; pTrans = pTrans->Transporter)
			pSelect = pTrans;

		if (ObjectClass::CurrentObjects->Count != 1 || !pSelect->IsSelected)
			MapClass::UnselectAll();

		if (!pSelect->Select())
			TacticalClass::Instance->SetTacticalPosition(&pSelect->Location);
	}
}

void TacticalButtonsClass::HeroSwitch()
{
	this->HeroVisible = !this->HeroVisible;

	MessageListClass::Instance->PrintMessage
	(
		(this->HeroVisible ?
			GeneralUtils::LoadStringUnlessMissing("TXT_HEROS_VISIBLE", L"Set heros info visible.") :
			GeneralUtils::LoadStringUnlessMissing("TXT_HEROS_INVISIBLE", L"Set heros info invisible.")),
		RulesClass::Instance->MessageDelay,
		HouseClass::CurrentPlayer->ColorSchemeIndex,
		true
	);
}

// Button index 71-100 : Select buttons
inline bool TacticalButtonsClass::IndexInSelectButtons()
{
	return this->ButtonIndex > 70 && this->ButtonIndex <= 100;
}

inline void TacticalButtonsClass::AddToCurrentSelect(TechnoTypeExt::ExtData* pTypeExt, int count, int checkIndex)
{
	const auto groupID = pTypeExt->GetSelectionGroupID();
	const int currentCounts = this->CurrentSelectCameo.size();

	for (int i = checkIndex; i < currentCounts; ++i)
	{
		if (this->CurrentSelectCameo[i].TypeExt->GetSelectionGroupID() == groupID)
		{
			this->CurrentSelectCameo[i].Count += count;
			return;
		}
	}

	SelectRecordStruct select { pTypeExt, count };
	this->CurrentSelectCameo.push_back(select);
}

void TacticalButtonsClass::SelectedTrigger(int buttonIndex, bool select)
{
	if (ScenarioClass::Instance->UserInputLocked || !Phobos::Config::SelectedDisplay_Enable)
		return;

	const int currentCounts = this->CurrentSelectCameo.size();

	if (buttonIndex > (currentCounts + 70))
		return;

	const auto pTypeExt = this->CurrentSelectCameo[buttonIndex - 71].TypeExt;
	const auto groupID = pTypeExt->GetSelectionGroupID();
	const auto shiftPressed = InputManagerClass::Instance->IsForceSelectKeyPressed();

	if (select)
	{
		if (shiftPressed)
		{
			std::vector<ObjectClass*> deselects;
			deselects.reserve(ObjectClass::CurrentObjects->Count);

			for (const auto& pCurrent : ObjectClass::CurrentObjects())
			{
				if (const auto pCurrentTypeExt = TechnoTypeExt::ExtMap.Find(pCurrent->GetTechnoType()))
				{
					if (pCurrentTypeExt->GetSelectionGroupID() == groupID)
						continue;
				}

				deselects.push_back(pCurrent);
			}

			for (const auto& pDeselect : deselects)
				pDeselect->Deselect();
		}
		else
		{
			const int counts = ObjectClass::CurrentObjects->Count;
			std::vector<ObjectClass*> selects;
			selects.reserve(counts);
			std::vector<ObjectClass*> deselects;
			deselects.reserve(counts);

			for (const auto& pCurrent : ObjectClass::CurrentObjects())
			{
				if (const auto pCurrentTypeExt = TechnoTypeExt::ExtMap.Find(pCurrent->GetTechnoType()))
				{
					if (pCurrentTypeExt->GetSelectionGroupID() == groupID)
					{
						selects.push_back(pCurrent);
						continue;
					}
				}

				deselects.push_back(pCurrent);
			}

			for (const auto& pDeselect : deselects)
				pDeselect->Deselect();

			const int size = selects.size();
			const int random = Unsorted::CurrentFrame % size;

			for (int i = 0; i < size; ++i)
			{
				if (i != random)
					selects[i]->Deselect();
			}
		}
	}
	else
	{
		if (shiftPressed)
		{
			std::vector<ObjectClass*> deselects;
			deselects.reserve(ObjectClass::CurrentObjects->Count);

			for (const auto& pCurrent : ObjectClass::CurrentObjects())
			{
				if (const auto pCurrentTypeExt = TechnoTypeExt::ExtMap.Find(pCurrent->GetTechnoType()))
				{
					if (pCurrentTypeExt->GetSelectionGroupID() != groupID)
						continue;
				}

				deselects.push_back(pCurrent);
			}

			for (const auto& pDeselect : deselects)
				pDeselect->Deselect();
		}
		else
		{
			std::vector<ObjectClass*> selects;
			selects.reserve(ObjectClass::CurrentObjects->Count);

			for (const auto& pCurrent : ObjectClass::CurrentObjects())
			{
				if (const auto pCurrentTypeExt = TechnoTypeExt::ExtMap.Find(pCurrent->GetTechnoType()))
				{
					if (pCurrentTypeExt->GetSelectionGroupID() == groupID)
						selects.push_back(pCurrent);
				}
			}

			selects[Unsorted::CurrentFrame % selects.size()]->Deselect();
		}
	}
}

void TacticalButtonsClass::SelectedUpdate()
{
	this->UpdateSelect = false;

	if (this->CurrentSelectCameo.size())
		this->CurrentSelectCameo.clear();

	if (ObjectClass::CurrentObjects->Count <= 0)
	{
		this->RecordIndex = 71;
		return;
	}

	this->CurrentSelectCameo.reserve(10);

	std::map<int, int> CurrentSelectInfantry;
	std::map<int, int> CurrentSelectUnit;
	std::map<int, int> CurrentSelectAircraft;
	std::map<int, int> CurrentSelectBuilding;

	for (const auto& pCurrent : ObjectClass::CurrentObjects())
	{
		const auto absType = pCurrent->WhatAmI();

		if (absType == AbstractType::Infantry)
			++CurrentSelectInfantry[static_cast<InfantryClass*>(pCurrent)->Type->ArrayIndex];
		else if (absType == AbstractType::Unit)
			++CurrentSelectUnit[static_cast<UnitClass*>(pCurrent)->Type->ArrayIndex];
		else if (absType == AbstractType::Aircraft)
			++CurrentSelectAircraft[static_cast<AircraftClass*>(pCurrent)->Type->ArrayIndex];
		else if (absType == AbstractType::Building)
			++CurrentSelectBuilding[static_cast<BuildingClass*>(pCurrent)->Type->ArrayIndex];
	}

	if (CurrentSelectInfantry.size())
	{
		for (const auto& [index, count] : CurrentSelectInfantry)
		{
			if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(InfantryTypeClass::Array->Items[index]))
				this->AddToCurrentSelect(pTypeExt, count, 0);
		}
	}

	if (CurrentSelectUnit.size())
	{
		const int checkStart = this->CurrentSelectCameo.size();

		for (const auto& [index, count] : CurrentSelectUnit)
		{
			if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(UnitTypeClass::Array->Items[index]))
				this->AddToCurrentSelect(pTypeExt, count, checkStart);
		}
	}

	if (CurrentSelectAircraft.size())
	{
		const int checkStart = this->CurrentSelectCameo.size();

		for (const auto& [index, count] : CurrentSelectAircraft)
		{
			if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(AircraftTypeClass::Array->Items[index]))
				this->AddToCurrentSelect(pTypeExt, count, checkStart);
		}
	}

	if (CurrentSelectBuilding.size())
	{
		const int checkStart = this->CurrentSelectCameo.size();

		for (const auto& [index, count] : CurrentSelectBuilding)
		{
			if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(BuildingTypeClass::Array->Items[index]))
				this->AddToCurrentSelect(pTypeExt, count, checkStart);
		}
	}

	const int maxIndex = this->CurrentSelectCameo.size() + 70;

	if (this->RecordIndex > maxIndex)
		this->RecordIndex = maxIndex;
}

void TacticalButtonsClass::SelectedDraw()
{
	if (!Phobos::Config::SelectedDisplay_Enable)
		return;

	if (this->UpdateSelect)
		this->SelectedUpdate();

	const int currentCounts = Math::min(30, static_cast<int>(this->CurrentSelectCameo.size()));
	const auto pRulesExt = RulesExt::Global();

	if (currentCounts > 1 || (currentCounts && this->CurrentSelectCameo[0].Count > 1)) // Mass
	{
		Point2D position { 0, DSurface::Composite->GetHeight() - 80 };
		Point2D textPosition { 1, position.Y + 1 };
		RectangleStruct drawRect { 0, position.Y, 60, 48 };

		const int gap = Math::min(60, ((DSurface::Composite->GetWidth() << 1) / (5 * currentCounts)));
		RectangleStruct surfaceRect { 0, 0, (gap * (currentCounts - 1) + 60), (position.Y + 48) };

		const int maxIndex = currentCounts + 70;
		const COLORREF color = Drawing::RGB_To_Int(Drawing::TooltipColor);
		TextPrintType printType = TextPrintType::Background | TextPrintType::FullShadow | TextPrintType::Point8;

		for (int i = 71; i < this->RecordIndex && i <= maxIndex; ++i, position.X += gap, drawRect.X = position.X, textPosition.X += gap)
		{
			const auto pSelect = this->CurrentSelectCameo[i - 71];
			const auto pTypeExt = pSelect.TypeExt;

			if (const auto CameoPCX = pTypeExt->CameoPCX.GetSurface())
			{
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			}
			else if (const auto pSHP = pTypeExt->OwnerObject()->GetCameo())
			{
				do
				{
					const auto pCameoRef = pSHP->AsReference();
					char pFilename[0x20];
					strcpy_s(pFilename, pRulesExt->MissingCameo.data());
					_strlwr_s(pFilename);

					if (!_stricmp(pCameoRef->Filename, GameStrings::XXICON_SHP))
					{
						const auto absType = pTypeExt->OwnerObject()->WhatAmI();
						BSurface* MissingCameoPCX = nullptr;

						if (absType == AbstractType::InfantryType && (MissingCameoPCX = pRulesExt->SelectedInfantryMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::UnitType && (MissingCameoPCX = pRulesExt->SelectedVehicleMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::AircraftType && (MissingCameoPCX = pRulesExt->SelectedAircraftMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::BuildingType && (MissingCameoPCX = pRulesExt->SelectedBuildingMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (strstr(pFilename, ".pcx"))
						{
							PCX::Instance->LoadFile(pFilename);

							if (MissingCameoPCX = PCX::Instance->GetSurface(pFilename), MissingCameoPCX)
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);

							break;
						}
					}

					RectangleStruct rect { 0, 0, (position.X + 60), surfaceRect.Height };
					DSurface::Composite->DrawSHP(pTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL), pSHP, 0, &position, &rect,
						BlitterFlags::bf_400, 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
				}
				while (false);
			}

			const int count = pSelect.Count;

			if (count > 1)
			{
				wchar_t text[0x20];
				swprintf_s(text, L"%d", count);
				DSurface::Composite->DrawTextA(text, &surfaceRect, &textPosition, color, 0, printType);
			}
		}

		position.X = surfaceRect.Width - 60;
		drawRect.X = position.X;

		printType |= TextPrintType::Right;
		textPosition.X = surfaceRect.Width - 1;

		for (int i = maxIndex; i > this->RecordIndex; --i, position.X -= gap, drawRect.X = position.X, textPosition.X -= gap)
		{
			const auto pSelect = this->CurrentSelectCameo[i - 71];
			const auto pTypeExt = pSelect.TypeExt;

			if (const auto CameoPCX = pTypeExt->CameoPCX.GetSurface())
			{
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			}
			else if (const auto pSHP = pTypeExt->OwnerObject()->GetCameo())
			{
				do
				{
					const auto pCameoRef = pSHP->AsReference();
					char pFilename[0x20];
					strcpy_s(pFilename, pRulesExt->MissingCameo.data());
					_strlwr_s(pFilename);

					if (!_stricmp(pCameoRef->Filename, GameStrings::XXICON_SHP))
					{
						const auto absType = pTypeExt->OwnerObject()->WhatAmI();
						BSurface* MissingCameoPCX = nullptr;

						if (absType == AbstractType::InfantryType && (MissingCameoPCX = pRulesExt->SelectedInfantryMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::UnitType && (MissingCameoPCX = pRulesExt->SelectedVehicleMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::AircraftType && (MissingCameoPCX = pRulesExt->SelectedAircraftMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::BuildingType && (MissingCameoPCX = pRulesExt->SelectedBuildingMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (strstr(pFilename, ".pcx"))
						{
							PCX::Instance->LoadFile(pFilename);

							if (MissingCameoPCX = PCX::Instance->GetSurface(pFilename), MissingCameoPCX)
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);

							break;
						}
					}

					RectangleStruct rect { 0, 0, (position.X + 60), surfaceRect.Height };
					DSurface::Composite->DrawSHP(pTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL), pSHP, 0, &position, &rect,
						BlitterFlags::bf_400, 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
				}
				while (false);
			}

			const int count = pSelect.Count;

			if (count > 1)
			{
				wchar_t text[0x20];
				swprintf_s(text, L"%d", count);
				DSurface::Composite->DrawTextA(text, &surfaceRect, &textPosition, color, 0, printType);
			}
		}

		const bool hovered = this->ButtonIndex == this->RecordIndex;

		if (hovered)
		{
			position.Y -= 10;
			drawRect.Y -= 10;
			textPosition.Y -= 10;

			ColorStruct fillColor { 0, 0, 0 };
			RectangleStruct fillRect { position.X, position.Y + 48, 60, 10 };
			DSurface::Composite->FillRectTrans(&fillRect, &fillColor, 60);
		}

		if (this->RecordIndex <= maxIndex)
		{
			const auto pSelect = this->CurrentSelectCameo[this->RecordIndex - 71];
			const auto pTypeExt = pSelect.TypeExt;

			if (const auto CameoPCX = pTypeExt->CameoPCX.GetSurface())
			{
				PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
			}
			else if (const auto pSHP = pTypeExt->OwnerObject()->GetCameo())
			{
				do
				{
					const auto pCameoRef = pSHP->AsReference();
					char pFilename[0x20];
					strcpy_s(pFilename, pRulesExt->MissingCameo.data());
					_strlwr_s(pFilename);

					if (!_stricmp(pCameoRef->Filename, GameStrings::XXICON_SHP))
					{
						const auto absType = pTypeExt->OwnerObject()->WhatAmI();
						BSurface* MissingCameoPCX = nullptr;

						if (absType == AbstractType::InfantryType && (MissingCameoPCX = pRulesExt->SelectedInfantryMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::UnitType && (MissingCameoPCX = pRulesExt->SelectedVehicleMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::AircraftType && (MissingCameoPCX = pRulesExt->SelectedAircraftMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (absType == AbstractType::BuildingType && (MissingCameoPCX = pRulesExt->SelectedBuildingMissingPCX.GetSurface(), MissingCameoPCX))
						{
							PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
							break;
						}
						else if (strstr(pFilename, ".pcx"))
						{
							PCX::Instance->LoadFile(pFilename);

							if (MissingCameoPCX = PCX::Instance->GetSurface(pFilename), MissingCameoPCX)
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);

							break;
						}
					}

					RectangleStruct rect { 0, 0, (position.X + 60), (position.Y + 48) };
					DSurface::Composite->DrawSHP(pTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL), pSHP, 0, &position, &rect,
						BlitterFlags::bf_400, 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
				}
				while (false);
			}

			const int count = pSelect.Count;

			if (count > 1)
			{
				wchar_t text[0x20];
				swprintf_s(text, L"%d", count);
				DSurface::Composite->DrawTextA(text, &surfaceRect, &textPosition, color, 0, printType);
			}
		}
	}
	else if (currentCounts) // Only 1
	{
		for (const auto& pCurrent : ObjectClass::CurrentObjects())
		{
			if (const auto pThis = abstract_cast<TechnoClass*>(pCurrent))
			{
				const auto pType = pThis->GetTechnoType();
				const auto pTypeExt = this->CurrentSelectCameo[0].TypeExt;

				if (pTypeExt->OwnerObject() == pType)
				{
					TextPrintType printType = TextPrintType::Center | TextPrintType::Point8;
					COLORREF color = Drawing::RGB_To_Int(Drawing::TooltipColor);
					RectangleStruct drawRect { 0, DSurface::Composite->GetHeight() - 80, 180, 48};
					{
						DSurface::Composite->FillRect(&drawRect, COLOR_BLACK);
						drawRect.X += 60;
						drawRect.Width = 120;
						DSurface::Composite->DrawRect(&drawRect, color);
						drawRect.X -= 60;
					}
					drawRect.Width = 60;
					Point2D position { 60, drawRect.Y };
					RectangleStruct surfaceRect { 0, 0, 180, position.Y + 48 };

					auto CameoPCX = pTypeExt->CameoPCX.GetSurface();
					auto pSHP = pType->GetCameo();
					auto pPal = pTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL);

					position += Point2D { 60, 3 };
					{
						const auto pHouse = HouseClass::CurrentPlayer();
						auto name = pType->UIName;

						if (!pHouse->IsAlliedWith(pThis) && !pHouse->IsObserver())
						{
							if (pThis->IsDisguised() && !pThis->GetCell()->DisguiseSensors_InclHouse(pHouse->ArrayIndex))
							{
								const auto pDisguiseType = TechnoTypeExt::GetTechnoType(pThis->Disguise);

								if (const auto pDisguiseTypeExt = TechnoTypeExt::ExtMap.Find(pDisguiseType))
								{
									if (const auto pFakeType = pDisguiseTypeExt->FakeOf)
									{
										if (const auto pFakeTypeExt = TechnoTypeExt::ExtMap.Find(pFakeType))
										{
											if (const auto fakeName = pFakeTypeExt->EnemyUIName.Get().Text)
												name = fakeName;
											else
												name = pFakeType->UIName;

											if (const auto FakeCameoPCX = pFakeTypeExt->CameoPCX.GetSurface())
											{
												CameoPCX = FakeCameoPCX;
											}
											else if (const auto pFakeSHP = pFakeType->GetCameo())
											{
												pSHP = pFakeSHP;
												pPal = pFakeTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL);
											}
										}
										else
										{
											name = pFakeType->UIName;
											pSHP = pFakeType->GetCameo();
											pPal = FileSystem::CAMEO_PAL;
										}
									}
									else
									{
										if (const auto fakeName = pDisguiseTypeExt->EnemyUIName.Get().Text)
											name = fakeName;
										else
											name = pDisguiseType->UIName;

										if (const auto disguiseCameoPCX = pDisguiseTypeExt->CameoPCX.GetSurface())
										{
											CameoPCX = disguiseCameoPCX;
										}
										else if (const auto pDisguiseSHP = pDisguiseType->GetCameo())
										{
											pSHP = pDisguiseSHP;
											pPal = pDisguiseTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL);
										}
									}
								}
								else
								{
									name = pDisguiseType->UIName;
									pSHP = pDisguiseType->GetCameo();
									pPal = FileSystem::CAMEO_PAL;
								}
							}
							else if (const auto pFakeType = pTypeExt->FakeOf)
							{
								if (const auto pFakeTypeExt = TechnoTypeExt::ExtMap.Find(pFakeType))
								{
									if (const auto fakeName = pFakeTypeExt->EnemyUIName.Get().Text)
										name = fakeName;
									else
										name = pFakeType->UIName;

									if (const auto FakeCameoPCX = pFakeTypeExt->CameoPCX.GetSurface())
									{
										CameoPCX = FakeCameoPCX;
									}
									else if (const auto pFakeSHP = pFakeType->GetCameo())
									{
										pSHP = pFakeSHP;
										pPal = pFakeTypeExt->CameoPal.GetOrDefaultConvert(FileSystem::CAMEO_PAL);
									}
								}
								else
								{
									name = pFakeType->UIName;
									pSHP = pFakeType->GetCameo();
									pPal = FileSystem::CAMEO_PAL;
								}
							}
							else if (const auto enemyName = pTypeExt->EnemyUIName.Get().Text)
							{
								name = enemyName;
							}
						}

						if (name)
						{
							size_t length = Math::min(wcslen(name), static_cast<size_t>(31));

							for (auto check = name; *check != L'\0'; ++check)
							{
								if (*check == L'\n')
								{
									length = static_cast<size_t>(check - name);
									break;
								}
							}

							wchar_t text1[0x20] = {0};
							wcsncpy_s(text1, name, length);
							text1[length] = L'\0';
							DSurface::Composite->DrawTextA(text1, &surfaceRect, &position, color, 0, printType);
						}
					}

					position.Y += 15;
					{
						int value = -1, maxValue = 0;
						TechnoExt::GetValuesForDisplay(pThis, pTypeExt->SelectedInfo_UpperType, value, maxValue);

						if (value >= 0 && maxValue > 0)
						{
							if (pTypeExt->SelectedInfo_UpperColor.Get() == ColorStruct{ 0, 0, 0 })
							{
								const auto pRules = RulesClass::Instance();
								const auto ratio = static_cast<double>(value) / maxValue;
								color = (ratio > pRules->ConditionYellow) ? 0x67EC : (ratio > pRules->ConditionRed ? 0xFFEC : 0xF986);
							}
							else
							{
								color = Drawing::RGB_To_Int(pTypeExt->SelectedInfo_UpperColor);
							}

							wchar_t text2[0x20] = {0};

							position.X += 10;
							printType &= ~TextPrintType::Center;
							swprintf_s(text2, L"%d", maxValue);
							DSurface::Composite->DrawTextA(text2, &surfaceRect, &position, color, 0, printType);

							position.X -= 20;
							printType |= TextPrintType::Right;
							swprintf_s(text2, L"%d", value);
							DSurface::Composite->DrawTextA(text2, &surfaceRect, &position, color, 0, printType);

							position.X += 10;
							printType &= ~TextPrintType::Right;
							printType |= TextPrintType::Center;
							DSurface::Composite->DrawTextA(L"/", &surfaceRect, &position, color, 0, printType);
						}
						else
						{
							DSurface::Composite->DrawTextA(L"-- / --", &surfaceRect, &position, color, 0, printType);
						}
					}

					position.Y += 13;
					{
						int value = -1, maxValue = 0;
						TechnoExt::GetValuesForDisplay(pThis, pTypeExt->SelectedInfo_BelowType, value, maxValue);

						if (value >= 0 && maxValue > 0)
						{
							if (pTypeExt->SelectedInfo_BelowColor.Get() == ColorStruct{ 0, 0, 0 })
							{
								const auto pRules = RulesClass::Instance();
								const auto ratio = static_cast<double>(value) / maxValue;
								color = (ratio > pRules->ConditionYellow) ? 0x67EC : (ratio > pRules->ConditionRed ? 0xFFEC : 0xF986);
							}
							else
							{
								color = Drawing::RGB_To_Int(pTypeExt->SelectedInfo_BelowColor);
							}

							wchar_t text3[0x20] = {0};

							position.X += 10;
							printType &= ~TextPrintType::Center;
							swprintf_s(text3, L"%d", maxValue);
							DSurface::Composite->DrawTextA(text3, &surfaceRect, &position, color, 0, printType);

							position.X -= 20;
							printType |= TextPrintType::Right;
							swprintf_s(text3, L"%d", value);
							DSurface::Composite->DrawTextA(text3, &surfaceRect, &position, color, 0, printType);

							position.X += 10;
							printType &= ~TextPrintType::Right;
							printType |= TextPrintType::Center;
							DSurface::Composite->DrawTextA(L"/", &surfaceRect, &position, color, 0, printType);
						}
						else
						{
							DSurface::Composite->DrawTextA(L"-- / --", &surfaceRect, &position, color, 0, printType);
						}
					}

					if (CameoPCX)
					{
						PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, CameoPCX);
					}
					else if (pSHP)
					{
						const auto pCameoRef = pSHP->AsReference();
						char pFilename[0x20];
						strcpy_s(pFilename, pRulesExt->MissingCameo.data());
						_strlwr_s(pFilename);

						if (!_stricmp(pCameoRef->Filename, GameStrings::XXICON_SHP))
						{
							const auto absType = pTypeExt->FakeOf ? pTypeExt->FakeOf->WhatAmI() : pType->WhatAmI();
							BSurface* MissingCameoPCX = nullptr;

							if (absType == AbstractType::InfantryType && (MissingCameoPCX = pRulesExt->SelectedInfantryMissingPCX.GetSurface(), MissingCameoPCX))
							{
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
								return;
							}
							else if (absType == AbstractType::UnitType && (MissingCameoPCX = pRulesExt->SelectedVehicleMissingPCX.GetSurface(), MissingCameoPCX))
							{
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
								return;
							}
							else if (absType == AbstractType::AircraftType && (MissingCameoPCX = pRulesExt->SelectedAircraftMissingPCX.GetSurface(), MissingCameoPCX))
							{
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
								return;
							}
							else if (absType == AbstractType::BuildingType && (MissingCameoPCX = pRulesExt->SelectedBuildingMissingPCX.GetSurface(), MissingCameoPCX))
							{
								PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);
								return;
							}
							else if (strstr(pFilename, ".pcx"))
							{
								PCX::Instance->LoadFile(pFilename);

								if (MissingCameoPCX = PCX::Instance->GetSurface(pFilename), MissingCameoPCX)
									PCX::Instance->BlitToSurface(&drawRect, DSurface::Composite, MissingCameoPCX);

								return;
							}
						}

						position -= Point2D { 120, 31 };
						surfaceRect.Width = 60;

						DSurface::Composite->DrawSHP(pPal, pSHP, 0, &position, &surfaceRect, BlitterFlags::bf_400, 0, 0, ZGradient::Ground, 1000, 0, 0, 0, 0, 0);
					}

					return;
				}
			}
		}
	}
}

void TacticalButtonsClass::SelectedSwitch()
{
	Phobos::Config::SelectedDisplay_Enable = !Phobos::Config::SelectedDisplay_Enable;

	if (Phobos::Config::SelectedDisplay_Enable)
	{
		this->UpdateSelect = true;
		MessageListClass::Instance->PrintMessage
		(
			GeneralUtils::LoadStringUnlessMissing("TXT_SELECT_VISIBLE", L"Set select info visible."),
			RulesClass::Instance->MessageDelay,
			HouseClass::CurrentPlayer->ColorSchemeIndex,
			true
		);
	}
	else
	{
		this->RecordIndex = 71;
		MessageListClass::Instance->PrintMessage
		(
			GeneralUtils::LoadStringUnlessMissing("TXT_SELECT_INVISIBLE", L"Set select info invisible."),
			RulesClass::Instance->MessageDelay,
			HouseClass::CurrentPlayer->ColorSchemeIndex,
			true
		);
	}
}

// Hooks

// Mouse trigger hooks
DEFINE_HOOK(0x6931A5, ScrollClass_WindowsProcedure_PressLeftMouseButton, 0x6)
{
	enum { SkipGameCode = 0x6931B4 };

	const auto pButtons = &TacticalButtonsClass::Instance;

	if (!pButtons->MouseIsOverTactical())
	{
		pButtons->PressedInButtonsLayer = true;
		pButtons->PressDesignatedButton(0);

		R->Stack(STACK_OFFSET(0x28, 0x8), 0);
		R->EAX(Action::None);
		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x693268, ScrollClass_WindowsProcedure_ReleaseLeftMouseButton, 0x5)
{
	enum { SkipGameCode = 0x693276 };

	const auto pButtons = &TacticalButtonsClass::Instance;

	if (pButtons->PressedInButtonsLayer)
	{
		pButtons->PressedInButtonsLayer = false;
		pButtons->PressDesignatedButton(1);

		R->Stack(STACK_OFFSET(0x28, 0x8), 0);
		R->EAX(Action::None);
		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x69330E, ScrollClass_WindowsProcedure_PressRightMouseButton, 0x6)
{
	enum { SkipGameCode = 0x69334A };

	const auto pButtons = &TacticalButtonsClass::Instance;

	if (!pButtons->MouseIsOverTactical())
	{
		pButtons->PressedInButtonsLayer = true;
		pButtons->PressDesignatedButton(2);

		return SkipGameCode;
	}

	return 0;
}

DEFINE_HOOK(0x693397, ScrollClass_WindowsProcedure_ReleaseRightMouseButton, 0x6)
{
	enum { SkipGameCode = 0x6933CB };

	const auto pButtons = &TacticalButtonsClass::Instance;

	if (pButtons->PressedInButtonsLayer)
	{
		pButtons->PressedInButtonsLayer = false;
		pButtons->PressDesignatedButton(3);

		return SkipGameCode;
	}

	return 0;
}

// Mouse suspend hooks
DEFINE_HOOK(0x692F85, ScrollClass_MouseUpdate_SkipMouseLongPress, 0x7)
{
	enum { CheckMousePress = 0x692F8E, CheckMouseNoPress = 0x692FDC };

	GET(ScrollClass*, pThis, EBX);

	// 555A: AnyMouseButtonDown
	return (pThis->unknown_byte_554A && !TacticalButtonsClass::Instance.PressedInButtonsLayer) ? CheckMousePress : CheckMouseNoPress;
}

DEFINE_HOOK(0x69300B, ScrollClass_MouseUpdate_SkipMouseActionUpdate, 0x6)
{
	enum { SkipGameCode = 0x69301A };

	const auto mousePosition = WWMouseClass::Instance->XY1;
	const auto pButtons = &TacticalButtonsClass::Instance;
	pButtons->SetMouseButtonIndex(&mousePosition);

	if (pButtons->MouseIsOverTactical())
		return 0;

	R->Stack(STACK_OFFSET(0x30, -0x24), 0);
	R->EAX(Action::None);
	return SkipGameCode;
}

// Buttons display hooks
DEFINE_HOOK(0x6D4941, TacticalClass_Render_DrawButtonCameo, 0x6)
{
	const auto pButtons = &TacticalButtonsClass::Instance;
	pButtons->FPSCounterDraw();

	// TODO New buttons (The later draw, the higher layer)

	pButtons->SelectedDraw();
	pButtons->HerosDraw();
	pButtons->SWSidebarDraw();

	return 0;
}

// SW buttons update hooks
DEFINE_HOOK(0x4F9283, HouseClass_Update_RecheckTechTree, 0x5)
{
	GET(HouseClass*, pHouse, ESI);

	if (pHouse == HouseClass::CurrentPlayer)
		TacticalButtonsClass::Instance.SWSidebarRecheck();

	return 0;
}

DEFINE_HOOK(0x6A6314, SidebarClass_AddCameo_SupportSWButtons, 0x8)
{
	enum { SkipThisCameo = 0x6A65F5 };

	GET_STACK(const AbstractType, absType, STACK_OFFSET(0x14, 0x4));
	REF_STACK(int, index, STACK_OFFSET(0x14, 0x8));

	return (absType != AbstractType::Special || SuperWeaponTypeClass::Array->Count <= index || TacticalButtonsClass::Instance.SWSidebarAdd(index)) ? 0 : SkipThisCameo;
}

// Extra function hooks
DEFINE_HOOK(0x6AAF46, SelectClass_Action_ButtonClick1, 0x6)
{
	enum { SkipClearMouse = 0x6AB95A };

	GET(const int, index, ESI);

	return TacticalButtonsClass::Instance.SWQuickLaunch(index) ? SkipClearMouse : 0;
}

// SW buttons trigger hooks
DEFINE_HOOK_AGAIN(0x6AAD2F, SelectClass_Action_ButtonClick2, 0x7)
DEFINE_HOOK(0x6AB94F, SelectClass_Action_ButtonClick2, 0xB)
{
	enum { ForceEffective = 0x6AAE7C };

	if (!TacticalButtonsClass::Instance.DummyAction)
		return 0;

	GET(TacticalButtonsClass::DummySelectClass* const , pThis, EDI);

	R->Stack(STACK_OFFSET(0xAC, -0x98), pThis->SWIndex);
	return ForceEffective;
}

DEFINE_HOOK(0x6AB961, SelectClass_Action_ButtonClick3, 0x7)
{
	enum { SkipControlAction = 0x6AB975 };

	const auto pButtons = &TacticalButtonsClass::Instance;

	if (!pButtons->DummyAction)
		return 0;

	pButtons->DummyAction = false;

	return SkipControlAction;
}

// Heros hooks
DEFINE_HOOK(0x7015C9, TechnoClass_SetOwningHouse_ChangeHeroOwner, 0x6)
{
	GET(TechnoClass* const, pThis, ESI);
	GET(HouseClass* const, pNewOwner, EBP);

	const auto pExt = TechnoExt::ExtMap.Find(pThis);

	if (pExt->TypeExtData->UniqueTechno)
	{
		const auto pOldOwner = pThis->Owner;

		if (pOldOwner->IsControlledByCurrentPlayer())
		{
			if (!pNewOwner->IsControlledByCurrentPlayer())
			{
				auto& vec = ScenarioExt::Global()->OwnedHeros;
				vec.erase(std::remove(vec.begin(), vec.end(), pExt), vec.end());
			}
		}
		else if (pNewOwner->IsControlledByCurrentPlayer())
		{
			auto& vec = ScenarioExt::Global()->OwnedHeros;

			if (std::find(vec.begin(), vec.end(), pExt) == vec.end())
				vec.push_back(pExt);
		}
	}

	return 0;
}

// Select hooks
DEFINE_HOOK_AGAIN(0x5F4718, ObjectClass_Select, 0x7)
DEFINE_HOOK(0x5F46AE, ObjectClass_Select, 0x7)
{
	GET(ObjectClass*, pThis, ESI);

	pThis->IsSelected = true;

	if (RulesExt::Global()->SelectionFlashDuration > 0 && pThis->GetOwningHouse()->IsControlledByCurrentPlayer())
		pThis->Flash(RulesExt::Global()->SelectionFlashDuration);

	TacticalButtonsClass::Instance.UpdateSelect = true;

	return 0;
}

DEFINE_HOOK(0x5F44FC, ObjectClass_Deselect, 0x7)
{
	GET(ObjectClass*, pThis, ESI);

	pThis->IsSelected = false;

	TacticalButtonsClass::Instance.UpdateSelect = true;

	return 0;
}

// Shortcuts keys hooks
DEFINE_HOOK(0x533E69, UnknownClass_sub_533D20_LoadKeyboardCodeFromINI, 0x6)
{
	GET(CommandClass*, pCommand, ESI);
	GET(int, key, EDI);

	const auto pButtons = &TacticalButtonsClass::Instance;
	const auto name = pCommand->GetName();
	char buffer[29];

	for (int i = 1; i <= 10; ++i)
	{
		sprintf_s(buffer, "SW Sidebar Shortcuts Num %02d", i);

		if (!_strcmpi(name, buffer))
			pButtons->SWSidebarRecord(i, key);
	}

	return 0;
}

DEFINE_HOOK(0x5FB992, UnknownClass_sub_5FB320_SaveKeyboardCodeToINI, 0x6)
{
	GET(CommandClass*, pCommand, ECX);
	GET(int, key, EAX);

	const auto pButtons = &TacticalButtonsClass::Instance;
	const auto name = pCommand->GetName();
	char buffer[30];

	for (int i = 1; i <= 10; ++i)
	{
		sprintf_s(buffer, "SW Sidebar Shortcuts Num %02d", i);

		if (!_strcmpi(name, buffer))
			pButtons->SWSidebarRecord(i, key);
	}

	return 0;
}
