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
};

class TracingTrajectory final : public PhobosTrajectory
{
public:
	TracingTrajectory() : PhobosTrajectory(TrajectoryFlag::Tracing)
		, TheDuration { 0 }
		, ExistTimer {}
	{}

	TracingTrajectory(PhobosTrajectoryType const* pType) : PhobosTrajectory(TrajectoryFlag::Tracing)
		, TheDuration { 0 }
		, ExistTimer {}
	{
		auto const pFinalType = static_cast<const TracingTrajectoryType*>(pType);

		this->TheDuration = pFinalType->TheDuration;
	}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;

	virtual void OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity) override;
	virtual bool OnAI(BulletClass* pBullet) override;
	virtual void OnAIPreDetonate(BulletClass* pBullet) override;
	virtual void OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition) override;
	virtual TrajectoryCheckReturnType OnAITargetCoordCheck(BulletClass* pBullet) override;
	virtual TrajectoryCheckReturnType OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno) override;

	int TheDuration;
	CDTimerClass ExistTimer;

private:

};
