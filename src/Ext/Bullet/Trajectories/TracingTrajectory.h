#pragma once

#include "PhobosTrajectory.h"

class TracingTrajectoryType final : public PhobosTrajectoryType
{
public:
	TracingTrajectoryType() : PhobosTrajectoryType(TrajectoryFlag::Tracing)
		, TheDuration { 0 }
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual PhobosTrajectory* CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;

	Valueable<int> TheDuration;

private:
	template <typename T>
	void Serialize(T& Stm);
};

class TracingTrajectory final : public PhobosTrajectory
{
public:
	TracingTrajectory(noinit_t) :PhobosTrajectory { noinit_t{} } { }

	TracingTrajectory(TracingTrajectoryType const* trajType) : PhobosTrajectory(TrajectoryFlag::Tracing, trajType->Trajectory_Speed)
		, Type { trajType }
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

	const TracingTrajectoryType* Type;
	CDTimerClass ExistTimer;

private:
	template <typename T>
	void Serialize(T& Stm);
};
