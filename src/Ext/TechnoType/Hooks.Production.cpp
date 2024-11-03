#include "Body.h"
#include <Ext/House/Body.h>

DEFINE_HOOK(0x5F7A89, ObjectTypeClass_FindFactory_End, 0x5)
{
	GET(ObjectTypeClass* const, pType, ECX);
	GET_STACK(bool, allowOccupied, STACK_OFFSET(0, 0x4));
	GET_STACK(bool, requirePower, STACK_OFFSET(0, 0x8));
	GET_STACK(bool, requireCanBuild, STACK_OFFSET(0, 0xC));
	GET_STACK(HouseClass* const, pHouse, STACK_OFFSET(0, 0x10));



	return 0;
}
/*
{
	GET(AircraftTypeClass* const, pType, EDI);
	GET(BuildingClass* const, pBuilding, ESI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

	if (pTypeExt && pTypeExt->ThisIsAJumpjet)
	{
		R->EAX(true);
	}
	else
	{
		R->EAX(pBuilding->HasFreeLink());
	}

	return 0x5F79F9;
}

DEFINE_HOOK(0x5F79C7, ObjectTypeClass_FindFactory_ThisIsAJumpjet2, 0x6)
{
	GET(TechnoTypeClass* const, pType, EDI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

	if (pTypeExt && pTypeExt->ThisIsAJumpjet)
	{
		return 0x5F7A09;
	}

	return 0;
}*/
DEFINE_HOOK(0x443C71, BuildingClass_KickOutUnit_ThisIsAJumpjet, 0x6)
{
	GET(TechnoClass* const, pProduct, EDI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pProduct->GetTechnoType());

	if (pTypeExt && pTypeExt->ThisIsAJumpjet)
	{
		R->EDI(pTypeExt->ThisIsAJumpjet->CreateObject(pProduct->Owner));
		pProduct->UnInit();
	}

	return 0;
}

DEFINE_HOOK(0x44409C, BuildingClass_KickOutUnit_ImAJumpjetFromAirport1, 0x6)
{
	GET(TechnoClass* const, pProduct, EDI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pProduct->GetTechnoType());

	if (pTypeExt && pTypeExt->ImAJumpjetFromAirport)
	{
		return 0x444159;
	}

	return 0;
}

DEFINE_HOOK(0x4445FB, BuildingClass_KickOutUnit_ImAJumpjetFromAirport2, 0x6)
{
	GET(TechnoClass* const, pProduct, EDI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pProduct->GetTechnoType());

	if (pTypeExt && pTypeExt->ImAJumpjetFromAirport)
	{
		return 0x444638;
	}

	return 0;
}
