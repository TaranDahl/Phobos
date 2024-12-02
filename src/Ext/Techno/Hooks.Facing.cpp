#include "Body.h"

#include <JumpjetLocomotionClass.h>
#include <BulletTypeClass.h>

#include <Ext/WeaponType/Body.h>

#pragma region UnitsFacing

// Would it be better to rewrite the entire UpdateRotation() ?
DEFINE_HOOK(0x7369D6, UnitClass_UpdateRotation_StopUnitIdleAction, 0xA)
{
	enum { SkipGameCode = 0x736A8E };

	GET(UnitClass* const, pThis, ESI);

	const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type);

	if (!pTypeExt)
		return 0;

	if (const auto pWeaponStruct = pThis->GetTurretWeapon())
	{
		const auto pWeapon = pWeaponStruct->WeaponType;
		const auto pWeaponTypeExt = WeaponTypeExt::ExtMap.Find(pWeapon);

		if (pWeapon && (!pWeapon->OmniFire || (pWeaponTypeExt && pWeaponTypeExt->OmniFire_TurnToTarget)))
		{
			if (pWeaponStruct->TurretLocked)
			{
				pTypeExt->SetTurretLimitedDir(pThis, pThis->PrimaryFacing.Current());
			}
			else
			{
				const auto targetDir = pThis->GetTargetDirection(pThis->Target);
				pTypeExt->SetTurretLimitedDir(pThis, targetDir);

				if (pTypeExt->Turret_BodyRotation_Enable && !pThis->Locomotor->Is_Moving_Now())
				{
					const auto curDir = pThis->PrimaryFacing.Current();
					const auto tgtDir = pTypeExt->GetBodyDesiredDir(curDir, targetDir);

					if (abs(static_cast<short>(static_cast<short>(tgtDir.Raw) - static_cast<short>(curDir.Raw))) >= 8192)
						pThis->PrimaryFacing.SetDesired(tgtDir);
				}
			}
		}
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x736AEA, UnitClass_UpdateRotation_ApplyUnitIdleAction, 0x6)
{
	enum { SkipGameCode = 0x736BE2 };

	GET(UnitClass* const, pThis, ESI);

	const auto pExt = TechnoExt::ExtMap.Find(pThis);

	if (!pExt)
		return 0;

	const auto pTypeExt = pExt->TypeExtData;

	if (!pExt->TypeExtData)
		return 0;

	// Turning to target?
	if (pThis->SecondaryFacing.IsRotating())
	{
		// Repeatedly check TurretSpins and IsRotating() seems unnecessary
		pThis->unknown_bool_6AF = true;

		if (!pExt || (!pExt->UnitIdleActionTimer.IsTicking() && !pExt->UnitIdleActionGapTimer.IsTicking() && !pExt->UnitIdleIsSelected))
			return SkipGameCode;
	}

	const auto currentMission = pThis->CurrentMission;

	// Busy in attacking or driver dead?
	if (pThis->Target || (Unsorted::CurrentFrame - pThis->unknown_int_120) < (RulesClass::Instance->GuardAreaTargetingDelay + 5) || (currentMission == Mission::Harmless && pThis->Owner == HouseClass::FindSpecial()))
	{
		if (pTypeExt->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
			pExt->StopIdleAction();

		return SkipGameCode;
	}

	const auto pWeaponStruct = pThis->GetTurretWeapon();

	// Turret locked?
	if (pWeaponStruct && pWeaponStruct->WeaponType && pWeaponStruct->TurretLocked)
	{
		if (pTypeExt->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
			pExt->StopIdleAction();

		if (!pThis->BunkerLinkedItem && pThis->Type->Speed && (!pThis->Type->IsSimpleDeployer || !pThis->Deployed))
			pTypeExt->SetTurretLimitedDir(pThis, pThis->PrimaryFacing.Current());

		return SkipGameCode;
	}

	// Point to mouse
	if (SessionClass::IsSingleplayer() && pThis->Owner->IsControlledByCurrentPlayer())
	{
		if (pTypeExt->UnitIdlePointToMouse.Get(RulesExt::Global()->UnitIdlePointToMouse))
			pExt->ManualIdleAction();

		if (pExt->UnitIdleIsSelected)
			return SkipGameCode;
	}

	const auto pDestination = pThis->Destination;
	// Bugfix: Align jumpjet turret's facing with body's
	// When jumpjets arrived at their FootClass::Destination, they seems stuck at the Move mission
	// and therefore the turret facing was set to DirStruct{atan2(0,0)}==DirType::East at 0x736BBB
	// that's why they will come back to normal when giving stop command explicitly
	// so the best way is to fix the Mission if necessary, but I don't know how to do it
	// so I skipped jumpjets check temporarily
	if (pDestination && !locomotion_cast<JumpjetLocomotionClass*>(pThis->Locomotor))
	{
		if (pTypeExt->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
			pExt->StopIdleAction();

		if (!pThis->BunkerLinkedItem && pThis->Type->Speed && (!pThis->Type->IsSimpleDeployer || !pThis->Deployed))
			pTypeExt->SetTurretLimitedDir(pThis, pThis->GetTargetDirection(pDestination));

		return SkipGameCode;
	}

	// Idle main
	if (pTypeExt->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
	{
		if (currentMission == Mission::Guard || currentMission == Mission::Sticky)
		{
			pExt->ApplyIdleAction();
			return SkipGameCode;
		}

		pExt->StopIdleAction();
	}

	if (!pThis->BunkerLinkedItem && pThis->Type->Speed && (!pThis->Type->IsSimpleDeployer || !pThis->Deployed))
		pTypeExt->SetTurretLimitedDir(pThis, pThis->PrimaryFacing.Current());

	return SkipGameCode;
}

#pragma endregion

#pragma region CheckFacing

DEFINE_HOOK(0x7410BB, UnitClass_GetFireError_CheckFacingError, 0x8)
{
	enum { NoNeedToCheck = 0x74132B, ContinueCheck = 0x7410C3 };

	GET(const FireError, fireError, EAX);

	if (fireError == FireError::OK)
		return ContinueCheck;

	GET(UnitClass* const, pThis, ESI);

	return (fireError == FireError::REARM && !pThis->Type->Turret && !pThis->IsWarpingIn()) ? ContinueCheck : NoNeedToCheck;
}

DEFINE_HOOK(0x7412BB, UnitClass_GetFireError_CheckFacingDeviation, 0x7)
{
	enum { SkipGameCode = 0x7412D4 };

	GET(UnitClass* const, pThis, ESI);
	GET(AbstractClass* const, pTarget, EBP);
	GET(BulletTypeClass* const, pBulletType, EDX);
	GET(DirStruct* const, pTgtDir, EAX);

	*pTgtDir = pThis->GetTargetDirection(pTarget);

	if (pThis->Type->Turret)
	{
		if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type))
			*pTgtDir = pTypeExt->GetTurretDesiredDir(*pTgtDir);
	}

	R->EBX(pBulletType->ROT ? 16 : 8);
	return SkipGameCode;
}

#pragma endregion
