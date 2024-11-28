#pragma once

#include "PhobosTrajectory.h"

enum class TraceTargetMode
{
	Connection = 0,
	Map = 1,
	Body = 2,
	Turret = 3,
	Rotate = 4,
};

class TracingTrajectoryType final : public PhobosTrajectoryType
{
public:
	TracingTrajectoryType() : PhobosTrajectoryType()
		, TraceMode { TraceTargetMode::Connection }
		, TheDuration { 0 }
		, TolerantTime { -1 }
		, AboveWeaponRange { true }
		, PeacefullyVanish { false }
		, TraceTheTarget { true }
		, CreateAtTarget { false }
		, CreateCoord { { 0, 0, 0 } }
		, OffsetCoord { { 0, 0, 0 } }
		, Weapons {}
		, WeaponCount {}
		, WeaponDelay {}
		, WeaponTimer { 0 }
		, WeaponCycle { -1 }
		, Synchronize { true }
	{ }

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual std::unique_ptr<PhobosTrajectory> CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;
	virtual TrajectoryFlag Flag() const override { return TrajectoryFlag::Tracing; }

	Valueable<TraceTargetMode> TraceMode;
	Valueable<int> TheDuration;
	Valueable<int> TolerantTime;
	Valueable<bool> AboveWeaponRange;
	Valueable<bool> PeacefullyVanish;
	Valueable<bool> TraceTheTarget;
	Valueable<bool> CreateAtTarget;
	Valueable<CoordStruct> CreateCoord;
	Valueable<CoordStruct> OffsetCoord;
	ValueableVector<WeaponTypeClass*> Weapons;
	ValueableVector<int> WeaponCount;
	ValueableVector<int> WeaponDelay;
	Valueable<int> WeaponTimer;
	Valueable<int> WeaponCycle;
	Valueable<bool> Synchronize;

private:
	template <typename T>
	void Serialize(T& Stm);
};

class TracingTrajectory final : public PhobosTrajectory
{
public:
	TracingTrajectory(noinit_t) { }

	TracingTrajectory(TracingTrajectoryType const* trajType) : Type { trajType }
		, WeaponIndex { 0 }
		, WeaponCount { 0 }
		, WeaponCycle { trajType->WeaponCycle }
		, ExistTimer {}
		, WeaponTimer {}
		, TolerantTimer {}
		, TechnoInTransport { false }
		, NotMainWeapon { false }
		, FLHCoord {}
		, BuildingCoord {}
		, FirepowerMult { 1.0 }
		, RotateRadian { 0.0 }
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
	int WeaponIndex;
	int WeaponCount;
	int WeaponCycle;
	CDTimerClass ExistTimer;
	CDTimerClass WeaponTimer;
	CDTimerClass TolerantTimer;
	bool TechnoInTransport;
	bool NotMainWeapon;
	CoordStruct FLHCoord;
	CoordStruct BuildingCoord;
	double FirepowerMult;
	double RotateRadian;

private:
	template <typename T>
	void Serialize(T& Stm);

	void GetTechnoFLHCoord(BulletClass* pBullet, TechnoClass* pTechno);
	void InitializeDuration(BulletClass* pBullet, int duration);
	bool InvalidFireCondition(TechnoClass* pTechno);
	bool BulletDetonatePreCheck(BulletClass* pBullet);
	void ChangeVelocity(BulletClass* pBullet);
	void FireTracingWeapon(BulletClass* pBullet);
};
