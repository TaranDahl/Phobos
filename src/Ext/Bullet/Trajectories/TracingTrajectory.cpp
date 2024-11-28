#include "TracingTrajectory.h"
#include "DisperseTrajectory.h"
#include "StraightTrajectory.h"
#include "EngraveTrajectory.h"

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
#include <Utilities/EnumFunctions.h>
#include <Utilities/Helpers.Alex.h>

std::unique_ptr<PhobosTrajectory> TracingTrajectoryType::CreateInstance() const
{
	return std::make_unique<TracingTrajectory>(this);
}

template<typename T>
void TracingTrajectoryType::Serialize(T& Stm)
{
	Stm
		.Process(this->TraceMode)
		.Process(this->TheDuration)
		.Process(this->TolerantTime)
		.Process(this->AboveWeaponRange)
		.Process(this->PeacefullyVanish)
		.Process(this->TraceTheTarget)
		.Process(this->CreateAtTarget)
		.Process(this->CreateCoord)
		.Process(this->OffsetCoord)
		.Process(this->Weapons)
		.Process(this->WeaponCount)
		.Process(this->WeaponDelay)
		.Process(this->WeaponTimer)
		.Process(this->WeaponCycle)
		.Process(this->Synchronize)
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
	pINI->ReadString(pSection, "Trajectory.Tracing.TraceMode", "", Phobos::readBuffer);
	if (INIClass::IsBlank(Phobos::readBuffer))
		this->TraceMode = TraceTargetMode::Connection;
	else if (_stricmp(Phobos::readBuffer, "map") == 0)
		this->TraceMode = TraceTargetMode::Map;
	else if (_stricmp(Phobos::readBuffer, "body") == 0)
		this->TraceMode = TraceTargetMode::Body;
	else if (_stricmp(Phobos::readBuffer, "turret") == 0)
		this->TraceMode = TraceTargetMode::Turret;
	else if (_stricmp(Phobos::readBuffer, "rotate") == 0)
		this->TraceMode = TraceTargetMode::Rotate;
	else
		this->TraceMode = TraceTargetMode::Connection;

	this->TheDuration.Read(exINI, pSection, "Trajectory.Tracing.TheDuration");
	this->TolerantTime.Read(exINI, pSection, "Trajectory.Tracing.TolerantTime");
	this->AboveWeaponRange.Read(exINI, pSection, "Trajectory.Tracing.AboveWeaponRange");
	this->PeacefullyVanish.Read(exINI, pSection, "Trajectory.Tracing.PeacefullyVanish");
	this->TraceTheTarget.Read(exINI, pSection, "Trajectory.Tracing.TraceTheTarget");
	this->CreateAtTarget.Read(exINI, pSection, "Trajectory.Tracing.CreateAtTarget");
	this->CreateCoord.Read(exINI, pSection, "Trajectory.Tracing.CreateCoord");
	this->OffsetCoord.Read(exINI, pSection, "Trajectory.Tracing.OffsetCoord");
	this->Weapons.Read(exINI, pSection, "Trajectory.Tracing.Weapons");
	this->WeaponCount.Read(exINI, pSection, "Trajectory.Tracing.WeaponCount");
	this->WeaponDelay.Read(exINI, pSection, "Trajectory.Tracing.WeaponDelay");
	this->WeaponTimer.Read(exINI, pSection, "Trajectory.Tracing.WeaponTimer");

	if (this->WeaponTimer < 0)
		this->WeaponTimer = 0;

	this->WeaponCycle.Read(exINI, pSection, "Trajectory.Tracing.WeaponCycle");
	this->Synchronize.Read(exINI, pSection, "Trajectory.Tracing.Synchronize");
}

template<typename T>
void TracingTrajectory::Serialize(T& Stm)
{
	Stm
		.Process(this->Type)
		.Process(this->WeaponIndex)
		.Process(this->WeaponCount)
		.Process(this->ExistTimer)
		.Process(this->WeaponTimer)
		.Process(this->TolerantTimer)
		.Process(this->TechnoInTransport)
		.Process(this->NotMainWeapon)
		.Process(this->FLHCoord)
		.Process(this->BuildingCoord)
		.Process(this->FirepowerMult)
		.Process(this->RotateRadian)
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

	if (!pType->Weapons.empty() && !pType->WeaponCount.empty())
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

	if (pType->TraceMode == TraceTargetMode::Rotate)
	{
		const auto theOffset = pType->OffsetCoord.Get();
		const auto radius = Point2D{theOffset.X,theOffset.Y}.Magnitude();

		if (abs(radius) > 1e-10)
			this->RotateRadian = pType->Trajectory_Speed / radius;
	}

	const auto theCoords = pType->CreateCoord.Get();
	auto theOffset = theCoords;

	if (theCoords.X != 0 || theCoords.Y != 0)
	{
		const auto& theSource = pBullet->SourceCoords;
		const auto& theTarget = pBullet->TargetCoords;
		const auto rotateAngle = Math::atan2(theTarget.Y - theSource.Y , theTarget.X - theSource.X);

		theOffset.X = static_cast<int>(theCoords.X * Math::cos(rotateAngle) + theCoords.Y * Math::sin(rotateAngle));
		theOffset.Y = static_cast<int>(theCoords.X * Math::sin(rotateAngle) - theCoords.Y * Math::cos(rotateAngle));
	}

	if (pType->CreateAtTarget)
	{
		if (const auto pTarget = pBullet->Target)
			pBullet->SetLocation(pTarget->GetCoords() + theOffset);
		else
			pBullet->SetLocation(pBullet->TargetCoords + theOffset);
	}
	else
	{
		pBullet->SetLocation(pBullet->SourceCoords + theOffset);
	}

	this->InitializeDuration(pBullet, pType->TheDuration);
}

bool TracingTrajectory::OnAI(BulletClass* pBullet)
{
	const auto pTechno = pBullet->Owner;

	if (!this->NotMainWeapon && this->InvalidFireCondition(pTechno))
		return true;

	if (this->BulletDetonatePreCheck(pBullet))
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
	if (this->Type->PeacefullyVanish)
	{
		pBullet->Health = 0;
		pBullet->Limbo();
		pBullet->UnInit();
	}
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
		|| (pTechno->InLimbo && !pTechno->Transporter)
		|| pTechno->IsSinking
		|| pTechno->Health <= 0
		|| this->TechnoInTransport != static_cast<bool>(pTechno->Transporter)
		|| pTechno->Deactivated
		|| pTechno->TemporalTargetingMe
		|| pTechno->BeingWarpedOut
		|| pTechno->IsUnderEMP());
}

bool TracingTrajectory::BulletDetonatePreCheck(BulletClass* pBullet)
{
	const auto pType = this->Type;

	if (!pBullet->Target && !pType->TolerantTime)
		return true;

	const auto pTechno = pBullet->Owner;

	if (!pType->TraceTheTarget && !pTechno)
		return true;

	if (pType->Synchronize)
	{
		if (pTechno)
		{
			if (pBullet->Target != pTechno->Target && !pType->TolerantTime)
				return true;

			pBullet->Target = pTechno->Target;
		}
	}

	const auto pTarget = pBullet->Target;

	if (pTarget)
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

	if (!pType->AboveWeaponRange && pTechno && !this->NotMainWeapon)
	{
		if (const auto pWeapon = pBullet->WeaponType)
		{
			auto source = pTechno->GetCoords();
			auto target = pBullet->TargetCoords;

			if (pTechno->IsInAir())
			{
				source.Z = 0;
				target.Z = 0;
			}

			if (static_cast<int>(source.DistanceFrom(target)) >= (pWeapon->Range + 256))
				return true;
		}
	}

	return false;
}

void TracingTrajectory::ChangeVelocity(BulletClass* pBullet)
{
	const auto pType = this->Type;

	const auto destination = pType->TraceTheTarget ? pBullet->TargetCoords : pBullet->Owner->GetCoords();
	const auto theCoords = pType->OffsetCoord.Get();
	auto theOffset = theCoords;

	if (theCoords.X != 0 || theCoords.Y != 0)
	{
		switch (pType->TraceMode)
		{
		case TraceTargetMode::Map:
		{
			break;
		}
		case TraceTargetMode::Body:
		{
			if (const auto pTechno = abstract_cast<TechnoClass*>(pType->TraceTheTarget ? pBullet->Target : pBullet->Owner))
			{
				const auto rotateAngle = -(pTechno->PrimaryFacing.Current().GetRadian<32>());

				theOffset.X = static_cast<int>(theCoords.X * Math::cos(rotateAngle) + theCoords.Y * Math::sin(rotateAngle));
				theOffset.Y = static_cast<int>(theCoords.X * Math::sin(rotateAngle) - theCoords.Y * Math::cos(rotateAngle));
			}
			else
			{
				theOffset.X = 0;
				theOffset.Y = 0;
			}

			break;
		}
		case TraceTargetMode::Turret:
		{
			if (const auto pTechno = abstract_cast<TechnoClass*>(pType->TraceTheTarget ? pBullet->Target : pBullet->Owner))
			{
				const auto rotateAngle = (pTechno->HasTurret() ? -(pTechno->TurretFacing().GetRadian<32>()) : -(pTechno->PrimaryFacing.Current().GetRadian<32>()));

				theOffset.X = static_cast<int>(theCoords.X * Math::cos(rotateAngle) + theCoords.Y * Math::sin(rotateAngle));
				theOffset.Y = static_cast<int>(theCoords.X * Math::sin(rotateAngle) - theCoords.Y * Math::cos(rotateAngle));
			}
			else
			{
				theOffset.X = 0;
				theOffset.Y = 0;
			}

			break;
		}
		case TraceTargetMode::Rotate:
		{
			const auto distanceCoords = pBullet->Location - destination;
			const auto radius = Point2D{theCoords.X,theCoords.Y}.Magnitude();

			if ((radius * 1.2) > Point2D{distanceCoords.X,distanceCoords.Y}.Magnitude())
			{
				const auto rotateAngle = Math::atan2(distanceCoords.Y, distanceCoords.X) + this->RotateRadian;

				theOffset.X = static_cast<int>(radius * Math::cos(rotateAngle));
				theOffset.Y = static_cast<int>(radius * Math::sin(rotateAngle));
			}
			else
			{
				theOffset.X = 0;
				theOffset.Y = 0;
			}

			break;
		}
		default:
		{
			const auto& theSource = pBullet->SourceCoords;
			const auto& theTarget = pBullet->TargetCoords;

			const auto rotateAngle = Math::atan2(theTarget.Y - theSource.Y , theTarget.X - theSource.X);

			theOffset.X = static_cast<int>(theCoords.X * Math::cos(rotateAngle) + theCoords.Y * Math::sin(rotateAngle));
			theOffset.Y = static_cast<int>(theCoords.X * Math::sin(rotateAngle) - theCoords.Y * Math::cos(rotateAngle));
			break;
		}
		}
	}

	const auto distanceCoords = ((destination + theOffset) - pBullet->Location);
	const auto distance = distanceCoords.Magnitude();

	pBullet->Velocity.X = static_cast<double>(distanceCoords.X);
	pBullet->Velocity.Y = static_cast<double>(distanceCoords.Y);
	pBullet->Velocity.Z = static_cast<double>(distanceCoords.Z);

	if (pType->Trajectory_Speed > 0 && distance > pType->Trajectory_Speed)
		pBullet->Velocity *= (pType->Trajectory_Speed / distance);
}

void TracingTrajectory::FireTracingWeapon(BulletClass* pBullet)
{
	const auto pType = this->Type;
	WeaponTypeClass* pWeapon = nullptr;

	if (const int validWeapons = pType->Weapons.size())
	{
		if (this->WeaponIndex < validWeapons)
		{
			pWeapon = pType->Weapons[this->WeaponIndex];
		}
		else if (this->WeaponCycle < 0 || --this->WeaponCycle > 0)
		{
			this->WeaponIndex %= validWeapons;
			pWeapon = pType->Weapons[this->WeaponIndex];
		}
		else
		{
			this->WeaponTimer.Stop();
			return;
		}
	}
	else
	{
		this->WeaponTimer.Stop();
		return;
	}

	if (!pWeapon)
	{
		++this->WeaponIndex;
		return;
	}

	if (const int validCounts = pType->WeaponCount.size())
	{
		const auto count = pType->WeaponCount[(this->WeaponIndex < validCounts) ? this->WeaponIndex : (validCounts - 1)];

		if (!count)
		{
			++this->WeaponIndex;
			return;
		}
		else if (count > 0 && ++this->WeaponCount >= count)
		{
			++this->WeaponIndex;
			this->WeaponCount = 0;
		}
	}
	else
	{
		this->WeaponTimer.Stop();
		return;
	}

	const auto pTechno = pBullet->Owner;
	const auto pOwner = pTechno ? pTechno->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse;
	const auto pWeaponExt = WeaponTypeExt::ExtMap.Find(pWeapon);

	AbstractClass* pTarget = nullptr;

	if (pType->TraceTheTarget)
	{
		pTarget = pBullet;
	}
	else if (pType->Synchronize)
	{
		pTarget = pBullet->Target;
	}
	else
	{
		const auto vec = Helpers::Alex::getCellSpreadItems(pBullet->Location, (pWeapon->Range / 256.0), pWeapon->Projectile->AA);

		for (const auto& pOpt : vec)
		{
			if (!pOpt->IsAlive || !pOpt->IsOnMap || pOpt->InLimbo || pOpt->IsSinking || pOpt->Health <= 0)
				continue;

			const auto pOptType = pOpt->GetTechnoType();

			if (!pOptType->LegalTarget || pOpt == pTechno)
				continue;

			const auto absType = pOpt->WhatAmI();

			if (pOwner->IsAlliedWith(pOpt->Owner))
				continue;

			const auto pCell = pOpt->GetCell();

			if (absType == AbstractType::Infantry && pOpt->IsDisguisedAs(pOwner) && !pCell->DisguiseSensors_InclHouse(pOwner->ArrayIndex))
				continue;

			if (absType == AbstractType::Unit && pOpt->IsDisguised() && !pCell->DisguiseSensors_InclHouse(pOwner->ArrayIndex))
				continue;

			if (pOpt->CloakState == CloakState::Cloaked && !pCell->Sensors_InclHouse(pOwner->ArrayIndex))
				continue;

			if (MapClass::GetTotalDamage(100, pWeapon->Warhead, pOptType->Armor, 0) == 0)
				continue;

			if (pWeaponExt && (!EnumFunctions::IsTechnoEligible(pOpt, pWeaponExt->CanTarget) || !pWeaponExt->HasRequiredAttachedEffects(pOpt, pTechno)))
				continue;

			pTarget = pOpt;
			break;
		}

		if (!pTarget)
			pTarget = pBullet->Target;
	}

	if (!pTarget)
		return;

	if (const int validDelays = pType->WeaponDelay.size())
	{
		const auto delay = pType->WeaponDelay[(this->WeaponIndex < validDelays) ? this->WeaponIndex : (validDelays - 1)];
		this->WeaponTimer.Start((delay > 0) ? delay : 1);
	}

	const auto pTransporter = pTechno->Transporter;
	const auto targetCoords = pTarget->GetCoords();

	auto fireCoord = pBullet->SourceCoords;

	if (pType->TraceTheTarget)
	{
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
	}
	else
	{
		fireCoord = pBullet->Location;
	}

	const auto finalDamage = static_cast<int>(pWeapon->Damage * this->FirepowerMult);

	if (const auto pCreateBullet = pWeapon->Projectile->CreateBullet(pTarget, pTechno, finalDamage, pWeapon->Warhead, pWeapon->Speed, pWeapon->Bright))
	{
		pCreateBullet->WeaponType = pWeapon;
		const auto pBulletExt = BulletExt::ExtMap.Find(pCreateBullet);
		pBulletExt->FirerHouse = pOwner;
		pCreateBullet->MoveTo(fireCoord, BulletVelocity::Empty);

		if (pBulletExt->Trajectory)
		{
			const auto flag = pBulletExt->Trajectory->Flag();

			if (flag == TrajectoryFlag::Disperse)
			{
				const auto pTrajectory = static_cast<DisperseTrajectory*>(pBulletExt->Trajectory.get());
				pTrajectory->FirepowerMult = this->FirepowerMult;
			}
			else if (flag == TrajectoryFlag::Straight)
			{
				const auto pTrajectory = static_cast<StraightTrajectory*>(pBulletExt->Trajectory.get());
				pTrajectory->FirepowerMult = this->FirepowerMult;
			}
			else if (flag == TrajectoryFlag::Engrave)
			{
				const auto pTrajectory = static_cast<EngraveTrajectory*>(pBulletExt->Trajectory.get());
				pTrajectory->NotMainWeapon = true;
			}
			else if (flag == TrajectoryFlag::Tracing)
			{
				const auto pTrajectory = static_cast<TracingTrajectory*>(pBulletExt->Trajectory.get());
				pTrajectory->FirepowerMult = this->FirepowerMult;
				pTrajectory->NotMainWeapon = true;
			}
		}

		const auto animCounts = pWeapon->Anim.Count;

		if (animCounts > 0)
		{
			int animIndex = 0;

			if (animCounts % 8 == 0)
			{
				if (pBulletExt->Trajectory)
					animIndex = static_cast<int>((Math::atan2(pCreateBullet->Velocity.Y , pCreateBullet->Velocity.X) + Math::TwoPi + Math::Pi) * animCounts / Math::TwoPi - (animCounts / 8) + 0.5) % animCounts;
				else
					animIndex = static_cast<int>((Math::atan2(targetCoords.Y - fireCoord.Y , targetCoords.X - fireCoord.X) + Math::TwoPi + Math::Pi) * animCounts / Math::TwoPi - (animCounts / 8) + 0.5) % animCounts;
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

	if (pWeapon->IsLaser)
	{
		if (pWeapon->IsHouseColor || pWeaponExt->Laser_IsSingleColor)
		{
			auto const pLaser = GameCreate<LaserDrawClass>(fireCoord, targetCoords, ((pWeapon->IsHouseColor && pOwner) ? pOwner->LaserColor : pWeapon->LaserInnerColor), ColorStruct { 0, 0, 0 }, ColorStruct { 0, 0, 0 }, pWeapon->LaserDuration);
			pLaser->IsHouseColor = true;
			pLaser->Thickness = pWeaponExt->LaserThickness;
			pLaser->IsSupported = (pLaser->Thickness > 3);
		}
		else
		{
			auto const pLaser = GameCreate<LaserDrawClass>(fireCoord, targetCoords, pWeapon->LaserInnerColor, pWeapon->LaserOuterColor, pWeapon->LaserOuterSpread, pWeapon->LaserDuration);
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
			pEBolt->Fire(fireCoord, targetCoords, 0);
		}
	}

	if (pWeapon->IsRadBeam)
	{
		const bool isTemporal = pWeapon->Warhead && pWeapon->Warhead->Temporal;

		if (const auto pRadBeam = RadBeam::Allocate(isTemporal ? RadBeamType::Temporal : RadBeamType::RadBeam))
		{
			pRadBeam->SetCoordsSource(fireCoord);
			pRadBeam->SetCoordsTarget(targetCoords);
			pRadBeam->Color = (pWeaponExt->Beam_IsHouseColor && pOwner) ? pOwner->LaserColor : pWeaponExt->Beam_Color.Get(isTemporal ? RulesClass::Instance->ChronoBeamColor : RulesClass::Instance->RadColor);
			pRadBeam->Period = pWeaponExt->Beam_Duration;
			pRadBeam->Amplitude = pWeaponExt->Beam_Amplitude;
		}
	}

	if (const auto pPSType = pWeapon->AttachedParticleSystem)
		GameCreate<ParticleSystemClass>(pPSType, fireCoord, pTarget, pBullet->Owner, targetCoords, pOwner);
}
