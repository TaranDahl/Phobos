#include "SWSidebarClass.h"
#include <CommandClass.h>
#include <sstream>

#include <Ext/House/Body.h>
#include <Ext/Side/Body.h>
#include <Ext/SWType/Body.h>

SWSidebarClass SWSidebarClass::Instance;
CommandClass* SWSidebarClass::Commands[10];

// =============================
// functions

bool SWSidebarClass::AddColumn()
{
	auto& columns = this->Columns;

	if (static_cast<int>(columns.size()) >= Phobos::UI::SuperWeaponSidebar_MaxColumns)
		return false;

	const int cameoWidth = 60;
	const auto column = DLLCreate<SWColumnClass>(SWButtonClass::StartID + SuperWeaponTypeClass::Array->Count + 1 + static_cast<int>(columns.size()), 0, 0, cameoWidth + Phobos::UI::SuperWeaponSidebar_Interval, Phobos::UI::SuperWeaponSidebar_CameoHeight);

	if (!column)
		return false;

	column->Zap();
	GScreenClass::Instance->AddButton(column);
	return true;
}

bool SWSidebarClass::RemoveColumn()
{
	auto& columns = this->Columns;

	if (columns.empty())
		return false;

	if (const auto backColumn = columns.back())
	{
		AnnounceInvalidPointer(SWSidebarClass::Instance.CurrentColumn, backColumn);
		GScreenClass::Instance->RemoveButton(backColumn);

		DLLDelete(backColumn);
		columns.erase(columns.end() - 1);
		return true;
	}

	return false;
}

void SWSidebarClass::InitClear()
{
	this->CurrentColumn = nullptr;
	this->CurrentButton = nullptr;

	if (const auto toggleButton = this->ToggleButton)
	{
		this->ToggleButton = nullptr;
		GScreenClass::Instance->RemoveButton(toggleButton);
		DLLDelete(toggleButton);
	}

	auto& columns = this->Columns;

	for (const auto column : columns)
	{
		column->ClearButtons();
		GScreenClass::Instance->RemoveButton(column);
		DLLDelete(column);
	}

	columns.clear();
}

bool SWSidebarClass::AddButton(int superIdx)
{
	auto& columns = this->Columns;

	if (columns.empty() && !this->AddColumn())
		return false;

	if (std::any_of(columns.begin(), columns.end(), [superIdx](SWColumnClass* column) { return std::any_of(column->Buttons.begin(), column->Buttons.end(), [superIdx](SWButtonClass* button) { return button->SuperIndex == superIdx; }); }))
		return true;

	const bool success = columns.back()->AddButton(superIdx);

	if (success)
		SidebarExt::Global()->SWSidebar_Indices.AddUnique(superIdx);

	return success;
}

void SWSidebarClass::SortButtons()
{
	auto& columns = this->Columns;

	if (columns.empty())
	{
		if (const auto toggleButton = this->ToggleButton)
			toggleButton->UpdatePosition();

		return;
	}

	std::vector<SWButtonClass*> vec_Buttons;
	vec_Buttons.reserve(this->GetMaximumButtonCount());

	for (const auto column : columns)
	{
		for (const auto button : column->Buttons)
			vec_Buttons.emplace_back(button);

		column->ClearButtons(false);
	}

	const unsigned int ownerBits = 1u << HouseClass::CurrentPlayer->Type->ArrayIndex;

	std::stable_sort(vec_Buttons.begin(), vec_Buttons.end(), [ownerBits](SWButtonClass* const a, SWButtonClass* const b)
		{
			const auto pExtA = SWTypeExt::ExtMap.Find(SuperWeaponTypeClass::Array->GetItemOrDefault(a->SuperIndex));
			const auto pExtB = SWTypeExt::ExtMap.Find(SuperWeaponTypeClass::Array->GetItemOrDefault(b->SuperIndex));

			if (pExtB && (pExtB->SuperWeaponSidebar_PriorityHouses & ownerBits) && (!pExtA || !(pExtA->SuperWeaponSidebar_PriorityHouses & ownerBits)))
				return false;

			if ((!pExtB || !(pExtB->SuperWeaponSidebar_PriorityHouses & ownerBits)) && pExtA && (pExtA->SuperWeaponSidebar_PriorityHouses & ownerBits))
				return true;

			return BuildType::SortsBefore(AbstractType::Special, a->SuperIndex, AbstractType::Special, b->SuperIndex);
		});

	const int buttonCount = static_cast<int>(vec_Buttons.size());
	const int cameoWidth = 60, cameoHeight = 48;
	const int maximum = Phobos::UI::SuperWeaponSidebar_Max;
	const int cameoHarfInterval = (Phobos::UI::SuperWeaponSidebar_CameoHeight - cameoHeight) / 2;
	int location_Y = (DSurface::ViewBounds().Height - std::min(buttonCount, maximum) * Phobos::UI::SuperWeaponSidebar_CameoHeight) / 2;
	Point2D location = { Phobos::UI::SuperWeaponSidebar_LeftOffset, location_Y };
	location_Y -= cameoHarfInterval;
	int rowIdx = 0, columnIdx = 0;

	for (const auto button : vec_Buttons)
	{
		const auto column = columns[columnIdx];

		if (rowIdx == 0)
			column->SetPosition(location.X - Phobos::UI::SuperWeaponSidebar_LeftOffset, location.Y - (SideExt::ExtMap.Find(SideClass::Array->Items[ScenarioClass::Instance->PlayerSideIndex])->SuperWeaponSidebar_TopPCX.GetSurface() ? 20 : 0));

		column->Buttons.emplace_back(button);
		button->SetColumn(columnIdx);
		button->SetPosition(location.X, location.Y);
		rowIdx++;

		if (rowIdx >= maximum - columnIdx)
		{
			rowIdx = 0;
			columnIdx++;
			location_Y += Phobos::UI::SuperWeaponSidebar_CameoHeight / 2;
			location.X += cameoWidth + Phobos::UI::SuperWeaponSidebar_Interval;
			location.Y = location_Y + cameoHarfInterval;
		}
		else
		{
			location.Y += Phobos::UI::SuperWeaponSidebar_CameoHeight;
		}
	}

	for (const auto column : columns)
		column->SetHeight(column->Buttons.size() * Phobos::UI::SuperWeaponSidebar_CameoHeight);

	if (const auto toggleButton = this->ToggleButton)
		toggleButton->UpdatePosition();
}

int SWSidebarClass::GetMaximumButtonCount()
{
	const int firstColumn = Phobos::UI::SuperWeaponSidebar_Max;
	const int columns = std::min(firstColumn, Phobos::UI::SuperWeaponSidebar_MaxColumns);
	return (firstColumn + (firstColumn - (columns - 1))) * columns / 2;
}

bool SWSidebarClass::IsEnabled()
{
	return SidebarExt::Global()->SWSidebar_Enable;
}

// Hooks

DEFINE_HOOK(0x692419, DisplayClass_ProcessClickCoords_SWSidebar, 0x7)
{
	enum { DoNothing = 0x6925FC };

	if (SWSidebarClass::IsEnabled() && SWSidebarClass::Instance.CurrentColumn)
		return DoNothing;

	const auto toggleButton = SWSidebarClass::Instance.ToggleButton;

	return toggleButton && toggleButton->IsHovering ? DoNothing : 0;
}

DEFINE_HOOK(0x4F92FB, HouseClass_UpdateTechTree_SWSidebar, 0x7)
{
	enum { SkipGameCode = 0x4F9302 };

	GET(HouseClass*, pHouse, ESI);

	pHouse->AISupers();

	if (pHouse->IsCurrentPlayer())
	{
		auto& sidebar = SWSidebarClass::Instance;

		for (const auto& column : sidebar.Columns)
		{
			std::vector<int> removeButtons;

			for (const auto& button : column->Buttons)
			{
				if (HouseClass::CurrentPlayer->Supers[button->SuperIndex]->IsPresent)
					continue;

				removeButtons.push_back(button->SuperIndex);
			}

			for (const auto& index : removeButtons)
			{
				if (column->RemoveButton(index))
					SidebarExt::Global()->SWSidebar_Indices.Remove(index);
			}
		}

		SWSidebarClass::Instance.SortButtons();
		int removes = 0;

		for (const auto& column : sidebar.Columns)
		{
			if (column->Buttons.empty())
				++removes;
		}

		for (; removes > 0; --removes)
			sidebar.RemoveColumn();
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x6A6316, SidebarClass_AddCameo_SuperWeapon_SWSidebar, 0x6)
{
	enum { ReturnFalse = 0x6A65FF };

	GET_STACK(AbstractType, whatAmI, STACK_OFFSET(0x14, 0x4));
	GET_STACK(int, index, STACK_OFFSET(0x14, 0x8));

	switch (whatAmI)
	{
	case AbstractType::Super:
	case AbstractType::SuperWeaponType:
	case AbstractType::Special:
		if (SWSidebarClass::Instance.AddButton(index))
			return ReturnFalse;

		break;

	default:
		break;
	}

	return 0;
}

DEFINE_HOOK(0x6A5082, SidebarClass_InitClear_InitializeSWSidebar, 0x5)
{
	SWSidebarClass::Instance.InitClear();
	return 0;
}

DEFINE_HOOK(0x6A5839, SidebarClass_InitIO_InitializeSWSidebar, 0x5)
{
	if (!Phobos::UI::SuperWeaponSidebar || Unsorted::ArmageddonMode)
		return 0;

	if (const auto pSideExt = SideExt::ExtMap.Find(SideClass::Array->Items[ScenarioClass::Instance->PlayerSideIndex]))
	{
		if (const auto toggleButton = DLLCreate<ToggleSWButtonClass>(SWButtonClass::StartID + SuperWeaponTypeClass::Array->Count, 0, 0, 10, 50))
		{
			toggleButton->Zap();
			GScreenClass::Instance->AddButton(toggleButton);
			SWSidebarClass::Instance.ToggleButton = toggleButton;
			toggleButton->UpdatePosition();
		}
	}

	for (const auto superIdx : SidebarExt::Global()->SWSidebar_Indices)
		SWSidebarClass::Instance.AddButton(superIdx);

	return 0;
}
