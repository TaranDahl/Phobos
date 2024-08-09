#pragma once

#include "PhobosTrajectory.h"

class ParabolaTrajectoryType final : public PhobosTrajectoryType
{
public:
	ParabolaTrajectoryType() : PhobosTrajectoryType(TrajectoryFlag::Parabola)
		, DetonationDistance { Leptons(102) }
		, TargetSnapDistance { Leptons(128) }
		, OffsetCoord { { 0, 0, 0 } }
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, UseDisperseBurst { false }
		, LeadTimeCalculate { true }
		, ThrowHeight { 0 }
		, LaunchAngle { 0 }
		, AxisOfRotation { { 0, 0, 0 } }
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual PhobosTrajectory* CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;

	Valueable<Leptons> DetonationDistance;
	Valueable<Leptons> TargetSnapDistance;
	Valueable<CoordStruct> OffsetCoord;
	Valueable<int> RotateCoord;
	Valueable<bool> MirrorCoord;
	Valueable<bool> UseDisperseBurst;
	Valueable<bool> LeadTimeCalculate;
	Valueable<int> ThrowHeight;
	Valueable<int> LaunchAngle;
	Valueable<CoordStruct> AxisOfRotation;
};

class ParabolaTrajectory final : public PhobosTrajectory
{
public:
	ParabolaTrajectory() : PhobosTrajectory(TrajectoryFlag::Parabola)
		, DetonationDistance { Leptons(102) }
		, TargetSnapDistance { Leptons(128) }
		, OffsetCoord {}
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, UseDisperseBurst { false }
		, LeadTimeCalculate { true }
		, ThrowHeight { 0 }
		, LaunchAngle { 0 }
		, AxisOfRotation {}
	{}

	ParabolaTrajectory(PhobosTrajectoryType const* pType) : PhobosTrajectory(TrajectoryFlag::Parabola)
		, DetonationDistance { Leptons(102) }
		, TargetSnapDistance { Leptons(128) }
		, OffsetCoord {}
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, UseDisperseBurst { false }
		, LeadTimeCalculate { true }
		, ThrowHeight { 0 }
		, LaunchAngle { 0 }
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
	CoordStruct OffsetCoord;
	int RotateCoord;
	bool MirrorCoord;
	bool UseDisperseBurst;
	bool LeadTimeCalculate;
	int ThrowHeight;
	int LaunchAngle;
	CoordStruct AxisOfRotation;

private:

};
