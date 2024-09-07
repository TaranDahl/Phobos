
#include <UnitClass.h>

#include <Ext/Techno/Body.h>
#include <Ext/TechnoType/Body.h>
#include <Utilities/Macro.h>
#include <Utilities/TemplateDef.h>

DEFINE_HOOK(0x7464C5, UnitClass_ReceiveGunner_UpdateRecord, 0x6)
{
	GET(TechnoClass* const, pThis, ESI);
	GET(FootClass* const, pGunner, EDI);

	auto const pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->Gunner = pGunner;

	return 0;
}

DEFINE_HOOK(0x74652D, UnitClass_RemoveGunner_UpdateRecord, 0x6)
{
	GET(TechnoClass* const, pThis, EDI);
	//GET(FootClass* const, pGunner, ESI);

	auto const pExt = TechnoExt::ExtMap.Find(pThis);
	pExt->Gunner = 0;

	return 0;
}
/*
DEFINE_HOOK(0x7327B2, TechnoClass_PlayerOwnedAliveAndNamed_GunnerGroupAs, 0x5)
{
	enum { SameType = 0x7327B7, NotSameType = 0x7327BE };

	GET(TechnoClass* const, pThis, ESI);
	GET(const char*, pID, EDI);

	auto const pType = pThis->GetTechnoType();
	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

	bool isSameType;
	Debug::LogAndMessage("Here\n");
	Debug::LogAndMessage("%s\n", pType->ID);
	if (pType->Gunner && pTypeExt && pTypeExt->UseGunnerGroupAs)
	{
		auto const pGunner = TechnoExt::ExtMap.Find(pThis)->Gunner;
		//auto const pGunnerTypeExt = pGunner ? TechnoTypeExt::ExtMap.Find(pGunner->GetTechnoType()) : 0;
		Debug::LogAndMessage("%s\n", TechnoTypeExt::GetGunnerGroupID(pGunner->GetTechnoType()));
		isSameType = TechnoTypeExt::HasGunnerGroupID(pGunner->GetTechnoType(), pID);
	}
	else
	{
		isSameType = TechnoTypeExt::HasSelectionGroupID(pThis->GetTechnoType(), pID);
	}
	
	if (isSameType)
	{
		__asm { pop edi }
		return isSameType;
	}

	return NotSameType;
}*/

DEFINE_HOOK_AGAIN(0x4ABD9D, DisplayClass_LeftMouseButtonUp_GunnerGroupAs, A)
DEFINE_HOOK_AGAIN(0x4ABE58, DisplayClass_LeftMouseButtonUp_GunnerGroupAs, A)
DEFINE_HOOK(0x4ABD6C, DisplayClass_LeftMouseButtonUp_GunnerGroupAs, A)
{
	GET(ObjectClass*, pThis, ESI);
	R->EAX(TechnoTypeExt::GetSelectionGroupID(pThis->GetType()));
	return R->Origin() + 13;
}

DEFINE_HOOK(0x6DA665, sub_6DA5C0_GunnerGroupAs, A)
{
	GET(ObjectClass*, pThis, ESI);
	R->EAX(TechnoTypeExt::GetSelectionGroupID(pThis->GetType()));
	return R->Origin() + 13;
}
