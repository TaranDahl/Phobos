#include "HerosInfo.h"

#include <HouseClass.h>
#include <Utilities/GeneralUtils.h>

const char* HerosInfoCommandClass::GetName() const
{
	return "Heros Display";
}

const wchar_t* HerosInfoCommandClass::GetUIName() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_HEROS_INFO", L"heros display");
}

const wchar_t* HerosInfoCommandClass::GetUICategory() const
{
	return CATEGORY_CONTROL;
}

const wchar_t* HerosInfoCommandClass::GetUIDescription() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_HEROS_INFO_DESC", L"Switch between visible/invisible modes for heros info display");
}

void HerosInfoCommandClass::Execute(WWKey eInput) const
{
	Phobos::Config::HerosDisplay_Enable = !Phobos::Config::HerosDisplay_Enable;

	MessageListClass::Instance->PrintMessage
	(
		(Phobos::Config::HerosDisplay_Enable ?
			GeneralUtils::LoadStringUnlessMissing("TXT_HEROS_VISIBLE", L"Set heros info visible.") :
			GeneralUtils::LoadStringUnlessMissing("TXT_HEROS_INVISIBLE", L"Set heros info invisible.")),
		RulesClass::Instance->MessageDelay,
		HouseClass::CurrentPlayer->ColorSchemeIndex,
		true
	);
}
