#pragma once
#include <TechnoTypeClass.h>

#include <Helpers/Macro.h>
#include <Utilities/Container.h>
#include <Utilities/TemplateDef.h>

#include <New/Type/ShieldTypeClass.h>
#include <New/Type/LaserTrailTypeClass.h>
#include <New/Type/AttachEffectTypeClass.h>
#include <New/Type/Affiliated/InterceptorTypeClass.h>
#include <New/Type/Affiliated/PassengerDeletionTypeClass.h>
#include <New/Type/DigitalDisplayTypeClass.h>
#include <New/Type/Affiliated/DroppodTypeClass.h>

class Matrix3D;

class TechnoTypeExt
{
public:
	using base_type = TechnoTypeClass;

	static constexpr DWORD Canary = 0x11111111;
	static constexpr size_t ExtPointerOffset = 0xDF4;

	class ExtData final : public Extension<TechnoTypeClass>
	{
	public:
		Valueable<bool> HealthBar_Hide;
		Valueable<CSFText> UIDescription;
		Valueable<bool> LowSelectionPriority;
		PhobosFixedString<0x20> GroupAs;
		Valueable<int> RadarJamRadius;
		Nullable<int> InhibitorRange;
		Nullable<int> DesignatorRange;
		Valueable<float> FactoryPlant_Multiplier;
		Valueable<Leptons> MindControlRangeLimit;

		std::unique_ptr<InterceptorTypeClass> InterceptorType;

		Valueable<PartialVector3D<int>> TurretOffset;
		Nullable<bool> TurretShadow;
		Valueable<int> ShadowIndex_Frame;
		std::map<int, int> ShadowIndices;
		Valueable<bool> Spawner_LimitRange;
		Valueable<int> Spawner_ExtraLimitRange;
		Nullable<int> Spawner_DelayFrames;
		Valueable<bool> Spawner_AttackImmediately;
		Nullable<bool> Harvester_Counted;
		Valueable<bool> Promote_IncludeSpawns;
		Valueable<bool> ImmuneToCrit;
		Valueable<bool> MultiMindControl_ReleaseVictim;
		Valueable<int> CameoPriority;
		Valueable<bool> NoManualMove;
		Nullable<int> InitialStrength;
		Valueable<bool> ReloadInTransport;
		Valueable<bool> ForbidParallelAIQueues;

		Valueable<ShieldTypeClass*> ShieldType;
		std::unique_ptr<PassengerDeletionTypeClass> PassengerDeletionType;
		std::unique_ptr<DroppodTypeClass> DroppodType;

		Valueable<int> Ammo_AddOnDeploy;
		Valueable<int> Ammo_AutoDeployMinimumAmount;
		Valueable<int> Ammo_AutoDeployMaximumAmount;
		Valueable<int> Ammo_DeployUnlockMinimumAmount;
		Valueable<int> Ammo_DeployUnlockMaximumAmount;

		Nullable<AutoDeathBehavior> AutoDeath_Behavior;
		Valueable<AnimTypeClass*> AutoDeath_VanishAnimation;
		Valueable<bool> AutoDeath_OnAmmoDepletion;
		Valueable<int> AutoDeath_AfterDelay;
		ValueableVector<TechnoTypeClass*> AutoDeath_TechnosDontExist;
		Valueable<bool> AutoDeath_TechnosDontExist_Any;
		Valueable<bool> AutoDeath_TechnosDontExist_AllowLimboed;
		Valueable<AffectedHouse> AutoDeath_TechnosDontExist_Houses;
		ValueableVector<TechnoTypeClass*> AutoDeath_TechnosExist;
		Valueable<bool> AutoDeath_TechnosExist_Any;
		Valueable<bool> AutoDeath_TechnosExist_AllowLimboed;
		Valueable<AffectedHouse> AutoDeath_TechnosExist_Houses;

		Valueable<SlaveChangeOwnerType> Slaved_OwnerWhenMasterKilled;
		NullableIdx<VocClass> SlavesFreeSound;
		NullableIdx<VocClass> SellSound;
		NullableIdx<VoxClass> EVA_Sold;

		Nullable<bool> CombatAlert;
		Nullable<bool> CombatAlert_NotBuilding;
		Nullable<bool> CombatAlert_UseFeedbackVoice;
		Nullable<bool> CombatAlert_UseAttackVoice;
		Nullable<bool> CombatAlert_UseEVA;
		NullableIdx<VoxClass> CombatAlert_EVA;

		NullableIdx<VocClass> VoiceCreated;
		NullableIdx<VocClass> VoicePickup; // Used by carryalls instead of VoiceMove if set.

		Nullable<AnimTypeClass*> WarpOut;
		Nullable<AnimTypeClass*> WarpIn;
		Nullable<AnimTypeClass*> WarpAway;
		Nullable<bool> ChronoTrigger;
		Nullable<int> ChronoDistanceFactor;
		Nullable<int> ChronoMinimumDelay;
		Nullable<int> ChronoRangeMinimum;
		Nullable<int> ChronoDelay;
		Nullable<int> ChronoSpherePreDelay;
		Nullable<int> ChronoSphereDelay;

		Valueable<WeaponTypeClass*> WarpInWeapon;
		Nullable<WeaponTypeClass*> WarpInMinRangeWeapon;
		Valueable<WeaponTypeClass*> WarpOutWeapon;
		Valueable<bool> WarpInWeapon_UseDistanceAsDamage;

		int SubterraneanSpeed;
		Nullable<int> SubterraneanHeight;

		ValueableVector<AnimTypeClass*> OreGathering_Anims;
		ValueableVector<int> OreGathering_Tiberiums;
		ValueableVector<int> OreGathering_FramesPerDir;

		std::vector<std::vector<CoordStruct>> WeaponBurstFLHs;
		std::vector<std::vector<CoordStruct>> EliteWeaponBurstFLHs;
		std::vector<CoordStruct> AlternateFLHs;

		Valueable<bool> DestroyAnim_Random;
		Valueable<bool> NotHuman_RandomDeathSequence;

		Valueable<InfantryTypeClass*> DefaultDisguise;
		Valueable<bool> UseDisguiseMovementSpeed;

		Nullable<int> OpenTopped_RangeBonus;
		Nullable<float> OpenTopped_DamageMultiplier;
		Nullable<int> OpenTopped_WarpDistance;
		Valueable<bool> OpenTopped_IgnoreRangefinding;
		Valueable<bool> OpenTopped_AllowFiringIfDeactivated;
		Valueable<bool> OpenTopped_ShareTransportTarget;
		Valueable<bool> OpenTopped_UseTransportRangeModifiers;
		Valueable<bool> OpenTopped_CheckTransportDisableWeapons;

		Valueable<bool> AutoFire;
		Valueable<bool> AutoFire_TargetSelf;

		Valueable<bool> AggressiveStance;
		Nullable<bool> AggressiveStance_Togglable;
		Valueable<bool> AggressiveStance_Exempt;
		ValueableIdx<VocClass> VoiceEnterAggressiveStance;
		ValueableIdx<VocClass> VoiceExitAggressiveStance;

		Valueable<bool> NoSecondaryWeaponFallback;
		Valueable<bool> NoSecondaryWeaponFallback_AllowAA;

		Valueable<int> NoAmmoWeapon;
		Valueable<int> NoAmmoAmount;

		Valueable<bool> JumpjetRotateOnCrash;
		Nullable<int> ShadowSizeCharacteristicHeight;

		Valueable<bool> DeployingAnim_AllowAnyDirection;
		Valueable<bool> DeployingAnim_KeepUnitVisible;
		Valueable<bool> DeployingAnim_ReverseForUndeploy;
		Valueable<bool> DeployingAnim_UseUnitDrawer;

		Valueable<CSFText> EnemyUIName;
		Valueable<int> ForceWeapon_Naval_Decloaked;
		Valueable<int> ForceWeapon_Cloaked;
		Valueable<int> ForceWeapon_Disguised;

		Valueable<bool> Ammo_Shared;
		Valueable<int> Ammo_Shared_Group;

		Nullable<SelfHealGainType> SelfHealGainType;
		Valueable<bool> Passengers_SyncOwner;
		Valueable<bool> Passengers_SyncOwner_RevertOnExit;

		Nullable<bool> IronCurtain_KeptOnDeploy;
		Nullable<IronCurtainEffect> IronCurtain_Effect;
		Nullable<WarheadTypeClass*> IronCurtain_KillWarhead;
		Nullable<bool> ForceShield_KeptOnDeploy;
		Nullable<IronCurtainEffect> ForceShield_Effect;
		Nullable<WarheadTypeClass*> ForceShield_KillWarhead;
		Valueable<bool> Explodes_KillPassengers;
		Valueable<bool> Explodes_DuringBuildup;
		Nullable<int> DeployFireWeapon;
		Valueable<TargetZoneScanType> TargetZoneScanType;

		Promotable<SHPStruct*> Insignia;
		Valueable<Vector3D<int>> InsigniaFrames;
		Promotable<int> InsigniaFrame;
		Nullable<bool> Insignia_ShowEnemy;
		std::vector<Promotable<SHPStruct*>> Insignia_Weapon;
		std::vector<Promotable<int>> InsigniaFrame_Weapon;
		std::vector<Vector3D<int>> InsigniaFrames_Weapon;

		Nullable<bool> JumpjetTilt;

		Nullable<bool> TiltsWhenCrushes_Vehicles;
		Nullable<bool> TiltsWhenCrushes_Overlays;
		Nullable<double> CrushForwardTiltPerFrame;
		Valueable<double> CrushOverlayExtraForwardTilt;
		Valueable<double> CrushSlowdownMultiplier;

		Valueable<bool> DigitalDisplay_Disable;
		ValueableVector<DigitalDisplayTypeClass*> DigitalDisplayTypes;

		Valueable<int> AmmoPipFrame;
		Valueable<int> EmptyAmmoPipFrame;
		Valueable<int> AmmoPipWrapStartFrame;
		Nullable<Point2D> AmmoPipSize;
		Valueable<Point2D> AmmoPipOffset;

		Valueable<bool> ShowSpawnsPips;
		Valueable<int> SpawnsPipFrame;
		Valueable<int> EmptySpawnsPipFrame;
		Nullable<Point2D> SpawnsPipSize;
		Valueable<Point2D> SpawnsPipOffset;

		Nullable<Leptons> SpawnDistanceFromTarget;
		Nullable<int> SpawnHeight;
		Nullable<int> LandingDir;

		Valueable<TechnoTypeClass*> Convert_HumanToComputer;
		Valueable<TechnoTypeClass*> Convert_ComputerToHuman;

		Valueable<double> CrateGoodie_RerollChance;

		Nullable<ColorStruct> Tint_Color;
		Valueable<double> Tint_Intensity;
		Valueable<AffectedHouse> Tint_VisibleToHouses;

		Valueable<WeaponTypeClass*> RevengeWeapon;
		Valueable<AffectedHouse> RevengeWeapon_AffectsHouses;

		AEAttachInfoTypeClass AttachEffects;

		ValueableVector<TechnoTypeClass*> BuildLimitGroup_Types;
		ValueableVector<int> BuildLimitGroup_Nums;
		Valueable<int> BuildLimitGroup_Factor;
		Valueable<bool> BuildLimitGroup_ContentIfAnyMatch;
		Valueable<bool> BuildLimitGroup_NotBuildableIfQueueMatch;
		ValueableVector<TechnoTypeClass*> BuildLimitGroup_ExtraLimit_Types;
		ValueableVector<int> BuildLimitGroup_ExtraLimit_Nums;
		ValueableVector<int> BuildLimitGroup_ExtraLimit_MaxCount;
		Valueable<int> BuildLimitGroup_ExtraLimit_MaxNum;

		Nullable<bool> RecountBurst;
		Nullable<bool> Turret_IdleRotate;
		Nullable<bool> Turret_PointToMouse;
		Nullable<int> TurretROT;
		Valueable<double> Turret_Restriction;
		Valueable<double> Turret_ExtraAngle;
		Valueable<bool> Turret_BodyOrientation;
		Valueable<double> Turret_BodyOrientationAngle;
		Valueable<bool> Turret_BodyOrientationSymmetric;
		Valueable<bool> CanBeBuiltOn;
		Valueable<bool> ExtraBaseNormal;
		Valueable<bool> ExtraBaseForAllyBuilding;
		Nullable<bool> Cameo_AlwaysExist;
		ValueableVector<TechnoTypeClass*> Cameo_OverrideTechnos;
		DWORD Cameo_RequiredHouses;
		bool IsMetTheEssentialConditions; // Not read from ini
		bool IsGreyCameoForCurrentPlayer; // Not read from ini
		bool IsGreyCameoAbandonedProduct; // Not read from ini
		Valueable<CSFText> UIDescription_Unbuildable;
		PhobosPCXFile CameoPCX;
		PhobosPCXFile GreyCameoPCX;
		Valueable<DisplayInfoType> SelectedInfo_UpperType;
		Valueable<ColorStruct> SelectedInfo_UpperColor;
		Valueable<DisplayInfoType> SelectedInfo_BelowType;
		Valueable<ColorStruct> SelectedInfo_BelowColor;
		Valueable<TechnoTypeClass*> FakeOf;
		CustomPalette CameoPal;
		Nullable<bool> NoRearm_UnderEMP;
		Nullable<bool> NoRearm_Temporal;
		Nullable<bool> NoReload_UnderEMP;
		Nullable<bool> NoReload_Temporal;
		Valueable<int> RateDown_Delay;
		Valueable<bool> RateDown_Reset;
		Valueable<int> RateDown_Cover_Value;
		Valueable<int> RateDown_Cover_AmmoBelow;
		Valueable<bool> UniqueTechno;
		Nullable<bool> NoQueueUpToEnter;
		Nullable<bool> NoQueueUpToUnload;
		Valueable<bool> CanManualReload;
		Valueable<bool> CanManualReload_ResetROF;
		Valueable<WarheadTypeClass*> CanManualReload_DetonateWarhead;
		Valueable<int> CanManualReload_DetonateConsume;

		Valueable<int> Spawner_RecycleRange;
		Valueable<AnimTypeClass*> Spawner_RecycleAnim;
		Nullable<int> AINormalTargetingDelay;
		Nullable<int> PlayerNormalTargetingDelay;
		Nullable<int> AIGuardAreaTargetingDelay;
		Nullable<int> PlayerGuardAreaTargetingDelay;
		Valueable<bool> KeepWarping;
		Nullable<int> KeepWarping_Distance;
		Valueable<bool> FiringByPassMovingCheck;
		Valueable<bool> SkipCrushSlowdown;
		Nullable<bool> PlayerGuardModePursuit;
		Nullable<Leptons> PlayerGuardModeStray;
		Nullable<double> PlayerGuardModeGuardRangeMultiplier;
		Nullable<Leptons> PlayerGuardModeGuardRangeAddend;
		Nullable<Leptons> PlayerGuardStationaryStray;
		Nullable<bool> AIGuardModePursuit;
		Nullable<Leptons> AIGuardModeStray;
		Nullable<double> AIGuardModeGuardRangeMultiplier;
		Nullable<Leptons> AIGuardModeGuardRangeAddend;
		Nullable<Leptons> AIGuardStationaryStray;
		Valueable<bool> Engineer_CanAutoFire;
		Valueable<bool> BunkerableAnyWay;
		Valueable<bool> Harvester_CanGuardArea;
		Valueable<int> DigStartROT;
		Valueable<int> DigInSpeed;
		Valueable<int> DigOutSpeed;
		Valueable<int> DigEndROT;
		Valueable<int> FlightClimb;
		Valueable<int> FlightCrash;
		Nullable<bool> ExplodeOnDestroy;
		Nullable<bool> FireDeathWeaponOnCrushed;
		Nullable<CoordStruct> ExitCoord;
		Valueable<bool> MissileSpawnUseOtherFLHs;
		Valueable<bool> HarvesterQuickUnloader;
		Nullable<bool> DistributeTargetingFrame;
		Valueable<bool> AttackMove_Follow;
		Valueable<bool> AttackMove_Follow_IncludeAir;
		Nullable<bool> AttackMove_StopWhenTargetAcquired;
		Valueable<bool> AttackMove_PursuitTarget;
		Valueable<TechnoTypeClass*> ThisIsAJumpjet;
		Valueable<bool> ImAJumpjetFromAirport;
		Valueable<bool> IgnoreRallyPoint;
		Nullable<bool> Sinkable;
		Valueable<int> SinkSpeed;
		Valueable<int> JumpjetSpeedType;

		Nullable<bool> KeepAlive;

		Valueable<double> FallingDownDamage;
		Nullable<double> FallingDownDamage_Water;

		Nullable<AnimTypeClass*> Wake;
		Nullable<AnimTypeClass*> Wake_Grapple;
		Nullable<AnimTypeClass*> Wake_Sinking;

		Nullable<bool> AttackMove_Aggressive;
		Nullable<bool> AttackMove_UpdateTarget;

		Valueable<bool> KeepTargetOnMove;
		Valueable<Leptons> KeepTargetOnMove_ExtraDistance;

		Valueable<int> Power;

		struct LaserTrailDataEntry
		{
			ValueableIdx<LaserTrailTypeClass> idxType;
			Valueable<CoordStruct> FLH;
			Valueable<bool> IsOnTurret;
			LaserTrailTypeClass* GetType() const { return LaserTrailTypeClass::Array[idxType].get(); }
		};

		std::vector<LaserTrailDataEntry> LaserTrailData;
		Valueable<bool> OnlyUseLandSequences;
		Nullable<CoordStruct> PronePrimaryFireFLH;
		Nullable<CoordStruct> ProneSecondaryFireFLH;
		Nullable<CoordStruct> DeployedPrimaryFireFLH;
		Nullable<CoordStruct> DeployedSecondaryFireFLH;
		std::vector<std::vector<CoordStruct>> CrouchedWeaponBurstFLHs;
		std::vector<std::vector<CoordStruct>> EliteCrouchedWeaponBurstFLHs;
		std::vector<std::vector<CoordStruct>> DeployedWeaponBurstFLHs;
		std::vector<std::vector<CoordStruct>> EliteDeployedWeaponBurstFLHs;

		ExtData(TechnoTypeClass* OwnerObject) : Extension<TechnoTypeClass>(OwnerObject)
			, HealthBar_Hide { false }
			, UIDescription {}
			, LowSelectionPriority { false }
			, GroupAs { NONE_STR }
			, RadarJamRadius { 0 }
			, InhibitorRange {}
			, DesignatorRange { }
			, FactoryPlant_Multiplier { 1.0 }
			, MindControlRangeLimit {}

			, InterceptorType { nullptr }

			, TurretOffset { { 0, 0, 0 } }
			, TurretShadow { }
			, ShadowIndices { }
			, ShadowIndex_Frame { 0 }
			, Spawner_LimitRange { false }
			, Spawner_ExtraLimitRange { 0 }
			, Spawner_DelayFrames {}
			, Spawner_AttackImmediately { false }
			, Harvester_Counted {}
			, Promote_IncludeSpawns { false }
			, ImmuneToCrit { false }
			, MultiMindControl_ReleaseVictim { false }
			, CameoPriority { 0 }
			, NoManualMove { false }
			, InitialStrength {}
			, ReloadInTransport { false }
			, ForbidParallelAIQueues { false }
			, ShieldType {}
			, PassengerDeletionType { nullptr }

			, WarpOut {}
			, WarpIn {}
			, WarpAway {}
			, ChronoTrigger {}
			, ChronoDistanceFactor {}
			, ChronoMinimumDelay {}
			, ChronoRangeMinimum {}
			, ChronoDelay {}
			, ChronoSpherePreDelay {}
			, ChronoSphereDelay {}
			, WarpInWeapon {}
			, WarpInMinRangeWeapon {}
			, WarpOutWeapon {}
			, WarpInWeapon_UseDistanceAsDamage { false }

			, SubterraneanSpeed { -1 }
			, SubterraneanHeight {}

			, OreGathering_Anims {}
			, OreGathering_Tiberiums {}
			, OreGathering_FramesPerDir {}
			, LaserTrailData {}
			, DestroyAnim_Random { true }
			, NotHuman_RandomDeathSequence { false }

			, DefaultDisguise {}
			, UseDisguiseMovementSpeed {}

			, OpenTopped_RangeBonus {}
			, OpenTopped_DamageMultiplier {}
			, OpenTopped_WarpDistance {}
			, OpenTopped_IgnoreRangefinding { false }
			, OpenTopped_AllowFiringIfDeactivated { true }
			, OpenTopped_ShareTransportTarget { true }
			, OpenTopped_UseTransportRangeModifiers { false }
			, OpenTopped_CheckTransportDisableWeapons { false }

			, AutoFire { false }
			, AutoFire_TargetSelf { false }

			, AggressiveStance { false }
			, AggressiveStance_Togglable {}
			, AggressiveStance_Exempt { false }
			, VoiceEnterAggressiveStance { -1 }
			, VoiceExitAggressiveStance { -1 }

			, NoSecondaryWeaponFallback { false }
			, NoSecondaryWeaponFallback_AllowAA { false }
			, NoAmmoWeapon { -1 }
			, NoAmmoAmount { 0 }
			, JumpjetRotateOnCrash { true }
			, ShadowSizeCharacteristicHeight { }
			, DeployingAnim_AllowAnyDirection { false }
			, DeployingAnim_KeepUnitVisible { false }
			, DeployingAnim_ReverseForUndeploy { true }
			, DeployingAnim_UseUnitDrawer { true }

			, Ammo_AddOnDeploy { 0 }
			, Ammo_AutoDeployMinimumAmount { -1 }
			, Ammo_AutoDeployMaximumAmount { -1 }
			, Ammo_DeployUnlockMinimumAmount { -1 }
			, Ammo_DeployUnlockMaximumAmount { -1 }

			, AutoDeath_Behavior { }
			, AutoDeath_VanishAnimation {}
			, AutoDeath_OnAmmoDepletion { false }
			, AutoDeath_AfterDelay { 0 }
			, AutoDeath_TechnosDontExist {}
			, AutoDeath_TechnosDontExist_Any { false }
			, AutoDeath_TechnosDontExist_AllowLimboed { false }
			, AutoDeath_TechnosDontExist_Houses { AffectedHouse::Owner }
			, AutoDeath_TechnosExist {}
			, AutoDeath_TechnosExist_Any { true }
			, AutoDeath_TechnosExist_AllowLimboed { true }
			, AutoDeath_TechnosExist_Houses { AffectedHouse::Owner }

			, Slaved_OwnerWhenMasterKilled { SlaveChangeOwnerType::Killer }
			, SlavesFreeSound {}
			, SellSound {}
			, EVA_Sold {}

			, CombatAlert {}
			, CombatAlert_NotBuilding {}
			, CombatAlert_UseFeedbackVoice {}
			, CombatAlert_UseAttackVoice {}
			, CombatAlert_UseEVA {}
			, CombatAlert_EVA {}

			, EnemyUIName {}

			, VoiceCreated {}
			, VoicePickup {}

			, ForceWeapon_Naval_Decloaked { -1 }
			, ForceWeapon_Cloaked { -1 }
			, ForceWeapon_Disguised { -1 }

			, Ammo_Shared { false }
			, Ammo_Shared_Group { -1 }

			, SelfHealGainType {}
			, Passengers_SyncOwner { false }
			, Passengers_SyncOwner_RevertOnExit { true }

			, OnlyUseLandSequences { false }

			, PronePrimaryFireFLH {}
			, ProneSecondaryFireFLH {}
			, DeployedPrimaryFireFLH {}
			, DeployedSecondaryFireFLH {}

			, IronCurtain_KeptOnDeploy {}
			, IronCurtain_Effect {}
			, IronCurtain_KillWarhead {}
			, ForceShield_KeptOnDeploy {}
			, ForceShield_Effect {}
			, ForceShield_KillWarhead {}

			, Explodes_KillPassengers { true }
			, Explodes_DuringBuildup { true }
			, DeployFireWeapon {}
			, TargetZoneScanType { TargetZoneScanType::Same }

			, Insignia {}
			, InsigniaFrames { { -1, -1, -1 } }
			, InsigniaFrame { -1 }
			, Insignia_ShowEnemy {}
			, Insignia_Weapon {}
			, InsigniaFrame_Weapon {}
			, InsigniaFrames_Weapon {}

			, JumpjetTilt {}

			, TiltsWhenCrushes_Vehicles {}
			, TiltsWhenCrushes_Overlays {}
			, CrushSlowdownMultiplier { 0.2 }
			, CrushForwardTiltPerFrame {}
			, CrushOverlayExtraForwardTilt { 0.02 }

			, DigitalDisplay_Disable { false }
			, DigitalDisplayTypes {}

			, AmmoPipFrame { 13 }
			, EmptyAmmoPipFrame { -1 }
			, AmmoPipWrapStartFrame { 14 }
			, AmmoPipSize {}
			, AmmoPipOffset { { 0,0 } }

			, ShowSpawnsPips { true }
			, SpawnsPipFrame { 1 }
			, EmptySpawnsPipFrame { 0 }
			, SpawnsPipSize {}
			, SpawnsPipOffset { { 0,0 } }

			, SpawnDistanceFromTarget {}
			, SpawnHeight {}
			, LandingDir {}
			, DroppodType {}

			, Convert_HumanToComputer { }
			, Convert_ComputerToHuman { }

			, CrateGoodie_RerollChance { 0.0 }

			, Tint_Color {}
			, Tint_Intensity { 0.0 }
			, Tint_VisibleToHouses { AffectedHouse::All }

			, RevengeWeapon {}
			, RevengeWeapon_AffectsHouses { AffectedHouse::All }

			, AttachEffects {}

			, BuildLimitGroup_Types {}
			, BuildLimitGroup_Nums {}
			, BuildLimitGroup_Factor { 1 }
			, BuildLimitGroup_ContentIfAnyMatch { false }
			, BuildLimitGroup_NotBuildableIfQueueMatch { false }
			, BuildLimitGroup_ExtraLimit_Types {}
			, BuildLimitGroup_ExtraLimit_Nums {}
			, BuildLimitGroup_ExtraLimit_MaxCount {}
			, BuildLimitGroup_ExtraLimit_MaxNum { 0 }

			, RecountBurst {}
			, Turret_IdleRotate {}
			, Turret_PointToMouse {}
			, TurretROT {}
			, Turret_Restriction { 180.0 }
			, Turret_ExtraAngle { 0.0 }
			, Turret_BodyOrientation { false }
			, Turret_BodyOrientationAngle { 0.0 }
			, Turret_BodyOrientationSymmetric { true }
			, CanBeBuiltOn { false }
			, ExtraBaseNormal { false }
			, ExtraBaseForAllyBuilding { false }
			, Cameo_AlwaysExist {}
			, Cameo_OverrideTechnos {}
			, Cameo_RequiredHouses { 0xFFFFFFFF }
			, IsMetTheEssentialConditions { false }
			, IsGreyCameoForCurrentPlayer { false }
			, IsGreyCameoAbandonedProduct { true }
			, UIDescription_Unbuildable {}
			, CameoPCX {}
			, GreyCameoPCX {}
			, SelectedInfo_UpperType { DisplayInfoType::Shield }
			, SelectedInfo_UpperColor { { 153, 153, 255 } }
			, SelectedInfo_BelowType { DisplayInfoType::Health }
			, SelectedInfo_BelowColor { { 0, 0, 0 } }
			, FakeOf {}
			, CameoPal {}
			, NoRearm_UnderEMP {}
			, NoRearm_Temporal {}
			, NoReload_UnderEMP {}
			, NoReload_Temporal {}
			, RateDown_Delay { 0 }
			, RateDown_Reset { false }
			, RateDown_Cover_Value { 0 }
			, RateDown_Cover_AmmoBelow { -2 }
			, UniqueTechno { false }
			, NoQueueUpToEnter {}
			, NoQueueUpToUnload {}
			, CanManualReload { false }
			, CanManualReload_ResetROF { true }
			, CanManualReload_DetonateWarhead {}
			, CanManualReload_DetonateConsume { 0 }

			, Spawner_RecycleRange { -1 }
			, Spawner_RecycleAnim { nullptr }
			, AINormalTargetingDelay {}
			, PlayerNormalTargetingDelay {}
			, AIGuardAreaTargetingDelay {}
			, PlayerGuardAreaTargetingDelay {}
			, KeepWarping { false }
			, KeepWarping_Distance {}
			, FiringByPassMovingCheck { false }
			, SkipCrushSlowdown { false }
			, PlayerGuardModePursuit {}
			, PlayerGuardModeStray {}
			, PlayerGuardModeGuardRangeMultiplier {}
			, PlayerGuardModeGuardRangeAddend {}
			, PlayerGuardStationaryStray {}
			, AIGuardModePursuit {}
			, AIGuardModeStray {}
			, AIGuardModeGuardRangeMultiplier {}
			, AIGuardModeGuardRangeAddend {}
			, AIGuardStationaryStray {}
			, Engineer_CanAutoFire { false }
			, BunkerableAnyWay { false }
			, Harvester_CanGuardArea { false }
			, DigStartROT { -1 }
			, DigInSpeed { -1 }
			, DigOutSpeed { -1 }
			, DigEndROT { -1 }
			, FlightClimb { -1 }
			, FlightCrash { -1 }
			, ExplodeOnDestroy {}
			, FireDeathWeaponOnCrushed {}
			, ExitCoord {}
			, MissileSpawnUseOtherFLHs { false }
			, HarvesterQuickUnloader { false }
			, DistributeTargetingFrame {}
			, AttackMove_Follow { false }
			, AttackMove_Follow_IncludeAir { false }
			, AttackMove_StopWhenTargetAcquired {}
			, AttackMove_PursuitTarget { false }
			, ThisIsAJumpjet { nullptr }
			, ImAJumpjetFromAirport { false }
			, IgnoreRallyPoint { false }
			, Sinkable { }
			, SinkSpeed { 5 }
			, JumpjetSpeedType { 3 }

			, KeepAlive {}

			, FallingDownDamage { 1.0 }
			, FallingDownDamage_Water {}

			, Wake { }
			, Wake_Grapple { }
			, Wake_Sinking { }

			, AttackMove_Aggressive {}
			, AttackMove_UpdateTarget {}

			, KeepTargetOnMove { false }
			, KeepTargetOnMove_ExtraDistance { Leptons(0) }

			, Power { }
		{ }

		virtual ~ExtData() = default;
		virtual void LoadFromINIFile(CCINIClass* pINI) override;
		virtual void Initialize() override;

		virtual void InvalidatePointer(void* ptr, bool bRemoved) override { }

		virtual void LoadFromStream(PhobosStreamReader& Stm) override;
		virtual void SaveToStream(PhobosStreamWriter& Stm) override;

		void ApplyTurretOffset(Matrix3D* mtx, double factor = 1.0);
		DirStruct GetTurretDesiredDir(DirStruct defaultDir);
		void SetTurretLimitedDir(FootClass* pThis, DirStruct desiredDir);
		short GetTurretLimitedRaw(short currentDirectionRaw);
		DirStruct GetBodyDesiredDir(DirStruct currentDir, DirStruct defaultDir);

		// Ares 0.A
		const char* GetSelectionGroupID() const;

	private:
		template <typename T>
		void Serialize(T& Stm);

		void ParseBurstFLHs(INI_EX& exArtINI, const char* pArtSection, std::vector<std::vector<CoordStruct>>& nFLH, std::vector<std::vector<CoordStruct>>& nEFlh, const char* pPrefixTag);

	};

	class ExtContainer final : public Container<TechnoTypeExt>
	{
	public:
		ExtContainer();
		~ExtContainer();
	};

	static ExtContainer ExtMap;

	static constexpr double AngleToRaw = (65536.0 / 360);

	static void ApplyTurretOffset(TechnoTypeClass* pType, Matrix3D* mtx, double factor = 1.0);
	static TechnoTypeClass* GetTechnoType(ObjectTypeClass* pType);

	static TechnoClass* CreateUnit(TechnoTypeClass* pType, CoordStruct location, DirType facing, DirType* secondaryFacing, HouseClass* pOwner,
		TechnoClass* pInvoker = nullptr, HouseClass* pInvokerHouse = nullptr, AnimTypeClass* pSpawnAnimType = nullptr, int spawnHeight = -1,
		bool alwaysOnGround = false, bool checkPathfinding = false, bool parachuteIfInAir = false, Mission mission = Mission::Guard, Mission* missionAI = nullptr);

	static int __fastcall RequirementsMetExtraCheck(void* pAresHouseExt, void* _, TechnoTypeClass* pType);
	static CanBuildResult CheckAlwaysExistCameo(TechnoTypeClass* pType, CanBuildResult canBuild);

	// Ares 0.A
	static const char* GetSelectionGroupID(ObjectTypeClass* pType);
	static bool HasSelectionGroupID(ObjectTypeClass* pType, const char* pID);
};
