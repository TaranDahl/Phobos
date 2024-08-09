#include "TracingTrajectory.h"
#include <Ext/BulletType/Body.h>

bool TracingTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);

	Stm
		.Process(this->TheDuration, false)
		.Process(this->RelockDelay, false)
		.Process(this->RelockRange, false)
		.Process(this->NoRelockROF, false)
		.Process(this->AddedRange, false)
		.Process(this->IsLaser, false)
		.Process(this->TargetLaser, false)
		.Process(this->FinishLaser, false)
		.Process(this->IsHouseColor, false)
		.Process(this->LaserColor, false)
		.Process(this->BlazeDelay, false)
		.Process(this->DamageDelay, false)
		;

	return true;
}

bool TracingTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);

	Stm
		.Process(this->TheDuration)
		.Process(this->RelockDelay)
		.Process(this->RelockRange)
		.Process(this->NoRelockROF)
		.Process(this->AddedRange)
		.Process(this->IsLaser)
		.Process(this->TargetLaser)
		.Process(this->FinishLaser)
		.Process(this->IsHouseColor)
		.Process(this->LaserColor)
		.Process(this->BlazeDelay)
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
	this->TheDuration.Read(exINI, pSection, "Trajectory.Tracing.TheDuration");
	this->RelockDelay.Read(exINI, pSection, "Trajectory.Tracing.RelockDelay");
	this->RelockRange.Read(exINI, pSection, "Trajectory.Tracing.RelockRange");
	this->NoRelockROF.Read(exINI, pSection, "Trajectory.Tracing.NoRelockROF");
	this->AddedRange.Read(exINI, pSection, "Trajectory.Tracing.AddedRange");
	this->IsLaser.Read(exINI, pSection, "Trajectory.Tracing.IsLaser");
	this->TargetLaser.Read(exINI, pSection, "Trajectory.Tracing.TargetLaser");
	this->FinishLaser.Read(exINI, pSection, "Trajectory.Tracing.FinishLaser");
	this->IsHouseColor.Read(exINI, pSection, "Trajectory.Tracing.IsHouseColor");
	this->LaserColor.Read(exINI, pSection, "Trajectory.Tracing.LaserColor");
	this->BlazeDelay.Read(exINI, pSection, "Trajectory.Tracing.BlazeDelay");
	this->DamageDelay.Read(exINI, pSection, "Trajectory.Tracing.DamageDelay");
}

bool TracingTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectory::Load(Stm, false);

	Stm
		.Process(this->TheDuration)
		.Process(this->RelockDelay)
		.Process(this->RelockRange)
		.Process(this->NoRelockROF)
		.Process(this->AddedRange)
		.Process(this->IsLaser)
		.Process(this->TargetLaser)
		.Process(this->FinishLaser)
		.Process(this->IsHouseColor)
		.Process(this->LaserColor)
		.Process(this->BlazeDelay)
		.Process(this->DamageDelay)
		.Process(this->RelockTimer)
		.Process(this->BlazeTimer)
		.Process(this->DamageTimer)
		;

	return true;
}

bool TracingTrajectory::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectory::Save(Stm);

	Stm
		.Process(this->TheDuration)
		.Process(this->RelockDelay)
		.Process(this->RelockRange)
		.Process(this->NoRelockROF)
		.Process(this->AddedRange)
		.Process(this->IsLaser)
		.Process(this->TargetLaser)
		.Process(this->FinishLaser)
		.Process(this->IsHouseColor)
		.Process(this->LaserColor)
		.Process(this->BlazeDelay)
		.Process(this->DamageDelay)
		.Process(this->RelockTimer)
		.Process(this->BlazeTimer)
		.Process(this->DamageTimer)
		;

	return true;
}

void TracingTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	auto const pType = this->GetTrajectoryType<TracingTrajectoryType>(pBullet);

	this->TheDuration = pType->TheDuration;
	this->RelockDelay = pType->RelockDelay;
	this->RelockRange = pType->RelockRange;
	this->NoRelockROF = pType->NoRelockROF;
	this->AddedRange = pType->AddedRange;
	this->IsLaser = pType->IsLaser;
	this->TargetLaser = pType->TargetLaser;
	this->FinishLaser = pType->FinishLaser;
	this->IsHouseColor = pType->IsHouseColor;
	this->LaserColor = pType->LaserColor;
	this->BlazeDelay = pType->BlazeDelay;
	this->DamageDelay = pType->DamageDelay;
}

bool TracingTrajectory::OnAI(BulletClass* pBullet)
{
	return true;
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
