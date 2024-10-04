#include "TracingTrajectory.h"
#include <Ext/BulletType/Body.h>

std::unique_ptr<PhobosTrajectory> TracingTrajectoryType::CreateInstance() const
{
	return std::make_unique<TracingTrajectory>(this);
}

template<typename T>
void TracingTrajectoryType::Serialize(T& Stm)
{
	Stm
		.Process(this->TheDuration, false)
		;
}

bool TracingTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);
	this->Serialize(Stm);
	return true;
}

bool TracingTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);
	const_cast<TracingTrajectoryType*>(this)->Serialize(Stm);
	return true;
}

void TracingTrajectoryType::Read(CCINIClass* const pINI, const char* pSection)
{
	INI_EX exINI(pINI);
	this->TheDuration.Read(exINI, pSection, "Trajectory.Tracing.TheDuration");
}

template<typename T>
void TracingTrajectory::Serialize(T& Stm)
{
	Stm
		.Process(this->Type)
		.Process(this->ExistTimer)
		;
}

bool TracingTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectory::Load(Stm, false);
	this->Serialize(Stm);
	return true;
}

bool TracingTrajectory::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectory::Save(Stm);
	const_cast<TracingTrajectory*>(this)->Serialize(Stm);
	return true;
}

void TracingTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	int duration = this->Type->TheDuration;

	if (duration <= 0)
	{
		if (auto const pWeapon = pBullet->WeaponType)
		{
			const int weaponROF = pBullet->WeaponType->ROF;

			if (weaponROF > 10)
				duration = weaponROF - 10;
			else
				duration = 1;
		}
		else
		{
			duration = 120;
		}
	}

	this->ExistTimer.Start(duration);
}

bool TracingTrajectory::OnAI(BulletClass* pBullet)
{
	if (auto const pTechno = pBullet->Owner)
		pBullet->Target = pTechno->Target;

	if (auto const pTarget = pBullet->Target)
		pBullet->TargetCoords = pTarget->GetCoords();

	const CoordStruct distanceCoords = pBullet->TargetCoords - pBullet->Location; // TODO Need to calculate 1 frame ahead
	const double distance = distanceCoords.Magnitude();

	pBullet->Velocity.X = static_cast<double>(distanceCoords.X);
	pBullet->Velocity.Y = static_cast<double>(distanceCoords.Y);
	pBullet->Velocity.Z = static_cast<double>(distanceCoords.Z);

	if (distance > this->Speed)
		pBullet->Velocity *= this->Speed / distance;

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
