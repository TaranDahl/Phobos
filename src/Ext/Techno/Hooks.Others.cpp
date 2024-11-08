#include "Body.h"

#include <EventClass.h>
#include <SpawnManagerClass.h>

#include <Ext/Building/Body.h>
#include <Ext/WeaponType/Body.h>
#include "Ext/BulletType/Body.h"

#pragma region NoBurstDelay

DEFINE_HOOK(0x5209EE, InfantryClass_UpdateFiring_BurstNoDelay, 0x5)
{
	enum { SkipVanillaFire = 0x520A57 };

	GET(InfantryClass* const, pThis, EBP);
	GET(const int, wpIdx, ESI);
	GET(AbstractClass* const, pTarget, EAX);

	if (const auto pWeapon = pThis->GetWeapon(wpIdx)->WeaponType)
	{
		if (pWeapon->Burst > 1)
		{
			if (const auto pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon))
			{
				if (pWeaponExt->Burst_NoDelay)
				{
					if (pThis->Fire(pTarget, wpIdx))
					{
						if (!pThis->CurrentBurstIndex)
							return SkipVanillaFire;

						auto rof = pThis->RearmTimer.TimeLeft;
						pThis->RearmTimer.Start(0);

						for (auto i = pThis->CurrentBurstIndex; i != pWeapon->Burst && pThis->GetFireError(pTarget, wpIdx, true) == FireError::OK && pThis->Fire(pTarget, wpIdx); ++i)
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

	if (const auto pWeapon = pThis->GetWeapon(wpIdx)->WeaponType)
	{
		if (pWeapon->Burst > 1)
		{
			if (const auto pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon))
			{
				if (pWeaponExt->Burst_NoDelay)
				{
					if (pThis->Fire(pTarget, wpIdx))
					{
						if (!pThis->CurrentBurstIndex)
							return SkipVanillaFire;

						auto rof = pThis->RearmTimer.TimeLeft;
						pThis->RearmTimer.Start(0);

						for (auto i = pThis->CurrentBurstIndex; i != pWeapon->Burst && pThis->GetFireError(pTarget, wpIdx, true) == FireError::OK && pThis->Fire(pTarget, wpIdx); ++i)
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

		if (const auto count = pAI->ConYards.Count)
		{
			const auto wayPoint = pAI->GetSpawnPosition();

			if (wayPoint != -1)
			{
				const auto center = ScenarioClass::Instance->GetWaypointCoords(wayPoint);
				auto newCenter = center;
				double distanceSquared = 131072.0;

				for (int i = 0; i < count; ++i)
				{
					if (const auto pBuilding = pAI->ConYards.GetItem(i))
					{
						if (pBuilding->IsAlive && pBuilding->Health && !pBuilding->InLimbo)
						{
							const auto newDistanceSquared = pBuilding->GetMapCoords().DistanceFromSquared(center);

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

#pragma region KickOutStuckUnits

// Kick out stuck units when the factory building is not busy
DEFINE_HOOK(0x450248, BuildingClass_UpdateFactory_KickOutStuckUnits, 0x6)
{
	GET(BuildingClass*, pThis, ESI);

	if (!(Unsorted::CurrentFrame % 15))
	{
		const auto pType = pThis->Type;

		if (pType->Factory == AbstractType::UnitType && pType->WeaponsFactory && !pType->Naval && pThis->QueuedMission != Mission::Unload)
		{
			const auto mission = pThis->CurrentMission;

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

	const auto mission = pThis->GetCurrentMission();

	return (mission == Mission::Unload || mission == Mission::Construction) ? ThisIsNotOK : ThisIsOK;
}

#pragma endregion

#pragma region GattlingNoRateDown

DEFINE_HOOK(0x70DE40, BuildingClass_sub_70DE40_GattlingRateDownDelay, 0xA)
{
	enum { Return = 0x70DE62 };

	GET(BuildingClass* const, pThis, ECX);
	GET_STACK(int, rateDown, STACK_OFFSET(0x0, 0x4));

	do
	{
		auto newValue = pThis->GattlingValue;

		if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
		{
			const auto pTypeExt = pExt->TypeExtData;

			if (pTypeExt->RateDown_Delay < 0)
				return Return;

			++pExt->AccumulatedGattlingValue;
			auto remain = pExt->AccumulatedGattlingValue;

			if (!pExt->ShouldUpdateGattlingValue)
				remain -= pTypeExt->RateDown_Delay;

			if (remain <= 0)
				return Return;

			// Time's up
			pExt->AccumulatedGattlingValue = 0;
			pExt->ShouldUpdateGattlingValue = true;

			if (pThis->Ammo <= pTypeExt->RateDown_Ammo)
				rateDown = pTypeExt->RateDown_Cover;

			if (!rateDown)
				break;

			newValue -= (rateDown * remain);
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

DEFINE_HOOK(0x70DE70, TechnoClass_sub_70DE70_GattlingRateDownReset, 0x5)
{
	GET(TechnoClass* const, pThis, ECX);

	if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
	{
		pExt->AccumulatedGattlingValue = 0;
		pExt->ShouldUpdateGattlingValue = false;
	}

	return 0;
}

DEFINE_HOOK(0x70E01E, TechnoClass_sub_70E000_GattlingRateDownDelay, 0x6)
{
	enum { SkipGameCode = 0x70E04D };

	GET(TechnoClass* const, pThis, ESI);
	GET_STACK(int, rateMult, STACK_OFFSET(0x10, 0x4));

	do
	{
		auto newValue = pThis->GattlingValue;

		if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
		{
			const auto pTypeExt = pExt->TypeExtData;

			if (pTypeExt->RateDown_Delay < 0)
				return SkipGameCode;

			pExt->AccumulatedGattlingValue += rateMult;
			auto remain = pExt->AccumulatedGattlingValue;

			if (!pExt->ShouldUpdateGattlingValue)
				remain -= pTypeExt->RateDown_Delay;

			if (remain <= 0 && rateMult)
				return SkipGameCode;

			// Time's up
			pExt->AccumulatedGattlingValue = 0;
			pExt->ShouldUpdateGattlingValue = true;

			if (!rateMult)
				break;

			const auto rateDown = (pThis->Ammo <= pTypeExt->RateDown_Ammo) ? pTypeExt->RateDown_Cover.Get() : pTypeExt->OwnerObject()->RateDown;

			if (!rateDown)
				break;

			newValue -= (rateDown * remain);
		}
		else
		{
			if (!rateMult)
				break;

			const auto rateDown = pThis->GetTechnoType()->RateDown;

			if (!rateDown)
				break;

			newValue -= (rateDown * rateMult);
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

#pragma region AirBarrier

DEFINE_HOOK(0x55B4E1, LogicClass_Update_UnmarkCellOccupationFlags, 0x5)
{
	const auto delay = RulesExt::Global()->CleanUpAirBarrier.Get();

	if (delay > 0 && !(Unsorted::CurrentFrame % delay))
	{
		MapClass* const pMap = MapClass::Instance;
		pMap->CellIteratorReset();

		for (auto pCell = pMap->CellIteratorNext(); pCell; pCell = pMap->CellIteratorNext())
		{
			if ((0xFF & pCell->OccupationFlags) && !pCell->FirstObject)
			{
				pCell->OccupationFlags &= 0x3F; // ~(Aircraft | Building)
				const auto flagO = pCell->OccupationFlags;
				pCell->OccupationFlags = 0;

				for (int i = 0; i < 8; ++i)
				{
					if (abstract_cast<FootClass*>(pCell->GetNeighbourCell(static_cast<FacingType>(i))->FirstObject))
					{
						pCell->OccupationFlags = flagO;
						break;
					}
				}
			}

			if ((0xFF & pCell->AltOccupationFlags) && !pCell->AltObject)
			{
				pCell->AltOccupationFlags &= 0x3F; // ~(Aircraft | Building)
				const auto flagA = pCell->AltOccupationFlags;
				pCell->AltOccupationFlags = 0;

				for (int i = 0; i < 8; ++i)
				{
					if (abstract_cast<FootClass*>(pCell->GetNeighbourCell(static_cast<FacingType>(i))->AltObject))
					{
						pCell->AltOccupationFlags = flagA;
						break;
					}
				}
			}
		}
	}

	return 0;
}

#pragma endregion
