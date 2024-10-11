#include "BombardTrajectory.h"
#include "Memory.h"

#include <AnimClass.h>
#include <Ext/Anim/Body.h>
#include <Ext/Bullet/Body.h>

std::unique_ptr<PhobosTrajectory> BombardTrajectoryType::CreateInstance() const
{
	return std::make_unique<BombardTrajectory>(this);
}

template<typename T>
void BombardTrajectoryType::Serialize(T& Stm)
{
	Stm
		.Process(this->Height)
		.Process(this->FallPercent)
		.Process(this->FallPercentShift)
		.Process(this->FallScatter_Max)
		.Process(this->FallScatter_Min)
		.Process(this->FallSpeed)
		.Process(this->DetonationDistance)
		.Process(this->DetonationHeight)
		.Process(this->EarlyDetonation)
		.Process(this->TargetSnapDistance)
		.Process(this->FreeFallOnTarget)
		.Process(this->LeadTimeCalculate)
		.Process(this->NoLaunch)
		.Process(this->TurningPointAnims)
		.Process(this->OffsetCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
		.Process(this->SubjectToGround)
		;
}

bool BombardTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);
	this->Serialize(Stm);
	return true;
}

bool BombardTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);
	const_cast<BombardTrajectoryType*>(this)->Serialize(Stm);
	return true;
}

void BombardTrajectoryType::Read(CCINIClass* const pINI, const char* pSection)
{
	INI_EX exINI(pINI);

	this->Height.Read(exINI, pSection, "Trajectory.Bombard.Height");
	this->FallPercent.Read(exINI, pSection, "Trajectory.Bombard.FallPercent");
	this->FallPercentShift.Read(exINI, pSection, "Trajectory.Bombard.FallPercentShift");
	this->FallScatter_Max.Read(exINI, pSection, "Trajectory.Bombard.FallScatter.Max");
	this->FallScatter_Min.Read(exINI, pSection, "Trajectory.Bombard.FallScatter.Min");
	this->FallSpeed.Read(exINI, pSection, "Trajectory.Bombard.FallSpeed");
	this->DetonationDistance.Read(exINI, pSection, "Trajectory.Bombard.DetonationDistance");
	this->DetonationHeight.Read(exINI, pSection, "Trajectory.Bombard.DetonationHeight");
	this->EarlyDetonation.Read(exINI, pSection, "Trajectory.Bombard.EarlyDetonation");
	this->TargetSnapDistance.Read(exINI, pSection, "Trajectory.Bombard.TargetSnapDistance");
	this->FreeFallOnTarget.Read(exINI, pSection, "Trajectory.Bombard.FreeFallOnTarget");
	this->LeadTimeCalculate.Read(exINI, pSection, "Trajectory.Bombard.LeadTimeCalculate");
	this->NoLaunch.Read(exINI, pSection, "Trajectory.Bombard.NoLaunch");
	this->TurningPointAnims.Read(exINI, pSection, "Trajectory.Bombard.TurningPointAnims");
	this->OffsetCoord.Read(exINI, pSection, "Trajectory.Bombard.OffsetCoord");
	this->RotateCoord.Read(exINI, pSection, "Trajectory.Bombard.RotateCoord");
	this->MirrorCoord.Read(exINI, pSection, "Trajectory.Bombard.MirrorCoord");
	this->UseDisperseBurst.Read(exINI, pSection, "Trajectory.Bombard.UseDisperseBurst");
	this->AxisOfRotation.Read(exINI, pSection, "Trajectory.Bombard.AxisOfRotation");
	this->SubjectToGround.Read(exINI, pSection, "Trajectory.Bombard.SubjectToGround");
}

template<typename T>
void BombardTrajectory::Serialize(T& Stm)
{
	Stm
		.Process(this->Type)
		.Process(this->Height)
		.Process(this->FallPercent)
		.Process(this->FallSpeed)
		.Process(this->OffsetCoord)
		.Process(this->IsFalling)
		.Process(this->RemainingDistance)
		.Process(this->LastTargetCoord)
		.Process(this->CountOfBurst)
		.Process(this->CurrentBurst)
		.Process(this->RotateAngle)
		.Process(this->AscendTime)
		;
}

bool BombardTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->Serialize(Stm);
	return true;
}

bool BombardTrajectory::Save(PhobosStreamWriter& Stm) const
{
	const_cast<BombardTrajectory*>(this)->Serialize(Stm);
	return true;
}

void BombardTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	const BombardTrajectoryType* const pType = this->Type;
	this->Height += pBullet->TargetCoords.Z;
	// use scaling since RandomRanged only support int
	const double fallPercentShift = ScenarioClass::Instance->Random.RandomRanged(0, static_cast<int>(200 * pType->FallPercentShift)) / 100.0;
	this->FallPercent += fallPercentShift - pType->FallPercentShift;
	this->LastTargetCoord = pBullet->TargetCoords;

	if (!this->FallSpeed)
		this->FallSpeed = pType->Trajectory_Speed;

	TechnoClass* const pOwner = pBullet->Owner;

	if (WeaponTypeClass* const pWeapon = pBullet->WeaponType)
		this->CountOfBurst = pWeapon->Burst;

	if (pOwner)
	{
		this->CurrentBurst = pOwner->CurrentBurstIndex;

		if (pType->MirrorCoord && pOwner->CurrentBurstIndex % 2 == 1)
			this->OffsetCoord.Y = -(this->OffsetCoord.Y);
	}

	if (!pType->NoLaunch)
	{
		if (pType->FreeFallOnTarget)
			this->CalculateLeadTime(pBullet);

		pBullet->Velocity.X = static_cast<double>(pBullet->TargetCoords.X - pBullet->SourceCoords.X) * this->FallPercent;
		pBullet->Velocity.Y = static_cast<double>(pBullet->TargetCoords.Y - pBullet->SourceCoords.Y) * this->FallPercent;
		pBullet->Velocity.Z = static_cast<double>(this->Height - pBullet->SourceCoords.Z);
		pBullet->Velocity *= pType->Trajectory_Speed / pBullet->Velocity.Magnitude();
	}
	else
	{
		CoordStruct SourceLocation { 0, 0, static_cast<int>(this->Height - pBullet->SourceCoords.Z) };

		this->IsFalling = true;
		double angel = ScenarioClass::Instance->Random.RandomDouble() * Math::TwoPi;
		double length = ScenarioClass::Instance->Random.RandomRanged(Leptons { pType->FallScatter_Min }, Leptons { pType->FallScatter_Max });
		int scatterX = static_cast<int>(length * Math::cos(angel));
		int scatterY = static_cast<int>(length * Math::sin(angel));

		if (!pType->FreeFallOnTarget)
		{
			SourceLocation.X = pBullet->SourceCoords.X + static_cast<int>((pBullet->TargetCoords.X - pBullet->SourceCoords.X) * this->FallPercent) + scatterX;
			SourceLocation.Y = pBullet->SourceCoords.Y + static_cast<int>((pBullet->TargetCoords.Y - pBullet->SourceCoords.Y) * this->FallPercent) + scatterY;
			pBullet->Limbo();
			pBullet->Unlimbo(SourceLocation, DirType::North);

			this->CalculateLeadTime(pBullet);

			pBullet->Velocity.X = static_cast<double>(pBullet->TargetCoords.X - SourceLocation.X);
			pBullet->Velocity.Y = static_cast<double>(pBullet->TargetCoords.Y - SourceLocation.Y);
			pBullet->Velocity.Z = static_cast<double>(pBullet->TargetCoords.Z - SourceLocation.Z);

			this->CalculateDisperseBurst(pBullet, pBullet->Velocity);
			this->RemainingDistance += static_cast<int>(pBullet->TargetCoords.DistanceFrom(SourceLocation) + this->FallSpeed);
			this->CalculateBulletVelocity(pBullet->Velocity);
		}
		else
		{
			const AbstractClass* const pTarget = pBullet->Target;

			if (pType->LeadTimeCalculate && pTarget && pTarget->GetCoords() != this->LastTargetCoord)
			{
				const CoordStruct extraOffsetCoord = pTarget->GetCoords() - this->LastTargetCoord;
				const int travelTime = static_cast<int>(sqrt(2 * (this->Height - pTarget->GetCoords().Z) / BulletTypeExt::GetAdjustedGravity(pBullet->Type)));
				pBullet->TargetCoords += extraOffsetCoord * (travelTime + 1);
			}

			SourceLocation.X = pBullet->TargetCoords.X + scatterX;
			SourceLocation.Y = pBullet->TargetCoords.Y + scatterY;
			pBullet->Limbo();
			pBullet->Unlimbo(SourceLocation, DirType::North);
			pBullet->Velocity.X = 0.0;
			pBullet->Velocity.Y = 0.0;
			pBullet->Velocity.Z = 0.0;
		}

		this->ApplyTurningPointAnim(pType->TurningPointAnims, SourceLocation, pOwner, pOwner ? pOwner->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse, true);
	}
}

bool BombardTrajectory::OnAI(BulletClass* pBullet)
{
	const BombardTrajectoryType* const pType = this->Type;

	// Close enough
	if (pBullet->TargetCoords.DistanceFrom(pBullet->Location) < pType->DetonationDistance.Get())
		return true;

	// Height
	if (pType->DetonationHeight >= 0)
	{
		if (pType->EarlyDetonation && (pBullet->Location.Z - pBullet->SourceCoords.Z) > pType->DetonationHeight)
			return true;
		else if (this->IsFalling && (pBullet->Location.Z - pBullet->SourceCoords.Z) < pType->DetonationHeight)
			return true;
	}

	// Ground, must be checked when free fall
	if (pType->SubjectToGround || (this->IsFalling && pType->FreeFallOnTarget))
	{
		if (MapClass::Instance->GetCellFloorHeight(pBullet->Location) >= (pBullet->Location.Z + 15))
			return true;
	}

	// Extra check for trajectory falling
	auto const pOwner = pBullet->Owner ? pBullet->Owner->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse;

	if (this->IsFalling && !pType->FreeFallOnTarget && this->BulletDetonatePreCheck(pBullet, pOwner))
		return true;

	return false;
}

void BombardTrajectory::OnAIPreDetonate(BulletClass* pBullet)
{
	const BombardTrajectoryType* const pType = this->Type;
	const Leptons TargetSnapDistance = pType->TargetSnapDistance;
	auto pTarget = abstract_cast<ObjectClass*>(pBullet->Target);
	auto pCoords = pTarget ? pTarget->GetCoords() : pBullet->Data.Location;

	if (pCoords.DistanceFrom(pBullet->Location) <= TargetSnapDistance)
	{
		auto const pExt = BulletExt::ExtMap.Find(pBullet);
		pExt->SnappedToTarget = true;
		pBullet->SetLocation(pCoords);
	}
}

void BombardTrajectory::OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition)
{
	const BombardTrajectoryType* const pType = this->Type;

	if (!this->IsFalling)
	{
		pSpeed->Z += BulletTypeExt::GetAdjustedGravity(pBullet->Type);

		if (pBullet->Location.Z + pBullet->Velocity.Z >= this->Height)
		{
			this->IsFalling = true;
			TechnoClass* const pTechno = pBullet->Owner;

			if (!pType->FreeFallOnTarget)
			{
				this->CalculateLeadTime(pBullet);

				pSpeed->X = static_cast<double>(pBullet->TargetCoords.X - pBullet->Location.X - pBullet->Velocity.X);
				pSpeed->Y = static_cast<double>(pBullet->TargetCoords.Y - pBullet->Location.Y - pBullet->Velocity.Y);
				pSpeed->Z = static_cast<double>(pBullet->TargetCoords.Z - pBullet->Location.Z - pBullet->Velocity.Z);

				pPosition->X = pBullet->Location.X + pBullet->Velocity.X;
				pPosition->Y = pBullet->Location.Y + pBullet->Velocity.Y;
				pPosition->Z = pBullet->Location.Z + pBullet->Velocity.Z;

				this->CalculateDisperseBurst(pBullet, *pSpeed);

				CoordStruct BulletLocation { static_cast<int>(pPosition->X), static_cast<int>(pPosition->Y), static_cast<int>(pPosition->Z) };

				this->RemainingDistance += static_cast<int>(pBullet->TargetCoords.DistanceFrom(BulletLocation) + this->FallSpeed);
				this->CalculateBulletVelocity(*pSpeed);
				this->ApplyTurningPointAnim(pType->TurningPointAnims, BulletLocation, pTechno, pTechno ? pTechno->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse, true);
			}
			else
			{
				pSpeed->X = 0.0;
				pSpeed->Y = 0.0;
				pSpeed->Z = 0.0;

				auto pExt = BulletExt::ExtMap.Find(pBullet);

				if (this->FallPercent != 1.0)
				{
					pExt->LaserTrails.clear();

					CoordStruct target = pBullet->TargetCoords;
					target.Z += static_cast<int>(pType->Height); // Use original height here
					pBullet->Limbo();
					pBullet->Unlimbo(target, DirType::North);

					pPosition->X = pBullet->TargetCoords.X;
					pPosition->Y = pBullet->TargetCoords.Y;
					pPosition->Z = pBullet->TargetCoords.Z + static_cast<int>(pType->Height);

					if (auto pTypeExt = pExt->TypeExtData)
					{
						auto pThis = pExt->OwnerObject();
						auto pOwner = pThis->Owner ? pThis->Owner->Owner : nullptr;

						for (auto const& idxTrail : pTypeExt->LaserTrail_Types)
						{
							if (auto const pLaserType = LaserTrailTypeClass::Array[idxTrail].get())
								pExt->LaserTrails.push_back(LaserTrailClass { pLaserType, pOwner });
						}
					}
				}
				else
				{
					pPosition->X = pBullet->TargetCoords.X;
					pPosition->Y = pBullet->TargetCoords.Y;
				}

				CoordStruct BulletLocation { static_cast<int>(pPosition->X), static_cast<int>(pPosition->Y), static_cast<int>(pPosition->Z) };
				this->ApplyTurningPointAnim(pType->TurningPointAnims, BulletLocation, pTechno, pTechno ? pTechno->Owner : pExt->FirerHouse, true);
			}
		}
		else
		{
			this->AscendTime += 1;
		}
	}
	else if (!pType->FreeFallOnTarget)
	{
		pSpeed->Z += BulletTypeExt::GetAdjustedGravity(pBullet->Type);
	}
}

TrajectoryCheckReturnType BombardTrajectory::OnAITargetCoordCheck(BulletClass* pBullet)
{
	return TrajectoryCheckReturnType::SkipGameCheck; // Bypass game checks entirely.
}

TrajectoryCheckReturnType BombardTrajectory::OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno)
{
	return TrajectoryCheckReturnType::SkipGameCheck; // Bypass game checks entirely.
}

void BombardTrajectory::CalculateLeadTime(BulletClass* pBullet)
{
	const BombardTrajectoryType* const pType = this->Type;
	const AbstractClass* const pTarget = pBullet->Target;

	CoordStruct theTargetCoords = pBullet->TargetCoords;
	CoordStruct theSourceCoords = pBullet->SourceCoords;

	if (pType->LeadTimeCalculate && pTarget)
	{
		theTargetCoords = pTarget->GetCoords();
		theSourceCoords = pBullet->Location;

		if (theTargetCoords != this->LastTargetCoord)
		{
			CoordStruct extraOffsetCoord = theTargetCoords - this->LastTargetCoord;
			CoordStruct lastSourceCoord = theSourceCoords - this->LastTargetCoord;
			CoordStruct targetSourceCoord = theSourceCoords - theTargetCoords;

			const CoordStruct realExtraOffsetCoord = extraOffsetCoord;

			if (pType->FreeFallOnTarget)
			{
				CoordStruct theTurningPointCoords = theTargetCoords;
				CoordStruct theLastTurningPointCoords = this->LastTargetCoord;
				theTurningPointCoords.Z += static_cast<int>(pType->Height); // Use original height here
				theLastTurningPointCoords.Z += static_cast<int>(pType->Height);

				if (this->FallPercent != 1.0)
				{
					theTurningPointCoords.X = theSourceCoords.X - static_cast<int>(targetSourceCoord.X * this->FallPercent);
					theTurningPointCoords.Y = theSourceCoords.Y - static_cast<int>(targetSourceCoord.Y * this->FallPercent);

					theLastTurningPointCoords.X = theSourceCoords.X - static_cast<int>(lastSourceCoord.X * this->FallPercent);
					theLastTurningPointCoords.Y = theSourceCoords.Y - static_cast<int>(lastSourceCoord.Y * this->FallPercent);

					extraOffsetCoord = theTurningPointCoords - theLastTurningPointCoords;
				}

				targetSourceCoord = theSourceCoords - theTurningPointCoords;
				lastSourceCoord = theSourceCoords - theLastTurningPointCoords;
			}

			const double theDistanceSquared = targetSourceCoord.MagnitudeSquared();

			const double targetSpeedSquared = extraOffsetCoord.MagnitudeSquared();
			const double targetSpeed = sqrt(targetSpeedSquared);

			const double crossFactor = lastSourceCoord.CrossProduct(targetSourceCoord).MagnitudeSquared();
			const double verticalDistanceSquared = crossFactor / targetSpeedSquared;

			const double horizonDistanceSquared = theDistanceSquared - verticalDistanceSquared;
			const double horizonDistance = sqrt(horizonDistanceSquared);

			const double straightSpeed = pType->FreeFallOnTarget ? pType->Trajectory_Speed : this->FallSpeed;
			const double straightSpeedSquared = straightSpeed * straightSpeed;

			const double baseFactor = straightSpeedSquared - targetSpeedSquared;
			const double squareFactor = baseFactor * verticalDistanceSquared + straightSpeedSquared * horizonDistanceSquared;

			if (squareFactor > 1e-10)
			{
				const double minusFactor = -(horizonDistance * targetSpeed);
				int travelTime = 0;

				if (abs(baseFactor) < 1e-10)
				{
					travelTime = abs(horizonDistance) > 1e-10 ? (static_cast<int>(theDistanceSquared / (2 * horizonDistance * targetSpeed)) + 1) : 0;
				}
				else
				{
					const int travelTimeM = static_cast<int>((minusFactor - sqrt(squareFactor)) / baseFactor);
					const int travelTimeP = static_cast<int>((minusFactor + sqrt(squareFactor)) / baseFactor);

					if (travelTimeM > 0 && travelTimeP > 0)
						travelTime = travelTimeM < travelTimeP ? travelTimeM : travelTimeP;
					else if (travelTimeM > 0)
						travelTime = travelTimeM;
					else if (travelTimeP > 0)
						travelTime = travelTimeP;

					if (pType->FreeFallOnTarget)
						travelTime += static_cast<int>(sqrt(2 * (this->Height - theTargetCoords.Z) / BulletTypeExt::GetAdjustedGravity(pBullet->Type)));

					travelTime += this->AscendTime;

					if (targetSourceCoord.MagnitudeSquared() >= lastSourceCoord.MagnitudeSquared())
						travelTime += 1;
				}

				theTargetCoords += realExtraOffsetCoord * travelTime / this->AscendTime;
			}
		}
	}

	pBullet->TargetCoords = theTargetCoords;

	if (!pType->LeadTimeCalculate && theTargetCoords == theSourceCoords && pBullet->Owner) //For disperse.
	{
		const CoordStruct theOwnerCoords = pBullet->Owner->GetCoords();
		this->RotateAngle = Math::atan2(theTargetCoords.Y - theOwnerCoords.Y , theTargetCoords.X - theOwnerCoords.X);
	}
	else
	{
		this->RotateAngle = Math::atan2(theTargetCoords.Y - theSourceCoords.Y , theTargetCoords.X - theSourceCoords.X);
	}

	if (this->OffsetCoord != CoordStruct::Empty)
	{
		pBullet->TargetCoords.X += static_cast<int>(this->OffsetCoord.X * Math::cos(this->RotateAngle) + this->OffsetCoord.Y * Math::sin(this->RotateAngle));
		pBullet->TargetCoords.Y += static_cast<int>(this->OffsetCoord.X * Math::sin(this->RotateAngle) - this->OffsetCoord.Y * Math::cos(this->RotateAngle));
		pBullet->TargetCoords.Z += this->OffsetCoord.Z;
	}

	if (pBullet->Type->Inaccurate)
	{
		auto const pTypeExt = BulletTypeExt::ExtMap.Find(pBullet->Type);
		const double offsetMult = 0.0004 * pBullet->SourceCoords.DistanceFrom(pBullet->TargetCoords);
		const int offsetMin = static_cast<int>(offsetMult * pTypeExt->BallisticScatter_Min.Get(Leptons(0)));
		const int offsetMax = static_cast<int>(offsetMult * pTypeExt->BallisticScatter_Max.Get(Leptons(RulesClass::Instance->BallisticScatter)));
		const int offsetDistance = ScenarioClass::Instance->Random.RandomRanged(offsetMin, offsetMax);
		pBullet->TargetCoords = MapClass::GetRandomCoordsNear(pBullet->TargetCoords, offsetDistance, false);
	}
}

void BombardTrajectory::CalculateDisperseBurst(BulletClass* pBullet, BulletVelocity& pVelocity)
{
	const BombardTrajectoryType* const pType = this->Type;
	const CoordStruct axis = pType->AxisOfRotation;

	if (!pType->UseDisperseBurst && abs(pType->RotateCoord) > 1e-10 && this->CountOfBurst > 1)
	{

		BulletVelocity rotationAxis
		{
			axis.X * Math::cos(this->RotateAngle) + axis.Y * Math::sin(this->RotateAngle),
			axis.X * Math::sin(this->RotateAngle) - axis.Y * Math::cos(this->RotateAngle),
			static_cast<double>(axis.Z)
		};

		const double rotationAxisLengthSquared = rotationAxis.MagnitudeSquared();

		if (abs(rotationAxisLengthSquared) > 1e-10)
		{
			double extraRotate = 0.0;
			rotationAxis *= 1 / sqrt(rotationAxisLengthSquared);

			if (pType->MirrorCoord)
			{
				if (pBullet->Owner && pBullet->Owner->CurrentBurstIndex % 2 == 1)
					rotationAxis *= -1;

				extraRotate = Math::Pi * (pType->RotateCoord * ((this->CurrentBurst / 2) / (this->CountOfBurst - 1.0) - 0.5)) / 180;
			}
			else
			{
				extraRotate = Math::Pi * (pType->RotateCoord * (this->CurrentBurst / (this->CountOfBurst - 1.0) - 0.5)) / 180;
			}

			const double cosRotate = Math::cos(extraRotate);
			pVelocity = (pVelocity * cosRotate) + (rotationAxis * ((1 - cosRotate) * (pVelocity * rotationAxis))) + (rotationAxis.CrossProduct(pVelocity) * Math::sin(extraRotate));
		}
	}
}

void BombardTrajectory::CalculateBulletVelocity(BulletVelocity& pVelocity)
{
	const double velocityLength = pVelocity.Magnitude();

	if (velocityLength > 1e-10)
		pVelocity *= this->FallSpeed / velocityLength;
	else
		this->RemainingDistance = 0;
}

bool BombardTrajectory::BulletDetonatePreCheck(BulletClass* pBullet, HouseClass* pOwner)
{
	this->RemainingDistance -= static_cast<int>(this->FallSpeed);

	if (this->RemainingDistance < 0)
		return true;

	if (this->RemainingDistance < this->FallSpeed)
	{
		pBullet->Velocity *= this->RemainingDistance / this->FallSpeed;
		this->RemainingDistance = 0;
	}

	return false;
}

void BombardTrajectory::ApplyTurningPointAnim(const std::vector<AnimTypeClass*>& AnimList, CoordStruct coords, TechnoClass* pTechno, HouseClass* pHouse, bool invoker, bool ownedObject)
{
	if (AnimList.empty())
		return;

	auto const pAnimType = AnimList[ScenarioClass::Instance->Random.RandomRanged(0, AnimList.size() - 1)];

	if (!pAnimType)
		return;

	auto const pAnim = GameCreate<AnimClass>(pAnimType, coords);

	if (!pAnim || !pTechno)
		return;

	AnimExt::SetAnimOwnerHouseKind(pAnim, pHouse ? pHouse : pTechno->Owner, nullptr, false, true);

	if (ownedObject)
		pAnim->SetOwnerObject(pTechno);

	if (invoker)
	{
		auto const pAnimExt = AnimExt::ExtMap.Find(pAnim);

		if (!pAnimExt)
			return;

		if (pHouse)
			pAnimExt->SetInvoker(pTechno, pHouse);
		else
			pAnimExt->SetInvoker(pTechno);
	}
}
