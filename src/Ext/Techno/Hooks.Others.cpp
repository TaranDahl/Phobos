#include "Body.h"

#include <EventClass.h>
#include <SpawnManagerClass.h>
#include <JumpjetLocomotionClass.h>

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
	if (RulesExt::Global()->AIBiasSpawnCell && !SessionClass::IsCampaign())
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

void __fastcall FindMovingInfOrVeh(CellClass* const pCell, const AbstractType findType)
{
	const auto flag = pCell->OccupationFlags;
	pCell->OccupationFlags = 0;

	for (int i = 0; i < 8; ++i)
	{
		for (auto pObject = pCell->GetNeighbourCell(static_cast<FacingType>(i))->FirstObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == findType && static_cast<FootClass*>(pObject)->Locomotor->Is_Moving_Now())
			{
				pCell->OccupationFlags = flag;
				return;
			}
		}
	}
}

void __fastcall FindMovingInfAndVeh(CellClass* const pCell)
{
	const auto flag = pCell->OccupationFlags;
	pCell->OccupationFlags = 0;
	bool inf = false;
	bool veh = false;

	for (int i = 0; i < 8; ++i)
	{
		for (auto pObject = pCell->GetNeighbourCell(static_cast<FacingType>(i))->FirstObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == AbstractType::Infantry)
			{
				if (!inf && static_cast<InfantryClass*>(pObject)->Locomotor->Is_Moving_Now())
				{
					pCell->OccupationFlags |= (flag & 0x1F);

					if (veh)
						return;

					inf = true;
				}
			}
			else if (absType == AbstractType::Unit)
			{
				if (!veh && static_cast<UnitClass*>(pObject)->Locomotor->Is_Moving_Now())
				{
					pCell->OccupationFlags |= (flag & 0x20);

					if (inf)
						return;

					veh = true;
				}
			}
		}
	}
}

void __fastcall FindAltMovingInfOrVeh(CellClass* const pCell, const AbstractType findType)
{
	const auto flag = pCell->AltOccupationFlags;
	pCell->AltOccupationFlags = 0;

	for (int i = 0; i < 8; ++i)
	{
		for (auto pObject = pCell->GetNeighbourCell(static_cast<FacingType>(i))->AltObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == findType && static_cast<FootClass*>(pObject)->Locomotor->Is_Moving_Now())
			{
				pCell->AltOccupationFlags = flag;
				return;
			}
		}
	}
}

void __fastcall FindAltMovingInfAndVeh(CellClass* const pCell)
{
	const auto flag = pCell->AltOccupationFlags;
	pCell->AltOccupationFlags = 0;
	bool inf = false;
	bool veh = false;

	for (int i = 0; i < 8; ++i)
	{
		for (auto pObject = pCell->GetNeighbourCell(static_cast<FacingType>(i))->AltObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == AbstractType::Infantry)
			{
				if (!inf && static_cast<InfantryClass*>(pObject)->Locomotor->Is_Moving_Now())
				{
					pCell->AltOccupationFlags |= (flag & 0x1F);

					if (veh)
						return;

					inf = true;
				}
			}
			else if (absType == AbstractType::Unit)
			{
				if (!veh && static_cast<UnitClass*>(pObject)->Locomotor->Is_Moving_Now())
				{
					pCell->AltOccupationFlags |= (flag & 0x20);

					if (inf)
						return;

					veh = true;
				}
			}
		}
	}
}

DEFINE_HOOK(0x55B4E1, LogicClass_Update_UnmarkCellOccupationFlags, 0x5)
{
	const auto delay = RulesExt::Global()->CleanUpAirBarrier.Get();

	if (delay > 0 && !(Unsorted::CurrentFrame % delay))
	{
		const auto pMap = MapClass::Instance();
		pMap->CellIteratorReset();

		for (auto pCell = pMap->CellIteratorNext(); pCell; pCell = pMap->CellIteratorNext())
		{
			if ((0xFF & pCell->OccupationFlags) && !pCell->FirstObject)
			{
				pCell->OccupationFlags &= 0x3F; // ~(Aircraft | Building)

				if (pCell->OccupationFlags & 0x1F)
				{
					if (pCell->OccupationFlags & 0x20)
						FindMovingInfAndVeh(pCell);
					else
						FindMovingInfOrVeh(pCell, AbstractType::Infantry);
				}
				else if (pCell->OccupationFlags & 0x20)
				{
					FindMovingInfOrVeh(pCell, AbstractType::Unit);
				}
			}

			if ((0xFF & pCell->AltOccupationFlags) && !pCell->AltObject)
			{
				pCell->AltOccupationFlags &= 0x3F; // ~(Aircraft | Building)

				if (pCell->AltOccupationFlags & 0x1F)
				{
					if (pCell->AltOccupationFlags & 0x20)
						FindAltMovingInfAndVeh(pCell);
					else
						FindAltMovingInfOrVeh(pCell, AbstractType::Infantry);
				}
				else if (pCell->AltOccupationFlags & 0x20)
				{
					FindAltMovingInfOrVeh(pCell, AbstractType::Unit);
				}
			}
		}
	}

	return 0;
}

#pragma endregion

#pragma region NoQueueUpToEnterAndUnload

bool __fastcall CanEnterNow(UnitClass* pTransport, FootClass* pPassenger)
{
	const auto pOwner = pTransport->Owner;

	if (!pOwner || !pOwner->IsAlliedWith(pPassenger) || pTransport->IsBeingWarpedOut())
		return false;

	if (pPassenger->IsMindControlled() || pPassenger->ParasiteEatingMe)
		return false;

	const auto pManager = pPassenger->CaptureManager;

	if (pManager && pManager->IsControllingSomething())
		return false;

	const auto passengerSize = pPassenger->GetTechnoType()->Size;
	const auto pTransportType = pTransport->Type;

	if (passengerSize > pTransportType->SizeLimit)
		return false;

	const auto maxSize = pTransportType->Passengers;
	const auto predictSize = pTransport->Passengers.GetTotalSize() + static_cast<int>(passengerSize);
	const auto pLink = pTransport->GetNthLink();
	const auto needCalculate = pLink && pLink != pPassenger;

	if (needCalculate)
	{
		const auto linkCell = pLink->GetCoords();
		const auto tranCell = pTransport->GetCoords();

		if (abs(linkCell.X - tranCell.X) <= 384 && abs(linkCell.Y - tranCell.Y) <= 384)
			return (predictSize <= (maxSize - pLink->GetTechnoType()->Size));
	}

	const auto remain = maxSize - predictSize;

	if (remain < 0)
		return false;

	if (needCalculate && remain < static_cast<int>(pLink->GetTechnoType()->Size))
		pLink->SendToFirstLink(RadioCommand::NotifyUnlink);

	return true;
}

DEFINE_HOOK(0x51A0D4, InfantryClass_UpdatePosition_NoQueueUpToEnter, 0x6)
{
	enum { EnteredThenReturn = 0x51A47E };

	GET(InfantryClass* const, pThis, ESI);

	if (!RulesExt::Global()->NoQueueUpToEnter)
		return 0;

	if (const auto pDest = abstract_cast<UnitClass*>(pThis->CurrentMission == Mission::Enter ? pThis->Destination : pThis->unknown_500))
	{
		if (pDest->Type->Passengers > 0)
		{
			const auto thisCell = pThis->GetCoords();
			const auto destCell = pDest->GetCoords();

			if (abs(thisCell.X - destCell.X) <= 384 && abs(thisCell.Y - destCell.Y) <= 384)
			{
				if (CanEnterNow(pDest, pThis)) // Replace send radio command: QueryCanEnter
				{
					if (const auto pTag = pDest->AttachedTag)
						pTag->RaiseEvent(TriggerEvent::EnteredBy, pThis, CellStruct::Empty);

					pThis->ArchiveTarget = nullptr;
					pThis->OnBridge = false;
					pThis->unknown_C4 = 0;
					pThis->GattlingValue = 0;
					pThis->CurrentGattlingStage = 0;

					if (const auto pMind = pThis->MindControlledBy)
					{
						if (const auto pManager = pMind->CaptureManager)
							pManager->FreeUnit(pThis);
					}

					pThis->Limbo();

					if (pDest->Type->OpenTopped)
						pDest->EnteredOpenTopped(pThis);

					pThis->Transporter = pDest;
					pDest->AddPassenger(pThis);
					pThis->Undiscover();

					pThis->unknown_500 = nullptr; // Added, to prevent passengers from wanting to get on after getting off
					pThis->SetSpeedPercentage(0.0); // Added, to stop the passengers and let OpenTopped work normally

					return EnteredThenReturn;
				}
			}
		}
	}

	return 0;
}

DEFINE_HOOK(0x73A5EA, UnitClass_UpdatePosition_NoQueueUpToEnter, 0x5)
{
	enum { EnteredThenReturn = 0x73A78C };

	GET(UnitClass* const, pThis, EBP);

	if (!RulesExt::Global()->NoQueueUpToEnter)
		return 0;

	if (const auto pDest = abstract_cast<UnitClass*>(pThis->CurrentMission == Mission::Enter ? pThis->Destination : pThis->unknown_500))
	{
		if (pDest->Type->Passengers > 0)
		{
			const auto thisCell = pThis->GetCoords();
			const auto destCell = pDest->GetCoords();

			if (abs(thisCell.X - destCell.X) <= 384 && abs(thisCell.Y - destCell.Y) <= 384)
			{
				if (CanEnterNow(pDest, pThis)) // Replace send radio command: QueryCanEnter
				{
					// I don't know why units have no trigger

					pThis->ArchiveTarget = nullptr;
					pThis->OnBridge = false;
					pThis->unknown_C4 = 0;
					pThis->GattlingValue = 0;
					pThis->CurrentGattlingStage = 0;

					if (const auto pMind = pThis->MindControlledBy)
					{
						if (const auto pManager = pMind->CaptureManager)
							pManager->FreeUnit(pThis);
					}

					pThis->Limbo();
					pDest->AddPassenger(pThis);

					if (pDest->Type->OpenTopped)
						pDest->EnteredOpenTopped(pThis);

					pThis->Transporter = pDest;

					if (pThis->Type->OpenTopped)
						pThis->SetTargetForPassengers(nullptr);

					pThis->Undiscover();

					pThis->unknown_500 = nullptr; // Added, to prevent passengers from wanting to get on after getting off
					pThis->SetSpeedPercentage(0.0); // Added, to stop the passengers and let OpenTopped work normally

					return EnteredThenReturn;
				}
			}
		}
	}

	return 0;
}

DEFINE_HOOK(0x73DC1E, UnitClass_Mission_Unload_NoQueueUpToUnload, 0xA)
{
	enum { QuickUnload = 0x73E2B7, VanillaUnload = 0x73E289 };

	GET(UnitClass* const, pThis, ESI);

	const int sound = pThis->GetTechnoType()->LeaveTransportSound;

	if (sound != -1)
		VoxClass::PlayAtPos(sound, &pThis->Location);

	if (RulesExt::Global()->NoQueueUpToUnload)
	{
		R->EAX(1); // Can be customized
		return QuickUnload;
	}

	return VanillaUnload;
}

#pragma endregion

#pragma region AggressiveAttackMove

DEFINE_HOOK(0x4C75DA, EventClass_RespondToEvent_Stop, 0x6)
{
	enum { SkipGameCode = 0x4C762A };

	GET(TechnoClass* const, pTechno, ESI);

	// Check aircrafts
	const auto pAircraft = abstract_cast<AircraftClass*>(pTechno);
	const bool commonAircraft = pAircraft && !pAircraft->Airstrike && !pAircraft->Spawned;

	// To avoid aircrafts overlap by keep link if is returning or is in airport now.
	if (!commonAircraft || pAircraft->CurrentMission != Mission::Enter || !pAircraft->DockNowHeadingTo || (pAircraft->DockNowHeadingTo != pAircraft->GetNthLink()))
		pTechno->SendToEachLink(RadioCommand::NotifyUnlink);

	// To avoid technos being unable to stop in attack move mega mission
	if (pTechno->vt_entry_4C4()) // pTechno->MegaMissionIsAttackMove()
		pTechno->vt_entry_4A8(); // pTechno->ClearMegaMissionData()

	// Clearing the current target should still be necessary for all technos
	pTechno->SetTarget(nullptr);

	if (commonAircraft)
	{
		if (pAircraft->Type->AirportBound)
		{
			// To avoid `AirportBound=yes` aircrafts pausing in the air and let they returning to air base immediately.
			if (!pAircraft->DockNowHeadingTo || (pAircraft->DockNowHeadingTo != pAircraft->GetNthLink())) // If the aircraft have no valid dock, try to find a new one
				pAircraft->EnterIdleMode(false, true);
		}
		else if (pAircraft->Ammo)
		{
			// To avoid `AirportBound=no` aircrafts ignoring the stop task or directly return to the airport.
			if (pAircraft->Destination && static_cast<int>(CellClass::Coord2Cell(pAircraft->Destination->GetCoords()).DistanceFromSquared(pAircraft->GetMapCoords())) > 2) // If the aircraft is moving, find the forward cell then stop in it
				pAircraft->SetDestination(pAircraft->GetCell()->GetNeighbourCell(static_cast<FacingType>(pAircraft->PrimaryFacing.Current().GetValue<3>())), true);
		}
		else if (!pAircraft->DockNowHeadingTo || (pAircraft->DockNowHeadingTo != pAircraft->GetNthLink()))
		{
			pAircraft->EnterIdleMode(false, true);
		}
		// Otherwise landing or idling normally without answering the stop command
	}
	else
	{
		// Check Jumpjets
		const auto pFoot = abstract_cast<FootClass*>(pTechno);
		const auto pJumpjetLoco = pFoot ? locomotion_cast<JumpjetLocomotionClass*>(pFoot->Locomotor) : nullptr;

		// To avoid jumpjets falling into a state of standing idly by
		if (!pJumpjetLoco) // If is not jumpjet, clear the destination is enough
			pTechno->SetDestination(nullptr, true);
		else if (!pFoot->Destination) // When in attack move and have had a target, the destination will be cleaned up, enter the guard mission can prevent the jumpjets stuck in a status of standing idly by
			pTechno->QueueMission(Mission::Guard, true);
		else if (static_cast<int>(CellClass::Coord2Cell(pFoot->Destination->GetCoords()).DistanceFromSquared(pTechno->GetMapCoords())) > 2) // If the jumpjet is moving, find the forward cell then stop in it
			pTechno->SetDestination(pTechno->GetCell()->GetNeighbourCell(static_cast<FacingType>(pJumpjetLoco->LocomotionFacing.Current().GetValue<3>())), true);
		// Otherwise landing or idling normally without answering the stop command
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x6F85AB, TechnoClass_CanAutoTargetObject_AggressiveAttackMove, 0x6)
{
	enum { ContinueCheck = 0x6F85BA, CanTarget = 0x6F8604 };

	GET(TechnoClass* const, pThis, EDI);

	return (!pThis->Owner->IsControlledByHuman() || (RulesExt::Global()->AttackMove_Aggressive && pThis->vt_entry_4C4())) ? CanTarget : ContinueCheck;
}

#pragma endregion

#pragma region AttackMindControlledDelay

bool __fastcall CanAttackMindControlled(TechnoClass* pControlled, TechnoClass* pRetaliator)
{
	const auto pMind = pControlled->MindControlledBy;

	if (!pMind || pRetaliator->Berzerk)
		return true;

	const auto pManager = pMind->CaptureManager;

	if (!pManager)
		return true;

	const auto pHome = pManager->GetOriginalOwner(pControlled);

	if (!pHome || !pHome->IsAlliedWith(pRetaliator))
		return true;

	const auto pExt = TechnoExt::ExtMap.Find(pControlled);

	if (!pExt)
		return true;

	return (Unsorted::CurrentFrame - pExt->LastBeControlledFrame) >= RulesExt::Global()->AttackMindControlledDelay;
}

DEFINE_HOOK(0x7089E8, TechnoClass_AllowedToRetaliate_AttackMindControlledDelay, 0x6)
{
	enum { CannotRetaliate = 0x708B17 };

	GET(TechnoClass* const, pThis, ESI);
	GET(TechnoClass* const, pAttacker, EBP);

	return CanAttackMindControlled(pAttacker, pThis) ? 0 : CannotRetaliate;
}

DEFINE_HOOK(0x6F7EA2, TechnoClass_CanAutoTargetObject_AttackMindControlledDelay, 0x6)
{
	enum { CannotSelect = 0x6F894F };

	GET(TechnoClass* const, pThis, EDI);
	GET(ObjectClass* const, pTarget, ESI);

	if (const auto pTechno = abstract_cast<TechnoClass*>(pTarget))
	{
		if (!CanAttackMindControlled(pTechno, pThis))
			return CannotSelect;
	}

	return 0;
}

#pragma endregion
