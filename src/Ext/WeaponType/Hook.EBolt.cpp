#include "Body.h"
#include <EBolt.h>
#include <Utilities/Macro.h>
#include <Helpers/Macro.h>

PhobosMap<EBolt*, const WeaponTypeExt::ExtData*> WeaponTypeExt::BoltWeaponMap;
const WeaponTypeExt::ExtData* WeaponTypeExt::BoltWeaponType = nullptr;

DEFINE_HOOK(0x6FD494, TechnoClass_FireEBolt_SetExtMap_AfterAres, 0x7)
{
	GET_STACK(WeaponTypeClass*, pWeapon, STACK_OFFSET(0x30, 0x8));
	GET(EBolt*, pBolt, EAX);

	if (pWeapon)
		WeaponTypeExt::BoltWeaponMap[pBolt] = WeaponTypeExt::ExtMap.Find(pWeapon);

	return 0;
}

DEFINE_HOOK(0x4C2951, EBolt_DTOR, 0x5)
{
	GET(EBolt*, pBolt, ECX);

	WeaponTypeExt::BoltWeaponMap.erase(pBolt);

	return 0;
}

DEFINE_HOOK(0x4C20BC, EBolt_DrawArcs, 0xB)
{
	enum { DoLoop = 0x4C20C7, Break = 0x4C2400 };

	GET_STACK(EBolt*, pBolt, 0x40);
	WeaponTypeExt::BoltWeaponType = WeaponTypeExt::BoltWeaponMap.get_or_default(pBolt);

	GET_STACK(int, plotIndex, STACK_OFFSET(0x408, -0x3E0));

	int arcCount = WeaponTypeExt::BoltWeaponType ? WeaponTypeExt::BoltWeaponType->Bolt_Arcs : 8;

	return plotIndex < arcCount ? DoLoop : Break;
}

DEFINE_HOOK(0x4C24E4, Ebolt_DrawFist_Disable, 0x8)
{
	GET_STACK(EBolt*, pBolt, 0x40);
	WeaponTypeExt::BoltWeaponType = WeaponTypeExt::BoltWeaponMap.get_or_default(pBolt);

	return (WeaponTypeExt::BoltWeaponType && WeaponTypeExt::BoltWeaponType->Bolt_Disable1) ? 0x4C2515 : 0;
}

DEFINE_HOOK(0x4C25FD, Ebolt_DrawSecond_Disable, 0xA)
{
	return (WeaponTypeExt::BoltWeaponType && WeaponTypeExt::BoltWeaponType->Bolt_Disable2) ? 0x4C262A : 0;
}

DEFINE_HOOK(0x4C26EE, Ebolt_DrawThird_Disable, 0x8)
{
	return (WeaponTypeExt::BoltWeaponType && WeaponTypeExt::BoltWeaponType->Bolt_Disable3) ? 0x4C2710 : 0;
}
