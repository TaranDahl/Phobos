#include "Body.h"

#include <JumpjetLocomotionClass.h>
#include <BulletTypeClass.h>

#pragma region UnitsFacing

// Would it be better to rewrite the entire UpdateRotation() ?
DEFINE_HOOK(0x7369A5, UnitClass_UpdateRotation_CheckTurnToTarget, 0x6)
{
	enum { SkipGameCode = 0x736A8E, ContinueGameCode = 0x7369B3 };

	GET(UnitClass* const, pThis, ESI);

	if (!pThis->unknown_bool_6AF)
		return ContinueGameCode;

	if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
	{
		if (pExt->UnitIdleActionTimer.IsTicking() || pExt->UnitIdleActionGapTimer.IsTicking() || pExt->UnitIdleIsSelected)
			return ContinueGameCode;
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x7369D6, UnitClass_UpdateRotation_StopUnitIdleAction, 0xA)
{
	enum { SkipGameCode = 0x736A8E };

	GET(UnitClass* const, pThis, ESI);

	TechnoTypeExt::ExtData* pTypeExt = nullptr;
	const auto pTarget = pThis->Target; // pThis->Target have been checked

	if (const auto pWeaponStruct = pThis->GetWeapon(pThis->SelectWeapon(pTarget))) // Vanilla is pThis->GetTurretWeapon()
	{
		if (const auto pWeapon = pWeaponStruct->WeaponType)
		{
			if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
				pExt->StopIdleAction();

			if (!pWeapon->OmniFire)
			{
				if (pWeaponStruct->TurretLocked)
				{
					pThis->SecondaryFacing.SetDesired(pThis->PrimaryFacing.Current());
				}
				else
				{
					const auto source = pThis->Location;
					const auto target = pTarget->GetCoords();
					const auto radian = Math::atan2(source.Y - target.Y, target.X - source.X);
					DirStruct tgtDir;

					if (pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type), pTypeExt)
					{
						const auto rotate = pTypeExt->Turret_SelfRotation_Angle * (Math::Pi / 180.0);

						if (pTypeExt->Turret_SelfRotation_Symmetric)
						{
							const auto curDir = pThis->SecondaryFacing.Current();
							const auto rightDir = DirStruct { radian + rotate };
							const auto leftDir = DirStruct { radian - rotate };

							if (abs(static_cast<short>(rightDir.Raw) - static_cast<short>(curDir.Raw)) < abs(static_cast<short>(leftDir.Raw) - static_cast<short>(curDir.Raw)))
								tgtDir = rightDir;
							else
								tgtDir = leftDir;
						}
						else
						{
							tgtDir = DirStruct { radian + rotate };
						}
					}

					pThis->SecondaryFacing.SetDesired(tgtDir);
				}
			}
		}
	}

	if (!pThis->Locomotor->Is_Moving_Now())
	{
		if ((pTypeExt || (pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type), pTypeExt)) && pTypeExt->Turret_BodyRotation_Enable)
		{
			const auto rotate = pTypeExt->Turret_BodyRotation_Angle * (Math::Pi / 180.0);
			const auto curDir = pThis->PrimaryFacing.Current();

			const auto source = pThis->Location;
			const auto target = pTarget->GetCoords();
			const auto radian = Math::atan2(source.Y - target.Y, target.X - source.X);
			DirStruct tgtDir;

			if (pTypeExt->Turret_BodyRotation_Symmetric)
			{
				const auto rightDir = DirStruct { radian + rotate };
				const auto leftDir = DirStruct { radian - rotate };

				if (abs(static_cast<short>(rightDir.Raw) - static_cast<short>(curDir.Raw)) < abs(static_cast<short>(leftDir.Raw) - static_cast<short>(curDir.Raw)))
					tgtDir = rightDir;
				else
					tgtDir = leftDir;
			}
			else
			{
				tgtDir = DirStruct { radian + rotate };
			}

			if (abs(static_cast<short>(tgtDir.Raw) - static_cast<short>(curDir.Raw)) >= 8192)
				pThis->PrimaryFacing.SetDesired(tgtDir);
		}
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x736AFB, UnitClass_UpdateRotation_CheckTurnToForward, 0x6)
{
	enum { SkipGameCode = 0x736BE2, ContinueGameCode = 0x736B21 };

	GET(UnitClass* const, pThis, ESI);

	// Repeatedly judging TurretSpins and IsRotating() is unnecessary
	pThis->unknown_bool_6AF = true;

	if (const auto pExt = TechnoExt::ExtMap.Find(pThis))
	{
		if (pExt->UnitIdleActionTimer.IsTicking() || pExt->UnitIdleActionGapTimer.IsTicking() || pExt->UnitIdleIsSelected)
			return ContinueGameCode;
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x736B7E, UnitClass_UpdateRotation_ApplyUnitIdleAction, 0xA)
{
	enum { SkipGameCode = 0x736BE2 };

	GET(UnitClass* const, pThis, ESI);

	const auto pWeaponStruct = pThis->GetTurretWeapon();
	const auto pExt = TechnoExt::ExtMap.Find(pThis);
	const auto currentMission = pThis->CurrentMission;

	if ((pWeaponStruct && pWeaponStruct->WeaponType && pWeaponStruct->TurretLocked) || (currentMission == Mission::Harmless && pThis->Owner == HouseClass::FindSpecial()))
	{
		// Vanilla TurretLocked state and driver been killed state
		if (pExt)
			pExt->StopIdleAction();

		pThis->SecondaryFacing.SetDesired(pThis->PrimaryFacing.Current());
	}
	else
	{
		// Point to mouse
		if (pExt && pExt->UnitIdleActionSelected && pThis->Owner->IsControlledByCurrentPlayer())
			pExt->ManualIdleAction();

		if (!pExt->UnitIdleIsSelected)
		{
			const auto pDestination = pThis->Destination;

			if (!pDestination || locomotion_cast<JumpjetLocomotionClass*>(pThis->Locomotor))
			{
				// Idle main
				if (pExt && pExt->UnitIdleAction && (currentMission == Mission::Guard || currentMission == Mission::Sticky))
					pExt->ApplyIdleAction();
				else
					pThis->SecondaryFacing.SetDesired(pThis->PrimaryFacing.Current());
			}
			else
			{
				// Turn to destination
				if (pExt)
					pExt->StopIdleAction();

				const auto source = pThis->Location;
				const auto target = pDestination->GetCoords();
				const auto tgtDir = DirStruct { Math::atan2(source.Y - target.Y, target.X - source.X) };
				pThis->SecondaryFacing.SetDesired(pThis->GetTargetDirection(pDestination));
			}
		}
	}

	return SkipGameCode;
}

#pragma endregion

#pragma region CheckFacing

DEFINE_HOOK(0x7410BB, UnitClass_GetFireError_CheckFacingError, 0x8)
{
	enum { NoNeedToCheck = 0x74132B, ContinueCheck = 0x7410C3 };

	GET(FireError, fireError, EAX);

	if (fireError == FireError::OK)
		return ContinueCheck;

	GET(UnitClass*, pThis, ESI);

	return (fireError == FireError::REARM && !pThis->Type->Turret && !pThis->IsWarpingIn()) ? ContinueCheck : NoNeedToCheck;
}

DEFINE_HOOK(0x7412BB, UnitClass_GetFireError_CheckFacingDeviation, 0x7)
{
	enum { SkipGameCode = 0x7412D4 };

	GET(UnitClass* const, pThis, ESI);
	GET(AbstractClass* const, pTarget, EBP);
	GET(BulletTypeClass* const, pBulletType, EDX);
	GET(DirStruct, curDir, EDI);
	GET(DirStruct*, pTargetDir, EAX);

	const auto source = pThis->Location;
	const auto target = pTarget->GetCoords();
	const auto radian = Math::atan2(source.Y - target.Y, target.X - source.X);

	if (pThis->Type->Turret)
	{
		if (const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pThis->Type))
		{
			const auto rotate = pTypeExt->Turret_SelfRotation_Angle * (Math::Pi / 180.0);

			if (pTypeExt->Turret_SelfRotation_Symmetric)
			{
				const auto rightDir = DirStruct { radian + rotate };
				const auto leftDir = DirStruct { radian - rotate };

				if (abs(static_cast<short>(rightDir.Raw) - static_cast<short>(curDir.Raw)) < abs(static_cast<short>(leftDir.Raw) - static_cast<short>(curDir.Raw)))
					*pTargetDir = rightDir;
				else
					*pTargetDir = leftDir;
			}
			else
			{
				*pTargetDir = DirStruct { radian + rotate };
			}
		}
		else
		{
			*pTargetDir = DirStruct { radian };
		}
	}
	else
	{
		*pTargetDir = DirStruct { radian };
	}

	R->EBX(pBulletType->ROT ? 16 : 8);
	R->EAX(pTargetDir);
	return SkipGameCode;
}

#pragma endregion
