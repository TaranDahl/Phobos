#include "Body.h"

#include <JumpjetLocomotionClass.h>
#include <BulletTypeClass.h>

#pragma region UnitsFacing

// Would it be better to rewrite the entire UpdateRotation() ?
DEFINE_HOOK(0x736A26, UnitClass_UpdateRotation_StopUnitIdleAction, 0x6)
{
	GET(UnitClass* const, pThis, ESI);
	GET(DirStruct* const, pTgtDir, EDX);

	const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type);

	if (pTypeExt)
	{
		constexpr double angleToRaw = (65536.0 / 360);

		if (pTypeExt->Turret_BodyRotation_Enable && !pThis->Locomotor->Is_Moving_Now())
		{
			const auto rotate = DirStruct { static_cast<int>(pTypeExt->Turret_BodyRotation_Angle * angleToRaw + 0.5) };
			const auto curDir = pThis->PrimaryFacing.Current();
			DirStruct tgtDir = *pTgtDir;

			if (pTypeExt->Turret_BodyRotation_Symmetric)
			{
				const auto rightDir = DirStruct { static_cast<short>(pTgtDir->Raw) + static_cast<short>(rotate.Raw) };
				const auto leftDir = DirStruct { static_cast<short>(pTgtDir->Raw) - static_cast<short>(rotate.Raw) };
				tgtDir = (abs(static_cast<short>(rightDir.Raw) - static_cast<short>(curDir.Raw)) < abs(static_cast<short>(leftDir.Raw) - static_cast<short>(curDir.Raw))) ? rightDir : leftDir;
			}
			else
			{
				tgtDir = DirStruct { static_cast<short>(pTgtDir->Raw) + static_cast<short>(rotate.Raw) };
			}

			if (abs(static_cast<short>(tgtDir.Raw) - static_cast<short>(curDir.Raw)) >= 8192)
				pThis->PrimaryFacing.SetDesired(tgtDir);
		}

		const auto rotate = DirStruct { static_cast<int>(pTypeExt->Turret_SelfRotation_Angle * angleToRaw + 0.5) };

		if (pTypeExt->Turret_SelfRotation_Symmetric)
		{
			const auto curDir = pThis->SecondaryFacing.Current();
			const auto rightDir = DirStruct { static_cast<short>(pTgtDir->Raw) + static_cast<short>(rotate.Raw) };
			const auto leftDir = DirStruct { static_cast<short>(pTgtDir->Raw) - static_cast<short>(rotate.Raw) };
			*pTgtDir = (abs(static_cast<short>(rightDir.Raw) - static_cast<short>(curDir.Raw)) < abs(static_cast<short>(leftDir.Raw) - static_cast<short>(curDir.Raw))) ? rightDir : leftDir;
		}
		else
		{
			*pTgtDir = DirStruct { static_cast<short>(pTgtDir->Raw) + static_cast<short>(rotate.Raw) };
		}
	}

	R->EDX<DirStruct*>(pTgtDir);

	return 0;
}

DEFINE_HOOK(0x736AEA, UnitClass_UpdateRotation_ApplyUnitIdleAction, 0x6)
{
	enum { SkipGameCode = 0x736BE2 };

	GET(UnitClass* const, pThis, ESI);

	const auto pExt = TechnoExt::ExtMap.Find(pThis);

	// Turning to target?
	if (pThis->SecondaryFacing.IsRotating())
	{
		// Repeatedly check TurretSpins and IsRotating() seems unnecessary
		pThis->unknown_bool_6AF = true;

		if (!pExt || (!pExt->UnitIdleActionTimer.IsTicking() && !pExt->UnitIdleActionGapTimer.IsTicking() && !pExt->UnitIdleIsSelected))
			return SkipGameCode;
	}

	const bool canCheck = pExt && pExt->TypeExtData;
	const auto currentMission = pThis->CurrentMission;

	// Busy in attacking or driver dead?
	if (pThis->Target || (Unsorted::CurrentFrame - pThis->unknown_int_120) < (RulesClass::Instance->GuardAreaTargetingDelay + 5) || (currentMission == Mission::Harmless && pThis->Owner == HouseClass::FindSpecial()))
	{
		if (canCheck && pExt->TypeExtData->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
			pExt->StopIdleAction();

		return SkipGameCode;
	}

	const auto pWeaponStruct = pThis->GetTurretWeapon();

	// Turret locked?
	if (pWeaponStruct && pWeaponStruct->WeaponType && pWeaponStruct->TurretLocked)
	{
		if (canCheck && pExt->TypeExtData->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
			pExt->StopIdleAction();

		if (!pThis->BunkerLinkedItem && pThis->Type->Speed && (!pThis->Type->IsSimpleDeployer || !pThis->Deployed))
			pThis->SecondaryFacing.SetDesired(pThis->PrimaryFacing.Current());

		return SkipGameCode;
	}

	// Point to mouse
	if (canCheck && SessionClass::IsSingleplayer() && pThis->Owner->IsControlledByCurrentPlayer())
	{
		if (pExt->TypeExtData->UnitIdlePointToMouse.Get(RulesExt::Global()->UnitIdlePointToMouse))
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
		if (canCheck && pExt->TypeExtData->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
			pExt->StopIdleAction();

		if (!pThis->BunkerLinkedItem && pThis->Type->Speed && (!pThis->Type->IsSimpleDeployer || !pThis->Deployed))
			pThis->SecondaryFacing.SetDesired(pThis->GetTargetDirection(pDestination));

		return SkipGameCode;
	}

	// Idle main
	if (canCheck && pExt->TypeExtData->UnitIdleRotateTurret.Get(RulesExt::Global()->UnitIdleRotateTurret))
	{
		if (currentMission == Mission::Guard || currentMission == Mission::Sticky)
		{
			pExt->ApplyIdleAction();
			return SkipGameCode;
		}

		pExt->StopIdleAction();
	}

	if (!pThis->BunkerLinkedItem && pThis->Type->Speed && (!pThis->Type->IsSimpleDeployer || !pThis->Deployed))
		pThis->SecondaryFacing.SetDesired(pThis->PrimaryFacing.Current());

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
	GET(const DirStruct, curDir, EDI);
	GET(DirStruct* const, pTgtDir, EAX);

	*pTgtDir = pThis->GetTargetDirection(pTarget);

	if (pThis->Type->Turret)
	{
		if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type))
		{
			constexpr double angleToRaw = (65536.0 / 360);
			const auto rotate = DirStruct { static_cast<int>(pTypeExt->Turret_SelfRotation_Angle * angleToRaw + 0.5) };

			if (pTypeExt->Turret_SelfRotation_Symmetric)
			{
				const auto rightDir = DirStruct { static_cast<short>(pTgtDir->Raw) + static_cast<short>(rotate.Raw) };
				const auto leftDir = DirStruct { static_cast<short>(pTgtDir->Raw) - static_cast<short>(rotate.Raw) };
				*pTgtDir = (abs(static_cast<short>(rightDir.Raw) - static_cast<short>(curDir.Raw)) < abs(static_cast<short>(leftDir.Raw) - static_cast<short>(curDir.Raw))) ? rightDir : leftDir;
			}
			else
			{
				*pTgtDir = DirStruct { static_cast<short>(pTgtDir->Raw) + static_cast<short>(rotate.Raw) };
			}
		}
	}

	R->EBX(pBulletType->ROT ? 16 : 8);
	return SkipGameCode;
}

#pragma endregion
