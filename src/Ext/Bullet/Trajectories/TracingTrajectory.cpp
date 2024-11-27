#include "TracingTrajectory.h"

#include <AnimClass.h>
#include <LaserDrawClass.h>
#include <EBolt.h>
#include <RadBeam.h>
#include <ParticleSystemClass.h>

#include <Ext/Bullet/Body.h>
#include <Ext/BulletType/Body.h>
#include <Ext/Techno/Body.h>
#include <Ext/WeaponType/Body.h>
#include <Ext/Anim/Body.h>

std::unique_ptr<PhobosTrajectory> TracingTrajectoryType::CreateInstance() const
{
	return std::make_unique<TracingTrajectory>(this);
}

template<typename T>
void TracingTrajectoryType::Serialize(T& Stm)
{
	Stm
		.Process(this->TheDuration)
		.Process(this->Synchronize)
		.Process(this->NoDetonation)
		.Process(this->CreateAtTarget)
		.Process(this->TolerantTime)
		.Process(this->WeaponType)
		.Process(this->WeaponCount)
		.Process(this->WeaponDelay)
		.Process(this->WeaponTimer)
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
	this->Synchronize.Read(exINI, pSection, "Trajectory.Tracing.Synchronize");
	this->NoDetonation.Read(exINI, pSection, "Trajectory.Tracing.NoDetonation");
	this->CreateAtTarget.Read(exINI, pSection, "Trajectory.Tracing.CreateAtTarget");
	this->TolerantTime.Read(exINI, pSection, "Trajectory.Tracing.TolerantTime");
	this->WeaponType.Read<true>(exINI, pSection, "Trajectory.Tracing.WeaponType");
	this->WeaponCount.Read(exINI, pSection, "Trajectory.Tracing.WeaponCount");
	this->WeaponDelay.Read(exINI, pSection, "Trajectory.Tracing.WeaponDelay");

	if (this->WeaponDelay <= 0)
		this->WeaponDelay = 1;

	this->WeaponTimer.Read(exINI, pSection, "Trajectory.Tracing.WeaponTimer");

	if (this->WeaponTimer < 0)
		this->WeaponTimer = 0;
}

template<typename T>
void TracingTrajectory::Serialize(T& Stm)
{
	Stm
		.Process(this->Type)
		.Process(this->WeaponCount)
		.Process(this->ExistTimer)
		.Process(this->WeaponTimer)
		.Process(this->TolerantTimer)
		.Process(this->TechnoInTransport)
		.Process(this->NotMainWeapon)
		.Process(this->FLHCoord)
		.Process(this->BuildingCoord)
		.Process(this->FirepowerMult)
		;
}

bool TracingTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->Serialize(Stm);
	return true;
}

bool TracingTrajectory::Save(PhobosStreamWriter& Stm) const
{
	const_cast<TracingTrajectory*>(this)->Serialize(Stm);
	return true;
}

void TracingTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	const auto pType = this->Type;

	if (this->WeaponCount)
		this->WeaponTimer.Start(pType->WeaponTimer);

	this->FLHCoord = pBullet->SourceCoords;

	if (const auto pFirer = pBullet->Owner)
	{
		this->TechnoInTransport = static_cast<bool>(pFirer->Transporter);
		this->FirepowerMult = pFirer->FirepowerMultiplier;

		if (const auto pExt = TechnoExt::ExtMap.Find(pFirer))
			this->FirepowerMult *= pExt->AE.FirepowerMultiplier;

		this->GetTechnoFLHCoord(pBullet, pFirer);
	}
	else
	{
		this->TechnoInTransport = false;
		this->NotMainWeapon = true;
	}

	if (pType->CreateAtTarget)
	{
		if (const auto pTarget = pBullet->Target)
			pBullet->SetLocation(pTarget->GetCoords());
		else
			pBullet->SetLocation(pBullet->TargetCoords);
	}

	this->InitializeDuration(pBullet, pType->TheDuration);
}

bool TracingTrajectory::OnAI(BulletClass* pBullet)
{
	const auto pTechno = pBullet->Owner;

	if (!this->NotMainWeapon && this->InvalidFireCondition(pTechno))
		return true;

	if (this->ChangeBulletTarget(pBullet))
		return true;

	this->ChangeVelocity(pBullet);

	if (this->WeaponTimer.Completed())
		this->FireTracingWeapon(pBullet);

	if (this->ExistTimer.Completed())
		return true;

	return false;
}

void TracingTrajectory::OnAIPreDetonate(BulletClass* pBullet)
{
	if (this->Type->NoDetonation)
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
	const auto pExt = TechnoExt::ExtMap.Find(pTechno);

	if (!pExt || !pExt->LastWeaponType || pExt->LastWeaponType->Projectile != pBullet->Type)
	{
		this->NotMainWeapon = true;
		return;
	}
	else if (pTechno->WhatAmI() == AbstractType::Building)
	{
		const auto pBuilding = static_cast<BuildingClass*>(pTechno);
		Matrix3D mtx;
		mtx.MakeIdentity();

		if (pTechno->HasTurret())
		{
			TechnoTypeExt::ApplyTurretOffset(pBuilding->Type, &mtx);
			mtx.RotateZ(static_cast<float>(pTechno->TurretFacing().GetRadian<32>()));
		}

		mtx.Translate(static_cast<float>(pExt->LastWeaponFLH.X), static_cast<float>(pExt->LastWeaponFLH.Y), static_cast<float>(pExt->LastWeaponFLH.Z));
		const auto result = mtx.GetTranslation();
		this->BuildingCoord = pBullet->SourceCoords - pBuilding->GetCoords() - CoordStruct { static_cast<int>(result.X), -static_cast<int>(result.Y), static_cast<int>(result.Z) };
	}

	this->FLHCoord = pExt->LastWeaponFLH;
}

void TracingTrajectory::InitializeDuration(BulletClass* pBullet, int duration)
{
	if (duration <= 0)
	{
		if (const auto pWeapon = pBullet->WeaponType)
			duration = (pWeapon->ROF > 10) ? pWeapon->ROF - 10 : 1;
		else
			duration = 120;
	}

	this->ExistTimer.Start(duration);
}

bool TracingTrajectory::InvalidFireCondition(TechnoClass* pTechno)
{
	return (!pTechno
		|| !pTechno->IsAlive
		|| !pTechno->IsOnMap
		|| pTechno->InLimbo
		|| pTechno->IsSinking
		|| pTechno->Health <= 0
		|| this->TechnoInTransport != static_cast<bool>(pTechno->Transporter)
		|| pTechno->Deactivated
		|| pTechno->TemporalTargetingMe
		|| pTechno->BeingWarpedOut
		|| pTechno->IsUnderEMP());
}

bool TracingTrajectory::ChangeBulletTarget(BulletClass* pBullet)
{
	const auto pType = this->Type;

	if (!pBullet->Target && !pType->TolerantTime)
		return true;

	if (pType->Synchronize)
	{
		if (const auto pTechno = pBullet->Owner)
		{
			if (pBullet->Target != pTechno->Target && !pType->TolerantTime)
				return true;

			pBullet->Target = pTechno->Target;
		}
	}

	if (const auto pTarget = pBullet->Target)
	{
		pBullet->TargetCoords = pTarget->GetCoords();
		this->TolerantTimer.Stop();
	}
	else if (pType->TolerantTime > 0)
	{
		if (this->TolerantTimer.Completed())
			return true;
		else if (!this->TolerantTimer.IsTicking())
			this->TolerantTimer.Start(pType->TolerantTime);
	}

	return false;
}

void TracingTrajectory::ChangeVelocity(BulletClass* pBullet)
{
	const auto pType = this->Type;
	const auto distanceCoords = pBullet->TargetCoords - pBullet->Location;
	const auto distance = distanceCoords.Magnitude();

	pBullet->Velocity.X = static_cast<double>(distanceCoords.X);
	pBullet->Velocity.Y = static_cast<double>(distanceCoords.Y);
	pBullet->Velocity.Z = static_cast<double>(distanceCoords.Z);

	if (pType->Trajectory_Speed > 0 && distance > pType->Trajectory_Speed)
		pBullet->Velocity *= pType->Trajectory_Speed / distance;
}

void TracingTrajectory::FireTracingWeapon(BulletClass* pBullet)
{
	const auto pType = this->Type;

	if (this->WeaponCount < 0 || --this->WeaponCount > 0)
		this->WeaponTimer.Start(pType->WeaponDelay);
	else
		this->WeaponTimer.Stop();

	const auto pTechno = pBullet->Owner;
	const auto pOwner = pTechno ? pTechno->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse;
	const auto pTarget = pBullet;

	const auto pTransporter = pTechno->Transporter;
	auto fireCoord = pBullet->SourceCoords;

	if (!this->NotMainWeapon && pTechno && (pTransporter || !pTechno->InLimbo))
	{
		if (pTechno->WhatAmI() != AbstractType::Building)
		{
			if (pTransporter)
				fireCoord = TechnoExt::GetFLHAbsoluteCoords(pTransporter, this->FLHCoord, pTransporter->HasTurret());
			else
				fireCoord = TechnoExt::GetFLHAbsoluteCoords(pTechno, this->FLHCoord, pTechno->HasTurret());
		}
		else
		{
			const auto pBuilding = static_cast<BuildingClass*>(pTechno);
			Matrix3D mtx;
			mtx.MakeIdentity();

			if (pTechno->HasTurret())
			{
				TechnoTypeExt::ApplyTurretOffset(pBuilding->Type, &mtx);
				mtx.RotateZ(static_cast<float>(pTechno->TurretFacing().GetRadian<32>()));
			}

			mtx.Translate(static_cast<float>(this->FLHCoord.X), static_cast<float>(this->FLHCoord.Y), static_cast<float>(this->FLHCoord.Z));
			const auto result = mtx.GetTranslation();
			fireCoord = pBuilding->GetCoords() + this->BuildingCoord + CoordStruct { static_cast<int>(result.X), -static_cast<int>(result.Y), static_cast<int>(result.Z) };
		}
	}

	const auto pWeapon = pType->WeaponType.Get();

	if (!pWeapon)
		return;

	const auto finalDamage = static_cast<int>(pWeapon->Damage * this->FirepowerMult);

	if (const auto pCreateBullet = pWeapon->Projectile->CreateBullet(pTarget, pTechno, finalDamage, pWeapon->Warhead, pWeapon->Speed, pWeapon->Bright))
	{
		pCreateBullet->WeaponType = pWeapon;
		const auto pBulletExt = BulletExt::ExtMap.Find(pCreateBullet);
		pBulletExt->FirerHouse = pOwner;
		pCreateBullet->MoveTo(fireCoord, BulletVelocity::Empty);

		const auto animCounts = pWeapon->Anim.Count;

		if (animCounts > 0)
		{
			int animIndex = 0;

			if (animCounts % 8 == 0)
			{
				if (pBulletExt->Trajectory)
				{
					animIndex = static_cast<int>((Math::atan2(pCreateBullet->Velocity.Y , pCreateBullet->Velocity.X) + Math::TwoPi + Math::Pi) * animCounts / Math::TwoPi - (animCounts / 8) + 0.5) % animCounts;
				}
				else
				{
					const auto theSourceCoord = fireCoord;
					const auto theTargetCoord = pTarget->GetCoords();
					animIndex = static_cast<int>((Math::atan2(theTargetCoord.Y - theSourceCoord.Y , theTargetCoord.X - theSourceCoord.X) + Math::TwoPi + Math::Pi) * animCounts / Math::TwoPi - (animCounts / 8) + 0.5) % animCounts;
				}
			}
			else
			{
				animIndex = ScenarioClass::Instance->Random.RandomRanged(0 , animCounts - 1);
			}

			if (const auto pAnimType = pWeapon->Anim[animIndex])
			{
				const auto pAnim = GameCreate<AnimClass>(pAnimType, fireCoord);
				pAnim->SetOwnerObject(pBullet->Owner);
				pAnim->Owner = pOwner;

				if (const auto pAnimExt = AnimExt::ExtMap.Find(pAnim))
					pAnimExt->SetInvoker(pBullet->Owner, pOwner);
			}
		}
	}
	else
	{
		return;
	}

	if (pWeapon->Report.Count > 0)
	{
		const auto reportIndex = pWeapon->Report.GetItem((pBullet->Owner ? pBullet->Owner->unknown_short_3C8 : ScenarioClass::Instance->Random.Random()) % pWeapon->Report.Count);

		if (reportIndex != -1)
			VocClass::PlayAt(reportIndex, fireCoord, nullptr);
	}

	const auto pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon);

	if (pWeapon->IsLaser)
	{
		if (pWeapon->IsHouseColor || pWeaponExt->Laser_IsSingleColor)
		{
			auto const pLaser = GameCreate<LaserDrawClass>(fireCoord, pTarget->GetCoords(), ((pWeapon->IsHouseColor && pOwner) ? pOwner->LaserColor : pWeapon->LaserInnerColor), ColorStruct { 0, 0, 0 }, ColorStruct { 0, 0, 0 }, pWeapon->LaserDuration);
			pLaser->IsHouseColor = true;
			pLaser->Thickness = pWeaponExt->LaserThickness;
			pLaser->IsSupported = (pLaser->Thickness > 3);
		}
		else
		{
			auto const pLaser = GameCreate<LaserDrawClass>(fireCoord, pTarget->GetCoords(), pWeapon->LaserInnerColor, pWeapon->LaserOuterColor, pWeapon->LaserOuterSpread, pWeapon->LaserDuration);
			pLaser->IsHouseColor = false;
			pLaser->Thickness = 3;
			pLaser->IsSupported = false;
		}
	}

	if (pWeapon->IsElectricBolt)
	{
		if (const auto pEBolt = GameCreate<EBolt>())
		{
			pEBolt->AlternateColor = pWeapon->IsAlternateColor;
			//TODO Weapon's Bolt.Color1, Bolt.Color2, Bolt.Color3(Ares)
			WeaponTypeExt::BoltWeaponMap[pEBolt] = pWeaponExt;
			pEBolt->Fire(fireCoord, pTarget->GetCoords(), 0);
		}
	}

	if (pWeapon->IsRadBeam)
	{
		const bool isTemporal = pWeapon->Warhead && pWeapon->Warhead->Temporal;

		if (const auto pRadBeam = RadBeam::Allocate(isTemporal ? RadBeamType::Temporal : RadBeamType::RadBeam))
		{
			pRadBeam->SetCoordsSource(fireCoord);
			pRadBeam->SetCoordsTarget(pTarget->GetCoords());
			pRadBeam->Color = (pWeaponExt->Beam_IsHouseColor && pOwner) ? pOwner->LaserColor : pWeaponExt->Beam_Color.Get(isTemporal ? RulesClass::Instance->ChronoBeamColor : RulesClass::Instance->RadColor);
			pRadBeam->Period = pWeaponExt->Beam_Duration;
			pRadBeam->Amplitude = pWeaponExt->Beam_Amplitude;
		}
	}

	if (const auto pPSType = pWeapon->AttachedParticleSystem)
		GameCreate<ParticleSystemClass>(pPSType, fireCoord, pTarget, pBullet->Owner, pTarget->GetCoords(), pOwner);
}
