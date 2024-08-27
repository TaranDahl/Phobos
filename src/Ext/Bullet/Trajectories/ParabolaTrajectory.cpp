#include "ParabolaTrajectory.h"
#include <Ext/Bullet/Body.h>
#include <Ext/BulletType/Body.h>
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
		.Process(this->OffsetCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
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
		.Process(this->OffsetCoord)
		.Process(this->RotateCoord)
		.Process(this->MirrorCoord)
		.Process(this->UseDisperseBurst)
		.Process(this->AxisOfRotation)
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
	this->OffsetCoord = pType->OffsetCoord;
	this->RotateCoord = pType->RotateCoord;
	this->MirrorCoord = pType->MirrorCoord;
	this->UseDisperseBurst = pType->UseDisperseBurst;
	this->AxisOfRotation = pType->AxisOfRotation;
	this->PrepareForOpenFire(pBullet);
}

bool ParabolaTrajectory::OnAI(BulletClass* pBullet)
{
	if (this->DetonationDistance > 0 && pBullet->TargetCoords.DistanceFrom(pBullet->Location) < this->DetonationDistance)
		return true;

	if (MapClass::Instance->GetCellFloorHeight(pBullet->Location) >= pBullet->Location.Z)
		return true;

	pBullet->Velocity.Z -= BulletTypeExt::GetAdjustedGravity(pBullet->Type);

	if (CellClass* const pCell = MapClass::Instance->TryGetCellAt(pBullet->Location))
		return false;
	else
		return true;
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
	pSpeed->Z += BulletTypeExt::GetAdjustedGravity(pBullet->Type); // We don't want to take the gravity into account
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
	int countOfBurst = 0;
	int currentBurst = 0;

	if (WeaponTypeClass* const pWeapon = pBullet->WeaponType)
		countOfBurst = pWeapon->Burst;

	if (TechnoClass* const pOwner = pBullet->Owner)
	{
		currentBurst = pOwner->CurrentBurstIndex;

		if (this->MirrorCoord && pOwner->CurrentBurstIndex % 2 == 1)
			this->OffsetCoord.Y = -(this->OffsetCoord.Y);
	}

	double rotateAngle = 0.0;
	CoordStruct theTargetCoords = pBullet->TargetCoords;
	CoordStruct theSourceCoords = pBullet->SourceCoords;
	rotateAngle = Math::atan2(theTargetCoords.Y - theSourceCoords.Y , theTargetCoords.X - theSourceCoords.X);

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
	this->CalculateBulletVelocity(pBullet, &theSourceCoords);

	if (!this->UseDisperseBurst && this->RotateCoord != 0 && countOfBurst > 1)
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

				extraRotate = Math::Pi * (this->RotateCoord * ((currentBurst / 2) / (countOfBurst - 1.0) - 0.5)) / 180;
			}
			else
			{
				extraRotate = Math::Pi * (this->RotateCoord * (currentBurst / (countOfBurst - 1.0) - 0.5)) / 180;
			}

			const double cosRotate = Math::cos(extraRotate);
			pBullet->Velocity = (pBullet->Velocity * cosRotate) + (rotationAxis * ((1 - cosRotate) * (pBullet->Velocity * rotationAxis))) + (rotationAxis.CrossProduct(pBullet->Velocity) * Math::sin(extraRotate));
		}
	}
}

void ParabolaTrajectory::CalculateBulletVelocity(BulletClass* pBullet, CoordStruct* pSourceCoords)
{
	const CoordStruct distanceCoords = pBullet->TargetCoords - *pSourceCoords;
	const double distance = distanceCoords.Magnitude();
	const double horizontalDistance = Point2D{ distanceCoords.X, distanceCoords.Y }.Magnitude();
	const double gravity = BulletTypeExt::GetAdjustedGravity(pBullet->Type);

	if (distance <= 0.0 || gravity <= 0.0)
	{
		pBullet->Velocity = BulletVelocity::Empty;
		return;
	}

	switch (this->OpenFireMode)
	{
	case 1: // Fixed max height and aim at the target
	{
		const int sourceHeight = pSourceCoords->Z, targetHeight = pBullet->TargetCoords.Z;
		const int maxHeight = distanceCoords.Z > 0 ? this->ThrowHeight + targetHeight : this->ThrowHeight + sourceHeight;
		pBullet->Velocity.Z = sqrt(2 * gravity * (maxHeight - sourceHeight));

		const double mult = sqrt(2 * (maxHeight - sourceHeight) / gravity) + sqrt(2 * (maxHeight - targetHeight) / gravity);
		pBullet->Velocity.X = distanceCoords.X / mult;
		pBullet->Velocity.Y = distanceCoords.Y / mult;
		break;
	}
	case 2: // Fixed fire angle and aim at the target
	{
		double radian = this->LaunchAngle * Math::Pi / 180.0;
		double velocity = (radian >= Math::HalfPi || radian <= -Math::HalfPi) ? 100.0 : this->SearchVelocity(horizontalDistance, distanceCoords.Z, gravity, radian);
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
		radian = (radian >= Math::HalfPi || radian <= 0.0) ? 30.0 : radian;
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
		radian = (radian >= Math::HalfPi || radian <= -Math::HalfPi) ? 30.0 : radian;
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
}

double ParabolaTrajectory::CheckEquation(double horizontalDistance, int distanceCoordsZ, double velocity, double gravity, double radian)
{
	double horizontalVelocity = velocity * cos(radian);
	double verticalVelocity = velocity * sin(radian);

	double upTime = verticalVelocity / gravity;
	double maxHeight = verticalVelocity * upTime - 0.5 * gravity * upTime * upTime;
	double downTime = sqrt(2 * (maxHeight - distanceCoordsZ) / gravity);
	double wholeTime = horizontalDistance / horizontalVelocity;

	return wholeTime - (upTime + downTime);
}

double ParabolaTrajectory::SearchVelocity(double horizontalDistance, int distanceCoordsZ, double gravity, double radian)
{
	const double delta = 1.0;
	double velocity = 50.0;

	for (int i = 0; i < 8; ++i) // Newton iteration method
	{
		const double differential = this->CheckEquation(horizontalDistance, distanceCoordsZ, velocity, gravity, radian);
		const double dDifferential = (this->CheckEquation(horizontalDistance, distanceCoordsZ, (velocity + delta), gravity, radian) - differential) / delta;

		if (abs(dDifferential) < 1e-10) // Unacceptable divisor
			return velocity;

		const double difference = differential / dDifferential;
		const double velocityNew = velocity - difference;

		if (abs(difference) < 26.0) // Tolerable error
			return velocityNew;

		velocity = velocityNew;
	}

	return 100.0; // Unsolvable
}
