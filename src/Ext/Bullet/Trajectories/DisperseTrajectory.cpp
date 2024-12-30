#include "DisperseTrajectory.h"
#include "StraightTrajectory.h"
#include "BombardTrajectory.h"
#include "EngraveTrajectory.h"
#include "ParabolaTrajectory.h"
#include "TracingTrajectory.h"

#include <AnimClass.h>
#include <LaserDrawClass.h>
#include <EBolt.h>
#include <RadBeam.h>
#include <ParticleSystemClass.h>
#include <ScenarioClass.h>
#include <AircraftTrackerClass.h>

#include <Ext/Bullet/Body.h>
#include <Ext/BulletType/Body.h>
#include <Ext/Techno/Body.h>
#include <Ext/WeaponType/Body.h>
#include <Ext/Anim/Body.h>
#include <Utilities/EnumFunctions.h>
#include <Utilities/Helpers.Alex.h>

std::unique_ptr<PhobosTrajectory> DisperseTrajectoryType::CreateInstance() const
{
	return std::make_unique<DisperseTrajectory>(this);
}

template<typename T>
void DisperseTrajectoryType::Serialize(T& Stm)
{
	Stm
		.Process(this->UniqueCurve)
		.Process(this->PreAimCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->FacingCoord)
		.Process(this->ReduceCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
		.Process(this->LaunchSpeed)
		.Process(this->Acceleration)
		.Process(this->ROT)
		.Process(this->LockDirection)
		.Process(this->CruiseEnable)
		.Process(this->CruiseUnableRange)
		.Process(this->LeadTimeCalculate)
		.Process(this->TargetSnapDistance)
		.Process(this->RetargetRadius)
		.Process(this->RetargetAllies)
		.Process(this->SuicideShortOfROT)
		.Process(this->SuicideAboveRange)
		.Process(this->SuicideIfNoWeapon)
		.Process(this->Weapons)
		.Process(this->WeaponBurst)
		.Process(this->WeaponCount)
		.Process(this->WeaponDelay)
		.Process(this->WeaponInitialDelay)
		.Process(this->WeaponEffectiveRange)
		.Process(this->WeaponSeparate)
		.Process(this->WeaponRetarget)
		.Process(this->WeaponLocation)
		.Process(this->WeaponTendency)
		.Process(this->WeaponHolistic)
		.Process(this->WeaponMarginal)
		.Process(this->WeaponToAllies)
		.Process(this->WeaponDoRepeat)
		;
}

bool DisperseTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);
	this->Serialize(Stm);
	return true;
}

bool DisperseTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);
	const_cast<DisperseTrajectoryType*>(this)->Serialize(Stm);
	return true;
}

void DisperseTrajectoryType::Read(CCINIClass* const pINI, const char* pSection)
{
	INI_EX exINI(pINI);

	this->Trajectory_Speed = Math::min(256.0, this->Trajectory_Speed);
	this->UniqueCurve.Read(exINI, pSection, "Trajectory.Disperse.UniqueCurve");
	this->PreAimCoord.Read(exINI, pSection, "Trajectory.Disperse.PreAimCoord");
	this->RotateCoord.Read(exINI, pSection, "Trajectory.Disperse.RotateCoord");
	this->MirrorCoord.Read(exINI, pSection, "Trajectory.Disperse.MirrorCoord");
	this->FacingCoord.Read(exINI, pSection, "Trajectory.Disperse.FacingCoord");
	this->ReduceCoord.Read(exINI, pSection, "Trajectory.Disperse.ReduceCoord");
	this->UseDisperseBurst.Read(exINI, pSection, "Trajectory.Disperse.UseDisperseBurst");
	this->AxisOfRotation.Read(exINI, pSection, "Trajectory.Disperse.AxisOfRotation");
	this->LaunchSpeed.Read(exINI, pSection, "Trajectory.Disperse.LaunchSpeed");
	this->LaunchSpeed = Math::clamp(this->LaunchSpeed, 0.001, 256.0);
	this->Acceleration.Read(exINI, pSection, "Trajectory.Disperse.Acceleration");
	this->ROT.Read(exINI, pSection, "Trajectory.Disperse.ROT");
	this->LockDirection.Read(exINI, pSection, "Trajectory.Disperse.LockDirection");
	this->CruiseEnable.Read(exINI, pSection, "Trajectory.Disperse.CruiseEnable");
	this->CruiseUnableRange.Read(exINI, pSection, "Trajectory.Disperse.CruiseUnableRange");
	this->CruiseUnableRange = Math::max(0.5, this->CruiseUnableRange);
	this->LeadTimeCalculate.Read(exINI, pSection, "Trajectory.Disperse.LeadTimeCalculate");
	this->TargetSnapDistance.Read(exINI, pSection, "Trajectory.Disperse.TargetSnapDistance");
	this->RetargetRadius.Read(exINI, pSection, "Trajectory.Disperse.RetargetRadius");
	this->RetargetAllies.Read(exINI, pSection, "Trajectory.Disperse.RetargetAllies");
	this->SuicideShortOfROT.Read(exINI, pSection, "Trajectory.Disperse.SuicideShortOfROT");
	this->SuicideAboveRange.Read(exINI, pSection, "Trajectory.Disperse.SuicideAboveRange");
	this->SuicideIfNoWeapon.Read(exINI, pSection, "Trajectory.Disperse.SuicideIfNoWeapon");
	this->Weapons.Read(exINI, pSection, "Trajectory.Disperse.Weapons");
	this->WeaponBurst.Read(exINI, pSection, "Trajectory.Disperse.WeaponBurst");
	this->WeaponCount.Read(exINI, pSection, "Trajectory.Disperse.WeaponCount");
	this->WeaponDelay.Read(exINI, pSection, "Trajectory.Disperse.WeaponDelay");
	this->WeaponDelay = Math::max(1, this->WeaponDelay);
	this->WeaponInitialDelay.Read(exINI, pSection, "Trajectory.Disperse.WeaponInitialDelay");
	this->WeaponEffectiveRange.Read(exINI, pSection, "Trajectory.Disperse.WeaponEffectiveRange");
	this->WeaponSeparate.Read(exINI, pSection, "Trajectory.Disperse.WeaponSeparate");
	this->WeaponRetarget.Read(exINI, pSection, "Trajectory.Disperse.WeaponRetarget");
	this->WeaponLocation.Read(exINI, pSection, "Trajectory.Disperse.WeaponLocation");
	this->WeaponTendency.Read(exINI, pSection, "Trajectory.Disperse.WeaponTendency");
	this->WeaponHolistic.Read(exINI, pSection, "Trajectory.Disperse.WeaponHolistic");
	this->WeaponMarginal.Read(exINI, pSection, "Trajectory.Disperse.WeaponMarginal");
	this->WeaponToAllies.Read(exINI, pSection, "Trajectory.Disperse.WeaponToAllies");
	this->WeaponDoRepeat.Read(exINI, pSection, "Trajectory.Disperse.WeaponDoRepeat");
}

template<typename T>
void DisperseTrajectory::Serialize(T& Stm)
{
	Stm
		.Process(this->Type)
		.Process(this->Speed)
		.Process(this->PreAimCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->SuicideAboveRange)
		.Process(this->WeaponCount)
		.Process(this->WeaponTimer)
		.Process(this->InStraight)
		.Process(this->Accelerate)
		.Process(this->TargetInTheAir)
		.Process(this->TargetIsTechno)
		.Process(this->OriginalDistance)
		.Process(this->CurrentBurst)
		.Process(this->ThisWeaponIndex)
		.Process(this->LastTargetCoord)
		.Process(this->PreAimDistance)
		.Process(this->LastReviseMult)
		.Process(this->FirepowerMult)
		;
}

bool DisperseTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->Serialize(Stm);
	return true;
}

bool DisperseTrajectory::Save(PhobosStreamWriter& Stm) const
{
	const_cast<DisperseTrajectory*>(this)->Serialize(Stm);
	return true;
}

void DisperseTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	const auto pType = this->Type;
	this->WeaponTimer.Start(pType->WeaponInitialDelay > 0 ? pType->WeaponInitialDelay : 0);
	this->TargetIsTechno = static_cast<bool>(abstract_cast<TechnoClass*>(pBullet->Target));
	this->OriginalDistance = static_cast<int>(pBullet->TargetCoords.DistanceFrom(pBullet->SourceCoords));
	this->LastTargetCoord = pBullet->TargetCoords;

	if (const auto pTarget = abstract_cast<ObjectClass*>(pBullet->Target))
		this->TargetInTheAir = (pTarget->GetHeight() > Unsorted::CellHeight);
	else
		this->TargetInTheAir = false;

	this->PreAimDistance = !pType->ReduceCoord ? (this->PreAimCoord.Magnitude() + this->Speed) : (this->PreAimCoord.Magnitude() * this->OriginalDistance / 2560 + this->Speed);

	if (const auto pFirer = pBullet->Owner)
	{
		this->CurrentBurst = pFirer->CurrentBurstIndex;
		this->FirepowerMult = pFirer->FirepowerMultiplier;

		if (const auto pExt = TechnoExt::ExtMap.Find(pFirer))
			this->FirepowerMult *= pExt->AE.FirepowerMultiplier;

		if (pType->MirrorCoord && pFirer->CurrentBurstIndex % 2 == 1)
			this->PreAimCoord.Y = -(this->PreAimCoord.Y);
	}

	if (pType->UniqueCurve)
	{
		pBullet->Velocity.X = 0;
		pBullet->Velocity.Y = 0;
		pBullet->Velocity.Z = 4.0;

		this->UseDisperseBurst = false;

		if (this->OriginalDistance < 1280)
			this->OriginalDistance = static_cast<int>(this->OriginalDistance * 1.2) + 512;
		else if (this->OriginalDistance > 3840)
			this->OriginalDistance = static_cast<int>(this->OriginalDistance * 0.4) + 512;
		else
			this->OriginalDistance = 2048;
	}
	else
	{
		if (this->PreAimCoord == CoordStruct::Empty)
		{
			this->InStraight = true;
			pBullet->Velocity.X = pBullet->TargetCoords.X - pBullet->SourceCoords.X;
			pBullet->Velocity.Y = pBullet->TargetCoords.Y - pBullet->SourceCoords.Y;
			pBullet->Velocity.Z = pBullet->TargetCoords.Z - pBullet->SourceCoords.Z;
		}
		else
		{
			this->InitializeBulletNotCurve(pBullet, pType->FacingCoord);
		}

		if (this->CalculateBulletVelocity(pBullet, this->Speed))
			this->SuicideAboveRange = 0.001;
	}
}

bool DisperseTrajectory::OnAI(BulletClass* pBullet)
{
	if (MapClass::Instance->GetCellFloorHeight(pBullet->Location) > pBullet->Location.Z)
		return true;

	const auto pType = this->Type;

	if (pBullet->TargetCoords.DistanceFrom(pBullet->Location) < (pType->UniqueCurve ? 154 : pType->TargetSnapDistance.Get()))
		return true;

	if (this->WeaponCount && (!pType->WeaponEffectiveRange.Get() || pBullet->TargetCoords.DistanceFrom(pBullet->Location) <= pType->WeaponEffectiveRange.Get()) && this->PrepareDisperseWeapon(pBullet))
		return true;

	if (pType->UniqueCurve ? this->CurveVelocityChange(pBullet) : this->NotCurveVelocityChange(pBullet))
		return true;

	return false;
}

void DisperseTrajectory::OnAIPreDetonate(BulletClass* pBullet)
{
	const auto pTarget = abstract_cast<ObjectClass*>(pBullet->Target);
	const auto coords = pTarget ? pTarget->GetCoords() : pBullet->Data.Location;
	const auto pType = this->Type;

	if (coords.DistanceFrom(pBullet->Location) <= pType->TargetSnapDistance.Get())
	{
		const auto pExt = BulletExt::ExtMap.Find(pBullet);
		pExt->SnappedToTarget = true;
		pBullet->SetLocation(coords);
	}

	if (pType->WeaponEffectiveRange.Get() < 0 && this->WeaponCount)
	{
		this->WeaponTimer.StartTime = 0;
		this->PrepareDisperseWeapon(pBullet);
	}
}

void DisperseTrajectory::OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition)
{
	pSpeed->Z += BulletTypeExt::GetAdjustedGravity(pBullet->Type);
}

TrajectoryCheckReturnType DisperseTrajectory::OnAITargetCoordCheck(BulletClass* pBullet)
{
	return TrajectoryCheckReturnType::SkipGameCheck;
}

TrajectoryCheckReturnType DisperseTrajectory::OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno)
{
	return TrajectoryCheckReturnType::SkipGameCheck;
}

void DisperseTrajectory::InitializeBulletNotCurve(BulletClass* pBullet, bool facing)
{
	const auto pType = this->Type;
	double rotateAngle = 0.0;
	const auto pFirer = pBullet->Owner;
	const auto theSource = pFirer ? pFirer->GetCoords() : pBullet->SourceCoords;

	if ((facing || (pBullet->TargetCoords.Y == theSource.Y && pBullet->TargetCoords.X == theSource.X)) && pFirer)
	{
		if (pFirer->HasTurret())
			rotateAngle = -(pFirer->TurretFacing().GetRadian<32>());
		else
			rotateAngle = -(pFirer->PrimaryFacing.Current().GetRadian<32>());
	}
	else
	{
		rotateAngle = Math::atan2(pBullet->TargetCoords.Y - theSource.Y , pBullet->TargetCoords.X - theSource.X);
	}

	const auto coordMult = (pType->ROT > 1e-10) ? (this->OriginalDistance / (32768 / pType->ROT)) : 1.0;

	if (pType->ReduceCoord && coordMult < 1.0)
	{
		CoordStruct theAimCoord
		{
			static_cast<int>(this->PreAimCoord.X * Math::cos(rotateAngle) + this->PreAimCoord.Y * Math::sin(rotateAngle)),
			static_cast<int>(this->PreAimCoord.X * Math::sin(rotateAngle) - this->PreAimCoord.Y * Math::cos(rotateAngle)),
			this->PreAimCoord.Z
		};

		auto theDistance = pBullet->TargetCoords - pBullet->SourceCoords;
		auto theDifferece = theDistance - theAimCoord;

		pBullet->Velocity.X = theAimCoord.X + (1 - coordMult) * theDifferece.X;
		pBullet->Velocity.Y = theAimCoord.Y + (1 - coordMult) * theDifferece.Y;
		pBullet->Velocity.Z = theAimCoord.Z + (1 - coordMult) * theDifferece.Z;
	}
	else
	{
		pBullet->Velocity.X = this->PreAimCoord.X * Math::cos(rotateAngle) + this->PreAimCoord.Y * Math::sin(rotateAngle);
		pBullet->Velocity.Y = this->PreAimCoord.X * Math::sin(rotateAngle) - this->PreAimCoord.Y * Math::cos(rotateAngle);
		pBullet->Velocity.Z = this->PreAimCoord.Z;
	}

	if (!this->UseDisperseBurst && std::abs(pType->RotateCoord) > 1e-10 && pBullet->WeaponType && pBullet->WeaponType->Burst > 1)
	{
		const auto axis = pType->AxisOfRotation.Get();

		BulletVelocity rotationAxis
		{
			axis.X * Math::cos(rotateAngle) + axis.Y * Math::sin(rotateAngle),
			axis.X * Math::sin(rotateAngle) - axis.Y * Math::cos(rotateAngle),
			static_cast<double>(axis.Z)
		};

		double extraRotate = 0.0;

		if (pType->MirrorCoord)
		{
			if (this->CurrentBurst % 2 == 1)
				rotationAxis *= -1;

			extraRotate = Math::Pi * (pType->RotateCoord * ((this->CurrentBurst / 2) / (pBullet->WeaponType->Burst - 1.0) - 0.5)) / 180;
		}
		else
		{
			extraRotate = Math::Pi * (pType->RotateCoord * (this->CurrentBurst / (pBullet->WeaponType->Burst - 1.0) - 0.5)) / 180;
		}

		pBullet->Velocity = this->RotateAboutTheAxis(pBullet->Velocity, rotationAxis, extraRotate);
	}
}

inline BulletVelocity DisperseTrajectory::RotateAboutTheAxis(BulletVelocity theSpeed, BulletVelocity theAxis, double theRadian)
{
	const auto theAxisLengthSquared = theAxis.MagnitudeSquared();

	if (std::abs(theAxisLengthSquared) < 1e-10)
		return theSpeed;

	theAxis *= 1 / sqrt(theAxisLengthSquared);
	const auto cosRotate = Math::cos(theRadian);

	return ((theSpeed * cosRotate) + (theAxis * ((1 - cosRotate) * (theSpeed * theAxis))) + (theAxis.CrossProduct(theSpeed) * Math::sin(theRadian)));
}

bool DisperseTrajectory::CalculateBulletVelocity(BulletClass* pBullet, double trajectorySpeed)
{
	const auto velocityLength = pBullet->Velocity.Magnitude();

	if (velocityLength > 1e-10)
		pBullet->Velocity *= trajectorySpeed / velocityLength;
	else
		return true;

	return false;
}

bool DisperseTrajectory::BulletRetargetTechno(BulletClass* pBullet)
{
	const auto pType = this->Type;
	bool check = false;

	if (this->TargetIsTechno)
	{
		if (!pBullet->Target)
			check = true;
		else if (const auto pTargetTechno = abstract_cast<TechnoClass*>(pBullet->Target))
			check = this->CheckTechnoIsInvalid(pTargetTechno);
	}

	if (!check)
		return false;

	if (pType->RetargetRadius < 0)
		return true;

	auto pOwner = pBullet->Owner ? pBullet->Owner->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse;

	if (!pOwner || pOwner->Defeated)
	{
		if (const auto pNeutral = HouseClass::FindNeutral())
			pOwner = pNeutral;
		else
			return false;
	}

	const auto retargetRange = pType->RetargetRadius * Unsorted::LeptonsPerCell;
	auto retargetCoords = pBullet->TargetCoords;
	TechnoClass* pNewTechno = nullptr;

	if (this->InStraight)
	{
		const auto futureVelocity = pBullet->Velocity * (retargetRange / this->Speed);
		retargetCoords.X = pBullet->Location.X + static_cast<int>(futureVelocity.X);
		retargetCoords.Y = pBullet->Location.Y + static_cast<int>(futureVelocity.Y);
		retargetCoords.Z = pBullet->Location.Z;
	}

	if (!this->TargetInTheAir) // Only get same type (on ground / in air)
	{
		const auto retargetCell = CellClass::Coord2Cell(retargetCoords);

		for (CellSpreadEnumerator thisCell(static_cast<size_t>(pType->RetargetRadius + 0.99)); thisCell; ++thisCell)
		{
			if (const auto pCell = MapClass::Instance->TryGetCellAt(*thisCell + retargetCell))
			{
				auto pObject = pCell->GetContent();

				while (pObject)
				{
					const auto pTechno = abstract_cast<TechnoClass*>(pObject);
					pObject = pObject->NextObject;

					if (!pTechno || this->CheckTechnoIsInvalid(pTechno))
						continue;

					const auto pTechnoType = pTechno->GetTechnoType();

					if (!pTechnoType->LegalTarget)
						continue;

					const auto absType = pTechno->WhatAmI();

					if (absType == AbstractType::Building && static_cast<BuildingClass*>(pTechno)->Type->InvisibleInGame)
						continue;

					const auto pHouse = pTechno->Owner;

					if (pOwner->IsAlliedWith(pHouse))
					{
						if (!pType->RetargetAllies)
							continue;
					}
					else
					{
						if (!pType->RetargetAllies && absType == AbstractType::Infantry && pTechno->IsDisguisedAs(pOwner) && !pCell->DisguiseSensors_InclHouse(pOwner->ArrayIndex))
							continue;

						if (absType == AbstractType::Unit && pTechno->IsDisguised() && !pCell->DisguiseSensors_InclHouse(pOwner->ArrayIndex))
							continue;

						if (pTechno->CloakState == CloakState::Cloaked && !pCell->Sensors_InclHouse(pOwner->ArrayIndex))
							continue;
					}

					if (MapClass::GetTotalDamage(100, pBullet->WH, pTechnoType->Armor, 0) == 0)
						continue;

					if (pTechno->GetCoords().DistanceFrom(retargetCoords) > retargetRange)
						continue;

					const auto pWeapon = pBullet->WeaponType;

					if (pWeapon)
					{
						const auto pFirer = pBullet->Owner;

						if (pTechno->GetCoords().DistanceFrom(pFirer ? pFirer->GetCoords() : pBullet->SourceCoords) > pWeapon->Range)
							continue;

						if (!this->CheckWeaponCanTarget(WeaponTypeExt::ExtMap.Find(pWeapon), pFirer, pTechno))
							continue;
					}

					pNewTechno = pTechno;
					break;
				}
			}

			if (pNewTechno)
				break;
		}
	}
	else
	{
		const auto airTracker = &AircraftTrackerClass::Instance;
		airTracker->FillCurrentVector(MapClass::Instance->GetCellAt(retargetCoords), Game::F2I(pType->RetargetRadius));

		for (auto pTechno = airTracker->Get(); pTechno; pTechno = airTracker->Get())
		{
			if (this->CheckTechnoIsInvalid(pTechno))
				continue;

			const auto pTechnoType = pTechno->GetTechnoType();

			if (!pTechnoType->LegalTarget)
				continue;

			const auto pHouse = pTechno->Owner;

			if (pOwner->IsAlliedWith(pHouse))
			{
				if (!pType->RetargetAllies)
					continue;
			}
			else if (const auto pCell = pTechno->GetCell())
			{
				if (pTechno->CloakState == CloakState::Cloaked && !pCell->Sensors_InclHouse(pOwner->ArrayIndex))
					continue;
			}

			if (MapClass::GetTotalDamage(100, pBullet->WH, pTechnoType->Armor, 0) == 0)
				continue;

			if (pTechno->GetCoords().DistanceFrom(retargetCoords) > retargetRange)
				continue;

			const auto pWeapon = pBullet->WeaponType;

			if (pWeapon)
			{
				const auto pFirer = pBullet->Owner;

				if (pTechno->GetCoords().DistanceFrom(pFirer ? pFirer->GetCoords() : pBullet->SourceCoords) > pWeapon->Range)
					continue;

				if (!this->CheckWeaponCanTarget(WeaponTypeExt::ExtMap.Find(pWeapon), pFirer, pTechno))
					continue;
			}

			pNewTechno = pTechno;
			break;
		}
	}

	if (pNewTechno)
	{
		pBullet->SetTarget(pNewTechno);
		pBullet->TargetCoords = pNewTechno->GetCoords();
		this->LastTargetCoord = pBullet->TargetCoords;
	}

	return false;
}

inline bool DisperseTrajectory::CheckTechnoIsInvalid(TechnoClass* pTechno)
{
	return (!pTechno->IsAlive || !pTechno->IsOnMap || pTechno->InLimbo || pTechno->IsSinking || pTechno->Health <= 0);
}

inline bool DisperseTrajectory::CheckWeaponCanTarget(WeaponTypeExt::ExtData* pWeaponExt, TechnoClass* pFirer, TechnoClass* pTarget)
{
	return !pWeaponExt || (EnumFunctions::IsTechnoEligible(pTarget, pWeaponExt->CanTarget) && pWeaponExt->HasRequiredAttachedEffects(pTarget, pFirer));
}

bool DisperseTrajectory::CurveVelocityChange(BulletClass* pBullet)
{
	const auto pTarget = pBullet->Target;
	const auto pTargetTechno = abstract_cast<TechnoClass*>(pTarget);
	const bool checkValid = (pTarget && pTarget->WhatAmI() == AbstractType::Bullet) || (pTargetTechno && !CheckTechnoIsInvalid(pTargetTechno));
	auto targetLocation = pBullet->TargetCoords;

	if (checkValid)
		targetLocation = pTarget->GetCoords();

	pBullet->TargetCoords = targetLocation;

	if (!this->InStraight)
	{
		int offHeight = this->OriginalDistance - 1600;

		if (this->OriginalDistance < 3200)
			offHeight = this->OriginalDistance / 2;

		const CoordStruct horizonVelocity { targetLocation.X - pBullet->Location.X, targetLocation.Y - pBullet->Location.Y, 0 };
		const auto horizonDistance = horizonVelocity.Magnitude();

		if (horizonDistance > 1e-10)
		{
			auto horizonMult = std::abs(pBullet->Velocity.Z / 64.0) / horizonDistance;
			pBullet->Velocity.X += horizonMult * horizonVelocity.X;
			pBullet->Velocity.Y += horizonMult * horizonVelocity.Y;
			const auto horizonLength = sqrt(pBullet->Velocity.X * pBullet->Velocity.X + pBullet->Velocity.Y * pBullet->Velocity.Y);

			if (horizonLength > 64.0)
			{
				horizonMult = 64.0 / horizonLength;
				pBullet->Velocity.X *= horizonMult;
				pBullet->Velocity.Y *= horizonMult;
			}
		}

		if ((pBullet->Location.Z - pBullet->SourceCoords.Z) < offHeight && this->Accelerate)
		{
			if (pBullet->Velocity.Z < 160.0)
				pBullet->Velocity.Z += 4.0;
		}
		else
		{
			this->Accelerate = false;
			const auto futureHeight = pBullet->Location.Z + 8 * pBullet->Velocity.Z;

			if (pBullet->Velocity.Z > -160.0)
				pBullet->Velocity.Z -= 4.0;

			if (futureHeight <= targetLocation.Z)
				this->InStraight = true;
			else if (futureHeight <= pBullet->SourceCoords.Z)
				this->InStraight = true;
		}
	}
	else
	{
		const auto timeMult = targetLocation.DistanceFrom(pBullet->Location) / 192.0;
		targetLocation.Z += static_cast<int>(timeMult * 48);

		if (checkValid)
		{
			targetLocation.X += static_cast<int>(timeMult * (targetLocation.X - this->LastTargetCoord.X));
			targetLocation.Y += static_cast<int>(timeMult * (targetLocation.Y - this->LastTargetCoord.Y));
		}

		if (this->ChangeBulletVelocity(pBullet, targetLocation, 24.0, true))
			return true;
	}

	return false;
}

bool DisperseTrajectory::NotCurveVelocityChange(BulletClass* pBullet)
{
	const auto pType = this->Type;

	if (this->SuicideAboveRange > 1e-10)
	{
		this->SuicideAboveRange -= this->Speed;

		if (this->SuicideAboveRange <= 1e-10)
			return true;
	}

	if (this->PreAimDistance > 1e-10)
		this->PreAimDistance -= this->Speed;

	bool velocityUp = false;

	if (this->Accelerate && pType->Acceleration != 0.0)
	{
		this->Speed += pType->Acceleration;

		if (pType->Acceleration > 0.0)
		{
			if (this->Speed >= pType->Trajectory_Speed)
			{
				this->Speed = pType->Trajectory_Speed;
				this->Accelerate = false;
			}
		}
		else if (this->Speed <= pType->Trajectory_Speed)
		{
			this->Speed = pType->Trajectory_Speed;
			this->Accelerate = false;
		}

		velocityUp = true;
	}

	if (!pType->LockDirection || !this->InStraight)
	{
		if (pType->RetargetRadius != 0 && this->BulletRetargetTechno(pBullet))
			return true;

		if (this->PreAimDistance <= 1e-10 && this->StandardVelocityChange(pBullet))
			return true;

		velocityUp = true;
	}

	if (velocityUp && this->CalculateBulletVelocity(pBullet, this->Speed))
		return true;

	return false;
}

bool DisperseTrajectory::StandardVelocityChange(BulletClass* pBullet)
{
	const auto pType = this->Type;
	const auto pTarget = pBullet->Target;
	const auto pTargetTechno = abstract_cast<TechnoClass*>(pTarget);
	const bool checkValid = (pTarget && pTarget->WhatAmI() == AbstractType::Bullet) || (pTargetTechno && !CheckTechnoIsInvalid(pTargetTechno));
	auto targetLocation = pBullet->TargetCoords;

	if (checkValid)
		targetLocation = pTarget->GetCoords();

	pBullet->TargetCoords = targetLocation;

	if (pType->LeadTimeCalculate && checkValid && pType->Trajectory_Speed > 64.0)
	{
		const auto leadSpeed = (pType->Trajectory_Speed + this->Speed) / 2;
		const auto timeMult = targetLocation.DistanceFrom(pBullet->Location) / leadSpeed;
		targetLocation += (targetLocation - this->LastTargetCoord) * timeMult;
	}

	if (pType->CruiseEnable && Point2D { targetLocation.X, targetLocation.Y }.DistanceFrom(Point2D { pBullet->Location.X, pBullet->Location.Y }) > (pType->CruiseUnableRange * Unsorted::LeptonsPerCell))
		targetLocation.Z = pBullet->Location.Z;

	const auto turningRadius = pType->ROT * this->Speed * this->Speed / 16384;

	return this->ChangeBulletVelocity(pBullet, targetLocation, turningRadius, false);
}

bool DisperseTrajectory::ChangeBulletVelocity(BulletClass* pBullet, CoordStruct targetLocation, double turningRadius, bool curve)
{
	const BulletVelocity targetVelocity
	{
		static_cast<double>(targetLocation.X - pBullet->Location.X),
		static_cast<double>(targetLocation.Y - pBullet->Location.Y),
		static_cast<double>(targetLocation.Z - pBullet->Location.Z)
	};

	const auto moveToVelocity = pBullet->Velocity;
	const auto futureVelocity = targetVelocity - moveToVelocity;

	auto reviseVelocity = BulletVelocity::Empty;
	auto directVelocity = BulletVelocity::Empty;

	const auto targetSquared = targetVelocity.MagnitudeSquared();
	const auto bulletSquared = moveToVelocity.MagnitudeSquared();
	const auto futureSquared = futureVelocity.MagnitudeSquared();

	const auto targetSide = sqrt(targetSquared);
	const auto bulletSide = sqrt(bulletSquared);

	const auto reviseMult = (targetSquared + bulletSquared - futureSquared);
	const auto reviseBase = 2 * targetSide * bulletSide;

	if (targetSide > 1e-10)
	{
		if (reviseMult < 0.001 * reviseBase && reviseMult > -0.001 * reviseBase)
		{
			const auto velocityMult = turningRadius / targetSide;
			pBullet->Velocity += targetVelocity * velocityMult;
		}
		else
		{
			const auto directLength = reviseBase * bulletSide / reviseMult;
			const auto velocityMult = directLength / targetSide;

			directVelocity = targetVelocity * velocityMult;

			if (directVelocity.IsCollinearTo(moveToVelocity))
			{
				if (reviseMult < 0)
					reviseVelocity.Z += turningRadius;
			}
			else
			{
				if (reviseMult > 0)
					reviseVelocity = directVelocity - moveToVelocity;
				else
					reviseVelocity = moveToVelocity - directVelocity;
			}

			const auto reviseLength = reviseVelocity.Magnitude();

			if (!curve && this->Type->SuicideShortOfROT && reviseMult < 0 && this->LastReviseMult > 0 && (this->InStraight || this->LastTargetCoord == pBullet->TargetCoords))
				return true;

			if (turningRadius < reviseLength)
			{
				reviseVelocity *= turningRadius / reviseLength;
				pBullet->Velocity += reviseVelocity;
			}
			else
			{
				pBullet->Velocity = targetVelocity;
				this->InStraight = true;
			}
		}
	}

	this->LastReviseMult = reviseMult;
	this->LastTargetCoord = pBullet->TargetCoords;

	if (curve)
	{
		auto trajectorySpeed = bulletSide;

		if (trajectorySpeed < 192.0)
			trajectorySpeed += 4.0;

		if (trajectorySpeed > 192.0)
			trajectorySpeed = 192.0;

		if (this->CalculateBulletVelocity(pBullet, trajectorySpeed))
			return true;
	}

	return false;
}

bool DisperseTrajectory::PrepareDisperseWeapon(BulletClass* pBullet)
{
	const auto pType = this->Type;

	if (this->WeaponTimer.Completed())
	{
		this->WeaponTimer.Start(pType->WeaponDelay);
		size_t validWeapons = 0;
		const auto burstSize = pType->WeaponBurst.size();

		if (burstSize)
			validWeapons = pType->Weapons.size();

		if (!validWeapons)
			return pType->SuicideIfNoWeapon;

		if (this->WeaponCount > 0)
			--this->WeaponCount;

		auto pOwner = pBullet->Owner ? pBullet->Owner->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse;

		if (!pOwner || pOwner->Defeated)
			pOwner = HouseClass::FindNeutral();

		const auto pTarget = pBullet->Target ? pBullet->Target : MapClass::Instance->TryGetCellAt(pBullet->TargetCoords);

		for (size_t weaponNum = 0; weaponNum < validWeapons; weaponNum++)
		{
			size_t curIndex = weaponNum;
			int burstCount = 0;

			if (static_cast<int>(burstSize) > this->ThisWeaponIndex)
				burstCount = pType->WeaponBurst[this->ThisWeaponIndex];
			else
				burstCount = pType->WeaponBurst[burstSize - 1];

			if (burstCount <= 0)
				continue;

			if (pType->WeaponSeparate)
			{
				curIndex = this->ThisWeaponIndex;
				weaponNum = validWeapons;

				this->ThisWeaponIndex++;
				this->ThisWeaponIndex %= validWeapons;
			}

			const auto pWeapon = pType->Weapons[curIndex];
			const auto pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon);

			if (!pType->WeaponRetarget)
			{
				if (pTarget)
				{
					for (int burstNum = 0; burstNum < burstCount; burstNum++)
					{
						this->CreateDisperseBullets(pBullet, pWeapon, pTarget, pOwner, burstNum, burstCount);
					}
				}

				continue;
			}

			int burstNow = 0;

			if (pType->WeaponTendency && burstCount > 0 && pTarget)
			{
				this->CreateDisperseBullets(pBullet, pWeapon, pTarget, pOwner, burstNow, burstCount);
				++burstNow;

				if (burstCount <= 1)
					continue;
			}

			const auto centerCoords = pType->WeaponLocation ? pBullet->Location : pBullet->TargetCoords;
			const auto centerCell = CellClass::Coord2Cell(centerCoords);

			std::vector<TechnoClass*> validTechnos;
			std::vector<ObjectClass*> validObjects;
			std::vector<CellClass*> validCells;

			const bool checkTechnos = (pWeaponExt->CanTarget & AffectedTarget::AllContents) != AffectedTarget::None;
			const bool checkObjects = pType->WeaponMarginal;
			const bool checkCells = (pWeaponExt->CanTarget & AffectedTarget::AllCells) != AffectedTarget::None;

			const int rangeSide = pWeapon->Range >> 7;
			const size_t initialSize = rangeSide * rangeSide;

			if (checkTechnos)
				validTechnos.reserve(initialSize);

			if (checkObjects)
				validObjects.reserve(initialSize >> 1);

			if (checkCells)
				validCells.reserve(initialSize);

			if (pType->WeaponHolistic || !this->TargetInTheAir || checkCells)
			{
				for (CellSpreadEnumerator thisCell(static_cast<size_t>((static_cast<double>(pWeapon->Range) / Unsorted::LeptonsPerCell) + 0.99)); thisCell; ++thisCell)
				{
					if (const auto pCell = MapClass::Instance->TryGetCellAt(*thisCell + centerCell))
					{
						if (checkCells && EnumFunctions::IsCellEligible(pCell, pWeaponExt->CanTarget, true, true))
							validCells.push_back(pCell);

						auto pObject = pCell->GetContent();

						while (pObject)
						{
							const auto pTechno = abstract_cast<TechnoClass*>(pObject);

							if (!pTechno)
							{
								if (checkObjects && pObject != pTarget)
								{
									const auto pObjType = pObject->GetType();

									if (pObjType && !pObjType->Immune)
										validObjects.push_back(pObject);
								}

								pObject = pObject->NextObject;
								continue;
							}

							pObject = pObject->NextObject;

							if (!checkTechnos || this->CheckTechnoIsInvalid(pTechno))
								continue;

							const auto pTechnoType = pTechno->GetTechnoType();

							if (!pTechnoType->LegalTarget)
								continue;

							if (pType->WeaponTendency && pTechno == pTarget)
								continue;

							const auto absType = pTechno->WhatAmI();

							if (absType == AbstractType::Building)
							{
								if (static_cast<BuildingClass*>(pTechno)->Type->InvisibleInGame)
									continue;

								if (std::find(validTechnos.begin(), validTechnos.end(), pTechno) != validTechnos.end())
									continue;
							}

							const auto pHouse = pTechno->Owner;

							if (pOwner->IsAlliedWith(pHouse))
							{
								if (!pType->WeaponToAllies)
									continue;
							}
							else
							{
								if (!pType->WeaponToAllies && absType == AbstractType::Infantry && pTechno->IsDisguisedAs(pOwner) && !pCell->DisguiseSensors_InclHouse(pOwner->ArrayIndex))
									continue;

								if (absType == AbstractType::Unit && pTechno->IsDisguised() && !pCell->DisguiseSensors_InclHouse(pOwner->ArrayIndex))
									continue;

								if (pTechno->CloakState == CloakState::Cloaked && !pCell->Sensors_InclHouse(pOwner->ArrayIndex))
									continue;
							}

							if (MapClass::GetTotalDamage(100, pWeapon->Warhead, pTechnoType->Armor, 0) == 0)
								continue;

							if (!this->CheckWeaponCanTarget(pWeaponExt, pBullet->Owner, pTechno))
								continue;

							validTechnos.push_back(pTechno);
						}
					}
				}
			}

			if ((pType->WeaponHolistic || this->TargetInTheAir) && checkTechnos)
			{
				const auto airTracker = &AircraftTrackerClass::Instance;
				airTracker->FillCurrentVector(MapClass::Instance->GetCellAt(centerCoords), Game::F2I(static_cast<double>(pWeapon->Range) / Unsorted::LeptonsPerCell));

				for (auto pTechno = airTracker->Get(); pTechno; pTechno = airTracker->Get())
				{
					if (this->CheckTechnoIsInvalid(pTechno))
						continue;

					const auto pTechnoType = pTechno->GetTechnoType();

					if (!pTechnoType->LegalTarget)
						continue;

					if (pType->WeaponTendency && pTechno == pTarget)
						continue;

					const auto pHouse = pTechno->Owner;

					if (pOwner->IsAlliedWith(pHouse))
					{
						if (!pType->WeaponToAllies)
							continue;
					}
					else if (const auto pCell = pTechno->GetCell())
					{
						if (pTechno->CloakState == CloakState::Cloaked && !pCell->Sensors_InclHouse(pOwner->ArrayIndex))
							continue;
					}

					if (MapClass::GetTotalDamage(100, pWeapon->Warhead, pTechnoType->Armor, 0) == 0)
						continue;

					if (!this->CheckWeaponCanTarget(pWeaponExt, pBullet->Owner, pTechno))
						continue;

					validTechnos.push_back(pTechno);
				}
			}

			int validTechnoNums = validTechnos.size();
			int validObjectNums = validObjects.size();
			int validCellNums = validCells.size();
			std::vector<AbstractClass*> validTargets;
			validTargets.reserve(burstCount);

			// TODO Simplify these codes
			if (pType->WeaponDoRepeat)
			{
				if (validTechnoNums)
				{
					int currentCount = burstNow + validTechnoNums;

					for (; currentCount <= burstCount; currentCount += validTechnoNums)
					{
						for (int burstNum = 0; burstNum < validTechnoNums; ++burstNum)
						{
							const auto pNewTarget = validTechnos[burstNum];
							validTargets.push_back(pNewTarget);
						}
					}

					for (auto burstNum = currentCount - validTechnoNums; burstNum < burstCount; ++burstNum)
					{
						const auto randomIndex = ScenarioClass::Instance->Random.RandomRanged(0, validTechnoNums - 1);
						const auto pNewTarget = validTechnos[randomIndex];
						validTargets.push_back(pNewTarget);
						std::swap(validTechnos[randomIndex], validTechnos[--validTechnoNums]);
					}
				}
				else if (validObjectNums)
				{
					int currentCount = burstNow + validObjectNums;

					for (; currentCount <= burstCount; currentCount += validObjectNums)
					{
						for (int burstNum = 0; burstNum < validObjectNums; ++burstNum)
						{
							const auto pNewTarget = validObjects[burstNum];
							validTargets.push_back(pNewTarget);
						}
					}

					for (auto burstNum = currentCount - validObjectNums; burstNum < burstCount; ++burstNum)
					{
						const auto randomIndex = ScenarioClass::Instance->Random.RandomRanged(0, validObjectNums - 1);
						const auto pNewTarget = validObjects[randomIndex];
						validTargets.push_back(pNewTarget);
						std::swap(validObjects[randomIndex], validObjects[--validObjectNums]);
					}
				}
				else if (validCellNums)
				{
					int currentCount = burstNow + validCellNums;

					for (; currentCount <= burstCount; currentCount += validCellNums)
					{
						for (int burstNum = 0; burstNum < validCellNums; ++burstNum)
						{
							const auto pNewTarget = validCells[burstNum];
							validTargets.push_back(pNewTarget);
						}
					}

					for (auto burstNum = currentCount - validCellNums; burstNum < burstCount; ++burstNum)
					{
						const auto randomIndex = ScenarioClass::Instance->Random.RandomRanged(0, validCellNums - 1);
						const auto pNewTarget = validCells[randomIndex];
						validTargets.push_back(pNewTarget);
						std::swap(validCells[randomIndex], validCells[--validCellNums]);
					}
				}
			}
			else
			{
				if (burstCount - burstNow >= validTechnoNums)
				{
					for (int burstNum = 0; burstNum < validTechnoNums; ++burstNum)
					{
						const auto pNewTarget = validTechnos[burstNum];
						validTargets.push_back(pNewTarget);
					}

					const auto currentCount1 = burstNow + validTechnoNums;

					if (burstCount - currentCount1 >= validObjectNums)
					{
						for (int burstNum = 0; burstNum < validObjectNums; ++burstNum)
						{
							const auto pNewTarget = validObjects[burstNum];
							validTargets.push_back(pNewTarget);
						}

						const auto currentCount2 = currentCount1 + validObjectNums;

						if (burstCount - currentCount2 >= validCellNums)
						{
							for (int burstNum = 0; burstNum < validCellNums; ++burstNum)
							{
								const auto pNewTarget = validCells[burstNum];
								validTargets.push_back(pNewTarget);
							}
						}
						else
						{
							for (auto burstNum = currentCount2; burstNum < burstCount; ++burstNum)
							{
								const auto randomIndex = ScenarioClass::Instance->Random.RandomRanged(0, validCellNums - 1);
								const auto pNewTarget = validCells[randomIndex];
								validTargets.push_back(pNewTarget);
								std::swap(validCells[randomIndex], validCells[--validCellNums]);
							}
						}
					}
					else
					{
						for (auto burstNum = currentCount1; burstNum < burstCount; ++burstNum)
						{
							const auto randomIndex = ScenarioClass::Instance->Random.RandomRanged(0, validObjectNums - 1);
							const auto pNewTarget = validObjects[randomIndex];
							validTargets.push_back(pNewTarget);
							std::swap(validObjects[randomIndex], validObjects[--validObjectNums]);
						}
					}
				}
				else
				{
					for (auto burstNum = burstNow; burstNum < burstCount; ++burstNum)
					{
						const auto randomIndex = ScenarioClass::Instance->Random.RandomRanged(0, validTechnoNums - 1);
						const auto pNewTarget = validTechnos[randomIndex];
						validTargets.push_back(pNewTarget);
						std::swap(validTechnos[randomIndex], validTechnos[--validTechnoNums]);
					}
				}
			}

			for (const auto& pNewTarget : validTargets)
			{
				this->CreateDisperseBullets(pBullet, pWeapon, pNewTarget, pOwner, burstNow, burstCount);
				++burstNow;
			}
		}
	}

	if(pType->SuicideIfNoWeapon && !this->WeaponCount)
		return true;

	return false;
}

void DisperseTrajectory::CreateDisperseBullets(BulletClass* pBullet, WeaponTypeClass* pWeapon, AbstractClass* pTarget, HouseClass* pOwner, int curBurst, int maxBurst)
{
	const auto finalDamage = static_cast<int>(pWeapon->Damage * this->FirepowerMult);

	if (const auto pCreateBullet = pWeapon->Projectile->CreateBullet(pTarget, pBullet->Owner, finalDamage, pWeapon->Warhead, pWeapon->Speed, pWeapon->Bright))
	{
		BulletExt::SimulatedFiringInfos(pCreateBullet, pWeapon, pOwner, WeaponTypeExt::ExtMap.Find(pWeapon)->ProjectileRange.Get());
		BulletExt::SimulatedFiringVelocity(pCreateBullet, pBullet->Location, false);

		const auto pBulletExt = BulletExt::ExtMap.Find(pCreateBullet);
		const auto pTraj = pBulletExt->Trajectory.get();

		if (pTraj)
		{
			const auto flag = pTraj->Flag();

			if (flag == TrajectoryFlag::Disperse)
			{
				const auto pTrajectory = static_cast<DisperseTrajectory*>(pTraj);
				const auto pTrajType = pTrajectory->Type;
				pTrajectory->FirepowerMult = this->FirepowerMult;

				//The created bullet's velocity calculation has been completed, so we should stack the calculations.
				if (pTrajectory->UseDisperseBurst && std::abs(pTrajType->RotateCoord) > 1e-10 && curBurst >= 0 && maxBurst > 1 && !pTrajType->UniqueCurve && pTrajectory->PreAimCoord != CoordStruct::Empty)
					this->DisperseBurstSubstitution(pCreateBullet, pTrajType->AxisOfRotation.Get(), pTrajType->RotateCoord, curBurst, maxBurst, pTrajType->MirrorCoord);
			}
			else if (flag == TrajectoryFlag::Straight)
			{
				const auto pTrajectory = static_cast<StraightTrajectory*>(pTraj);
				const auto pTrajType = pTrajectory->Type;
				pTrajectory->FirepowerMult = this->FirepowerMult;

				//The straight trajectory bullets has LeadTimeCalculate=true are not calculate its velocity yet.
				if (pTrajectory->UseDisperseBurst && std::abs(pTrajType->RotateCoord) > 1e-10 && curBurst >= 0 && maxBurst > 1)
				{
					if (pTrajType->LeadTimeCalculate && abstract_cast<FootClass*>(pTarget))
					{
						pTrajectory->CurrentBurst = curBurst;
						pTrajectory->CountOfBurst = maxBurst;
						pTrajectory->UseDisperseBurst = false;
					}
					else
					{
						this->DisperseBurstSubstitution(pCreateBullet, pTrajType->AxisOfRotation.Get(), pTrajType->RotateCoord, curBurst, maxBurst, pTrajType->MirrorCoord);
					}
				}
			}
			else if (flag == TrajectoryFlag::Bombard)
			{
				const auto pTrajectory = static_cast<BombardTrajectory*>(pTraj);
				const auto pTrajType = pTrajectory->Type;

				//The bombard trajectory bullets without NoLaunch and FreeFallOnTarget can change the velocity.
				if (pTrajectory->UseDisperseBurst && std::abs(pTrajType->RotateCoord) > 1e-10 && curBurst >= 0 && maxBurst > 1 && (!pTrajType->NoLaunch || !pTrajType->FreeFallOnTarget))
				{
					pTrajectory->CurrentBurst = curBurst;
					pTrajectory->CountOfBurst = maxBurst;
					pTrajectory->UseDisperseBurst = false;

					if (!pTrajType->NoLaunch || !pTrajType->LeadTimeCalculate || !abstract_cast<FootClass*>(pTarget))
						this->DisperseBurstSubstitution(pCreateBullet, pTrajType->AxisOfRotation.Get(), pTrajType->RotateCoord, curBurst, maxBurst, pTrajType->MirrorCoord);
				}
			}
			else if (flag == TrajectoryFlag::Engrave)
			{
				const auto pTrajectory = static_cast<EngraveTrajectory*>(pTraj);
				pTrajectory->NotMainWeapon = true;
			}
			else if (flag == TrajectoryFlag::Parabola)
			{
				const auto pTrajectory = static_cast<ParabolaTrajectory*>(pTraj);
				const auto pTrajType = pTrajectory->Type;

				//The parabola trajectory bullets has LeadTimeCalculate=true are not calculate its velocity yet.
				if (pTrajectory->UseDisperseBurst && std::abs(pTrajType->RotateCoord) > 1e-10 && curBurst >= 0 && maxBurst > 1)
				{
					if (pTrajType->LeadTimeCalculate && abstract_cast<FootClass*>(pTarget))
					{
						pTrajectory->CurrentBurst = curBurst;
						pTrajectory->CountOfBurst = maxBurst;
						pTrajectory->UseDisperseBurst = false;
					}
					else
					{
						this->DisperseBurstSubstitution(pCreateBullet, pTrajType->AxisOfRotation.Get(), pTrajType->RotateCoord, curBurst, maxBurst, pTrajType->MirrorCoord);
					}
				}
			}
			else if (flag == TrajectoryFlag::Tracing)
			{
				const auto pTrajectory = static_cast<TracingTrajectory*>(pTraj);
				pTrajectory->FirepowerMult = this->FirepowerMult;
				pTrajectory->NotMainWeapon = true;
			}
		}

		BulletExt::SimulatedFiringAnim(pCreateBullet, pWeapon, pOwner, pTraj, false);
		BulletExt::SimulatedFiringReport(pCreateBullet, pWeapon);
		BulletExt::SimulatedFiringLaser(pCreateBullet, pWeapon, pOwner);
		BulletExt::SimulatedFiringElectricBolt(pCreateBullet, pWeapon);
		BulletExt::SimulatedFiringRadBeam(pCreateBullet, pWeapon, pOwner);
		BulletExt::SimulatedFiringParticleSystem(pCreateBullet, pWeapon, pOwner);
	}
}

void DisperseTrajectory::DisperseBurstSubstitution(BulletClass* pBullet, CoordStruct axis, double rotateCoord, int curBurst, int maxBurst, bool mirror)
{
	const auto createBulletTargetToSource = pBullet->TargetCoords - pBullet->SourceCoords;
	const auto rotateAngle = Math::atan2(createBulletTargetToSource.Y , createBulletTargetToSource.X);

	BulletVelocity rotationAxis
	{
		axis.X * Math::cos(rotateAngle) + axis.Y * Math::sin(rotateAngle),
		axis.X * Math::sin(rotateAngle) - axis.Y * Math::cos(rotateAngle),
		static_cast<double>(axis.Z)
	};

	double extraRotate = 0.0;

	if (mirror)
	{
		if (curBurst % 2 == 1)
			rotationAxis *= -1;

		extraRotate = Math::Pi * (rotateCoord * ((curBurst / 2) / (maxBurst - 1.0) - 0.5)) / 180;
	}
	else
	{
		extraRotate = Math::Pi * (rotateCoord * (curBurst / (maxBurst - 1.0) - 0.5)) / 180;
	}

	pBullet->Velocity = this->RotateAboutTheAxis(pBullet->Velocity, rotationAxis, extraRotate);
}
