#pragma once

#include "PhobosTrajectory.h"

class BombardTrajectoryType final : public PhobosTrajectoryType
{
public:
	BombardTrajectoryType() : PhobosTrajectoryType(TrajectoryFlag::Bombard)
		, Height { 0.0 }
		, FallPercent { 1.0 }
		, FallPercentShift { 0.0 }
		, FallScatterRange { Leptons(0) }
		, FallSpeed { 0.0 }
		, TargetSnapDistance { Leptons(128) }
		, FreeFallOnTarget { true }
		, NoLaunch { false }
		, TurningPointAnim {}
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual PhobosTrajectory* CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;

	Valueable<double> Height;
	Valueable<double> FallPercent;
	Valueable<double> FallPercentShift;
	Valueable<Leptons> FallScatterRange;
	Valueable<double> FallSpeed;
	Valueable<Leptons> TargetSnapDistance;
	Valueable<bool> FreeFallOnTarget;
	Valueable<bool> NoLaunch;
	Nullable<AnimTypeClass*> TurningPointAnim;

private:
	template <typename T>
	void Serialize(T& Stm);
};

class BombardTrajectory final : public PhobosTrajectory
{
public:
	BombardTrajectory(noinit_t) :PhobosTrajectory { noinit_t{} } { }

	BombardTrajectory(PhobosTrajectoryType const* pType) : PhobosTrajectory(TrajectoryFlag::Bombard)
		, IsFalling { false }
		, RemainingDistance { 1 }
		, Height { 0.0 }
		, FallPercent { 1.0 }
		, FallPercentShift { 0.0 }
		, FallScatterRange { Leptons(0) }
		, FallSpeed { 0.0 }
		, TargetSnapDistance { Leptons(128) }
		, FreeFallOnTarget { true }
		, NoLaunch { false }
		, TurningPointAnim {}
	{}

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;

	virtual void OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity) override;
	virtual bool OnAI(BulletClass* pBullet) override;
	virtual void OnAIPreDetonate(BulletClass* pBullet) override;
	virtual void OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition) override;
	virtual TrajectoryCheckReturnType OnAITargetCoordCheck(BulletClass* pBullet) override;
	virtual TrajectoryCheckReturnType OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno) override;

	bool IsFalling;
	int RemainingDistance;
	double Height;
	double FallPercent;
	double FallPercentShift;
	Leptons FallScatterRange;
	double FallSpeed;
	Leptons TargetSnapDistance;
	bool FreeFallOnTarget;
	bool NoLaunch;
	AnimTypeClass* TurningPointAnim;

private:
	template <typename T>
	void Serialize(T& Stm);

	void ApplyTurningPointAnim(BulletClass* pBullet, CoordStruct Position);
	bool BulletDetonatePreCheck(BulletClass* pBullet, HouseClass* pOwner, double StraightSpeed);
};
