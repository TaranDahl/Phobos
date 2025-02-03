#pragma once

#include "PhobosTrajectory.h"
#include <Ext/WeaponType/Body.h>

class DisperseTrajectoryType final : public PhobosTrajectoryType
{
public:
	DisperseTrajectoryType() : PhobosTrajectoryType()
		, UniqueCurve { false }
		, PreAimCoord { { 0, 0, 0 } }
		, RotateCoord { 0 }
		, MirrorCoord { true }
		, FacingCoord { false }
		, ReduceCoord { true }
		, UseDisperseBurst { false }
		, AxisOfRotation { { 0, 0, 1 } }
		, LaunchSpeed { 0 }
		, Acceleration { 10.0 }
		, ROT { 30.0 }
		, LockDirection { false }
		, CruiseEnable { false }
		, CruiseUnableRange { Leptons(1280) }
		, CruiseAltitude { 800 }
		, CruiseAlongLevel { false }
		, LeadTimeCalculate { true }
		, RecordSourceCoord { false }
		, RetargetAllies { false }
		, RetargetRadius { 0 }
		, TargetSnapDistance { Leptons(128) }
		, SuicideAboveRange { 0 }
		, SuicideShortOfROT { true }
		, SuicideIfNoWeapon { true }
		, Weapons {}
		, WeaponBurst {}
		, WeaponCount { 0 }
		, WeaponDelay { 1 }
		, WeaponInitialDelay { 0 }
		, WeaponEffectiveRange { Leptons(0) }
		, WeaponSeparate { false }
		, WeaponRetarget { false }
		, WeaponLocation { false }
		, WeaponTendency { false }
		, WeaponHolistic { false }
		, WeaponMarginal { false }
		, WeaponToAllies { false }
		, WeaponDoRepeat { false }
	{ }

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual std::unique_ptr<PhobosTrajectory> CreateInstance() const override;
	virtual void Read(CCINIClass* const pINI, const char* pSection) override;
	virtual TrajectoryFlag Flag() const override { return TrajectoryFlag::Disperse; }

	Valueable<bool> UniqueCurve;
	Valueable<CoordStruct> PreAimCoord;
	Valueable<double> RotateCoord;
	Valueable<bool> MirrorCoord;
	Valueable<bool> FacingCoord;
	Valueable<bool> ReduceCoord;
	Valueable<bool> UseDisperseBurst;
	Valueable<CoordStruct> AxisOfRotation;
	Valueable<double> LaunchSpeed;
	Valueable<double> Acceleration;
	Valueable<double> ROT;
	Valueable<bool> LockDirection;
	Valueable<bool> CruiseEnable;
	Valueable<Leptons> CruiseUnableRange;
	Valueable<int> CruiseAltitude;
	Valueable<bool> CruiseAlongLevel;
	Valueable<bool> LeadTimeCalculate;
	Valueable<bool> RecordSourceCoord;
	Valueable<bool> RetargetAllies;
	Valueable<double> RetargetRadius;
	Valueable<Leptons> TargetSnapDistance;
	Valueable<double> SuicideAboveRange;
	Valueable<bool> SuicideShortOfROT;
	Valueable<bool> SuicideIfNoWeapon;
	ValueableVector<WeaponTypeClass*> Weapons;
	ValueableVector<int> WeaponBurst;
	Valueable<int> WeaponCount;
	Valueable<int> WeaponDelay;
	Valueable<int> WeaponInitialDelay;
	Valueable<Leptons> WeaponEffectiveRange;
	Valueable<bool> WeaponSeparate;
	Valueable<bool> WeaponRetarget;
	Valueable<bool> WeaponLocation;
	Valueable<bool> WeaponTendency;
	Valueable<bool> WeaponHolistic;
	Valueable<bool> WeaponMarginal;
	Valueable<bool> WeaponToAllies;
	Valueable<bool> WeaponDoRepeat;

private:
	template <typename T>
	void Serialize(T& Stm);
};

class DisperseTrajectory final : public PhobosTrajectory
{
public:
	DisperseTrajectory(noinit_t) { }

	DisperseTrajectory(DisperseTrajectoryType const* trajType) : Type { trajType }
		, Speed { trajType->LaunchSpeed }
		, PreAimCoord { trajType->PreAimCoord.Get() }
		, UseDisperseBurst { trajType->UseDisperseBurst }
		, CruiseEnable { trajType->CruiseEnable }
		, SuicideAboveRange { trajType->SuicideAboveRange * Unsorted::LeptonsPerCell }
		, WeaponCount { trajType->WeaponCount }
		, WeaponTimer {}
		, InStraight { false }
		, Accelerate { true }
		, TargetInTheAir { false }
		, TargetIsTechno { false }
		, OriginalDistance { 0 }
		, CurrentBurst { 0 }
		, ThisWeaponIndex { 0 }
		, LastTargetCoord {}
		, PreAimDistance { 0 }
		, LastReviseMult { 0 }
		, FLHCoord {}
		, BuildingCoord {}
		, FirepowerMult { 1.0 }
	{ }

	virtual bool Load(PhobosStreamReader& Stm, bool RegisterForChange) override;
	virtual bool Save(PhobosStreamWriter& Stm) const override;
	virtual TrajectoryFlag Flag() const override { return TrajectoryFlag::Disperse; }
	virtual void OnUnlimbo(BulletClass* pBullet, CoordStruct* pCoord, BulletVelocity* pVelocity) override;
	virtual bool OnAI(BulletClass* pBullet) override;
	virtual void OnAIPreDetonate(BulletClass* pBullet) override;
	virtual void OnAIVelocity(BulletClass* pBullet, BulletVelocity* pSpeed, BulletVelocity* pPosition) override;
	virtual TrajectoryCheckReturnType OnAITargetCoordCheck(BulletClass* pBullet) override;
	virtual TrajectoryCheckReturnType OnAITechnoCheck(BulletClass* pBullet, TechnoClass* pTechno) override;

	const DisperseTrajectoryType* Type;
	double Speed;
	CoordStruct PreAimCoord;
	bool UseDisperseBurst;
	bool CruiseEnable;
	double SuicideAboveRange;
	int WeaponCount;
	CDTimerClass WeaponTimer;
	bool InStraight;
	bool Accelerate;
	bool TargetInTheAir;
	bool TargetIsTechno;
	int OriginalDistance;
	int CurrentBurst;
	int ThisWeaponIndex;
	CoordStruct LastTargetCoord;
	double PreAimDistance;
	double LastReviseMult;
	CoordStruct FLHCoord;
	CoordStruct BuildingCoord;
	double FirepowerMult;

private:
	template <typename T>
	void Serialize(T& Stm);

	void GetTechnoFLHCoord(BulletClass* pBullet, TechnoClass* pTechno);
	void InitializeBulletNotCurve(BulletClass* pBullet, bool facing);
	inline BulletVelocity RotateAboutTheAxis(BulletVelocity theSpeed, BulletVelocity theAxis, double theRadian);
	bool CalculateBulletVelocity(BulletClass* pBullet, double trajectorySpeed);
	bool BulletRetargetTechno(BulletClass* pBullet);
	inline bool CheckTechnoIsInvalid(TechnoClass* pTechno);
	inline bool CheckWeaponCanTarget(WeaponTypeExt::ExtData* pWeaponExt, TechnoClass* pFirer, TechnoClass* pTarget);
	bool CurveVelocityChange(BulletClass* pBullet);
	bool NotCurveVelocityChange(BulletClass* pBullet);
	bool StandardVelocityChange(BulletClass* pBullet);
	bool ChangeBulletVelocity(BulletClass* pBullet, CoordStruct targetLocation, double turningRadius, bool curve);
	bool PrepareDisperseWeapon(BulletClass* pBullet);
	void CreateDisperseBullets(BulletClass* pBullet, WeaponTypeClass* pWeapon, AbstractClass* pTarget, HouseClass* pOwner, int curBurst, int maxBurst);
	void DisperseBurstSubstitution(BulletClass* pBullet, CoordStruct axis, double rotateCoord, int curBurst, int maxBurst, bool mirror);
};
