#pragma once

#include "PhobosTrajectory.h"

class ParabolaTrajectoryType final : public PhobosTrajectoryType
{
public:
	ParabolaTrajectoryType() : PhobosTrajectoryType(TrajectoryFlag::Parabola)
		, DetonationDistance { Leptons(102) }
		, TargetSnapDistance { Leptons(128) }
		, OpenFireMode { 0 }
		, ThrowHeight { 600 }
		, LaunchAngle { 30.0 }
		, OffsetCoord { { 0, 0, 0 } }
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, UseDisperseBurst { false }
		, AxisOfRotation { { 0, 0, 0 } }
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual PhobosTrajectory* CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;

	Valueable<Leptons> DetonationDistance;
	Valueable<Leptons> TargetSnapDistance;
	Valueable<int> OpenFireMode;
	Valueable<int> ThrowHeight;
	Valueable<double> LaunchAngle;
	Valueable<CoordStruct> OffsetCoord;
	Valueable<int> RotateCoord;
	Valueable<bool> MirrorCoord;
	Valueable<bool> UseDisperseBurst;
	Valueable<CoordStruct> AxisOfRotation;
};

class ParabolaTrajectory final : public PhobosTrajectory
{
public:
	ParabolaTrajectory() : PhobosTrajectory(TrajectoryFlag::Parabola)
		, DetonationDistance { Leptons(102) }
		, TargetSnapDistance { Leptons(128) }
		, OpenFireMode { 0 }
		, ThrowHeight { 600 }
		, LaunchAngle { 30.0 }
		, OffsetCoord {}
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, UseDisperseBurst { false }
		, AxisOfRotation {}
	{}

	ParabolaTrajectory(PhobosTrajectoryType const* pType) : PhobosTrajectory(TrajectoryFlag::Parabola)
		, DetonationDistance { Leptons(102) }
		, TargetSnapDistance { Leptons(128) }
		, OpenFireMode { 0 }
		, ThrowHeight { 600 }
		, LaunchAngle { 30.0 }
		, OffsetCoord {}
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, UseDisperseBurst { false }
		, AxisOfRotation {}
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;

	virtual void OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity) override;
	virtual bool OnAI(BulletClass* pBullet) override;
	virtual void OnAIPreDetonate(BulletClass* pBullet) override;
	virtual void OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition) override;
	virtual TrajectoryCheckReturnType OnAITargetCoordCheck(BulletClass* pBullet) override;
	virtual TrajectoryCheckReturnType OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno) override;

	Leptons DetonationDistance;
	Leptons TargetSnapDistance;
	int OpenFireMode;
	int ThrowHeight;
	double LaunchAngle;
	CoordStruct OffsetCoord;
	int RotateCoord;
	bool MirrorCoord;
	bool UseDisperseBurst;
	CoordStruct AxisOfRotation;

private:
	void PrepareForOpenFire(BulletClass* pBullet);
	void CalculateBulletVelocity(BulletClass* pBullet, CoordStruct* pSourceCoords);
	double CheckEquation(double horizontalDistance, int distanceCoordsZ, double velocity, double gravity, double radian);
	double SearchVelocity(double horizontalDistance, int distanceCoordsZ, double gravity, double radian);
};
