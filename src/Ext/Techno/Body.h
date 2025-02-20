#pragma once
#include <InfantryClass.h>
#include <AnimClass.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>
#include <Utilities/Macro.h>
#include <New/Entity/ShieldClass.h>
#include <New/Entity/LaserTrailClass.h>
#include <New/Entity/AttachEffectClass.h>

class BulletClass;

class TechnoExt
{
public:
	using base_type = TechnoClass;

	static constexpr DWORD Canary = 0x55555555;
	static constexpr size_t ExtPointerOffset = 0x34C;

	class ExtData final : public Extension<TechnoClass>
	{
	public:
		TechnoTypeExt::ExtData* TypeExtData;
		std::unique_ptr<ShieldClass> Shield;
		std::vector<LaserTrailClass> LaserTrails;
		std::vector<std::unique_ptr<AttachEffectClass>> AttachedEffects;
		AttachEffectTechnoProperties AE;
		bool SubterraneanHarvFreshFromFactory;
		AbstractClass* SubterraneanHarvRallyDest;
		bool ReceiveDamage;
		bool LastKillWasTeamTarget;
		CDTimerClass PassengerDeletionTimer;
		ShieldTypeClass* CurrentShieldType;
		int LastWarpDistance;
		CDTimerClass ChargeTurretTimer; // Used for charge turrets instead of RearmTimer if weapon has ChargeTurret.Delays set.
		CDTimerClass AutoDeathTimer;
		AnimTypeClass* MindControlRingAnimType;
		int DamageNumberOffset;
		int Strafe_BombsDroppedThisRound;
		int CurrentAircraftWeaponIndex;
		bool IsInTunnel;
		bool IsBurrowed;
		bool HasBeenPlacedOnMap; // Set to true on first Unlimbo() call.
		CDTimerClass DeployFireTimer;
		bool SkipTargetChangeResetSequence;
		bool ForceFullRearmDelay;
		bool LastRearmWasFullDelay;
		bool CanCloakDuringRearm; // Current rearm timer was started by DecloakToFire=no weapon.
		int WHAnimRemainingCreationInterval;
		bool UnitIdleIsSelected;
		CDTimerClass UnitIdleActionTimer;
		CDTimerClass UnitIdleActionGapTimer;
		CDTimerClass UnitAutoDeployTimer;
		WeaponTypeClass* LastWeaponType;
		CoordStruct LastWeaponFLH;
		int LastHurtFrame;
		int BeControlledThreatFrame;
		DWORD LastTargetID;
		int AccumulatedGattlingValue;
		bool ShouldUpdateGattlingValue;
		int ScatteringStopFrame;
		int MyTargetingFrame;
		int AttackMoveFollowerTempCount;
		CellClass* AutoTargetedWallCell;
		bool HasCachedClickMission;
		Mission CachedMission;
		AbstractClass* CachedCell;
		AbstractClass* CachedTarget;
		bool HasCachedClickEvent;
		EventType CachedEventType;
		CellClass* FiringObstacleCell; // Set on firing if there is an obstacle cell between target and techno, used for updating WaveClass target etc.
		bool IsDetachingForCloak; // Used for checking animation detaching, set to true before calling Detach_All() on techno when this anim is attached to and to false after when cloaking only.

		// Used for Passengers.SyncOwner.RevertOnExit instead of TechnoClass::InitialOwner / OriginallyOwnedByHouse,
		// as neither is guaranteed to point to the house the TechnoClass had prior to entering transport and cannot be safely overridden.
		HouseClass* OriginalPassengerOwner;
		bool HasRemainingWarpInDelay;          // Converted from object with Teleport Locomotor to one with a different Locomotor while still phasing in OR set if ChronoSphereDelay > 0.
		int LastWarpInDelay;                   // Last-warp in delay for this unit, used by HasCarryoverWarpInDelay.
		bool IsBeingChronoSphered;             // Set to true on units currently being ChronoSphered, does not apply to Ares-ChronoSphere'd buildings or Chrono reinforcements.
		bool KeepTargetOnMove;

		bool AggressiveStance;                  // Aggressive stance that will auto target buildings

		ExtData(TechnoClass* OwnerObject) : Extension<TechnoClass>(OwnerObject)
			, TypeExtData { nullptr }
			, Shield {}
			, LaserTrails {}
			, AttachedEffects {}
			, AE {}
			, SubterraneanHarvFreshFromFactory { false }
			, SubterraneanHarvRallyDest { nullptr }
			, ReceiveDamage { false }
			, LastKillWasTeamTarget { false }
			, PassengerDeletionTimer {}
			, CurrentShieldType { nullptr }
			, LastWarpDistance {}
			, ChargeTurretTimer {}
			, AutoDeathTimer {}
			, MindControlRingAnimType { nullptr }
			, DamageNumberOffset { INT32_MIN }
			, Strafe_BombsDroppedThisRound { 0 }
			, CurrentAircraftWeaponIndex {}
			, IsInTunnel { false }
			, IsBurrowed { false }
			, HasBeenPlacedOnMap { false }
			, DeployFireTimer {}
			, SkipTargetChangeResetSequence { false }
			, ForceFullRearmDelay { false }
			, LastRearmWasFullDelay { false }
			, CanCloakDuringRearm { false }
			, WHAnimRemainingCreationInterval { 0 }
			, UnitIdleIsSelected { false }
			, UnitIdleActionTimer {}
			, UnitIdleActionGapTimer {}
			, UnitAutoDeployTimer {}
			, LastWeaponType {}
			, LastWeaponFLH {}
			, LastHurtFrame { 0 }
			, BeControlledThreatFrame { 0 }
			, LastTargetID { 0xFFFFFFFF }
			, AccumulatedGattlingValue { 0 }
			, ShouldUpdateGattlingValue { false }
			, ScatteringStopFrame { 0 }
			, MyTargetingFrame { ScenarioClass::Instance->Random.RandomRanged(0,15) }
			, AttackMoveFollowerTempCount { 0 }
			, AutoTargetedWallCell{ nullptr }
			, HasCachedClickMission { false }
			, CachedMission { Mission::None }
			, CachedCell { nullptr }
			, CachedTarget { nullptr }
			, HasCachedClickEvent { false }
			, CachedEventType { EventType::LAST_EVENT }
			, FiringObstacleCell {}
			, IsDetachingForCloak { false }
			, OriginalPassengerOwner {}
			, HasRemainingWarpInDelay { false }
			, LastWarpInDelay { 0 }
			, IsBeingChronoSphered { false }
			, AggressiveStance { false }
			, KeepTargetOnMove { false }
		{ }

		void OnEarlyUpdate();

		void ApplyInterceptor();
		bool CheckDeathConditions(bool isInLimbo = false);
		void DepletedAmmoActions();
		void EatPassengers();
		void UpdateShield();
		void UpdateOnTunnelEnter();
		void ApplySpawnLimitRange();
		void UpdateTypeData(TechnoTypeClass* currentType);
		void UpdateLaserTrails();
		void UpdateAttachEffects();
		void UpdateGattlingRateDownReset();
		void UpdateCumulativeAttachEffects(AttachEffectTypeClass* pAttachEffectType, AttachEffectClass* pRemoved = nullptr);
		void RecalculateStatMultipliers();
		void UpdateTemporal();
		void UpdateMindControlAnim();
		void UpdateRecountBurst();
		void UpdateRearmInEMPState();
		void UpdateRearmInTemporal();
		void InitializeLaserTrails();
		void InitializeAttachEffects();
		void UpdateSelfOwnedAttachEffects();
		bool HasAttachedEffects(std::vector<AttachEffectTypeClass*> attachEffectTypes, bool requireAll, bool ignoreSameSource, TechnoClass* pInvoker, AbstractClass* pSource, std::vector<int> const* minCounts, std::vector<int> const* maxCounts) const;
		int GetAttachedEffectCumulativeCount(AttachEffectTypeClass* pAttachEffectType, bool ignoreSameSource = false, TechnoClass* pInvoker = nullptr, AbstractClass* pSource = nullptr) const;
		void InitializeDisplayInfo();
		void StopIdleAction();
		void ApplyIdleAction();
		void ManualIdleAction();
		void StopRotateWithNewROT(int ROT = -1);
		void UpdateCachedClick();

		virtual ~ExtData() override;
		virtual void InvalidatePointer(void* ptr, bool bRemoved) override;
		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

		void InitAggressiveStance();
		bool GetAggressiveStance() const;
		void ToggleAggressiveStance();
		bool CanToggleAggressiveStance();

	private:
		template <typename T>
		void Serialize(T& Stm);
	};

	class ExtContainer final : public Container<TechnoExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	struct DrawFrameStruct
	{
		int TopLength;
		int TopFrame;
		SHPStruct* TopPipSHP;
		int MidLength;
		int MidFrame;
		SHPStruct* MidPipSHP;
		int MaxLength;
		int BrdFrame;
		Point2D* Location;
		RectangleStruct* Bounds;
	};

	static ExtContainer ExtMap;

	static UnitClass* Deployer;

	static bool LoadGlobals(PhobosStreamReader& Stm);
	static bool SaveGlobals(PhobosStreamWriter& Stm);

	static bool IsActive(TechnoClass* pThis);
	static bool IsActiveIgnoreEMP(TechnoClass* pThis);

	static bool IsHarvesting(TechnoClass* pThis);
	static bool HasAvailableDock(TechnoClass* pThis);

	static CoordStruct GetFLHAbsoluteCoords(TechnoClass* pThis, CoordStruct flh, bool turretFLH = false);

	static CoordStruct GetBurstFLH(TechnoClass* pThis, int weaponIndex, bool& FLHFound);
	static CoordStruct GetSimpleFLH(InfantryClass* pThis, int weaponIndex, bool& FLHFound);

	static void ChangeOwnerMissionFix(FootClass* pThis);
	static void KillSelf(TechnoClass* pThis, AutoDeathBehavior deathOption, AnimTypeClass* pVanishAnimation, bool isInLimbo = false);
	static void ApplyMindControlRangeLimit(TechnoClass* pThis);
	static void ObjectKilledBy(TechnoClass* pThis, TechnoClass* pKiller);
	static void UpdateSharedAmmo(TechnoClass* pThis);
	static double GetCurrentSpeedMultiplier(FootClass* pThis);
	static void DrawSelfHealPips(TechnoClass* pThis, Point2D* pLocation, RectangleStruct* pBounds);
	static void DrawInsignia(TechnoClass* pThis, Point2D* pLocation, RectangleStruct* pBounds);
	static void ApplyGainedSelfHeal(TechnoClass* pThis);
	static void SyncInvulnerability(TechnoClass* pFrom, TechnoClass* pTo);
	static CoordStruct PassengerKickOutLocation(TechnoClass* pThis, FootClass* pPassenger, int maxAttempts);
	static bool AllowedTargetByZone(TechnoClass* pThis, TechnoClass* pTarget, TargetZoneScanType zoneScanType, WeaponTypeClass* pWeapon = nullptr, bool useZone = false, int zone = -1);
	static void UpdateAttachedAnimLayers(TechnoClass* pThis);
	static bool ConvertToType(FootClass* pThis, TechnoTypeClass* toType);
	static bool CanDeployIntoBuilding(UnitClass* pThis, bool noDeploysIntoDefaultValue = false);
	static bool IsTypeImmune(TechnoClass* pThis, TechnoClass* pSource);
	static int GetTintColor(TechnoClass* pThis, bool invulnerability, bool airstrike, bool berserk);
	static int GetCustomTintColor(TechnoClass* pThis);
	static int GetCustomTintIntensity(TechnoClass* pThis);
	static void ApplyCustomTintValues(TechnoClass* pThis, int& color, int& intensity);
	static void DrawFactoryProgress(BuildingClass* pThis, RectangleStruct* pBounds, Point2D basePosition);
	static void DrawSuperProgress(BuildingClass* pThis, RectangleStruct* pBounds, Point2D basePosition);
	static void DrawIronCurtainProgress(TechnoClass* pThis, RectangleStruct* pBounds, Point2D basePosition, bool isBuilding, bool isInfantry);
	static void DrawTemporalProgress(TechnoClass* pThis, RectangleStruct* pBounds, Point2D basePosition, bool isBuilding, bool isInfantry);
	static void DrawVanillaStyleFootBar(DrawFrameStruct* pDraw);
	static void DrawVanillaStyleBuildingBar(DrawFrameStruct* pDraw);
	static Point2D GetScreenLocation(TechnoClass* pThis);
	static Point2D GetFootSelectBracketPosition(TechnoClass* pThis, Anchor anchor);
	static Point2D GetBuildingSelectBracketPosition(TechnoClass* pThis, BuildingSelectBracketPosition bracketPosition);
	static void ProcessDigitalDisplays(TechnoClass* pThis);
	static void GetValuesForDisplay(TechnoClass* pThis, DisplayInfoType infoType, int& value, int& maxValue);

	// WeaponHelpers.cpp
	static int PickWeaponIndex(TechnoClass* pThis, TechnoClass* pTargetTechno, AbstractClass* pTarget, int weaponIndexOne, int weaponIndexTwo, bool allowFallback = true, bool allowAAFallback = true);
	static void FireWeaponAtSelf(TechnoClass* pThis, WeaponTypeClass* pWeaponType);
	static bool CanFireNoAmmoWeapon(TechnoClass* pThis, int weaponIndex);
	static WeaponTypeClass* GetDeployFireWeapon(TechnoClass* pThis, int& weaponIndex);
	static WeaponTypeClass* GetDeployFireWeapon(TechnoClass* pThis);
	static WeaponTypeClass* GetCurrentWeapon(TechnoClass* pThis, int& weaponIndex, bool getSecondary = false);
	static WeaponTypeClass* GetCurrentWeapon(TechnoClass* pThis, bool getSecondary = false);
	static int GetWeaponIndexAgainstWall(TechnoClass* pThis, OverlayTypeClass* pWallOverlayType);
};
