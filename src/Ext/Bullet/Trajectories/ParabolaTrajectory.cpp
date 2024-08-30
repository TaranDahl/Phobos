#include "ParabolaTrajectory.h"
#include <Ext/Bullet/Body.h>
#include <Ext/BulletType/Body.h>
#include <Ext/WarheadType/Body.h>
#include <ScenarioClass.h>

bool ParabolaTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectoryType::Load(Stm, false);

	Stm
		.Process(this->DetonationDistance, false)
		.Process(this->TargetSnapDistance, false)
		.Process(this->OpenFireMode, false)
		.Process(this->ThrowHeight, false)
		.Process(this->LaunchAngle, false)
		.Process(this->LeadTimeCalculate, false)
		.Process(this->BounceTimes, false)
		.Process(this->BounceOnWater, false)
		.Process(this->BounceDetonate, false)
		.Process(this->BounceAttenuation, false)
		.Process(this->BounceCoefficient, false)
		.Process(this->OffsetCoord, false)
		.Process(this->RotateCoord, false)
		.Process(this->MirrorCoord, false)
		.Process(this->UseDisperseBurst, false)
		.Process(this->AxisOfRotation, false)
		;

	return true;
}

bool ParabolaTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectoryType::Save(Stm);

	Stm
		.Process(this->DetonationDistance)
		.Process(this->TargetSnapDistance)
		.Process(this->OpenFireMode)
		.Process(this->ThrowHeight)
		.Process(this->LaunchAngle)
		.Process(this->LeadTimeCalculate)
		.Process(this->BounceTimes)
		.Process(this->BounceOnWater)
		.Process(this->BounceDetonate)
		.Process(this->BounceAttenuation)
		.Process(this->BounceCoefficient)
		.Process(this->OffsetCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
		;

	return true;
}

PhobosTrajectory* ParabolaTrajectoryType::CreateInstance() const
{
	return new ParabolaTrajectory(this);
}

void ParabolaTrajectoryType::Read(CCINIClass* const pINI, const char* pSection)
{
	INI_EX exINI(pINI);
	this->DetonationDistance.Read(exINI, pSection, "Trajectory.Parabola.DetonationDistance");
	this->TargetSnapDistance.Read(exINI, pSection, "Trajectory.Parabola.TargetSnapDistance");
	this->OpenFireMode.Read(exINI, pSection, "Trajectory.Parabola.OpenFireMode");
	this->ThrowHeight.Read(exINI, pSection, "Trajectory.Parabola.ThrowHeight");
	this->LaunchAngle.Read(exINI, pSection, "Trajectory.Parabola.LaunchAngle");
	this->LeadTimeCalculate.Read(exINI, pSection, "Trajectory.Parabola.LeadTimeCalculate");
	this->BounceTimes.Read(exINI, pSection, "Trajectory.Parabola.BounceTimes");
	this->BounceOnWater.Read(exINI, pSection, "Trajectory.Parabola.BounceOnWater");
	this->BounceDetonate.Read(exINI, pSection, "Trajectory.Parabola.BounceDetonate");
	this->BounceAttenuation.Read(exINI, pSection, "Trajectory.Parabola.BounceAttenuation");
	this->BounceCoefficient.Read(exINI, pSection, "Trajectory.Parabola.BounceCoefficient");
	this->OffsetCoord.Read(exINI, pSection, "Trajectory.Parabola.OffsetCoord");
	this->RotateCoord.Read(exINI, pSection, "Trajectory.Parabola.RotateCoord");
	this->MirrorCoord.Read(exINI, pSection, "Trajectory.Parabola.MirrorCoord");
	this->UseDisperseBurst.Read(exINI, pSection, "Trajectory.Parabola.UseDisperseBurst");
	this->AxisOfRotation.Read(exINI, pSection, "Trajectory.Parabola.AxisOfRotation");
}

bool ParabolaTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	this->PhobosTrajectory::Load(Stm, false);

	Stm
		.Process(this->DetonationDistance)
		.Process(this->TargetSnapDistance)
		.Process(this->OpenFireMode)
		.Process(this->ThrowHeight)
		.Process(this->LaunchAngle)
		.Process(this->LeadTimeCalculate)
		.Process(this->BounceTimes)
		.Process(this->BounceOnWater)
		.Process(this->BounceDetonate)
		.Process(this->BounceAttenuation)
		.Process(this->BounceCoefficient)
		.Process(this->OffsetCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
		.Process(this->ShouldDetonate)
		.Process(this->ShouldBounce)
		.Process(this->LastTargetCoord)
		.Process(this->CurrentBurst)
		.Process(this->CountOfBurst)
		.Process(this->WaitOneFrame)
		.Process(this->LastVelocity)
		;

	return true;
}

bool ParabolaTrajectory::Save(PhobosStreamWriter& Stm) const
{
	this->PhobosTrajectory::Save(Stm);

	Stm
		.Process(this->DetonationDistance)
		.Process(this->TargetSnapDistance)
		.Process(this->OpenFireMode)
		.Process(this->ThrowHeight)
		.Process(this->LaunchAngle)
		.Process(this->LeadTimeCalculate)
		.Process(this->BounceTimes)
		.Process(this->BounceOnWater)
		.Process(this->BounceDetonate)
		.Process(this->BounceAttenuation)
		.Process(this->BounceCoefficient)
		.Process(this->OffsetCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
		.Process(this->ShouldDetonate)
		.Process(this->ShouldBounce)
		.Process(this->LastTargetCoord)
		.Process(this->CurrentBurst)
		.Process(this->CountOfBurst)
		.Process(this->WaitOneFrame)
		.Process(this->LastVelocity)
		;

	return true;
}

void ParabolaTrajectory::OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity)
{
	auto const pType = this->GetTrajectoryType<ParabolaTrajectoryType>(pBullet);

	this->DetonationDistance = pType->DetonationDistance;
	this->TargetSnapDistance = pType->TargetSnapDistance;
	this->OpenFireMode = pType->OpenFireMode;
	this->ThrowHeight = pType->ThrowHeight > 0 ? pType->ThrowHeight : 600;
	this->LaunchAngle = pType->LaunchAngle;
	this->LeadTimeCalculate = pType->LeadTimeCalculate;
	this->BounceTimes = pType->BounceTimes;
	this->BounceOnWater = pType->BounceOnWater;
	this->BounceDetonate = pType->BounceDetonate;
	this->BounceAttenuation = pType->BounceAttenuation;
	this->BounceCoefficient = pType->BounceCoefficient;
	this->OffsetCoord = pType->OffsetCoord;
	this->RotateCoord = pType->RotateCoord;
	this->MirrorCoord = pType->MirrorCoord;
	this->UseDisperseBurst = pType->UseDisperseBurst;
	this->AxisOfRotation = pType->AxisOfRotation;
	this->ShouldDetonate = false;
	this->ShouldBounce = false;
	this->LastTargetCoord = pBullet->TargetCoords;
	this->CurrentBurst = 0;
	this->CountOfBurst = 0;
	this->LastVelocity = BulletVelocity::Empty;
	pBullet->Velocity = BulletVelocity::Empty;

	if (WeaponTypeClass* const pWeapon = pBullet->WeaponType)
		this->CountOfBurst = pWeapon->Burst;

	if (TechnoClass* const pOwner = pBullet->Owner)
	{
		this->CurrentBurst = pOwner->CurrentBurstIndex;

		if (this->MirrorCoord && pOwner->CurrentBurstIndex % 2 == 1)
			this->OffsetCoord.Y = -(this->OffsetCoord.Y);
	}

	if (!this->LeadTimeCalculate || !abstract_cast<FootClass*>(pBullet->Target))
		this->PrepareForOpenFire(pBullet);
	else
		this->WaitOneFrame.Start(1);
}

bool ParabolaTrajectory::OnAI(BulletClass* pBullet)
{
	if (this->WaitOneFrame.IsTicking() && this->BulletPrepareCheck(pBullet))
		return false;

	if (this->ShouldDetonate || (this->BounceTimes <= 0 && this->DetonationDistance > 0 && pBullet->TargetCoords.DistanceFrom(pBullet->Location) < this->DetonationDistance))
		return true;

	CellClass* const pCell = MapClass::Instance->TryGetCellAt(pBullet->Location);

	if (!pCell)
		return true;

	const double gravity = BulletTypeExt::GetAdjustedGravity(pBullet->Type);

	if (this->ShouldBounce && this->BounceTimes > 0)
	{
		if (pCell->LandType == LandType::Water && !this->BounceOnWater)
			return true;

		--this->BounceTimes;
		this->ShouldBounce = false;

		const BulletVelocity groundNormalVector = this->GetGroundNormalVector(pBullet, pCell);
		pBullet->Velocity = (this->LastVelocity - groundNormalVector * (this->LastVelocity * groundNormalVector) * 2) * this->BounceCoefficient;
		pBullet->Velocity.Z -= gravity;

		if (this->BounceDetonate)
		{
			TechnoClass* const pFirer = pBullet->Owner;
			HouseClass* const pOwner = pFirer ? pFirer->Owner : BulletExt::ExtMap.Find(pBullet)->FirerHouse;
			WarheadTypeExt::DetonateAt(pBullet->WH, pBullet->Location, pFirer, pBullet->Health, pOwner);
		}

		if (const int damage = pBullet->Health)
		{
			if (const int newDamage = static_cast<int>(damage * this->BounceAttenuation))
				pBullet->Health = newDamage;
			else
				pBullet->Health = damage > 0 ? 1 : -1;
		}

		return false;
	}

	do
	{
		const int cellHeight = MapClass::Instance->GetCellFloorHeight(pBullet->Location);

		if (cellHeight >= pBullet->Location.Z + static_cast<int>(gravity) + 1)
			return true;

		pBullet->Velocity.Z -= gravity;

		const CoordStruct futureCoords
		{
			pBullet->Location.X + static_cast<int>(pBullet->Velocity.X),
			pBullet->Location.Y + static_cast<int>(pBullet->Velocity.Y),
			pBullet->Location.Z + static_cast<int>(pBullet->Velocity.Z)
		};

		const int futureHeight = MapClass::Instance->GetCellFloorHeight(futureCoords);

		if (futureHeight >= futureCoords.Z)
		{
			this->LastVelocity = pBullet->Velocity;
			const double mult = abs((pBullet->Location.Z - futureHeight) / pBullet->Velocity.Z);

			if (mult < 1.0) // Make it fall on the ground without penetrating underground
				pBullet->Velocity *= mult;

			break;
		}

		return false;
	}
	while (false);

	if (this->BounceTimes > 0)
		this->ShouldBounce = true;
	else
		this->ShouldDetonate = true;

	return false;
}

void ParabolaTrajectory::OnAIPreDetonate(BulletClass* pBullet)
{
	if (this->TargetSnapDistance <= 0)
		return;

	const ObjectClass* const pTarget = abstract_cast<ObjectClass*>(pBullet->Target);
	const CoordStruct coords = pTarget ? pTarget->GetCoords() : pBullet->Data.Location;

	if (coords.DistanceFrom(pBullet->Location) <= this->TargetSnapDistance)
	{
		auto const pExt = BulletExt::ExtMap.Find(pBullet);
		pExt->SnappedToTarget = true;
		pBullet->SetLocation(coords);
	}
}

void ParabolaTrajectory::OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition)
{
	pSpeed->Z += BulletTypeExt::GetAdjustedGravity(pBullet->Type); // Seems like this is useless
}

TrajectoryCheckReturnType ParabolaTrajectory::OnAITargetCoordCheck(BulletClass* pBullet)
{
	return TrajectoryCheckReturnType::SkipGameCheck;
}

TrajectoryCheckReturnType ParabolaTrajectory::OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno)
{
	return TrajectoryCheckReturnType::SkipGameCheck;
}

void ParabolaTrajectory::PrepareForOpenFire(BulletClass* pBullet)
{
	const AbstractClass* const pTarget = pBullet->Target;
	bool leadTimeCalculate = this->LeadTimeCalculate && pTarget;
	CoordStruct theTargetCoords = leadTimeCalculate ? pTarget->GetCoords() : pBullet->TargetCoords;
	CoordStruct theSourceCoords = leadTimeCalculate ? pBullet->Location : pBullet->SourceCoords;
	leadTimeCalculate &= theTargetCoords != this->LastTargetCoord;
	double rotateAngle = 0.0;

	if (!this->LeadTimeCalculate && theTargetCoords == theSourceCoords && pBullet->Owner) //For disperse.
	{
		const CoordStruct theOwnerCoords = pBullet->Owner->GetCoords();
		rotateAngle = Math::atan2(theTargetCoords.Y - theOwnerCoords.Y , theTargetCoords.X - theOwnerCoords.X);
	}
	else
	{
		rotateAngle = Math::atan2(theTargetCoords.Y - theSourceCoords.Y , theTargetCoords.X - theSourceCoords.X);
	}

	if (this->OffsetCoord != CoordStruct::Empty)
	{
		theTargetCoords.X += static_cast<int>(this->OffsetCoord.X * Math::cos(rotateAngle) + this->OffsetCoord.Y * Math::sin(rotateAngle));
		theTargetCoords.Y += static_cast<int>(this->OffsetCoord.X * Math::sin(rotateAngle) - this->OffsetCoord.Y * Math::cos(rotateAngle));
		theTargetCoords.Z += this->OffsetCoord.Z;
	}

	if (pBullet->Type->Inaccurate)
	{
		auto const pTypeExt = BulletTypeExt::ExtMap.Find(pBullet->Type);
		const double offsetMult = 0.0004 * theSourceCoords.DistanceFrom(theTargetCoords);
		const int offsetMin = static_cast<int>(offsetMult * pTypeExt->BallisticScatter_Min.Get(Leptons(0)));
		const int offsetMax = static_cast<int>(offsetMult * pTypeExt->BallisticScatter_Max.Get(Leptons(RulesClass::Instance->BallisticScatter)));
		const int offsetDistance = ScenarioClass::Instance->Random.RandomRanged(offsetMin, offsetMax);
		theTargetCoords = MapClass::GetRandomCoordsNear(theTargetCoords, offsetDistance, false);
	}

	pBullet->TargetCoords = theTargetCoords;
	const double gravity = BulletTypeExt::GetAdjustedGravity(pBullet->Type);

	if (gravity <= 0.0)
	{
		pBullet->Velocity = BulletVelocity::Empty;
		this->ShouldDetonate = true;
		return;
	}

	if (leadTimeCalculate)
		this->CalculateBulletVelocityLeadTime(pBullet, &theSourceCoords, gravity);
	else
		this->CalculateBulletVelocityRightNow(pBullet, &theSourceCoords, gravity);

	if (!this->UseDisperseBurst && this->RotateCoord != 0 && this->CountOfBurst > 1)
	{
		BulletVelocity rotationAxis
		{
			this->AxisOfRotation.X * Math::cos(rotateAngle) + this->AxisOfRotation.Y * Math::sin(rotateAngle),
			this->AxisOfRotation.X * Math::sin(rotateAngle) - this->AxisOfRotation.Y * Math::cos(rotateAngle),
			static_cast<double>(this->AxisOfRotation.Z)
		};

		const double rotationAxisLengthSquared = rotationAxis.MagnitudeSquared();

		if (rotationAxisLengthSquared != 0)
		{
			double extraRotate = 0;
			rotationAxis *= 1 / sqrt(rotationAxisLengthSquared);

			if (this->MirrorCoord)
			{
				if (pBullet->Owner && pBullet->Owner->CurrentBurstIndex % 2 == 1)
					rotationAxis *= -1;

				extraRotate = Math::Pi * (this->RotateCoord * ((this->CurrentBurst / 2) / (this->CountOfBurst - 1.0) - 0.5)) / 180;
			}
			else
			{
				extraRotate = Math::Pi * (this->RotateCoord * (this->CurrentBurst / (this->CountOfBurst - 1.0) - 0.5)) / 180;
			}

			const double cosRotate = Math::cos(extraRotate);
			pBullet->Velocity = (pBullet->Velocity * cosRotate) + (rotationAxis * ((1 - cosRotate) * (pBullet->Velocity * rotationAxis))) + (rotationAxis.CrossProduct(pBullet->Velocity) * Math::sin(extraRotate));
		}
	}
}

bool ParabolaTrajectory::BulletPrepareCheck(BulletClass* pBullet)
{
	if (this->WaitOneFrame.HasTimeLeft())
		return true;

	this->PrepareForOpenFire(pBullet);
	this->WaitOneFrame.Stop();

	return false;
}

void ParabolaTrajectory::CalculateBulletVelocityLeadTime(BulletClass* pBullet, CoordStruct* pSourceCoords, double gravity)
{
	CoordStruct targetCoords = pBullet->Target->GetCoords();
	CoordStruct offsetCoords = pBullet->TargetCoords - targetCoords;

	switch (this->OpenFireMode)
	{
	case 1: // Fixed max height and aim at the target
	{
		const double meetTime = this->SearchFixedHeightMeetTime(pSourceCoords, &targetCoords, &offsetCoords, gravity);
		const CoordStruct distanceCoords = pBullet->TargetCoords + (targetCoords - this->LastTargetCoord) * meetTime - *pSourceCoords;

		if (meetTime <= 0.0 || distanceCoords.Magnitude() <= 0.0)
			break;

		pBullet->Velocity.X = distanceCoords.X / meetTime;
		pBullet->Velocity.Y = distanceCoords.Y / meetTime;

		const int sourceHeight = pSourceCoords->Z, targetHeight = sourceHeight + distanceCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight)) + gravity / 2;
		return;
	}
	case 2: // Fixed fire angle and aim at the target
	{
		double radian = this->LaunchAngle * Math::Pi / 180.0;
		radian = (radian >= Math::HalfPi || radian <= -Math::HalfPi) ? (Math::HalfPi / 3) : radian;
		const double meetTime = this->SearchFixedAngleMeetTime(pSourceCoords, &targetCoords, &offsetCoords, radian, gravity);
		const CoordStruct distanceCoords = pBullet->TargetCoords + (targetCoords - this->LastTargetCoord) * meetTime - *pSourceCoords;

		if (meetTime <= 0.0 || distanceCoords.Magnitude() <= 0.0)
			break;

		pBullet->Velocity.X = distanceCoords.X / meetTime;
		pBullet->Velocity.Y = distanceCoords.Y / meetTime;

		const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();
		const double horizontalVelocity = horizontalDistance / meetTime;
		pBullet->Velocity.Z = horizontalVelocity * tan(radian) + gravity / 2;
		return;
	}
	case 3: // Fixed horizontal speed and fixed max height
	{
		double horizontalSpeed = this->GetTrajectorySpeed(pBullet);
		horizontalSpeed = horizontalSpeed > 256.0 ? 256.0 : horizontalSpeed;
		const double meetTime = this->SolveFixedSpeedMeetTime(pSourceCoords, &targetCoords, &offsetCoords, horizontalSpeed);
		const CoordStruct distanceCoords = pBullet->TargetCoords + (targetCoords - this->LastTargetCoord) * meetTime - *pSourceCoords;

		if (meetTime <= 0.0 || distanceCoords.Magnitude() <= 0.0)
			break;

		const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();
		const double mult = horizontalDistance > 0.0 ? horizontalSpeed / horizontalDistance : 1.0;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;

		const int sourceHeight = pSourceCoords->Z, targetHeight = sourceHeight + distanceCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight)) + gravity / 2;
		return;
	}
	case 4: // Fixed max height and fixed fire angle
	{
		const double meetTime = this->SearchFixedHeightMeetTime(pSourceCoords, &targetCoords, &offsetCoords, gravity);
		const CoordStruct distanceCoords = pBullet->TargetCoords + (targetCoords - this->LastTargetCoord) * meetTime - *pSourceCoords;

		if (meetTime <= 0.0 || distanceCoords.Magnitude() <= 0.0)
			break;

		const int sourceHeight = pSourceCoords->Z, targetHeight = sourceHeight + distanceCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight)) + gravity / 2;

		double radian = this->LaunchAngle * Math::Pi / 180.0;
		radian = (radian >= Math::HalfPi || radian <= 0.0) ? (Math::HalfPi / 3) : radian;
		const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();
		const double mult = (pBullet->Velocity.Z / tan(radian)) / horizontalDistance;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		return;
	}
	case 5: // Fixed horizontal speed and fixed fire angle
	{
		double horizontalSpeed = this->GetTrajectorySpeed(pBullet);
		horizontalSpeed = horizontalSpeed > 256.0 ? 256.0 : horizontalSpeed;
		const double meetTime = this->SolveFixedSpeedMeetTime(pSourceCoords, &targetCoords, &offsetCoords, horizontalSpeed);
		const CoordStruct distanceCoords = pBullet->TargetCoords + (targetCoords - this->LastTargetCoord) * meetTime - *pSourceCoords;

		if (meetTime <= 0.0 || distanceCoords.Magnitude() <= 0.0)
			break;

		const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();
		const double mult = horizontalDistance > 0.0 ? horizontalSpeed / horizontalDistance : 1.0;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		const double horizontalVelocity = horizontalDistance * mult;

		double radian = this->LaunchAngle * Math::Pi / 180.0;
		radian = (radian >= Math::HalfPi || radian <= -Math::HalfPi) ? (Math::HalfPi / 3) : radian;
		pBullet->Velocity.Z = horizontalVelocity * tan(radian) + gravity / 2;
		return;
	}
	default: // Fixed horizontal speed and aim at the target
	{
		double horizontalSpeed = this->GetTrajectorySpeed(pBullet);
		horizontalSpeed = horizontalSpeed > 256.0 ? 256.0 : horizontalSpeed;
		const double meetTime = this->SolveFixedSpeedMeetTime(pSourceCoords, &targetCoords, &offsetCoords, horizontalSpeed);
		const CoordStruct distanceCoords = pBullet->TargetCoords + (targetCoords - this->LastTargetCoord) * meetTime - *pSourceCoords;

		if (meetTime <= 0.0 || distanceCoords.Magnitude() <= 0.0)
			break;

		const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();
		const double mult = horizontalDistance > 0.0 ? horizontalSpeed / horizontalDistance : 1.0;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		pBullet->Velocity.Z = distanceCoords.Z * mult + (gravity * horizontalDistance) / (2 * horizontalSpeed) + gravity / 2;
		return;
	}
	}

	this->CalculateBulletVelocityRightNow(pBullet, pSourceCoords, gravity);
}

void ParabolaTrajectory::CalculateBulletVelocityRightNow(BulletClass* pBullet, CoordStruct* pSourceCoords, double gravity)
{
	const CoordStruct distanceCoords = pBullet->TargetCoords - *pSourceCoords;
	const double distance = distanceCoords.Magnitude();
	const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();

	if (distance <= 0.0)
	{
		pBullet->Velocity = BulletVelocity::Empty;
		this->ShouldDetonate = true;
		return;
	}

	switch (this->OpenFireMode)
	{
	case 1: // Fixed max height and aim at the target
	{
		const int sourceHeight = pSourceCoords->Z, targetHeight = pBullet->TargetCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight));

		const double meetTime = sqrt(2 * (maxHeight - sourceHeight) / gravity) + sqrt(2 * (maxHeight - targetHeight) / gravity);
		pBullet->Velocity.X = distanceCoords.X / meetTime;
		pBullet->Velocity.Y = distanceCoords.Y / meetTime;
		break;
	}
	case 2: // Fixed fire angle and aim at the target
	{
		double radian = this->LaunchAngle * Math::Pi / 180.0;
		double velocity = (radian >= Math::HalfPi || radian <= -Math::HalfPi) ? 100.0 : this->SearchVelocity(horizontalDistance, distanceCoords.Z, radian, gravity);
		pBullet->Velocity.Z = velocity * sin(radian);

		const double mult = velocity * cos(radian) / horizontalDistance;
		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		break;
	}
	case 3: // Fixed horizontal speed and fixed max height
	{
		const int sourceHeight = pSourceCoords->Z, targetHeight = pBullet->TargetCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight));

		double horizontalSpeed = this->GetTrajectorySpeed(pBullet);
		horizontalSpeed = horizontalSpeed > 256.0 ? 256.0 : horizontalSpeed;
		const double mult = horizontalDistance > 0.0 ? horizontalSpeed / horizontalDistance : 1.0;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		break;
	}
	case 4: // Fixed max height and fixed fire angle
	{
		const int sourceHeight = pSourceCoords->Z, targetHeight = pBullet->TargetCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight));

		double radian = this->LaunchAngle * Math::Pi / 180.0;
		radian = (radian >= Math::HalfPi || radian <= 0.0) ? (Math::HalfPi / 3) : radian;
		const double mult = (pBullet->Velocity.Z / tan(radian)) / horizontalDistance;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		break;
	}
	case 5: // Fixed horizontal speed and fixed fire angle
	{
		double horizontalSpeed = this->GetTrajectorySpeed(pBullet);
		horizontalSpeed = horizontalSpeed > 256.0 ? 256.0 : horizontalSpeed;
		const double mult = horizontalDistance > 0.0 ? horizontalSpeed / horizontalDistance : 1.0;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;

		double radian = this->LaunchAngle * Math::Pi / 180.0;
		radian = (radian >= Math::HalfPi || radian <= -Math::HalfPi) ? (Math::HalfPi / 3) : radian;
		pBullet->Velocity.Z = horizontalSpeed * tan(radian);
		break;
	}
	default: // Fixed horizontal speed and aim at the target
	{
		double horizontalSpeed = this->GetTrajectorySpeed(pBullet);
		horizontalSpeed = horizontalSpeed > 256.0 ? 256.0 : horizontalSpeed;
		const double mult = horizontalDistance > 0.0 ? horizontalSpeed / horizontalDistance : 1.0;

		pBullet->Velocity.X = distanceCoords.X * mult;
		pBullet->Velocity.Y = distanceCoords.Y * mult;
		pBullet->Velocity.Z = distanceCoords.Z * mult + (gravity * horizontalDistance) / (2 * horizontalSpeed);
		break;
	}
	}

	pBullet->Velocity.Z += gravity / 2; // Offset the gravity effect of the first time update
}

double ParabolaTrajectory::SearchVelocity(double horizontalDistance, int distanceCoordsZ, double radian, double gravity)
{
	const double mult = sin(2 * radian);
	double velocity = abs(mult) > 1e-10 ? sqrt(horizontalDistance * gravity / mult) : 0.0;
	velocity += distanceCoordsZ / gravity;
	velocity = velocity > 10.0 ? velocity : 10.0;
	const double delta = 1e-6;

	for (int i = 0; i < 10; ++i) // Newton iteration method
	{
		const double differential = this->CheckVelocityEquation(horizontalDistance, distanceCoordsZ, velocity, radian, gravity);
		const double dDifferential = (this->CheckVelocityEquation(horizontalDistance, distanceCoordsZ, (velocity + delta), radian, gravity) - differential) / delta;

		if (abs(dDifferential) < 1e-10) // Unacceptable divisor
			return velocity;

		const double difference = differential / dDifferential;
		const double velocityNew = velocity - difference;

		if (abs(difference) < 8.0) // Tolerable error
			return velocityNew;

		velocity = velocityNew;
	}

	return 10.0; // Unsolvable
}

double ParabolaTrajectory::CheckVelocityEquation(double horizontalDistance, int distanceCoordsZ, double velocity, double radian, double gravity)
{
	const double horizontalVelocity = velocity * cos(radian);
	const double verticalVelocity = velocity * sin(radian);

	const double upTime = verticalVelocity / gravity;
	const double maxHeight = 0.5 * verticalVelocity * upTime;
	const double downTime = sqrt(2 * (maxHeight - distanceCoordsZ) / gravity);
	const double wholeTime = horizontalDistance / horizontalVelocity;

	return wholeTime - (upTime + downTime);
}

double ParabolaTrajectory::SolveFixedSpeedMeetTime(CoordStruct* pSourceCrd, CoordStruct* pTargetCrd, CoordStruct* pOffsetCrd, double horizontalSpeed)
{
	const Point2D targetSpeedCrd { pTargetCrd->X - this->LastTargetCoord.X, pTargetCrd->Y - this->LastTargetCoord.Y };
	const Point2D destinationCrd { pTargetCrd->X + pOffsetCrd->X - pSourceCrd->X, pTargetCrd->Y + pOffsetCrd->Y - pSourceCrd->Y };
	const double targetSpeedSquared = targetSpeedCrd.MagnitudeSquared();
	const double factor = 2 * (targetSpeedCrd * destinationCrd) - horizontalSpeed;
	const double delta = factor * factor - 4 * targetSpeedSquared * destinationCrd.MagnitudeSquared();

	if (delta >= 0)
	{
		const double timeP = 0.5 * (-factor + sqrt(delta)) / targetSpeedSquared;
		const double timeM = 0.5 * (-factor - sqrt(delta)) / targetSpeedSquared;

		if (timeM > 0)
			return timeM;

		return timeP;
	}

	return -1.0;
}

double ParabolaTrajectory::SearchFixedHeightMeetTime(CoordStruct* pSourceCrd, CoordStruct* pTargetCrd, CoordStruct* pOffsetCrd, double gravity)
{
	const double delta = 1e-5;
	double meetTime = (this->ThrowHeight << 2) / gravity;

	for (int i = 0; i < 10; ++i)
	{
		const double differential = this->CheckFixedHeightEquation(pSourceCrd, pTargetCrd, pOffsetCrd, meetTime, gravity);
		const double dDifferential = (this->CheckFixedHeightEquation(pSourceCrd, pTargetCrd, pOffsetCrd, (meetTime + delta), gravity) - differential) / delta;

		if (abs(dDifferential) < 1e-10)
			return meetTime;

		const double difference = differential / dDifferential;
		const double meetTimeNew = meetTime - difference;

		if (abs(difference) < 1.0)
			return meetTimeNew;

		meetTime = meetTimeNew;
	}

	return -1.0;
}

double ParabolaTrajectory::CheckFixedHeightEquation(CoordStruct* pSourceCrd, CoordStruct* pTargetCrd, CoordStruct* pOffsetCrd, double meetTime, double gravity)
{
	const int meetHeight = static_cast<int>((pTargetCrd->Z - this->LastTargetCoord.Z) * meetTime) + pTargetCrd->Z + pOffsetCrd->Z;
	const int maxHeight = meetHeight > pSourceCrd->Z ? this->ThrowHeight + meetHeight : this->ThrowHeight + pSourceCrd->Z;
	return sqrt((maxHeight - pSourceCrd->Z) * 2 / gravity) + sqrt((maxHeight - meetHeight) * 2 / gravity) - meetTime;
}

double ParabolaTrajectory::SearchFixedAngleMeetTime(CoordStruct* pSourceCrd, CoordStruct* pTargetCrd, CoordStruct* pOffsetCrd, double radian, double gravity)
{
	const double delta = 1e-5;
	double meetTime = 512 * sin(radian) / gravity;

	for (int i = 0; i < 10; ++i)
	{
		const double differential = this->CheckFixedAngleEquation(pSourceCrd, pTargetCrd, pOffsetCrd, meetTime, radian, gravity);
		const double dDifferential = (this->CheckFixedAngleEquation(pSourceCrd, pTargetCrd, pOffsetCrd, (meetTime + delta), radian, gravity) - differential) / delta;

		if (abs(dDifferential) < 1e-10)
			return meetTime;

		const double difference = differential / dDifferential;
		const double meetTimeNew = meetTime - difference;

		if (abs(difference) < 1.0)
			return meetTimeNew;

		meetTime = meetTimeNew;
	}

	return -1.0;
}

double ParabolaTrajectory::CheckFixedAngleEquation(CoordStruct* pSourceCrd, CoordStruct* pTargetCrd, CoordStruct* pOffsetCrd, double meetTime, double radian, double gravity)
{
	const CoordStruct distanceCoords = (*pTargetCrd - this->LastTargetCoord) * meetTime + *pTargetCrd + *pOffsetCrd - *pSourceCrd;
	const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();

	const double horizontalVelocity = horizontalDistance / meetTime;
	const double verticalVelocity = horizontalVelocity * tan(radian);

	const double upTime = verticalVelocity / gravity;
	const double maxHeight = 0.5 * verticalVelocity * upTime;
	const double downTime = sqrt(2 * (maxHeight - distanceCoords.Z) / gravity);

	return upTime + downTime - meetTime;
}

BulletVelocity ParabolaTrajectory::GetGroundNormalVector(BulletClass* pBullet, CellClass* pCell)
{
	if (pCell->Tile_Is_Cliff() && (pBullet->Location.Z + 15) < pCell->GetCoords().Z)
	{
		const short reverseSgnX = pBullet->Velocity.X != 0.0 ? (pBullet->Velocity.X > 0.0 ? -1 : 1) : 0;
		const short reverseSgnY = pBullet->Velocity.Y != 0.0 ? (pBullet->Velocity.Y > 0.0 ? -1 : 1) : 0;
		CellStruct curCell = pCell->MapCoords;

		if (abs(pBullet->Velocity.X) > abs(pBullet->Velocity.Y))
		{
			curCell.X += reverseSgnX;

			if (this->CheckCellIsCliff(curCell))
			{
				curCell.X -= reverseSgnX;
				curCell.Y += reverseSgnY;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();

				curCell.X -= reverseSgnX;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(2 * reverseSgnY), 0.0 }.Normalized();

				return BulletVelocity{ 0.0, static_cast<double>(reverseSgnY), 0.0 };
			}

			curCell.Y -= reverseSgnY;

			if (this->CheckCellIsCliff(curCell))
			{
				curCell.Y += 2 * reverseSgnY;
				curCell.X -= reverseSgnX;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(2 * reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();

				curCell.X -= reverseSgnX;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();

				return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(2 * reverseSgnY), 0.0 }.Normalized();
			}

			curCell.Y += 2 * reverseSgnY;
			curCell.X -= reverseSgnX;

			if (this->CheckCellIsCliff(curCell))
				return BulletVelocity{ static_cast<double>(reverseSgnX), 0.0, 0.0 };

			curCell.X -= reverseSgnX;

			if (this->CheckCellIsCliff(curCell))
				return BulletVelocity{ static_cast<double>(2 * reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();
		}
		else
		{
			curCell.Y += reverseSgnY;

			if (this->CheckCellIsCliff(curCell))
			{
				curCell.Y -= reverseSgnY;
				curCell.X += reverseSgnX;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();

				curCell.Y -= reverseSgnY;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(2 * reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();

				return BulletVelocity{ static_cast<double>(reverseSgnX), 0.0, 0.0 };
			}

			curCell.X -= reverseSgnX;

			if (this->CheckCellIsCliff(curCell))
			{
				curCell.X += 2 * reverseSgnX;
				curCell.Y -= reverseSgnY;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(2 * reverseSgnY), 0.0 }.Normalized();

				curCell.Y -= reverseSgnY;

				if (this->CheckCellIsCliff(curCell))
					return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();

				return BulletVelocity{ static_cast<double>(2 * reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();
			}

			curCell.X += 2 * reverseSgnX;
			curCell.Y -= reverseSgnY;

			if (this->CheckCellIsCliff(curCell))
				return BulletVelocity{ 0.0, static_cast<double>(reverseSgnY), 0.0 };

			curCell.Y -= reverseSgnY;

			if (this->CheckCellIsCliff(curCell))
				return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(2 * reverseSgnY), 0.0 }.Normalized();
		}

		return BulletVelocity{ static_cast<double>(reverseSgnX), static_cast<double>(reverseSgnY), 0.0 }.Normalized();
	}

	const int subX = pBullet->Location.X & 0xFF;
	const int subY = pBullet->Location.Y & 0xFF;

	CoordStruct checkPoint { subX, subY, pCell->GetFloorHeight(Point2D{ subX, subY }) };
	CoordStruct surfaceX = checkPoint - CoordStruct{ subX + 13, subY, pCell->GetFloorHeight(Point2D{ subX + 13, subY }) };
	CoordStruct surfaceY = checkPoint - CoordStruct{ subX, subY + 13, pCell->GetFloorHeight(Point2D{ subX, subY + 13 }) };
	CoordStruct normalVector = surfaceY.CrossProduct(surfaceX).Normalized();

	return BulletVelocity{ static_cast<double>(normalVector.X), static_cast<double>(normalVector.Y), static_cast<double>(normalVector.Z) };
}

bool ParabolaTrajectory::CheckCellIsCliff(CellStruct cell)
{
	if (CellClass* const pCell = MapClass::Instance->TryGetCellAt(cell))
	{
		if (pCell->Tile_Is_Cliff())
			return true;
	}

	return false;
}
