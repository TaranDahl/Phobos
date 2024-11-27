#pragma once

#include "PhobosTrajectory.h"

class TracingTrajectoryType final : public PhobosTrajectoryType
{
public:
	TracingTrajectoryType() : PhobosTrajectoryType()
		, TheDuration { 0 }
		, NoDetonation { false }
		, CreateAtTarget { false }
		, TolerantTime { -1 }
		, WeaponType {}
		, WeaponCount { 0 }
		, WeaponDelay { 1 }
		, WeaponTimer { 0 }
	{ }

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual std::unique_ptr<PhobosTrajectory> CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;
	virtual TrajectoryFlag Flag() const override { return TrajectoryFlag::Tracing; }

	Valueable<int> TheDuration;
	Valueable<bool> NoDetonation;
	Valueable<bool> CreateAtTarget;
	Valueable<int> TolerantTime;
	Valueable<WeaponTypeClass*> WeaponType;
	Valueable<int> WeaponCount;
	Valueable<int> WeaponDelay;
	Valueable<int> WeaponTimer;

private:
	template <typename T>
	void Serialize(T& Stm);
};

class TracingTrajectory final : public PhobosTrajectory
{
public:
	TracingTrajectory(noinit_t) { }

	TracingTrajectory(TracingTrajectoryType const* trajType) : Type { trajType }
		, WeaponCount { trajType->WeaponCount }
		, ExistTimer {}
		, WeaponTimer {}
		, TolerantTimer {}
		, TechnoInTransport { false }
		, NotMainWeapon { false }
		, FLHCoord {}
		, BuildingCoord {}
		, FirepowerMult { 1.0 }
	{ }

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual TrajectoryFlag Flag() const override { return TrajectoryFlag::Tracing; }
	virtual void OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity) override;
	virtual bool OnAI(BulletClass* pBullet) override;
	virtual void OnAIPreDetonate(BulletClass* pBullet) override;
	virtual void OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition) override;
	virtual TrajectoryCheckReturnType OnAITargetCoordCheck(BulletClass* pBullet) override;
	virtual TrajectoryCheckReturnType OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno) override;

	const TracingTrajectoryType* Type;
	int WeaponCount;
	CDTimerClass ExistTimer;
	CDTimerClass WeaponTimer;
	CDTimerClass TolerantTimer;
	bool TechnoInTransport;
	bool NotMainWeapon;
	CoordStruct FLHCoord;
	CoordStruct BuildingCoord;
	double FirepowerMult;

private:
	template <typename T>
	void Serialize(T& Stm);

	void GetTechnoFLHCoord(BulletClass* pBullet, TechnoClass* pTechno);
	void InitializeDuration(BulletClass* pBullet, int duration);
	bool ChangeBulletTarget(BulletClass* pBullet);
	void ChangeVelocity(BulletClass* pBullet);
	void FireTracingWeapon(BulletClass* pBullet);
};
