#include "TracingTrajectory.h"
#include <Ext/BulletType/Body.h>

bool TracingTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);

	Stm
		.Process(this->TheDuration, false)
		;

	return true;
}

bool TracingTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);

	Stm
		.Process(this->TheDuration)
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
}

bool TracingTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectory::Load(Stm, false);

	Stm
		.Process(this->TheDuration)
		.Process(this->ExistTimer)
		;

	return true;
}

bool TracingTrajectory::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectory::Save(Stm);

	Stm
		.Process(this->TheDuration)
		.Process(this->ExistTimer)
		;

	return true;
}

void TracingTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	if (this->TheDuration <= 0)
	{
		if (auto const pWeapon = pBullet->WeaponType)
		{
			const int weaponROF = pBullet->WeaponType->ROF;

			if (weaponROF > 10)
				this->TheDuration = weaponROF - 10;
			else
				this->TheDuration = 1;
		}
		else
		{
			this->TheDuration = 120;
		}
	}

	this->ExistTimer.Start(this->TheDuration);
}

bool TracingTrajectory::OnAI(BulletClass* pBullet)
{
	if (auto const pTechno = pBullet->Owner)
		pBullet->Target = pTechno->Target;

	if (auto const pTarget = pBullet->Target)
		pBullet->TargetCoords = pTarget->GetCoords();

	const double trajectorySpeed = this->GetTrajectorySpeed(pBullet);
	const CoordStruct distanceCoords = pBullet->TargetCoords - pBullet->Location; // TODO Need to calculate 1 frame ahead
	const double distance = distanceCoords.Magnitude();

	pBullet->Velocity.X = static_cast<double>(distanceCoords.X);
	pBullet->Velocity.Y = static_cast<double>(distanceCoords.Y);
	pBullet->Velocity.Z = static_cast<double>(distanceCoords.Z);

	if (distance > trajectorySpeed)
		pBullet->Velocity *= trajectorySpeed / distance;

	if (!this->ExistTimer.Completed())
		return false;

	return true;
}

void TracingTrajectory::OnAIPreDetonate(BulletClass* pBullet)
{
//	pBullet->UnInit(); //Prevent damage again.
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
