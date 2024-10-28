#include "Body.h"

#include <Ext/Building/Body.h>
#include <Ext/WeaponType/Body.h>

#pragma region NoBurstDelay

DEFINE_HOOK(0x5209EE, InfantryClass_UpdateFiring_BurstNoDelay, 0x5)
{
	enum { SkipVanillaFire = 0x520A57 };

	GET(InfantryClass* const, pThis, EBP);
	GET(const int, wpIdx, ESI);
	GET(AbstractClass* const, pTarget, EAX);

	if (const WeaponTypeClass* const pWeapon = pThis->GetWeapon(wpIdx)->WeaponType)
	{
		if (pWeapon->Burst > 1)
		{
			if (const WeaponTypeExt::ExtData* const pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon))
			{
				if (pWeaponExt->Burst_NoDelay)
				{
					if (pThis->Fire(pTarget, wpIdx))
					{
						int rof = pThis->RearmTimer.TimeLeft;
						pThis->RearmTimer.Start(0);

						for (int i = 1; i != pWeapon->Burst && pThis->GetFireError(pTarget, wpIdx, true) == FireError::OK && pThis->Fire(pTarget, wpIdx); ++i)
						{
							rof = pThis->RearmTimer.TimeLeft;
							pThis->RearmTimer.Start(0);
						}

						pThis->RearmTimer.Start(rof);
					}

					return SkipVanillaFire;
				}
			}
		}
	}

	return 0;
}

DEFINE_HOOK(0x736F67, UnitClass_UpdateFiring_BurstNoDelay, 0x6)
{
	enum { SkipVanillaFire = 0x737063 };

	GET(UnitClass* const, pThis, ESI);
	GET(const int, wpIdx, EDI);
	GET(AbstractClass* const, pTarget, EAX);

	if (const WeaponTypeClass* const pWeapon = pThis->GetWeapon(wpIdx)->WeaponType)
	{
		if (pWeapon->Burst > 1)
		{
			if (const WeaponTypeExt::ExtData* const pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon))
			{
				if (pWeaponExt->Burst_NoDelay)
				{
					if (pThis->Fire(pTarget, wpIdx))
					{
						int rof = pThis->RearmTimer.TimeLeft;
						pThis->RearmTimer.Start(0);

						for (int i = 1; i != pWeapon->Burst && pThis->GetFireError(pTarget, wpIdx, true) == FireError::OK && pThis->Fire(pTarget, wpIdx); ++i)
						{
							rof = pThis->RearmTimer.TimeLeft;
							pThis->RearmTimer.Start(0);
						}

						pThis->RearmTimer.Start(rof);
					}

					return SkipVanillaFire;
				}
			}
		}
	}

	return 0;
}

#pragma endregion

#pragma region KickOutStuckUnits

// Kick out stuck units when the factory building is not busy
DEFINE_HOOK(0x450248, BuildingClass_UpdateFactory_KickOutStuckUnits, 0x6)
{
	GET(BuildingClass*, pThis, ESI);

	if (!(Unsorted::CurrentFrame % 15))
	{
		BuildingTypeClass* const pType = pThis->Type;

		if (pType->Factory == AbstractType::UnitType && pType->WeaponsFactory && !pType->Naval && pThis->QueuedMission != Mission::Unload)
		{
			const Mission mission = pThis->CurrentMission;

			if (mission == Mission::Guard || (mission == Mission::Unload && pThis->MissionStatus == 1))
				BuildingExt::KickOutStuckUnits(pThis);
		}
	}

	return 0;
}

// Should not kick out units if the factory building is in construction process
DEFINE_HOOK(0x4444A0, BuildingClass_KickOutUnit_NoKickOutInConstruction, 0xA)
{
	enum { ThisIsOK = 0x444565, ThisIsNotOK = 0x4444B3};

	GET(BuildingClass* const, pThis, ESI);

	const Mission mission = pThis->GetCurrentMission();

	return (mission == Mission::Unload || mission == Mission::Construction) ? ThisIsNotOK : ThisIsOK;
}

#pragma endregion

#pragma region AIConstructionYard

DEFINE_HOOK(0x740A11, UnitClass_Mission_Guard_AINonAutoDeploy, 0x6)
{
	enum { SkipGameCode = 0x740A50 };

	GET(UnitClass*, pMCV, ESI);

	return (RulesExt::Global()->AINonAutoDeploy && pMCV->Owner->NumConYards > 0) ? SkipGameCode : 0;
}

DEFINE_HOOK(0x739889, UnitClass_TryToDeploy_AISetBaseCenter, 0x6)
{
	enum { SkipGameCode = 0x73992B };

	GET(UnitClass*, pMCV, EBP);

	return (RulesExt::Global()->AISetBaseCenter && pMCV->Owner->NumConYards > 1) ? SkipGameCode : 0;
}

DEFINE_HOOK(0x4FD538, HouseClass_AIHouseUpdate_CheckAIBaseCenter, 0x7)
{
	if (RulesExt::Global()->AIBiasSpawnCell && SessionClass::Instance->GameMode != GameMode::Campaign)
	{
		GET(HouseClass*, pAI, EBX);

		if (const int count = pAI->ConYards.Count)
		{
			const int wayPoint = pAI->GetSpawnPosition();

			if (wayPoint != -1)
			{
				const CellStruct center = ScenarioClass::Instance->GetWaypointCoords(wayPoint);
				CellStruct newCenter = center;
				double distanceSquared = 131072.0;

				for (int i = 0; i < count; ++i)
				{
					if (BuildingClass* const pBuilding = pAI->ConYards.GetItem(i))
					{
						if (pBuilding->IsAlive && pBuilding->Health && !pBuilding->InLimbo)
						{
							const double newDistanceSquared = pBuilding->GetMapCoords().DistanceFromSquared(center);

							if (newDistanceSquared < distanceSquared)
							{
								distanceSquared = newDistanceSquared;
								newCenter = pBuilding->GetMapCoords();
							}
						}
					}
				}

				if (newCenter != center)
				{
					pAI->BaseSpawnCell = newCenter;
					pAI->Base.BaseNodes.Items->MapCoords = newCenter;
					pAI->Base.Center = newCenter;
				}
			}
		}
	}

	return 0;
}

#pragma endregion

#pragma region GattlingNoRateDown

DEFINE_HOOK(0x70DE40, BuildingClass_sub_70DE40_GattlingNoRateDown, 0xA)
{
	enum { Return = 0x70DE62 };

	GET(BuildingClass* const, pThis, ECX);
	GET_STACK(int, rateDown, STACK_OFFSET(0x0, 0x4));

	do
	{
		int newValue = pThis->GattlingValue;

		if (TechnoExt::ExtData* const pExt = TechnoExt::ExtMap.Find(pThis))
		{
			TechnoTypeExt::ExtData* const pTypeExt = pExt->TypeExtData;

			if (!pTypeExt->RateDown_Delay)
				return 0;

			if (pTypeExt->RateDown_Delay < 0)
				return Return;

			pExt->AccumulatedGattlingValue++;
			int remain = pExt->AccumulatedGattlingValue;

			if (!pExt->ShouldUpdateGattlingValue)
				remain -= pTypeExt->RateDown_Delay;

			if (remain <= 0)
				return Return;

			if (!rateDown)
				break;

			newValue -= (rateDown * remain);
			pExt->AccumulatedGattlingValue = 0;
			pExt->ShouldUpdateGattlingValue = true;
		}
		else
		{
			if (!rateDown)
				break;

			newValue -= rateDown;
		}

		if (newValue <= 0)
			break;

		pThis->GattlingValue = newValue;

		return Return;
	}
	while (false);

	pThis->GattlingValue = 0;

	return Return;
}

DEFINE_HOOK(0x70DE70, TechnoClass_sub_70DE70_GattlingNoRateDown, 0x5)
{
	GET(TechnoClass* const, pThis, ECX);

	if (TechnoExt::ExtData* const pExt = TechnoExt::ExtMap.Find(pThis))
	{
		pExt->AccumulatedGattlingValue = 0;
		pExt->ShouldUpdateGattlingValue = false;
	}

	return 0;
}

DEFINE_HOOK(0x70E01E, TechnoClass_sub_70E000_GattlingNoRateDown, 0x6)
{
	enum { SkipGameCode = 0x70E04D };

	GET(TechnoClass* const, pThis, ESI);
	GET_STACK(int, rateMult, STACK_OFFSET(0x10, 0x4));

	do
	{
		int newValue = pThis->GattlingValue;

		if (TechnoExt::ExtData* const pExt = TechnoExt::ExtMap.Find(pThis))
		{
			TechnoTypeExt::ExtData* const pTypeExt = pExt->TypeExtData;

			if (!pTypeExt->RateDown_Delay)
				return 0;

			if (pTypeExt->RateDown_Delay < 0)
				return SkipGameCode;

			pExt->AccumulatedGattlingValue += rateMult;
			int remain = pExt->AccumulatedGattlingValue;

			if (!pExt->ShouldUpdateGattlingValue)
				remain -= pTypeExt->RateDown_Delay;

			if (remain <= 0)
				return SkipGameCode;

			const int rateDown = pTypeExt->OwnerObject()->RateDown;

			if (!rateDown)
				break;

			newValue -= (rateDown * remain);
			pExt->AccumulatedGattlingValue = 0;
			pExt->ShouldUpdateGattlingValue = true;
		}
		else
		{
			const int rateDown = pThis->GetTechnoType()->RateDown;

			if (!rateDown)
				break;

			newValue -= rateDown * rateMult;
		}

		if (newValue <= 0)
			break;

		pThis->GattlingValue = newValue;

		return SkipGameCode;
	}
	while (false);

	pThis->GattlingValue = 0;

	return SkipGameCode;
}

#pragma endregion
