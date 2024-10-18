#include "Body.h"

#include <Ext/WarheadType/Body.h>
#include <Ext/WeaponType/Body.h>
#include <Ext/TEvent/Body.h>
#include <Ext/House/Body.h>

#include <VoxClass.h>
#include <RadarEventClass.h>
#include <TacticalClass.h>

namespace ReceiveDamageTemp
{
	bool SkipLowDamageCheck = false;
}

// #issue 88 : shield logic
DEFINE_HOOK(0x701900, TechnoClass_ReceiveDamage_Shield, 0x6)
{
	GET(TechnoClass*, pThis, ECX);
	LEA_STACK(args_ReceiveDamage*, args, 0x4);

	auto const pRules = RulesExt::Global();
	auto const pWHExt = WarheadTypeExt::ExtMap.Find(args->WH);

	if (pRules->CombatAlert && *args->Damage > 1 && !pWHExt->CombatAlert_Suppress.Get(!pWHExt->Malicious || pWHExt->Nonprovocative))
	{
		do
		{
			const auto pHouse = pThis->Owner;

			if (!pHouse || (!pThis->IsOwnedByCurrentPlayer && !pHouse->IsInPlayerControl) || !pThis->IsInPlayfield)
				break;

			const auto pSourceHouse = args->SourceHouse;

			if (pRules->CombatAlert_SuppressIfAllyDamage && pHouse->IsAlliedWith(pSourceHouse))
				break;

			const auto pHouseExt = HouseExt::ExtMap.Find(pHouse);

			if (pHouseExt->CombatAlertTimer.HasTimeLeft())
				break;

			const auto pType = pThis->GetTechnoType();
			const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

			if (!pTypeExt || !pTypeExt->CombatAlert.Get(!pType->Insignificant && !pType->Spawned))
				break;

			const auto pBuilding = pThis->WhatAmI() == AbstractType::Building ? static_cast<BuildingClass*>(pThis) : nullptr;

			if (pRules->CombatAlert_IgnoreBuilding && pBuilding && !pTypeExt->CombatAlert_NotBuilding.Get(pBuilding->Type->IsVehicle()))
				break;

			const CoordStruct coordInMap = pThis->GetCoords();

			if (pRules->CombatAlert_SuppressIfInScreen)
			{
				TacticalClass* const pTactical = TacticalClass::Instance;
				const Point2D coordInScreen = pTactical->CoordsToScreen(coordInMap) - pTactical->TacticalPos;
				const RectangleStruct screenArea = DSurface::Composite->GetRect();

				if (screenArea.Width >= coordInScreen.X && screenArea.Height >= coordInScreen.Y && coordInScreen.X >= 0 && coordInScreen.Y >= 0) // check if the unit is in screen
					break;
			}

			pHouseExt->CombatAlertTimer.Start(pRules->CombatAlert_Interval);
			RadarEventClass::Create(RadarEventType::Combat, CellClass::Coord2Cell(coordInMap));
			int index = -1;

			if (!pRules->CombatAlert_MakeAVoice) // No one want to play two sound at a time, I guess?
				break;
			else if (pTypeExt->CombatAlert_UseFeedbackVoice.Get(pRules->CombatAlert_UseFeedbackVoice) && pType->VoiceFeedback.Count > 0) // Use VoiceFeedback first
				VocClass::PlayGlobal(pType->VoiceFeedback.GetItem(0), 0x2000, 1.0);
			else if (pTypeExt->CombatAlert_UseAttackVoice.Get(pRules->CombatAlert_UseAttackVoice) && pType->VoiceAttack.Count > 0) // Use VoiceAttack then
				VocClass::PlayGlobal(pType->VoiceAttack.GetItem(0), 0x2000, 1.0);
			else if (pTypeExt->CombatAlert_UseEVA.Get(pRules->CombatAlert_UseEVA)) // Use Eva finally
				index = pTypeExt->CombatAlert_EVA.Get(VoxClass::FindIndex((const char*)"EVA_UnitsInCombat"));

			if (index != -1)
				VoxClass::PlayIndex(index);
		}
		while (false);
	}

	if (!*args->Damage || args->IgnoreDefenses)
		return 0;

	//Calculate Damage Multiplier
	if (pWHExt)
	{
		const auto pFirerHouse = pThis->Owner;
		const auto pTargetHouse = args->SourceHouse;
		double multiplier = 1.0;

		if (!pFirerHouse || !pTargetHouse || !pFirerHouse->IsAlliedWith(pTargetHouse))
			multiplier = pWHExt->DamageEnemiesMultiplier.Get(pRules->DamageEnemiesMultiplier);
		else if (pFirerHouse != pTargetHouse)
			multiplier = pWHExt->DamageAlliesMultiplier.Get(pRules->DamageAlliesMultiplier);
		else
			multiplier = pWHExt->DamageOwnerMultiplier.Get(pRules->DamageOwnerMultiplier);

		if (multiplier != 1.0)
		{
			const int sgnDamage = *args->Damage > 0 ? 1 : -1;
			const int calculateDamage = static_cast<int>(*args->Damage * multiplier);
			*args->Damage = calculateDamage ? calculateDamage : sgnDamage;
		}
	}

	//Shield Receive Damage
	const auto pExt = TechnoExt::ExtMap.Find(pThis);

	if (const auto pShieldData = pExt->Shield.get())
	{
		if (!pShieldData->IsActive())
			return 0;

		const int nDamageLeft = pShieldData->ReceiveDamage(args);
		if (nDamageLeft >= 0)
		{
			*args->Damage = nDamageLeft;

			if (auto pTag = pThis->AttachedTag)
				pTag->RaiseEvent((TriggerEvent)PhobosTriggerEvent::ShieldBroken, pThis, CellStruct::Empty);
		}

		if (nDamageLeft == 0)
			ReceiveDamageTemp::SkipLowDamageCheck = true;
	}

	return 0;
}

DEFINE_HOOK(0x7019D8, TechnoClass_ReceiveDamage_SkipLowDamageCheck, 0x5)
{
	if (ReceiveDamageTemp::SkipLowDamageCheck)
	{
		ReceiveDamageTemp::SkipLowDamageCheck = false;
	}
	else
	{
		// Restore overridden instructions
		GET(int*, nDamage, EBX);
		if (*nDamage < 1)
			*nDamage = 1;
	}

	return 0x7019E3;
}

DEFINE_HOOK(0x702819, TechnoClass_ReceiveDamage_Decloak, 0xA)
{
	GET(TechnoClass* const, pThis, ESI);
	GET_STACK(WarheadTypeClass*, pWarhead, STACK_OFFSET(0xC4, 0xC));

	if (auto pExt = WarheadTypeExt::ExtMap.Find(pWarhead))
	{
		if (pExt->DecloakDamagedTargets)
			pThis->Uncloak(false);
	}

	return 0x702823;
}

DEFINE_HOOK(0x701DFF, TechnoClass_ReceiveDamage_FlyingStrings, 0x7)
{
	GET(TechnoClass* const, pThis, ESI);
	GET(int* const, pDamage, EBX);

	if (Phobos::DisplayDamageNumbers && *pDamage)
		GeneralUtils::DisplayDamageNumberString(*pDamage, DamageDisplayType::Regular, pThis->GetRenderCoords(), TechnoExt::ExtMap.Find(pThis)->DamageNumberOffset);

	return 0;
}

DEFINE_HOOK(0x702603, TechnoClass_ReceiveDamage_Explodes, 0x6)
{
	enum { SkipExploding = 0x702672, SkipKillingPassengers = 0x702669 };

	GET(TechnoClass*, pThis, ESI);

	const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	if (pThis->WhatAmI() == AbstractType::Building)
	{
		if (!pTypeExt->Explodes_DuringBuildup && (pThis->CurrentMission == Mission::Construction || pThis->CurrentMission == Mission::Selling))
			return SkipExploding;
	}

	if (!pTypeExt->Explodes_KillPassengers)
		return SkipKillingPassengers;

	return 0;
}

DEFINE_HOOK(0x702672, TechnoClass_ReceiveDamage_RevengeWeapon, 0x5)
{
	GET(TechnoClass*, pThis, ESI);
	GET_STACK(TechnoClass*, pSource, STACK_OFFSET(0xC4, 0x10));
	GET_STACK(WarheadTypeClass*, pWarhead, STACK_OFFSET(0xC4, 0xC));

	if (pSource)
	{
		auto const pExt = TechnoExt::ExtMap.Find(pThis);
		auto const pTypeExt = pExt->TypeExtData;
		auto const pWHExt = WarheadTypeExt::ExtMap.Find(pWarhead);
		bool hasFilters = pWHExt->SuppressRevengeWeapons_Types.size() > 0;

		if (pTypeExt && pTypeExt->RevengeWeapon && EnumFunctions::CanTargetHouse(pTypeExt->RevengeWeapon_AffectsHouses, pThis->Owner, pSource->Owner))
		{
			if (!pWHExt->SuppressRevengeWeapons || (hasFilters && !pWHExt->SuppressRevengeWeapons_Types.Contains(pTypeExt->RevengeWeapon)))
				WeaponTypeExt::DetonateAt(pTypeExt->RevengeWeapon, pSource, pThis);
		}

		for (auto& attachEffect : pExt->AttachedEffects)
		{
			if (!attachEffect->IsActive())
				continue;

			auto const pType = attachEffect->GetType();

			if (!pType->RevengeWeapon)
				continue;

			if (pWHExt->SuppressRevengeWeapons && (!hasFilters || pWHExt->SuppressRevengeWeapons_Types.Contains(pType->RevengeWeapon)))
				continue;

			if (EnumFunctions::CanTargetHouse(pType->RevengeWeapon_AffectsHouses, pThis->Owner, pSource->Owner))
				WeaponTypeExt::DetonateAt(pType->RevengeWeapon, pSource, pThis);
		}
	}

	return 0;
}

// Issue #237 NotHuman additional animations support
// Author: Otamaa
DEFINE_HOOK(0x518505, InfantryClass_ReceiveDamage_NotHuman, 0x4)
{
	GET(InfantryClass* const, pThis, ESI);
	REF_STACK(args_ReceiveDamage const, receiveDamageArgs, STACK_OFFSET(0xD0, 0x4));

	// Die1-Die5 sequences are offset by 10
	constexpr auto Die = [](int x) { return x + 10; };

	int resultSequence = Die(1);
	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	if (pTypeExt->NotHuman_RandomDeathSequence.Get())
		resultSequence = ScenarioClass::Instance->Random.RandomRanged(Die(1), Die(5));

	if (receiveDamageArgs.WH)
	{
		if (auto const pWarheadExt = WarheadTypeExt::ExtMap.Find(receiveDamageArgs.WH))
		{
			int whSequence = pWarheadExt->NotHuman_DeathSequence.Get();
			if (whSequence > 0)
				resultSequence = Math::min(Die(whSequence), Die(5));
		}
	}

	R->ECX(pThis);
	pThis->PlayAnim(static_cast<Sequence>(resultSequence), true);

	return 0x518515;
}

DEFINE_HOOK(0x702050, TechnoClass_ReceiveDamage_AttachEffectExpireWeapon, 0x6)
{
	GET(TechnoClass*, pThis, ESI);

	auto const pExt = TechnoExt::ExtMap.Find(pThis);
	std::set<AttachEffectTypeClass*> cumulativeTypes;
	std::vector<WeaponTypeClass*> expireWeapons;

	for (auto const& attachEffect : pExt->AttachedEffects)
	{
		auto const pType = attachEffect->GetType();

		if (pType->ExpireWeapon && (pType->ExpireWeapon_TriggerOn & ExpireWeaponCondition::Death) != ExpireWeaponCondition::None)
		{
			if (!pType->Cumulative || !pType->ExpireWeapon_CumulativeOnlyOnce || !cumulativeTypes.contains(pType))
			{
				if (pType->Cumulative && pType->ExpireWeapon_CumulativeOnlyOnce)
					cumulativeTypes.insert(pType);

				expireWeapons.push_back(pType->ExpireWeapon);
			}
		}
	}

	auto const coords = pThis->GetCoords();
	auto const pOwner = pThis->Owner;

	for (auto const& pWeapon : expireWeapons)
	{
		WeaponTypeExt::DetonateAt(pWeapon, coords, pThis, pOwner, pThis);
	}

	return 0;
}

DEFINE_HOOK(0x701E18, TechnoClass_ReceiveDamage_ReflectDamage, 0x7)
{
	GET(TechnoClass*, pThis, ESI);
	GET(int*, pDamage, EBX);
	GET_STACK(TechnoClass*, pSource, STACK_OFFSET(0xC4, 0x10));
	GET_STACK(HouseClass*, pSourceHouse, STACK_OFFSET(0xC4, 0x1C));
	GET_STACK(WarheadTypeClass*, pWarhead, STACK_OFFSET(0xC4, 0xC));

	auto const pExt = TechnoExt::ExtMap.Find(pThis);
	auto const pWHExt = WarheadTypeExt::ExtMap.Find(pWarhead);

	if (pWHExt->Reflected)
		return 0;

	if (pExt->AE.ReflectDamage && *pDamage > 0 && (!pWHExt->SuppressReflectDamage || pWHExt->SuppressReflectDamage_Types.size() > 0))
	{
		for (auto& attachEffect : pExt->AttachedEffects)
		{
			if (!attachEffect->IsActive())
				continue;

			auto const pType = attachEffect->GetType();

			if (!pType->ReflectDamage)
				continue;

			if (pWHExt->SuppressReflectDamage && pWHExt->SuppressReflectDamage_Types.Contains(pType))
				continue;

			auto const pWH = pType->ReflectDamage_Warhead.Get(RulesClass::Instance->C4Warhead);
			int damage = static_cast<int>(*pDamage * pType->ReflectDamage_Multiplier);

			if (EnumFunctions::CanTargetHouse(pType->ReflectDamage_AffectsHouses, pThis->Owner, pSourceHouse))
			{
				auto const pWHExtRef = WarheadTypeExt::ExtMap.Find(pWH);
				pWHExtRef->Reflected = true;

				if (pType->ReflectDamage_Warhead_Detonate)
					WarheadTypeExt::DetonateAt(pWH, pSource, pThis, damage, pThis->Owner);
				else
					pSource->ReceiveDamage(&damage, 0, pWH, pThis, false, false, pThis->Owner);

				pWHExtRef->Reflected = false;
			}
		}
	}

	return 0;
}
