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
	if (!KeyIndex)
		return "SW Sidebar Display";

	_snprintf_s(Phobos::readBuffer, Phobos::readLength, "SW Sidebar Shortcuts Num %02d", KeyIndex);
	return Phobos::readBuffer;
}

template<size_t KeyIndex>
inline const wchar_t* SWShortcutsCommandClass<KeyIndex>::GetUIName() const
{
	if (!KeyIndex)
		return GeneralUtils::LoadStringUnlessMissing("TXT_EX_SW_SWITCH", L"SW sidebar display");

	_snprintf_s(Phobos::readBuffer, Phobos::readLength, "TXT_EX_SW_BUTTON_%02d", KeyIndex);
	_snwprintf_s(Phobos::wideBuffer, std::size(Phobos::wideBuffer), L"Quick Select SW %02d", KeyIndex);
	return GeneralUtils::LoadStringUnlessMissing(Phobos::readBuffer, Phobos::wideBuffer);
}

template<size_t KeyIndex>
inline const wchar_t* SWShortcutsCommandClass<KeyIndex>::GetUICategory() const
{
	return CATEGORY_CONTROL;
}

template<size_t KeyIndex>
inline const wchar_t* SWShortcutsCommandClass<KeyIndex>::GetUIDescription() const
{
	if (!KeyIndex)
		return GeneralUtils::LoadStringUnlessMissing("TXT_EX_SW_SWITCH_DESC", L"Switch between visible/invisible modes for exclusive SW sidebar");

	_snprintf_s(Phobos::readBuffer, Phobos::readLength, "TXT_EX_SW_BUTTON_%02d_DESC", KeyIndex);
	_snwprintf_s(Phobos::wideBuffer, std::size(Phobos::wideBuffer), L"Select No.%02d SW in left sidebar", KeyIndex);
	return GeneralUtils::LoadStringUnlessMissing(Phobos::readBuffer, Phobos::wideBuffer);
}

template<size_t KeyIndex>
inline void SWShortcutsCommandClass<KeyIndex>::Execute(WWKey eInput) const
{
	if (KeyIndex > 0)
	{
		TacticalButtonsClass::Instance.KeyboardCall = true;
		TacticalButtonsClass::Instance.SWSidebarTrigger(KeyIndex);
		return;
	}

	TacticalButtonsClass::Instance.SWSidebarSwitch();
}
