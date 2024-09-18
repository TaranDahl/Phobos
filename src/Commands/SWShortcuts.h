#pragma once

#include "Commands.h"

#include <Utilities/GeneralUtils.h>
#include <Misc/TacticalButtons.h>

// Super Weapon Sidebar Keyboard shortcut command class
template<size_t KeyIndex>
class SWShortcutsCommandClass : public CommandClass
{
public:
	// CommandClass
	virtual const char* GetName() const override;
	virtual const wchar_t* GetUIName() const override;
	virtual const wchar_t* GetUICategory() const override;
	virtual const wchar_t* GetUIDescription() const override;
	virtual void Execute(WWKey eInput) const override;
};

template<size_t KeyIndex>
inline const char* SWShortcutsCommandClass<KeyIndex>::GetName() const
{
	if (KeyIndex > 0)
	{
		char name[29];
		sprintf_s(name, "SW Sidebar Shortcuts Num %2d", KeyIndex);
		return name;
	}

	return "SW Sidebar Display";
}

template<size_t KeyIndex>
inline const wchar_t* SWShortcutsCommandClass<KeyIndex>::GetUIName() const
{
	if (KeyIndex > 0)
	{
		wchar_t name[20];
		swprintf_s(name, L"Quick Select SW %2d", KeyIndex);
		return StringTable::TryFetchString("TXT_SW_XX_FORWARD", name);
	}

	return GeneralUtils::LoadStringUnlessMissing("TXT_SELECT_CAPTURED", L"SW sidebar display");
}

template<size_t KeyIndex>
inline const wchar_t* SWShortcutsCommandClass<KeyIndex>::GetUICategory() const
{
	return CATEGORY_CONTROL;
}

template<size_t KeyIndex>
inline const wchar_t* SWShortcutsCommandClass<KeyIndex>::GetUIDescription() const
{
	if (KeyIndex > 0)
	{
		wchar_t name[34];
		swprintf_s(name, L"Select No.%02d SW in left sidebar", KeyIndex);
		return StringTable::TryFetchString("TXT_SW_XX_FORWARD_DESC", name);
	}

	return GeneralUtils::LoadStringUnlessMissing("TXT_SELECT_CAPTURED_DESC", L"Switch between visible/invisible modes for exclusive SW sidebar");
}

template<size_t KeyIndex>
inline void SWShortcutsCommandClass<KeyIndex>::Execute(WWKey eInput) const
{
	if (KeyIndex > 0)
	{
		TacticalButtonClass::Instance.KeyboardCall = true;
		TacticalButtonClass::TriggerButtonForSW(KeyIndex);
		return;
	}

	TacticalButtonClass::Instance.SuperVisible = !TacticalButtonClass::Instance.SuperVisible;
	TacticalButtonClass::RecheckButtonIndex();

	MessageListClass::Instance->PrintMessage
	(
		(TacticalButtonClass::Instance.SuperVisible ?
			GeneralUtils::LoadStringUnlessMissing("MSG:SWSidebarVisible", L"Set exclusive SW sidebar visible.") :
			GeneralUtils::LoadStringUnlessMissing("MSG:SWSidebarInvisible", L"Set exclusive SW sidebar invisible.")),
		RulesClass::Instance->MessageDelay,
		HouseClass::CurrentPlayer->ColorSchemeIndex,
		true
	);
}
