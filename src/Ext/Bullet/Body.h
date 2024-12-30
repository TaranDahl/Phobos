#pragma once
#include <BulletClass.h>

#include <Ext/BulletType/Body.h>
#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>
#include <New/Entity/LaserTrailClass.h>
#include "Trajectories/PhobosTrajectory.h"

class BulletExt
{
public:
	using base_type = BulletClass;

	static constexpr DWORD Canary = 0x2A2A2A2A;
	static constexpr size_t ExtPointerOffset = 0x18;

	class ExtData final : public Extension<BulletClass>
	{
	public:
		BulletTypeExt::ExtData* TypeExtData;
		HouseClass* FirerHouse;
		int CurrentStrength;
		bool IsInterceptor;
		InterceptedStatus InterceptedStatus;
		bool DetonateOnInterception;
		std::vector<LaserTrailClass> LaserTrails;
		bool SnappedToTarget; // Used for custom trajectory projectile target snap checks
		int DamageNumberOffset;

		TrajectoryPointer Trajectory;

		ExtData(BulletClass* OwnerObject) : Extension<BulletClass>(OwnerObject)
			, TypeExtData { nullptr }
			, FirerHouse { nullptr }
			, CurrentStrength { 0 }
			, IsInterceptor { false }
			, InterceptedStatus { InterceptedStatus::None }
			, DetonateOnInterception { true }
			, LaserTrails {}
			, Trajectory { nullptr }
			, SnappedToTarget { false }
			, DamageNumberOffset { INT32_MIN }
		{ }

		virtual ~ExtData() = default;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

		void InterceptBullet(TechnoClass* pSource, WeaponTypeClass* pWeapon);
		void ApplyRadiationToCell(CellStruct Cell, int Spread, int RadLevel);
		void InitializeLaserTrails();

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<BulletExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;

	static inline void SimulatedFiringInfos(BulletClass* pBullet, WeaponTypeClass* pWeapon, HouseClass* pHouse, int projectileRange);
	static inline void SimulatedFiringVelocity(BulletClass* pBullet, const CoordStruct& sourceCoords, bool randomVelocity);
	static inline void SimulatedFiringAnim(BulletClass* pBullet, WeaponTypeClass* pWeapon, HouseClass* pHouse, bool trajectory, bool attach);
	static inline void SimulatedFiringReport(BulletClass* pBullet, WeaponTypeClass* pWeapon);
	static inline void SimulatedFiringLaser(BulletClass* pBullet, WeaponTypeClass* pWeapon, HouseClass* pHouse);
	static inline void SimulatedFiringElectricBolt(BulletClass* pBullet, WeaponTypeClass* pWeapon);
	static inline void SimulatedFiringRadBeam(BulletClass* pBullet, WeaponTypeClass* pWeapon, HouseClass* pHouse);
	static inline void SimulatedFiringParticleSystem(BulletClass* pBullet, WeaponTypeClass* pWeapon, HouseClass* pHouse);
};
