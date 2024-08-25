#include <Ext/Techno/Body.h>

// Hooks that allow harvesters / weeders to work correctly with MovementZone=Subterannean (sic) - Starkku
#pragma region SubterraneanHarvesters

// Allow scanning for docks in all map zones.
DEFINE_HOOK(0x4DEFC6, FootClass_FindDock_SubterraneanHarvester, 0x5)
{
	GET(TechnoTypeClass*, pTechnoType, EAX);

	if (auto const pUnitType = abstract_cast<UnitTypeClass*>(pTechnoType))
	{
		if ((pUnitType->Harvester || pUnitType->Weeder) && pUnitType->MovementZone == MovementZone::Subterrannean)
			R->ECX(MovementZone::Fly);
	}

	return 0;
}

// Allow scanning for ore in all map zones.
DEFINE_HOOK(0x4DCF86, FootClass_FindTiberium_SubterraneanHarvester, 0x5)
{
	enum { SkipGameCode = 0x4DCF9B };

	GET(MovementZone, mZone, ECX);

	if (mZone == MovementZone::Subterrannean)
		R->ECX(MovementZone::Fly);

	return 0;
}

// Allow scanning for weeds in all map zones.
DEFINE_HOOK(0x4DDB23, FootClass_FindWeeds_SubterraneanHarvester, 0x5)
{
	enum { SkipGameCode = 0x4DCF9B };

	GET(MovementZone, mZone, EAX);

	if (mZone == MovementZone::Subterrannean)
		R->EAX(MovementZone::Fly);

	return 0;
}

// Set rally point.
DEFINE_HOOK(0x44459A, BuildingClass_ExitObject_SubterraneanHarvester, 0x5)
{
	GET(TechnoClass*, pThis, EDI);

	if (auto const pUnit = abstract_cast<UnitClass*>(pThis))
	{
		auto const pType = pUnit->Type;

		if ((pType->Harvester || pType->Weeder) && pType->MovementZone == MovementZone::Subterrannean)
		{
			auto const pExt = TechnoExt::ExtMap.Find(pUnit);
			pExt->SubterraneanHarvFreshFromFactory = true;
			pExt->SubterraneanHarvRallyDest = pUnit->ArchiveTarget;
		}
	}

	return 0;
}

// Handle rally point once idle.
DEFINE_HOOK(0x7389B1, UnitClass_EnterIdleMode_SubterraneanHarvester, 0x6)
{
	enum { ReturnFromFunction = 0x738D21 };

	GET(UnitClass*, pThis, ESI);

	auto const pExt = TechnoExt::ExtMap.Find(pThis);

	if (pExt->SubterraneanHarvFreshFromFactory)
	{
		pThis->SetArchiveTarget(nullptr);
		pThis->ClearNavigationList();

		if (pThis->Destination && pExt->SubterraneanHarvRallyDest && pThis->Destination != pExt->SubterraneanHarvRallyDest && pThis->DistanceFrom(pThis->Destination) > Unsorted::LeptonsPerCell)
			pThis->SetDestination(pExt->SubterraneanHarvRallyDest, false);
		else
			pThis->SetDestination(nullptr, false);

		pExt->SubterraneanHarvFreshFromFactory = false;
		pExt->SubterraneanHarvRallyDest = nullptr;

		return ReturnFromFunction;
	}

	return 0;
}

#pragma endregion
