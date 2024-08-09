#include "TracingTrajectory.h"
#include <Ext/WarheadType/Body.h>
#include <Ext/BulletType/Body.h>
#include <Ext/Techno/Body.h>
#include <TacticalClass.h>
#include <LaserDrawClass.h>

bool TracingTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);

	Stm
		.Process(this->SourceCoord, false)
		.Process(this->TargetCoord, false)
		.Process(this->MirrorCoord, false)
		.Process(this->TheDuration, false)
		.Process(this->IsLaser, false)
		.Process(this->IsSupported, false)
		.Process(this->IsHouseColor, false)
		.Process(this->IsSingleColor, false)
		.Process(this->LaserInnerColor, false)
		.Process(this->LaserOuterColor, false)
		.Process(this->LaserOuterSpread, false)
		.Process(this->LaserThickness, false)
		.Process(this->LaserDuration, false)
		.Process(this->LaserDelay, false)
		.Process(this->DamageDelay, false)
		;

	return true;
}

bool TracingTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);

	Stm
		.Process(this->SourceCoord)
		.Process(this->TargetCoord)
		.Process(this->MirrorCoord)
		.Process(this->TheDuration)
		.Process(this->IsLaser)
		.Process(this->IsSupported)
		.Process(this->IsHouseColor)
		.Process(this->IsSingleColor)
		.Process(this->LaserInnerColor)
		.Process(this->LaserOuterColor)
		.Process(this->LaserOuterSpread)
		.Process(this->LaserThickness)
		.Process(this->LaserDuration)
		.Process(this->LaserDelay)
		.Process(this->DamageDelay)
		;

	return true;
}

PhobosTrajectory* TracingTrajectoryType::CreateInstance() const
{
	return new TracingTrajectory(this);
}

void TracingTrajectoryType::Read(CCINIClass* const pINI, const char* pSection)
{
	INI_EX exINI(pINI);
	this->SourceCoord.Read(exINI, pSection, "Trajectory.Tracing.SourceCoord");
	this->TargetCoord.Read(exINI, pSection, "Trajectory.Tracing.TargetCoord");
	this->MirrorCoord.Read(exINI, pSection, "Trajectory.Tracing.MirrorCoord");
	this->TheDuration.Read(exINI, pSection, "Trajectory.Tracing.TheDuration");
	this->IsLaser.Read(exINI, pSection, "Trajectory.Tracing.IsLaser");
	this->IsSupported.Read(exINI, pSection, "Trajectory.Tracing.IsSupported");
	this->IsHouseColor.Read(exINI, pSection, "Trajectory.Tracing.IsHouseColor");
	this->IsSingleColor.Read(exINI, pSection, "Trajectory.Tracing.IsSingleColor");
	this->LaserInnerColor.Read(exINI, pSection, "Trajectory.Tracing.LaserInnerColor");
	this->LaserOuterColor.Read(exINI, pSection, "Trajectory.Tracing.LaserOuterColor");
	this->LaserOuterSpread.Read(exINI, pSection, "Trajectory.Tracing.LaserOuterSpread");
	this->LaserThickness.Read(exINI, pSection, "Trajectory.Tracing.LaserThickness");
	this->LaserDuration.Read(exINI, pSection, "Trajectory.Tracing.LaserDuration");
	this->LaserDelay.Read(exINI, pSection, "Trajectory.Tracing.LaserDelay");
	this->DamageDelay.Read(exINI, pSection, "Trajectory.Tracing.DamageDelay");
}

bool TracingTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectory::Load(Stm, false);

	Stm
		.Process(this->SourceCoord)
		.Process(this->TargetCoord)
		.Process(this->MirrorCoord)
		.Process(this->TheDuration)
		.Process(this->IsLaser)
		.Process(this->IsSupported)
		.Process(this->IsHouseColor)
		.Process(this->IsSingleColor)
		.Process(this->LaserInnerColor)
		.Process(this->LaserOuterColor)
		.Process(this->LaserOuterSpread)
		.Process(this->LaserThickness)
		.Process(this->LaserDuration)
		.Process(this->LaserDelay)
		.Process(this->DamageDelay)
		.Process(this->LaserTimer)
		.Process(this->DamageTimer)
		.Process(this->TechnoInLimbo)
		.Process(this->NotMainWeapon)
		.Process(this->FLHCoord)
		.Process(this->BuildingCoord)
		.Process(this->TemporaryCoord)
		;

	return true;
}

bool TracingTrajectory::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectory::Save(Stm);

	Stm
		.Process(this->SourceCoord)
		.Process(this->TargetCoord)
		.Process(this->MirrorCoord)
		.Process(this->TheDuration)
		.Process(this->IsLaser)
		.Process(this->IsSupported)
		.Process(this->IsHouseColor)
		.Process(this->IsSingleColor)
		.Process(this->LaserInnerColor)
		.Process(this->LaserOuterColor)
		.Process(this->LaserOuterSpread)
		.Process(this->LaserThickness)
		.Process(this->LaserDuration)
		.Process(this->LaserDelay)
		.Process(this->DamageDelay)
		.Process(this->LaserTimer)
		.Process(this->DamageTimer)
		.Process(this->TechnoInLimbo)
		.Process(this->NotMainWeapon)
		.Process(this->FLHCoord)
		.Process(this->BuildingCoord)
		.Process(this->TemporaryCoord)
		;

	return true;
}

void TracingTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	auto const pType = this->GetTrajectoryType<TracingTrajectoryType>(pBullet);

	this->SourceCoord = pType->SourceCoord;
	this->TargetCoord = pType->TargetCoord;
	this->MirrorCoord = pType->MirrorCoord;
	this->TheDuration = pType->TheDuration;
	this->IsLaser = pType->IsLaser;
	this->IsSupported = pType->IsSupported;
	this->IsHouseColor = pType->IsHouseColor;
	this->IsSingleColor = pType->IsSingleColor;
	this->LaserInnerColor = pType->LaserInnerColor;
	this->LaserOuterColor = pType->LaserOuterColor;
	this->LaserOuterSpread = pType->LaserOuterSpread;
	this->LaserThickness = pType->LaserThickness > 0 ? pType->LaserThickness : 1;
	this->LaserDuration = pType->LaserDuration > 0 ? pType->LaserDuration : 1;
	this->LaserDelay = pType->LaserDelay > 0 ? pType->LaserDelay : 1;
	this->DamageDelay = pType->DamageDelay > 0 ? pType->DamageDelay : 1;
	this->LaserTimer.StartTime = 0;
	this->DamageTimer.StartTime = 0;
	this->FLHCoord = pBullet->SourceCoords;
	this->BuildingCoord = CoordStruct::Empty;
	this->TemporaryCoord = CoordStruct::Empty;

	if (pBullet->Owner)
	{
		this->TechnoInLimbo = static_cast<bool>(pBullet->Owner->Transporter);
		this->NotMainWeapon = false;

		this->GetTechnoFLHCoord(pBullet, pBullet->Owner);
		this->CheckMirrorCoord(pBullet->Owner);
		this->SetTracingDirection(pBullet, pBullet->Owner->GetCoords(), pBullet->TargetCoords);
	}
	else
	{
		this->TechnoInLimbo = false;
		this->NotMainWeapon = true;

		this->SetTracingDirection(pBullet, pBullet->SourceCoords, pBullet->TargetCoords);
	}

	double straightSpeed = this->GetTrajectorySpeed(pBullet);
	straightSpeed = straightSpeed > 128.0 ? 128.0 : straightSpeed;
	const double coordDistance = pBullet->Velocity.Magnitude();
	pBullet->Velocity *= (coordDistance > 0) ? (straightSpeed / coordDistance) : 0;

	if (this->TheDuration <= 0)
		this->TheDuration = static_cast<int>(coordDistance / straightSpeed) + 1;
}

bool TracingTrajectory::OnAI(BulletClass* pBullet)
{
	if ((!pBullet->Owner && !this->NotMainWeapon) || this->TechnoInLimbo != static_cast<bool>(pBullet->Owner->Transporter))
		return true;

	if (--this->TheDuration < 0)
		return true;
	else if (this->PlaceOnCorrectHeight(pBullet))
		return true;

	TechnoClass* const pTechno = pBullet->Owner;
	HouseClass* const pOwner = pBullet->Owner->Owner;

	if (this->IsLaser && this->LaserTimer.Completed())
		this->DrawTracingLaser(pBullet, pTechno, pOwner);

	if (this->DamageTimer.Completed())
		this->DetonateLaserWarhead(pBullet, pTechno, pOwner);

	return false;
}

void TracingTrajectory::OnAIPreDetonate(BulletClass* pBullet)
{
	pBullet->UnInit(); //Prevent damage again.
}

void TracingTrajectory::OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition)
{
	pSpeed->Z += BulletTypeExt::GetAdjustedGravity(pBullet->Type);
}

TrajectoryCheckReturnType TracingTrajectory::OnAITargetCoordCheck(BulletClass* pBullet)
{
	return TrajectoryCheckReturnType::SkipGameCheck;
}

TrajectoryCheckReturnType TracingTrajectory::OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno)
{
	return TrajectoryCheckReturnType::SkipGameCheck;
}

void TracingTrajectory::GetTechnoFLHCoord(BulletClass* pBullet, TechnoClass* pTechno)
{
	TechnoExt::ExtData* pExt = TechnoExt::ExtMap.Find(pTechno);

	if (!pExt || !pExt->LastWeaponType || pExt->LastWeaponType->Projectile != pBullet->Type)
	{
		this->NotMainWeapon = true;
		return;
	}
	else if (pTechno->WhatAmI() == AbstractType::Building)
	{
		const BuildingClass* const pBuilding = static_cast<BuildingClass*>(pTechno);
		Matrix3D mtx;
		mtx.MakeIdentity();

		if (pTechno->HasTurret())
		{
			TechnoTypeExt::ApplyTurretOffset(pBuilding->Type, &mtx);
			mtx.RotateZ(static_cast<float>(pTechno->TurretFacing().GetRadian<32>()));
		}

		mtx.Translate(static_cast<float>(pExt->LastWeaponFLH.X), static_cast<float>(pExt->LastWeaponFLH.Y), static_cast<float>(pExt->LastWeaponFLH.Z));
		auto const result = mtx.GetTranslation();
		this->BuildingCoord = pBullet->SourceCoords - pBuilding->GetCoords() - CoordStruct { static_cast<int>(result.X), -static_cast<int>(result.Y), static_cast<int>(result.Z) };
	}

	this->FLHCoord = pExt->LastWeaponFLH;
}

void TracingTrajectory::CheckMirrorCoord(TechnoClass* pTechno)
{
	if (pTechno->CurrentBurstIndex % 2 == 0)
		return;

	if (this->MirrorCoord)
	{
		this->SourceCoord.Y = -(this->SourceCoord.Y);
		this->TargetCoord.Y = -(this->TargetCoord.Y);
	}
}

void TracingTrajectory::SetTracingDirection(BulletClass* pBullet, CoordStruct theSource, CoordStruct theTarget)
{
	const double rotateAngle = Math::atan2(theTarget.Y - theSource.Y , theTarget.X - theSource.X);

	if (this->SourceCoord.X != 0 || this->SourceCoord.Y != 0)
	{
		theSource = theTarget;
		theSource.X += static_cast<int>(this->SourceCoord.X * Math::cos(rotateAngle) + this->SourceCoord.Y * Math::sin(rotateAngle));
		theSource.Y += static_cast<int>(this->SourceCoord.X * Math::sin(rotateAngle) - this->SourceCoord.Y * Math::cos(rotateAngle));
	}

	theSource.Z = this->GetFloorCoordHeight(pBullet, theSource);
	pBullet->SetLocation(theSource);

	theTarget.X += static_cast<int>(this->TargetCoord.X * Math::cos(rotateAngle) + this->TargetCoord.Y * Math::sin(rotateAngle));
	theTarget.Y += static_cast<int>(this->TargetCoord.X * Math::sin(rotateAngle) - this->TargetCoord.Y * Math::cos(rotateAngle));

	pBullet->Velocity.X = theTarget.X - theSource.X;
	pBullet->Velocity.Y = theTarget.Y - theSource.Y;
	pBullet->Velocity.Z = 0;
}

int TracingTrajectory::GetFloorCoordHeight(BulletClass* pBullet, CoordStruct coord)
{
	if (const CellClass* const pCell = MapClass::Instance->GetCellAt(coord))
	{
		const int onFloor = MapClass::Instance->GetCellFloorHeight(coord);
		const int onBridge = pCell->GetCoordsWithBridge().Z;

		if (pBullet->SourceCoords.Z >= onBridge || pBullet->TargetCoords.Z >= onBridge)
			return onBridge;

		return onFloor;
	}

	return coord.Z;
}

bool TracingTrajectory::PlaceOnCorrectHeight(BulletClass* pBullet)
{
	CoordStruct bulletCoords = pBullet->Location;

	if (this->TemporaryCoord != CoordStruct::Empty)
	{
		pBullet->SetLocation(this->TemporaryCoord);
		this->TemporaryCoord = CoordStruct::Empty;
	}

	CoordStruct futureCoords
	{
		bulletCoords.X + static_cast<int>(pBullet->Velocity.X),
		bulletCoords.Y + static_cast<int>(pBullet->Velocity.Y),
		bulletCoords.Z + static_cast<int>(pBullet->Velocity.Z)
	};

	const int checkDifference = this->GetFloorCoordHeight(pBullet, futureCoords) - futureCoords.Z;

	if (abs(checkDifference) >= 384)
	{
		if (pBullet->Type->SubjectToCliffs)
		{
			return true;
		}
		else if (checkDifference > 0)
		{
			bulletCoords.Z += checkDifference;
			pBullet->SetLocation(bulletCoords);
		}
		else
		{
			futureCoords.Z += checkDifference;
			this->TemporaryCoord = futureCoords;
		}
	}
	else
	{
		pBullet->Velocity.Z += checkDifference;
	}

	return false;
}

void TracingTrajectory::DrawTracingLaser(BulletClass* pBullet, TechnoClass* pTechno, HouseClass* pOwner)
{
	this->LaserTimer.Start(this->LaserDelay);
	LaserDrawClass* pLaser;
	CoordStruct fireCoord = pTechno->GetCoords();

	if (this->NotMainWeapon)
	{
		fireCoord = this->FLHCoord;
	}
	else if (pTechno->WhatAmI() != AbstractType::Building)
	{
		if (this->TechnoInLimbo)
			fireCoord = TechnoExt::GetFLHAbsoluteCoords(pTechno->Transporter, this->FLHCoord, pTechno->Transporter->HasTurret());
		else
			fireCoord = TechnoExt::GetFLHAbsoluteCoords(pTechno, this->FLHCoord, pTechno->HasTurret());
	}
	else
	{
		const BuildingClass* const pBuilding = static_cast<BuildingClass*>(pTechno);
		Matrix3D mtx;
		mtx.MakeIdentity();

		if (pTechno->HasTurret())
		{
			TechnoTypeExt::ApplyTurretOffset(pBuilding->Type, &mtx);
			mtx.RotateZ(static_cast<float>(pTechno->TurretFacing().GetRadian<32>()));
		}

		mtx.Translate(static_cast<float>(this->FLHCoord.X), static_cast<float>(this->FLHCoord.Y), static_cast<float>(this->FLHCoord.Z));
		auto const result = mtx.GetTranslation();
		fireCoord = pBuilding->GetCoords() + this->BuildingCoord + CoordStruct { static_cast<int>(result.X), -static_cast<int>(result.Y), static_cast<int>(result.Z) };
	}

	if (this->IsHouseColor)
	{
		pLaser = GameCreate<LaserDrawClass>(fireCoord, pBullet->Location, pOwner->LaserColor, ColorStruct { 0, 0, 0 }, ColorStruct { 0, 0, 0 }, this->LaserDuration);
		pLaser->IsHouseColor = true;
	}
	else if (this->IsSingleColor)
	{
		pLaser = GameCreate<LaserDrawClass>(fireCoord, pBullet->Location, this->LaserInnerColor, ColorStruct { 0, 0, 0 }, ColorStruct { 0, 0, 0 }, this->LaserDuration);
		pLaser->IsHouseColor = true;
	}
	else
	{
		pLaser = GameCreate<LaserDrawClass>(fireCoord, pBullet->Location, this->LaserInnerColor, this->LaserOuterColor, this->LaserOuterSpread, this->LaserDuration);
		pLaser->IsHouseColor = false;
	}

	pLaser->Thickness = this->LaserThickness;
	pLaser->IsSupported = this->IsSupported;
}

void TracingTrajectory::DetonateLaserWarhead(BulletClass* pBullet, TechnoClass* pTechno, HouseClass* pOwner)
{
	this->DamageTimer.Start(this->DamageDelay);
	WarheadTypeExt::DetonateAt(pBullet->WH, pBullet->Location, pTechno, pBullet->Health, pOwner);
}
