#include "DistributionMode.h"

#include <Ext/TechnoType/Body.h>
#include <Utilities/Helpers.Alex.h>
#include <Helpers/Macro.h>

int DistributionModeCommandClass::Mode = 0; // 0 = Off, 1 = Small range and same type only, 2 = Small range, 3 = Big range

const char* DistributionModeCommandClass::GetName() const
{
	return "Distribution Mode";
}

const wchar_t* DistributionModeCommandClass::GetUIName() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_DISTRIBUTION", L"Distribution mode");
}

const wchar_t* DistributionModeCommandClass::GetUICategory() const
{
	return CATEGORY_CONTROL;
}

const wchar_t* DistributionModeCommandClass::GetUIDescription() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_DISTRIBUTION_DESC", L"Automatically and averagely select similar targets around the original target");
}

void DistributionModeCommandClass::Execute(WWKey eInput) const
{
	DistributionModeCommandClass::Mode = ((DistributionModeCommandClass::Mode + 1) & 3);
	wchar_t text[0x20];
	swprintf_s(text, L"Distribution Mode < %d >", DistributionModeCommandClass::Mode);
	MessageListClass::Instance->PrintMessage(text, 600, 5, true);
}

DEFINE_HOOK(0x4AE818, DisplayClass_sub_4AE750_AutoDistribution, 0xA)
{
	enum { SkipGameCode = 0x4AE85C };

	GET(ObjectClass* const, pTarget, EBP);
	GET_STACK(const Action, action, STACK_OFFSET(0x20, 0xC));

	const auto count = ObjectClass::CurrentObjects->Count;

	if (count > 0)
	{
		const int mode = DistributionModeCommandClass::Mode;

		if (mode && count > 1 && action != Action::NoMove && (pTarget->AbstractFlags & AbstractFlags::Techno) != AbstractFlags::None && !pTarget->IsInAir())
		{
			const double range = (mode == 3) ? 15 : 7.5;
			const auto pItems = Helpers::Alex::getCellSpreadItems(pTarget->Location, range, false);
			std::map<TechnoClass*, int> record;
			int current = 1;

			for (const auto& pItem : pItems)
				record[pItem] = 0;

			for (const auto& pSelect : ObjectClass::CurrentObjects())
			{
				ObjectClass* pCanTarget = nullptr;
				ObjectClass* pNewTarget = nullptr;

				for (auto& [pItem, num] : record)
				{
					if (pSelect->MouseOverObject(pItem, false) == action && (mode != 1 || TechnoTypeExt::GetSelectionGroupID(pItem->GetTechnoType()) == TechnoTypeExt::GetSelectionGroupID(pTarget->GetTechnoType())))
					{
						pCanTarget = pItem;

						if (num < current)
						{
							++num;
							pNewTarget = pCanTarget;
							break;
						}
					}
				}

				if (!pNewTarget)
				{
					if (pCanTarget)
					{
						++current;
						pNewTarget = pCanTarget;
					}
				}

				if (pNewTarget)
					pSelect->ObjectClickedAction(action, pNewTarget, false);
				else
					pSelect->ObjectClickedAction(pSelect->MouseOverObject(pTarget, false), pTarget, false);

				Unsorted::MoveFeedback = false;
			}

			return SkipGameCode;
		}

		for (const auto& pSelect : ObjectClass::CurrentObjects())
		{
			pSelect->ObjectClickedAction(pSelect->MouseOverObject(pTarget, false), pTarget, false);
			Unsorted::MoveFeedback = false;
		}
	}

	return SkipGameCode;
}
