#pragma once

#include "PhobosTrajectory.h"

class TracingTrajectoryType final : public PhobosTrajectoryType
{
public:
	TracingTrajectoryType() : PhobosTrajectoryType(TrajectoryFlag::Tracing)
		, TheDuration { 0 }
		, RelockDelay { 20 }
		, RelockRange { 5.0 }
		, AddedRange { 5.0 }
		, NoRelockROF { 0 }
		, IsLaser { true }
		, TargetLaser { true }
		, FinishLaser { true }
		, IsHouseColor { false }
		, LaserColor { { 0, 0, 0 } }
		, BlazeDelay { 20 }
		, DamageDelay { 5 }
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual PhobosTrajectory* CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;

	Valueable<int> TheDuration;
	Valueable<int> RelockDelay;
	Valueable<double> RelockRange;
	Valueable<double> AddedRange;
	Valueable<int> NoRelockROF;
	Valueable<bool> IsLaser;
	Valueable<bool> TargetLaser;
	Valueable<bool> FinishLaser;
	Valueable<bool> IsHouseColor;
	Valueable<ColorStruct> LaserColor;
	Valueable<int> BlazeDelay;
	Valueable<int> DamageDelay;
};

class TracingTrajectory final : public PhobosTrajectory
{
public:
	TracingTrajectory() : PhobosTrajectory(TrajectoryFlag::Tracing)
		, RelockDelay { 20 }
		, RelockRange { 5.0 }
		, AddedRange { 5.0 }
		, NoRelockROF { 0 }
		, IsLaser { true }
		, TargetLaser { true }
		, FinishLaser { true }
		, IsHouseColor { false }
		, LaserColor {}
		, BlazeDelay { 20 }
		, DamageDelay { 5 }
		, RelockTimer {}
		, BlazeTimer {}
		, DamageTimer {}
		, ExistTimer {}
	{}

	TracingTrajectory(PhobosTrajectoryType const* pType) : PhobosTrajectory(TrajectoryFlag::Tracing)
		, RelockDelay { 20 }
		, RelockRange { 5.0 }
		, AddedRange { 5.0 }
		, NoRelockROF { 0 }
		, IsLaser { true }
		, TargetLaser { true }
		, FinishLaser { true }
		, IsHouseColor { false }
		, LaserColor {}
		, BlazeDelay { 20 }
		, DamageDelay { 5 }
		, RelockTimer {}
		, BlazeTimer {}
		, DamageTimer {}
		, ExistTimer {}
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;

	virtual void OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity) override;
	virtual bool OnAI(BulletClass* pBullet) override;
	virtual void OnAIPreDetonate(BulletClass* pBullet) override;
	virtual void OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition) override;
	virtual TrajectoryCheckReturnType OnAITargetCoordCheck(BulletClass* pBullet) override;
	virtual TrajectoryCheckReturnType OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno) override;

	int RelockDelay;
	double RelockRange;
	double AddedRange;
	int NoRelockROF;
	bool IsLaser;
	bool TargetLaser;
	bool FinishLaser;
	bool IsHouseColor;
	ColorStruct LaserColor;
	int BlazeDelay;
	int DamageDelay;
	CDTimerClass RelockTimer;
	CDTimerClass BlazeTimer;
	CDTimerClass DamageTimer;
	CDTimerClass ExistTimer;

private:

};
