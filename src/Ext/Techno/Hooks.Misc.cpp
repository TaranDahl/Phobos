#include "Body.h"

#include <SpawnManagerClass.h>
#include <Ext/WeaponType/Body.h>
#include "Ext/BulletType/Body.h"

DEFINE_HOOK(0x6B0B9C, SlaveManagerClass_Killed_DecideOwner, 0x6)
{
	enum { KillTheSlave = 0x6B0BDF, ChangeSlaveOwner = 0x6B0BB4 };

	GET(InfantryClass*, pSlave, ESI);

	if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pSlave->Type))
	{
		switch (pTypeExt->Slaved_OwnerWhenMasterKilled.Get())
		{
		case SlaveChangeOwnerType::Suicide:
			return KillTheSlave;

		case SlaveChangeOwnerType::Master:
			R->EAX(pSlave->Owner);
			return ChangeSlaveOwner;

		case SlaveChangeOwnerType::Neutral:
			if (auto pNeutral = HouseClass::FindNeutral())
			{
				R->EAX(pNeutral);
				return ChangeSlaveOwner;
			}

		default: // SlaveChangeOwnerType::Killer
			return 0x0;
		}
	}

	return 0x0;
}

// Fix slaves cannot always suicide due to armor multiplier or something
DEFINE_PATCH(0x6B0BF7,
	0x6A, 0x01  // push 1       // ignoreDefense=false->true
);

DEFINE_HOOK(0x6B7265, SpawnManagerClass_AI_UpdateTimer, 0x6)
{
	GET(SpawnManagerClass* const, pThis, ESI);

	if (pThis->Owner && pThis->Status == SpawnManagerStatus::Launching && pThis->CountDockedSpawns() != 0)
	{
		if (auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Owner->GetTechnoType()))
		{
			if (pTypeExt->Spawner_DelayFrames.isset())
				R->EAX(std::min(pTypeExt->Spawner_DelayFrames.Get(), 10));
		}
	}

	return 0;
}

DEFINE_HOOK_AGAIN(0x6B73BE, SpawnManagerClass_AI_SpawnTimer, 0x6)
DEFINE_HOOK(0x6B73AD, SpawnManagerClass_AI_SpawnTimer, 0x5)
{
	GET(SpawnManagerClass* const, pThis, ESI);

	if (pThis->Owner)
	{
		if (auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Owner->GetTechnoType()))
		{
			if (pTypeExt->Spawner_DelayFrames.isset())
				R->ECX(pTypeExt->Spawner_DelayFrames.Get());
		}
	}

	return 0;
}

DEFINE_HOOK_AGAIN(0x6B769F, SpawnManagerClass_AI_InitDestination, 0x7)
DEFINE_HOOK(0x6B7600, SpawnManagerClass_AI_InitDestination, 0x6)
{
	enum { SkipGameCode1 = 0x6B760E, SkipGameCode2 = 0x6B76DE };

	GET(SpawnManagerClass* const, pThis, ESI);
	GET(AircraftClass* const, pSpawnee, EDI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Owner->GetTechnoType());

	if (pTypeExt->Spawner_AttackImmediately)
	{
		pSpawnee->SetTarget(pThis->Target);
		pSpawnee->QueueMission(Mission::Attack, true);
		pSpawnee->IsReturningFromAttackRun = false;
	}
	else
	{
		auto const mapCoords = pThis->Owner->GetMapCoords();
		auto const pCell = MapClass::Instance->GetCellAt(mapCoords);
		pSpawnee->SetDestination(pCell->GetNeighbourCell(FacingType::North), true);
		pSpawnee->QueueMission(Mission::Move, false);
	}

	return R->Origin() == 0x6B7600 ? SkipGameCode1 : SkipGameCode2;
}

// I must not regroup my forces.
DEFINE_HOOK(0x739920, UnitClass_TryToDeploy_DisableRegroupAtNewConYard, 0x6)
{
	enum { SkipRegroup = 0x73992B };
	return RulesExt::Global()->GatherWhenMCVDeploy ? 0 : SkipRegroup;
}

DEFINE_HOOK(0x6F7891, TechnoClass_IsCloseEnough_CylinderRangefinding, 0x7)
{
	enum { SkipGameCode = 0x6F789A };

	GET(WeaponTypeClass* const, pWeaponType, EDI);
	GET(TechnoClass* const, pThis, ESI);

	bool cylinder = RulesExt::Global()->CylinderRangefinding;

	if (auto const pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeaponType))
		cylinder = pWeaponExt->CylinderRangefinding.Get(cylinder);

	R->AL(cylinder ? true : pThis->IsInAir());
	return SkipGameCode;
}

bool __fastcall BuildingTypeClass_CanUseWaypoint(BuildingTypeClass* pThis)
{
	return RulesExt::Global()->BuildingWaypoint;
}
DEFINE_JUMP(VTABLE, 0x7E4610, GET_OFFSET(BuildingTypeClass_CanUseWaypoint))

bool __fastcall AircraftTypeClass_CanUseWaypoint(AircraftTypeClass* pThis)
{
	return RulesExt::Global()->AircraftWaypoint;
}
DEFINE_JUMP(VTABLE, 0x7E2908, GET_OFFSET(AircraftTypeClass_CanUseWaypoint))

DEFINE_HOOK(0x6B77B4, SpawnManagerClass_Update_RecycleSpawned, 0x7)
{
	//enum { RecycleIsOk = 0x6B77FF, RecycleIsNotOk = 0x6B7838 };

	GET(SpawnManagerClass* const, pThis, ESI);
	GET(AircraftClass* const, pSpawned, EDI);
	GET(CellStruct* const, pSpawnerMapCrd, EBP);

	if (!pThis)
		return 0;

	auto const pSpawner = pThis->Owner;

	if (!pSpawner || !pSpawned)
		return 0;

	auto const pSpawnerType = pSpawner->GetTechnoType();
	auto const spawnedMapCrd = pSpawned->GetMapCoords();

	if (!pSpawnerType)
		return 0;

	auto const pSpawnerExt = TechnoTypeExt::ExtMap.Find(pSpawnerType);

	if (!pSpawnerExt)
		return 0;

	auto const spawnerCrd = pSpawner->GetCoords();
	auto const spawnedCrd = pSpawned->GetCoords();
	auto const deltaCrd = spawnedCrd - spawnerCrd;
	const int recycleRange = pSpawnerExt->Spawner_RecycleRange;

	do
	{
		if (recycleRange < 0)
		{
			if (pSpawner->WhatAmI() == AbstractType::Building)
			{
				if (deltaCrd.X > 182 || deltaCrd.Y > 182 || deltaCrd.Z >= 20)
					break;
			}
			else if (spawnedMapCrd != *pSpawnerMapCrd || deltaCrd.Z >= 20)
			{
				break;
			}
		}
		else if (deltaCrd.Magnitude() > recycleRange)
		{
			break;
		}

		if (pSpawnerExt->Spawner_RecycleAnim)
			GameCreate<AnimClass>(pSpawnerExt->Spawner_RecycleAnim, spawnedCrd);

		pSpawned->SetLocation(spawnerCrd);
		R->EAX(pSpawnerMapCrd);
	}
	while (false);

	return 0;
}

DEFINE_HOOK(0x481778, CellClass_ScatterContent_Fix, 0x6)
{
	enum { SkipGameCode = 0x481793 };
	GET(ObjectClass*, pObject, ESI);

	auto pTechno = abstract_cast<TechnoClass*>(pObject);

	R->CL(pTechno && pTechno->Owner->IsHumanPlayer && RulesClass::Instance()->PlayerScatter);
	return SkipGameCode;
}

DEFINE_HOOK(0x63745D, UnknownClass_PlanWaypoint_ContinuePlanningOnEnter, 0x6)
{
	enum { SkipDeselect = 0x637468 };

	GET(int, planResult, ESI);

	return (!planResult && !RulesExt::Global()->StopPlanningOnEnter) ? SkipDeselect : 0;
}

DEFINE_HOOK(0x637479, UnknownClass_PlanWaypoint_DisableMessage, 0x5)
{
	enum { SkipMessage = 0x637524 };
	return (!RulesExt::Global()->StopPlanningOnEnter) ? SkipMessage : 0;
}

DEFINE_HOOK(0x638D73, UnknownClass_CheckLastWaypoint_ContinuePlanningWaypoint, 0x5)
{
	enum { SkipDeselect = 0x638D8D, Deselect = 0x638D82 };

	GET(Action, action, EAX);

	if (!RulesExt::Global()->StopPlanningOnEnter)
		return SkipDeselect;
	else if (action == Action::Select || action == Action::ToggleSelect || action == Action::Capture)
		return Deselect;

	return SkipDeselect;
}

DEFINE_HOOK(0x6FA697, TechnoClass_Update_DontScanIfUnarmed, 0x6)
{
	enum { SkipTargeting = 0x6FA6F5 };

	GET(TechnoClass*, pThis, ESI);

	return pThis->IsArmed() ? 0 : SkipTargeting;
}

DEFINE_HOOK(0x709866, TechnoClass_TargetAndEstimateDamage_ScanDelayGuardArea, 0x6)
{
	GET(TechnoClass*, pThis, ESI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());
	auto const pOwner = pThis->Owner;
	auto const pRulesExt = RulesExt::Global();
	auto const pRules = RulesClass::Instance();
	int delay = 1;

	if (pOwner->IsHumanPlayer || pOwner->IsControlledByHuman())
		delay = pTypeExt->PlayerGuardAreaTargetingDelay.Get(pRulesExt->PlayerGuardAreaTargetingDelay.Get(pRules->GuardAreaTargetingDelay));
	else
		delay = pTypeExt->AIGuardAreaTargetingDelay.Get(pRulesExt->AIGuardAreaTargetingDelay.Get(pRules->GuardAreaTargetingDelay));

	R->ECX(delay);
	return 0;
}

DEFINE_HOOK(0x70989C, TechnoClass_TargetAndEstimateDamage_ScanDelayNormal, 0x6)
{
	GET(TechnoClass*, pThis, ESI);

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());
	auto const pOwner = pThis->Owner;
	auto const pRulesExt = RulesExt::Global();
	auto const pRules = RulesClass::Instance();
	int delay = (pThis->Location.X + pThis->Location.Y + Unsorted::CurrentFrame) % 3;

	if (pOwner->IsHumanPlayer || pOwner->IsControlledByHuman())
		delay += pTypeExt->PlayerNormalTargetingDelay.Get(pRulesExt->PlayerNormalTargetingDelay.Get(pRules->NormalTargetingDelay));
	else
		delay += pTypeExt->AINormalTargetingDelay.Get(pRulesExt->AINormalTargetingDelay.Get(pRules->NormalTargetingDelay));

	R->ECX(delay);
	return 0;
}

DEFINE_HOOK(0x702B31, TechnoClass_ReceiveDamage_ReturnFireCheck, 0x7)
{
	enum { SkipReturnFire = 0x702B47, NotSkip = 0 };

	GET(TechnoClass*, pThis, ESI);

	auto pOwner = pThis->Owner;

	if (!(pOwner->IsHumanPlayer || pOwner->IsInPlayerControl) || !RulesExt::Global()->PlayerReturnFire_Smarter)
		return NotSkip;

	auto pTarget = pThis->Target;

	if (pTarget)
		return SkipReturnFire;

	auto mission = pThis->CurrentMission;
	auto pThisFoot = abstract_cast<FootClass*>(pThis);

	if (!pThisFoot)
		return NotSkip;

	bool isJJMoving = pThisFoot->GetHeight() > Unsorted::CellHeight && mission == Mission::Move && pThisFoot->Locomotor->Is_Moving_Now();
	bool isMoving = isJJMoving || (mission == Mission::Move && pThisFoot->GetHeight() <= 0);

	return isMoving ? SkipReturnFire : NotSkip;
}

DEFINE_HOOK(0x6FC22A, TechnoClass_GetFireError_TargetingIronCurtain, 0x6)
{
	enum { CantFire = 0x6FC86A, GoOtherChecks = 0x6FC24D };

	GET(TechnoClass*, pThis, ESI);
	GET(ObjectClass*, pTarget, EBP);
	GET_STACK(int, wpIdx, STACK_OFFSET(0x20, 0x8));

	if (!pTarget->IsIronCurtained())
		return GoOtherChecks;

	auto pOwner = pThis->Owner;
	auto const pRules = RulesExt::Global();

	if ((pOwner->IsHumanPlayer || pOwner->IsInPlayerControl) ? pRules->PlayerAttackIronCurtain : pRules->AIAttackIronCurtain)
		return GoOtherChecks;

	auto pWpExt = WeaponTypeExt::ExtMap.Find(pThis->GetWeapon(wpIdx)->WeaponType);
	bool isHealing = pThis->CombatDamage(wpIdx) < 0;

	return (pWpExt && pWpExt->AttackIronCurtain.Get(isHealing)) ? GoOtherChecks : CantFire;
}

DEFINE_HOOK(0x6F50A9, TechnoClass_UpdatePosition_TemporalLetGo, 0x7)
{
	enum { LetGo = 0x6F50B4, SkipLetGo = 0x6F50B9 };

	GET(TechnoClass* const, pThis, ESI);
	GET(TemporalClass* const, pTemporal, ECX);

	if (!pTemporal || !pTemporal->Target)
		return SkipLetGo;

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	return (!pTypeExt || !pTypeExt->KeepWarping) ? LetGo : SkipLetGo;
}

DEFINE_HOOK(0x709A43, TechnoClass_EnterIdleMode_TemporalLetGo, 0x7)
{
	enum { LetGo = 0x709A54, SkipLetGo = 0x709A59 };

	GET(TechnoClass* const, pThis, ESI);
	GET(TemporalClass* const, pTemporal, ECX);

	if (!pTemporal || !pTemporal->Target)
		return SkipLetGo;

	auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->GetTechnoType());

	return (!pTypeExt || !pTypeExt->KeepWarping) ? LetGo : SkipLetGo;
}

DEFINE_HOOK(0x70023B, TechnoClass_MouseOverObject_AttackUnderGround, 0x5)
{
	enum { FireIsOK = 0x700246, FireIsNotOK = 0x70056C };

	GET(ObjectClass*, pObject, EDI);
	GET(TechnoClass*, pThis, ESI);
	GET(int, wpIdx, EAX);

	auto const pWeapon = pThis->GetWeapon(wpIdx)->WeaponType;
	auto const pProjExt = pWeapon ? BulletTypeExt::ExtMap.Find(pWeapon->Projectile) : 0;
	bool isSurfaced = pObject->IsSurfaced();

	if (!isSurfaced && (!pProjExt || !pProjExt->AU))
		return FireIsNotOK;

	return FireIsOK;
}
