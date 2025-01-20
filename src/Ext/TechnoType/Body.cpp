#include "Body.h"

#include <AircraftTrackerClass.h>
#include <AnimClass.h>
#include <FlyLocomotionClass.h>
#include <JumpjetLocomotionClass.h>
#include <TechnoTypeClass.h>
#include <StringTable.h>

#include <Ext/Anim/Body.h>
#include <Ext/BuildingType/Body.h>
#include <Ext/BulletType/Body.h>
#include <Ext/Techno/Body.h>

#include <Utilities/GeneralUtils.h>

TechnoTypeExt::ExtContainer TechnoTypeExt::ExtMap;

void TechnoTypeExt::ExtData::Initialize()
{
	this->ShieldType = ShieldTypeClass::FindOrAllocate(NONE_STR);
}

void TechnoTypeExt::ExtData::ApplyTurretOffset(Matrix3D* mtx, double factor)
{
	// Does not verify if the offset actually has all values parsed as it makes no difference, it will be 0 for the unparsed ones either way.
	auto offset = this->TurretOffset.GetEx();
	float x = static_cast<float>(offset->X * factor);
	float y = static_cast<float>(offset->Y * factor);
	float z = static_cast<float>(offset->Z * factor);

	mtx->Translate(x, y, z);
}

// Ares 0.A source
const char* TechnoTypeExt::ExtData::GetSelectionGroupID() const
{
	return GeneralUtils::IsValidString(this->GroupAs) ? this->GroupAs : this->OwnerObject()->ID;
}

const char* TechnoTypeExt::GetSelectionGroupID(ObjectTypeClass* pType)
{
	if (auto pExt = TechnoTypeExt::ExtMap.Find(static_cast<TechnoTypeClass*>(pType)))
		return pExt->GetSelectionGroupID();

	return pType->ID;
}

bool TechnoTypeExt::HasSelectionGroupID(ObjectTypeClass* pType, const char* pID)
{
	auto id = TechnoTypeExt::GetSelectionGroupID(pType);

	return (_strcmpi(id, pID) == 0);
}

void TechnoTypeExt::ExtData::ParseBurstFLHs(INI_EX& exArtINI, const char* pArtSection,
	std::vector<std::vector<CoordStruct>>& nFLH, std::vector<std::vector<CoordStruct>>& nEFlh, const char* pPrefixTag)
{
	char tempBuffer[32];
	char tempBufferFLH[48];
	auto pThis = this->OwnerObject();
	bool parseMultiWeapons = pThis->TurretCount > 0 && pThis->WeaponCount > 0;
	auto weaponCount = parseMultiWeapons ? pThis->WeaponCount : 2;
	nFLH.resize(weaponCount);
	nEFlh.resize(weaponCount);

	for (int i = 0; i < weaponCount; i++)
	{
		for (int j = 0; j < INT_MAX; j++)
		{
			_snprintf_s(tempBuffer, sizeof(tempBuffer), "%sWeapon%d", pPrefixTag, i + 1);
			auto prefix = parseMultiWeapons ? tempBuffer : i > 0 ? "%sSecondaryFire" : "%sPrimaryFire";
			_snprintf_s(tempBuffer, sizeof(tempBuffer), prefix, pPrefixTag);

			_snprintf_s(tempBufferFLH, sizeof(tempBufferFLH), "%sFLH.Burst%d", tempBuffer, j);
			Nullable<CoordStruct> FLH;
			FLH.Read(exArtINI, pArtSection, tempBufferFLH);

			_snprintf_s(tempBufferFLH, sizeof(tempBufferFLH), "Elite%sFLH.Burst%d", tempBuffer, j);
			Nullable<CoordStruct> eliteFLH;
			eliteFLH.Read(exArtINI, pArtSection, tempBufferFLH);

			if (FLH.isset() && !eliteFLH.isset())
				eliteFLH = FLH;
			else if (!FLH.isset() && !eliteFLH.isset())
				break;

			nFLH[i].push_back(FLH.Get());
			nEFlh[i].push_back(eliteFLH.Get());
		}
	}
}

//TODO: YRpp this with proper casting
TechnoTypeClass* TechnoTypeExt::GetTechnoType(ObjectTypeClass* pType)
{
	enum class IUnknownVtbl : DWORD
	{
		AircraftType = 0x7E2868,
		BuildingType = 0x7E4570,
		InfantryType = 0x7EB610,
		UnitType = 0x7F6218,
	};
	auto const vtThis = static_cast<IUnknownVtbl>(VTable::Get(pType));
	if (vtThis == IUnknownVtbl::AircraftType ||
		vtThis == IUnknownVtbl::BuildingType ||
		vtThis == IUnknownVtbl::InfantryType ||
		vtThis == IUnknownVtbl::UnitType)
	{
		return static_cast<TechnoTypeClass*>(pType);
	}

	return nullptr;
}

TechnoClass* TechnoTypeExt::CreateUnit(TechnoTypeClass* pType, CoordStruct location, DirType facing, DirType* secondaryFacing, HouseClass* pOwner, TechnoClass* pInvoker, HouseClass* pInvokerHouse,
	AnimTypeClass* pSpawnAnimType, int spawnHeight, bool alwaysOnGround, bool checkPathfinding, bool parachuteIfInAir, Mission mission, Mission* missionAI)
{
	auto const rtti = pType->WhatAmI();

	if (rtti == AbstractType::BuildingType)
		return nullptr;

	HouseClass* decidedOwner = pOwner && !pOwner->Defeated
		? pOwner : HouseClass::FindCivilianSide();

	auto pCell = MapClass::Instance->GetCellAt(location);
	auto const speedType = rtti != AbstractType::AircraftType ? pType->SpeedType : SpeedType::Wheel;
	auto const mZone = rtti != AbstractType::AircraftType ? pType->MovementZone : MovementZone::Normal;
	bool allowBridges = GroundType::Array[static_cast<int>(LandType::Clear)].Cost[static_cast<int>(speedType)] > 0.0;
	bool isBridge = allowBridges && pCell->ContainsBridge();
	int baseHeight = location.Z;
	bool inAir = location.Z >= Unsorted::CellHeight * 2;

	if (checkPathfinding && (!pCell || !pCell->IsClearToMove(speedType, false, false, -1, mZone, -1, isBridge)))
	{
		auto nCell = MapClass::Instance->NearByLocation(CellClass::Coord2Cell(location),
			speedType, -1, mZone, isBridge, 1, 1, true,
			false, false, isBridge, CellStruct::Empty, false, false);

		pCell = MapClass::Instance->TryGetCellAt(nCell);
		location = pCell->GetCoords();
	}

	if (pCell)
	{
		isBridge = allowBridges && pCell->ContainsBridge();
		int bridgeZ = isBridge ? CellClass::BridgeHeight : 0;
		int zCoord = alwaysOnGround ? INT32_MIN : baseHeight;
		int cellFloorHeight = MapClass::Instance->GetCellFloorHeight(location) + bridgeZ;

		if (!alwaysOnGround && spawnHeight >= 0)
			location.Z = cellFloorHeight + spawnHeight;
		else
			location.Z = Math::max(cellFloorHeight, zCoord);

		if (auto const pTechno = static_cast<FootClass*>(pType->CreateObject(decidedOwner)))
		{
			bool success = false;
			bool parachuted = false;
			pTechno->OnBridge = isBridge;

			if (rtti != AbstractType::AircraftType && parachuteIfInAir && !alwaysOnGround && inAir)
			{
				parachuted = true;
				success = pTechno->SpawnParachuted(location);
			}
			else if (!pCell->GetBuilding() || !checkPathfinding)
			{
				++Unsorted::IKnowWhatImDoing;
				success = pTechno->Unlimbo(location, facing);
				--Unsorted::IKnowWhatImDoing;
			}
			else
			{
				success = pTechno->Unlimbo(location, facing);
			}

			if (success)
			{
				if (secondaryFacing)
					pTechno->SecondaryFacing.SetCurrent(DirStruct(*secondaryFacing));

				if (pSpawnAnimType)
				{
					if (auto const pAnim = GameCreate<AnimClass>(pSpawnAnimType, location))
					{
						AnimExt::SetAnimOwnerHouseKind(pAnim, pInvokerHouse, nullptr, false, true);

						if (auto const pAnimExt = AnimExt::ExtMap.Find(pAnim))
							pAnimExt->SetInvoker(pInvoker, pInvokerHouse);
					}
				}

				if (!pTechno->InLimbo)
				{
					if (!alwaysOnGround)
					{
						inAir = pTechno->IsInAir();
						if (auto const pFlyLoco = locomotion_cast<FlyLocomotionClass*>(pTechno->Locomotor))
						{
							pTechno->SetLocation(location);
							bool airportBound = rtti == AbstractType::AircraftType && abstract_cast<AircraftTypeClass*>(pType)->AirportBound;

							if (pCell->GetContent() || airportBound)
								pTechno->EnterIdleMode(false, true);
							else
								pFlyLoco->Move_To(pCell->GetCoordsWithBridge());
						}
						else if (auto const pJJLoco = locomotion_cast<JumpjetLocomotionClass*>(pTechno->Locomotor))
						{
							pJJLoco->LocomotionFacing.SetCurrent(DirStruct(facing));

							if (pType->BalloonHover)
							{
								// Makes the jumpjet think it is hovering without actually moving.
								pJJLoco->State = JumpjetLocomotionClass::State::Hovering;
								pJJLoco->IsMoving = true;
								pJJLoco->DestinationCoords = location;
								pJJLoco->CurrentHeight = pType->JumpjetHeight;

								if (!inAir)
									AircraftTrackerClass::Instance->Add(pTechno);
							}
							else if (inAir)
							{
								// Order non-BalloonHover jumpjets to land.
								pJJLoco->Move_To(location);
							}
						}
						else if (inAir && !parachuted)
						{
							pTechno->IsFallingDown = true;
						}
					}

					auto newMission = mission;

					if (!decidedOwner->IsControlledByHuman() && missionAI)
						newMission = *missionAI;

					pTechno->QueueMission(mission, false);
				}

				if (!decidedOwner->Type->MultiplayPassive)
					decidedOwner->RecheckTechTree = true;
			}
			else
			{
				if (pTechno)
					pTechno->UnInit();
			}

			return pTechno;
		}
	}

	return nullptr;
}

DirStruct TechnoTypeExt::ExtData::GetTurretDesiredDir(DirStruct defaultDir)
{
	const auto turretExtraAngle = this->Turret_ExtraAngle.Get();

	if (!turretExtraAngle)
		return defaultDir;

	const auto rotate = DirStruct { static_cast<int>(turretExtraAngle * TechnoTypeExt::AngleToRaw + 0.5) };

	return DirStruct { static_cast<short>(defaultDir.Raw) + static_cast<short>(rotate.Raw) };
}

void TechnoTypeExt::ExtData::SetTurretLimitedDir(FootClass* pThis, DirStruct desiredDir)
{
	const auto turretRestrictAngle = this->Turret_Restriction.Get();
	const auto pBody = &pThis->PrimaryFacing;
	const auto pTurret = &pThis->SecondaryFacing;
	const auto destinationDir = this->GetTurretDesiredDir(desiredDir);

	if (turretRestrictAngle < 10.0)
	{
		const auto facing = pBody->Current();
		pTurret->SetCurrent(facing);
		pTurret->SetDesired(facing);

		if (!pThis->Locomotor->Is_Moving_Now())
			pBody->SetDesired(destinationDir);

		return;
	}

	if (turretRestrictAngle >= 180.0)
	{
		pTurret->SetDesired(destinationDir);
		return;
	}

	const auto desiredRaw = static_cast<short>(destinationDir.Raw);
	const auto turretRaw = static_cast<short>(pTurret->Current().Raw);
	const auto currentDir = pBody->Current();
	const auto bodyDir = this->GetTurretDesiredDir(currentDir);
	const auto bodyRaw = static_cast<short>(bodyDir.Raw);

	const auto currentDifference = static_cast<short>(turretRaw - bodyRaw);
	const auto desiredDifference = static_cast<short>(desiredRaw - bodyRaw);
	const auto restrictRaw = static_cast<short>(this->Turret_Restriction * TechnoTypeExt::AngleToRaw + 0.5);

	if (currentDifference < -restrictRaw)
	{
		const auto restrictDir = DirStruct {(bodyRaw - restrictRaw)};
		pTurret->SetCurrent(restrictDir);
		pTurret->SetDesired(restrictDir);
	}
	else if (currentDifference > restrictRaw)
	{
		const auto restrictDir = DirStruct {(bodyRaw + restrictRaw)};
		pTurret->SetCurrent(restrictDir);
		pTurret->SetDesired(restrictDir);
	}

	if ((desiredDifference < -restrictRaw || desiredDifference > restrictRaw) && !pThis->Locomotor->Is_Moving_Now())
		pBody->SetDesired(this->Turret_BodyOrientation ? this->GetBodyDesiredDir(currentDir, desiredDir) : desiredDir);

	pTurret->SetDesired(((currentDifference > 0 && desiredDifference < 0) || (currentDifference < 0 && desiredDifference > 0)) ? bodyDir : destinationDir);
}

short TechnoTypeExt::ExtData::GetTurretLimitedRaw(short currentDirectionRaw)
{
	const auto turretRestrictAngle = this->Turret_Restriction.Get();

	if (turretRestrictAngle < 0)
		return 0;

	if (turretRestrictAngle >= 180)
		return currentDirectionRaw;

	const auto restrictRaw = static_cast<short>(this->Turret_Restriction * TechnoTypeExt::AngleToRaw + 0.5);

	if (currentDirectionRaw < -restrictRaw)
		return -restrictRaw;

	if (currentDirectionRaw > restrictRaw)
		return restrictRaw;

	return currentDirectionRaw;
}

DirStruct TechnoTypeExt::ExtData::GetBodyDesiredDir(DirStruct currentDir, DirStruct defaultDir)
{
	const auto bodyAngle = this->Turret_BodyOrientationAngle.Get();

	if (!bodyAngle)
		return defaultDir;

	const auto rotateRaw = static_cast<short>(bodyAngle * TechnoTypeExt::AngleToRaw + 0.5);
	const auto rotate = DirStruct { this->GetTurretLimitedRaw(rotateRaw) };
	const auto rightDir = DirStruct { static_cast<short>(defaultDir.Raw) + static_cast<short>(rotate.Raw) };

	if (!this->Turret_BodyOrientationSymmetric)
		return rightDir;

	const auto leftDir = DirStruct { static_cast<short>(defaultDir.Raw) - static_cast<short>(rotate.Raw) };
	const auto rightDifference = static_cast<short>(static_cast<short>(rightDir.Raw) - static_cast<short>(currentDir.Raw));
	const auto leftDifference = static_cast<short>(static_cast<short>(leftDir.Raw) - static_cast<short>(currentDir.Raw));

	return (std::abs(rightDifference) < std::abs(leftDifference)) ? rightDir : leftDir;
}

// =============================
// load / save

void TechnoTypeExt::ExtData::LoadFromINIFile(CCINIClass* const pINI)
{
	auto pThis = this->OwnerObject();
	const char* pSection = pThis->ID;

	if (!pINI->GetSection(pSection))
		return;

	INI_EX exINI(pINI);

	this->HealthBar_Hide.Read(exINI, pSection, "HealthBar.Hide");
	this->UIDescription.Read(exINI, pSection, "UIDescription");
	this->LowSelectionPriority.Read(exINI, pSection, "LowSelectionPriority");
	this->MindControlRangeLimit.Read(exINI, pSection, "MindControlRangeLimit");
	this->FactoryPlant_Multiplier.Read(exINI, pSection, "FactoryPlant.Multiplier");

	this->Spawner_LimitRange.Read(exINI, pSection, "Spawner.LimitRange");
	this->Spawner_ExtraLimitRange.Read(exINI, pSection, "Spawner.ExtraLimitRange");
	this->Spawner_DelayFrames.Read(exINI, pSection, "Spawner.DelayFrames");
	this->Spawner_AttackImmediately.Read(exINI, pSection, "Spawner.AttackImmediately");

	this->Harvester_Counted.Read(exINI, pSection, "Harvester.Counted");
	if (!this->Harvester_Counted.isset() && pThis->Enslaves)
		this->Harvester_Counted = true;
	if (this->Harvester_Counted.Get())
		RulesExt::Global()->HarvesterTypes.AddUnique(pThis);

	this->Promote_IncludeSpawns.Read(exINI, pSection, "Promote.IncludeSpawns");
	this->ImmuneToCrit.Read(exINI, pSection, "ImmuneToCrit");
	this->MultiMindControl_ReleaseVictim.Read(exINI, pSection, "MultiMindControl.ReleaseVictim");
	this->NoManualMove.Read(exINI, pSection, "NoManualMove");
	this->InitialStrength.Read(exINI, pSection, "InitialStrength");
	if (this->InitialStrength.isset())
		this->InitialStrength = Math::clamp(this->InitialStrength, 1, pThis->Strength);

	this->ReloadInTransport.Read(exINI, pSection, "ReloadInTransport");
	this->ForbidParallelAIQueues.Read(exINI, pSection, "ForbidParallelAIQueues");
	this->ShieldType.Read<true>(exINI, pSection, "ShieldType");

	this->Ammo_AddOnDeploy.Read(exINI, pSection, "Ammo.AddOnDeploy");
	this->Ammo_AutoDeployMinimumAmount.Read(exINI, pSection, "Ammo.AutoDeployMinimumAmount");
	this->Ammo_AutoDeployMaximumAmount.Read(exINI, pSection, "Ammo.AutoDeployMaximumAmount");
	this->Ammo_DeployUnlockMinimumAmount.Read(exINI, pSection, "Ammo.DeployUnlockMinimumAmount");
	this->Ammo_DeployUnlockMaximumAmount.Read(exINI, pSection, "Ammo.DeployUnlockMaximumAmount");

	this->AutoDeath_Behavior.Read(exINI, pSection, "AutoDeath.Behavior");
	this->AutoDeath_VanishAnimation.Read(exINI, pSection, "AutoDeath.VanishAnimation");
	this->AutoDeath_OnAmmoDepletion.Read(exINI, pSection, "AutoDeath.OnAmmoDepletion");
	this->AutoDeath_AfterDelay.Read(exINI, pSection, "AutoDeath.AfterDelay");
	this->AutoDeath_TechnosDontExist.Read(exINI, pSection, "AutoDeath.TechnosDontExist");
	this->AutoDeath_TechnosDontExist_Any.Read(exINI, pSection, "AutoDeath.TechnosDontExist.Any");
	this->AutoDeath_TechnosDontExist_AllowLimboed.Read(exINI, pSection, "AutoDeath.TechnosDontExist.AllowLimboed");
	this->AutoDeath_TechnosDontExist_Houses.Read(exINI, pSection, "AutoDeath.TechnosDontExist.Houses");
	this->AutoDeath_TechnosExist.Read(exINI, pSection, "AutoDeath.TechnosExist");
	this->AutoDeath_TechnosExist_Any.Read(exINI, pSection, "AutoDeath.TechnosExist.Any");
	this->AutoDeath_TechnosExist_AllowLimboed.Read(exINI, pSection, "AutoDeath.TechnosExist.AllowLimboed");
	this->AutoDeath_TechnosExist_Houses.Read(exINI, pSection, "AutoDeath.TechnosExist.Houses");

	this->Slaved_OwnerWhenMasterKilled.Read(exINI, pSection, "Slaved.OwnerWhenMasterKilled");
	this->SlavesFreeSound.Read(exINI, pSection, "SlavesFreeSound");
	this->SellSound.Read(exINI, pSection, "SellSound");
	this->EVA_Sold.Read(exINI, pSection, "EVA.Sold");

	this->VoiceCreated.Read(exINI, pSection, "VoiceCreated");
	this->VoicePickup.Read(exINI, pSection, "VoicePickup");

	this->CameoPriority.Read(exINI, pSection, "CameoPriority");

	this->WarpOut.Read(exINI, pSection, "WarpOut");
	this->WarpIn.Read(exINI, pSection, "WarpIn");
	this->WarpAway.Read(exINI, pSection, "WarpAway");
	this->ChronoTrigger.Read(exINI, pSection, "ChronoTrigger");
	this->ChronoDistanceFactor.Read(exINI, pSection, "ChronoDistanceFactor");
	this->ChronoMinimumDelay.Read(exINI, pSection, "ChronoMinimumDelay");
	this->ChronoRangeMinimum.Read(exINI, pSection, "ChronoRangeMinimum");
	this->ChronoDelay.Read(exINI, pSection, "ChronoDelay");
	this->ChronoSpherePreDelay.Read(exINI, pSection, "ChronoSpherePreDelay");
	this->ChronoSphereDelay.Read(exINI, pSection, "ChronoSphereDelay");

	this->WarpInWeapon.Read<true>(exINI, pSection, "WarpInWeapon");
	this->WarpInMinRangeWeapon.Read<true>(exINI, pSection, "WarpInMinRangeWeapon");
	this->WarpOutWeapon.Read<true>(exINI, pSection, "WarpOutWeapon");
	this->WarpInWeapon_UseDistanceAsDamage.Read(exINI, pSection, "WarpInWeapon.UseDistanceAsDamage");

	exINI.ReadSpeed(pSection, "SubterraneanSpeed", &this->SubterraneanSpeed);
	this->SubterraneanHeight.Read(exINI, pSection, "SubterraneanHeight");

	this->OreGathering_Anims.Read(exINI, pSection, "OreGathering.Anims");
	this->OreGathering_Tiberiums.Read(exINI, pSection, "OreGathering.Tiberiums");
	this->OreGathering_FramesPerDir.Read(exINI, pSection, "OreGathering.FramesPerDir");

	this->DestroyAnim_Random.Read(exINI, pSection, "DestroyAnim.Random");
	this->NotHuman_RandomDeathSequence.Read(exINI, pSection, "NotHuman.RandomDeathSequence");

	this->DefaultDisguise.Read(exINI, pSection, "DefaultDisguise");
	this->UseDisguiseMovementSpeed.Read(exINI, pSection, "UseDisguiseMovementSpeed");

	this->OpenTopped_RangeBonus.Read(exINI, pSection, "OpenTopped.RangeBonus");
	this->OpenTopped_DamageMultiplier.Read(exINI, pSection, "OpenTopped.DamageMultiplier");
	this->OpenTopped_WarpDistance.Read(exINI, pSection, "OpenTopped.WarpDistance");
	this->OpenTopped_IgnoreRangefinding.Read(exINI, pSection, "OpenTopped.IgnoreRangefinding");
	this->OpenTopped_AllowFiringIfDeactivated.Read(exINI, pSection, "OpenTopped.AllowFiringIfDeactivated");
	this->OpenTopped_ShareTransportTarget.Read(exINI, pSection, "OpenTopped.ShareTransportTarget");
	this->OpenTopped_UseTransportRangeModifiers.Read(exINI, pSection, "OpenTopped.UseTransportRangeModifiers");
	this->OpenTopped_CheckTransportDisableWeapons.Read(exINI, pSection, "OpenTopped.CheckTransportDisableWeapons");

	this->AutoFire.Read(exINI, pSection, "AutoFire");
	this->AutoFire_TargetSelf.Read(exINI, pSection, "AutoFire.TargetSelf");

	this->AggressiveStance.Read(exINI, pSection, "AggressiveStance");
	this->AggressiveStance_Togglable.Read(exINI, pSection, "AggressiveStance.Togglable");
	this->VoiceEnterAggressiveStance.Read(exINI, pSection, "VoiceEnterAggressiveStance");
	this->VoiceExitAggressiveStance.Read(exINI, pSection, "VoiceExitAggressiveStance");

	this->NoSecondaryWeaponFallback.Read(exINI, pSection, "NoSecondaryWeaponFallback");
	this->NoSecondaryWeaponFallback_AllowAA.Read(exINI, pSection, "NoSecondaryWeaponFallback.AllowAA");

	this->JumpjetRotateOnCrash.Read(exINI, pSection, "JumpjetRotateOnCrash");
	this->ShadowSizeCharacteristicHeight.Read(exINI, pSection, "ShadowSizeCharacteristicHeight");

	this->DeployingAnim_AllowAnyDirection.Read(exINI, pSection, "DeployingAnim.AllowAnyDirection");
	this->DeployingAnim_KeepUnitVisible.Read(exINI, pSection, "DeployingAnim.KeepUnitVisible");
	this->DeployingAnim_ReverseForUndeploy.Read(exINI, pSection, "DeployingAnim.ReverseForUndeploy");
	this->DeployingAnim_UseUnitDrawer.Read(exINI, pSection, "DeployingAnim.UseUnitDrawer");

	this->EnemyUIName.Read(exINI, pSection, "EnemyUIName");
	this->ForceWeapon_Naval_Decloaked.Read(exINI, pSection, "ForceWeapon.Naval.Decloaked");
	this->ForceWeapon_Cloaked.Read(exINI, pSection, "ForceWeapon.Cloaked");
	this->ForceWeapon_Disguised.Read(exINI, pSection, "ForceWeapon.Disguised");
	this->Ammo_Shared.Read(exINI, pSection, "Ammo.Shared");
	this->Ammo_Shared_Group.Read(exINI, pSection, "Ammo.Shared.Group");
	this->SelfHealGainType.Read(exINI, pSection, "SelfHealGainType");
	this->Passengers_SyncOwner.Read(exINI, pSection, "Passengers.SyncOwner");
	this->Passengers_SyncOwner_RevertOnExit.Read(exINI, pSection, "Passengers.SyncOwner.RevertOnExit");

	this->IronCurtain_KeptOnDeploy.Read(exINI, pSection, "IronCurtain.KeptOnDeploy");
	this->IronCurtain_Effect.Read(exINI, pSection, "IronCurtain.Effect");
	this->IronCurtain_KillWarhead.Read<true>(exINI, pSection, "IronCurtain.KillWarhead");
	this->ForceShield_KeptOnDeploy.Read(exINI, pSection, "ForceShield.KeptOnDeploy");
	this->ForceShield_Effect.Read(exINI, pSection, "ForceShield.Effect");
	this->ForceShield_KillWarhead.Read<true>(exINI, pSection, "ForceShield.KillWarhead");

	this->Explodes_KillPassengers.Read(exINI, pSection, "Explodes.KillPassengers");
	this->Explodes_DuringBuildup.Read(exINI, pSection, "Explodes.DuringBuildup");
	this->DeployFireWeapon.Read(exINI, pSection, "DeployFireWeapon");
	this->TargetZoneScanType.Read(exINI, pSection, "TargetZoneScanType");

	this->Insignia.Read(exINI, pSection, "Insignia.%s");
	this->InsigniaFrames.Read(exINI, pSection, "InsigniaFrames");
	this->InsigniaFrame.Read(exINI, pSection, "InsigniaFrame.%s");
	this->Insignia_ShowEnemy.Read(exINI, pSection, "Insignia.ShowEnemy");

	this->TiltsWhenCrushes_Vehicles.Read(exINI, pSection, "TiltsWhenCrushes.Vehicles");
	this->TiltsWhenCrushes_Overlays.Read(exINI, pSection, "TiltsWhenCrushes.Overlays");
	this->CrushForwardTiltPerFrame.Read(exINI, pSection, "CrushForwardTiltPerFrame");
	this->CrushOverlayExtraForwardTilt.Read(exINI, pSection, "CrushOverlayExtraForwardTilt");
	this->CrushSlowdownMultiplier.Read(exINI, pSection, "CrushSlowdownMultiplier");

	this->DigitalDisplay_Disable.Read(exINI, pSection, "DigitalDisplay.Disable");
	this->DigitalDisplayTypes.Read(exINI, pSection, "DigitalDisplayTypes");

	this->AmmoPipFrame.Read(exINI, pSection, "AmmoPipFrame");
	this->EmptyAmmoPipFrame.Read(exINI, pSection, "EmptyAmmoPipFrame");
	this->AmmoPipWrapStartFrame.Read(exINI, pSection, "AmmoPipWrapStartFrame");
	this->AmmoPipSize.Read(exINI, pSection, "AmmoPipSize");
	this->AmmoPipOffset.Read(exINI, pSection, "AmmoPipOffset");

	this->ShowSpawnsPips.Read(exINI, pSection, "ShowSpawnsPips");
	this->SpawnsPipFrame.Read(exINI, pSection, "SpawnsPipFrame");
	this->EmptySpawnsPipFrame.Read(exINI, pSection, "EmptySpawnsPipFrame");
	this->SpawnsPipSize.Read(exINI, pSection, "SpawnsPipSize");
	this->SpawnsPipOffset.Read(exINI, pSection, "SpawnsPipOffset");

	this->SpawnDistanceFromTarget.Read(exINI, pSection, "SpawnDistanceFromTarget");
	this->SpawnHeight.Read(exINI, pSection, "SpawnHeight");
	this->LandingDir.Read(exINI, pSection, "LandingDir");

	this->Convert_HumanToComputer.Read(exINI, pSection, "Convert.HumanToComputer");
	this->Convert_ComputerToHuman.Read(exINI, pSection, "Convert.ComputerToHuman");

	this->CrateGoodie_RerollChance.Read(exINI, pSection, "CrateGoodie.RerollChance");

	this->Tint_Color.Read(exINI, pSection, "Tint.Color");
	this->Tint_Intensity.Read(exINI, pSection, "Tint.Intensity");
	this->Tint_VisibleToHouses.Read(exINI, pSection, "Tint.VisibleToHouses");

	this->RevengeWeapon.Read<true>(exINI, pSection, "RevengeWeapon");
	this->RevengeWeapon_AffectsHouses.Read(exINI, pSection, "RevengeWeapon.AffectsHouses");

	this->BuildLimitGroup_Types.Read(exINI, pSection, "BuildLimitGroup.Types");
	this->BuildLimitGroup_Nums.Read(exINI, pSection, "BuildLimitGroup.Nums");
	this->BuildLimitGroup_Factor.Read(exINI, pSection, "BuildLimitGroup.Factor");
	this->BuildLimitGroup_ContentIfAnyMatch.Read(exINI, pSection, "BuildLimitGroup.ContentIfAnyMatch");
	this->BuildLimitGroup_NotBuildableIfQueueMatch.Read(exINI, pSection, "BuildLimitGroup.NotBuildableIfQueueMatch");
	this->BuildLimitGroup_ExtraLimit_Types.Read(exINI, pSection, "BuildLimitGroup.ExtraLimit.Types");
	this->BuildLimitGroup_ExtraLimit_Nums.Read(exINI, pSection, "BuildLimitGroup.ExtraLimit.Nums");
	this->BuildLimitGroup_ExtraLimit_MaxCount.Read(exINI, pSection, "BuildLimitGroup.ExtraLimit.MaxCount");
	this->BuildLimitGroup_ExtraLimit_MaxNum.Read(exINI, pSection, "BuildLimitGroup.ExtraLimit.MaxNum");

	this->RecountBurst.Read(exINI, pSection, "RecountBurst");
	this->Turret_IdleRotate.Read(exINI, pSection, "Turret.IdleRotate");
	this->Turret_PointToMouse.Read(exINI, pSection, "Turret.PointToMouse");
	this->TurretROT.Read(exINI, pSection, "TurretROT");
	this->Turret_Restriction.Read(exINI, pSection, "Turret.Restriction");
	this->Turret_ExtraAngle.Read(exINI, pSection, "Turret.ExtraAngle");
	this->Turret_BodyOrientation.Read(exINI, pSection, "Turret.BodyOrientation");
	this->Turret_BodyOrientationAngle.Read(exINI, pSection, "Turret.BodyOrientationAngle");
	this->Turret_BodyOrientationSymmetric.Read(exINI, pSection, "Turret.BodyOrientationSymmetric");
	this->CanBeBuiltOn.Read(exINI, pSection, "CanBeBuiltOn");
	this->UnitBaseNormal.Read(exINI, pSection, "UnitBaseNormal");
	this->UnitBaseForAllyBuilding.Read(exINI, pSection, "UnitBaseForAllyBuilding");
	this->Cameo_AlwaysExist.Read(exINI, pSection, "Cameo.AlwaysExist");
	this->Cameo_AuxTechnos.Read(exINI, pSection, "Cameo.AuxTechnos");
	this->Cameo_NegTechnos.Read(exINI, pSection, "Cameo.NegTechnos");
	this->UIDescription_Unbuildable.Read(exINI, pSection, "UIDescription.Unbuildable");
	this->SelectedInfo_UpperType.Read(exINI, pSection, "SelectedInfo.UpperType");
	this->SelectedInfo_UpperColor.Read(exINI, pSection, "SelectedInfo.UpperColor");
	this->SelectedInfo_BelowType.Read(exINI, pSection, "SelectedInfo.BelowType");
	this->SelectedInfo_BelowColor.Read(exINI, pSection, "SelectedInfo.BelowColor");
	this->FakeOf.Read(exINI, pSection, "FakeOf");
	this->NoRearm_UnderEMP.Read(exINI, pSection, "NoRearm.UnderEMP");
	this->NoRearm_Temporal.Read(exINI, pSection, "NoRearm.Temporal");
	this->NoReload_UnderEMP.Read(exINI, pSection, "NoReload.UnderEMP");
	this->NoReload_Temporal.Read(exINI, pSection, "NoReload.Temporal");
	this->RateDown_Ammo.Read(exINI, pSection, "RateDown.Ammo");
	this->RateDown_Delay.Read(exINI, pSection, "RateDown.Delay");
	this->RateDown_Cover.Read(exINI, pSection, "RateDown.Cover");
	this->RateDown_Reset.Read(exINI, pSection, "RateDown.Reset");
	this->UniqueTechno.Read(exINI, pSection, "UniqueTechno");
	this->NoQueueUpToEnter.Read(exINI, pSection, "NoQueueUpToEnter");
	this->NoQueueUpToUnload.Read(exINI, pSection, "NoQueueUpToUnload");
	this->CanManualReload.Read(exINI, pSection, "CanManualReload");

	this->CombatAlert.Read(exINI, pSection, "CombatAlert");
	this->CombatAlert_NotBuilding.Read(exINI, pSection, "CombatAlert.NotBuilding");
	this->CombatAlert_UseFeedbackVoice.Read(exINI, pSection, "CombatAlert.UseFeedbackVoice");
	this->CombatAlert_UseAttackVoice.Read(exINI, pSection, "CombatAlert.UseAttackVoice");
	this->CombatAlert_UseEVA.Read(exINI, pSection, "CombatAlert.UseEVA");
	this->CombatAlert_EVA.Read(exINI, pSection, "CombatAlert.EVA");
	this->Spawner_RecycleRange.Read(exINI, pSection, "Spawner.RecycleRange");
	this->Spawner_RecycleAnim.Read(exINI, pSection, "Spawner.RecycleAnim");
	this->AINormalTargetingDelay.Read(exINI, pSection, "AINormalTargetingDelay");
	this->PlayerNormalTargetingDelay.Read(exINI, pSection, "PlayerNormalTargetingDelay");
	this->AIGuardAreaTargetingDelay.Read(exINI, pSection, "AIGuardAreaTargetingDelay");
	this->PlayerGuardAreaTargetingDelay.Read(exINI, pSection, "PlayerGuardAreaTargetingDelay");
	this->KeepWarping.Read(exINI, pSection, "KeepWarping");
	this->KeepWarping_Distance.Read(exINI, pSection, "KeepWarping.Distance");
	this->FiringByPassMovingCheck.Read(exINI, pSection, "FiringByPassMovingCheck");
	this->SkipCrushSlowdown.Read(exINI, pSection, "SkipCrushSlowdown");
	this->PlayerGuardModePursuit.Read(exINI, pSection, "PlayerGuardModePursuit");
	this->PlayerGuardModeStray.Read(exINI, pSection, "PlayerGuardModeStray");
	this->PlayerGuardModeGuardRangeMultiplier.Read(exINI, pSection, "PlayerGuardModeGuardRangeMultiplier");
	this->PlayerGuardModeGuardRangeAddend.Read(exINI, pSection, "PlayerGuardModeGuardRangeAddend");
	this->PlayerGuardStationaryStray.Read(exINI, pSection, "PlayerGuardStationaryStray");
	this->AIGuardModePursuit.Read(exINI, pSection, "AIGuardModePursuit");
	this->AIGuardModeStray.Read(exINI, pSection, "AIGuardModeStray");
	this->AIGuardModeGuardRangeMultiplier.Read(exINI, pSection, "AIGuardModeGuardRangeMultiplier");
	this->AIGuardModeGuardRangeAddend.Read(exINI, pSection, "AIGuardModeGuardRangeAddend");
	this->AIGuardStationaryStray.Read(exINI, pSection, "AIGuardStationaryStray");
	this->Engineer_CanAutoFire.Read(exINI, pSection, "Engineer.CanAutoFire");
	this->BunkerableAnyWay.Read(exINI, pSection, "BunkerableAnyWay");
	this->Harvester_CanGuardArea.Read(exINI, pSection, "Harvester.CanGuardArea");
	this->DigStartROT.Read(exINI, pSection, "DigStartROT");
	this->DigInSpeed.Read(exINI, pSection, "DigInSpeed");
	this->DigOutSpeed.Read(exINI, pSection, "DigOutSpeed");
	this->DigEndROT.Read(exINI, pSection, "DigEndROT");
	this->FlightClimb.Read(exINI, pSection, "FlightClimb");
	this->FlightCrash.Read(exINI, pSection, "FlightCrash");
	this->ExplodeOnDestroy.Read(exINI, pSection, "ExplodeOnDestroy");
	this->FireDeathWeaponOnCrushed.Read(exINI, pSection, "FireDeathWeaponOnCrushed");
	this->ExitCoord.Read(exINI, pSection, "ExitCoord");
	this->MissileSpawnUseOtherFLHs.Read(exINI, pSection, "MissileSpawnUseOtherFLHs");
	this->HarvesterQuickUnloader.Read(exINI, pSection, "HarvesterQuickUnloader");
	this->DistributeTargetingFrame.Read(exINI, pSection, "DistributeTargetingFrame");
	this->AttackMove_Follow.Read(exINI, pSection, "AttackMove.Follow");
	this->AttackMove_Follow_IncludeAir.Read(exINI, pSection, "AttackMove.Follow.IncludeAir");
	this->AttackMove_StopWhenTargetAcquired.Read(exINI, pSection, "AttackMove.StopWhenTargetAcquired");
	this->AttackMove_PursuitTarget.Read(exINI, pSection, "AttackMove.PursuitTarget");
	this->ThisIsAJumpjet.Read(exINI, pSection, "ThisIsAJumpjet");
	this->ImAJumpjetFromAirport.Read(exINI, pSection, "ImAJumpjetFromAirport");
	this->IgnoreRallyPoint.Read(exINI, pSection, "IgnoreRallyPoint");
	this->Sinkable.Read(exINI, pSection, "Sinkable");
	this->SinkSpeed.Read(exINI, pSection, "SinkSpeed");
	this->JumpjetSpeedType.Read(exINI, pSection, "JumpjetSpeedType");

	this->FallingDownDamage.Read(exINI, pSection, "FallingDownDamage");
	this->FallingDownDamage_Water.Read(exINI, pSection, "FallingDownDamage.Water");

	this->KeepTargetOnMove.Read(exINI, pSection, "KeepTargetOnMove");
	this->KeepTargetOnMove_ExtraDistance.Read(exINI, pSection, "KeepTargetOnMove.ExtraDistance");

	this->Wake.Read(exINI, pSection, "Wake");
	this->Wake_Grapple.Read(exINI, pSection, "Wake.Grapple");
	this->Wake_Sinking.Read(exINI, pSection, "Wake.Sinking");

	// Ares 0.2
	this->RadarJamRadius.Read(exINI, pSection, "RadarJamRadius");

	// Ares 0.9
	this->InhibitorRange.Read(exINI, pSection, "InhibitorRange");
	this->DesignatorRange.Read(exINI, pSection, "DesignatorRange");

	// Ares 0.A
	this->GroupAs.Read(pINI, pSection, "GroupAs");

	// Ares 0.C
	this->NoAmmoWeapon.Read(exINI, pSection, "NoAmmoWeapon");
	this->NoAmmoAmount.Read(exINI, pSection, "NoAmmoAmount");

	// Ares 3.0
	this->KeepAlive.Read(exINI, pSection, "KeepAlive");

	char tempBuffer[32];

	if (this->OwnerObject()->Gunner && this->Insignia_Weapon.empty())
	{
		int weaponCount = this->OwnerObject()->WeaponCount;
		this->Insignia_Weapon.resize(weaponCount);
		this->InsigniaFrame_Weapon.resize(weaponCount);
		this->InsigniaFrames_Weapon.resize(weaponCount);

		for (int i = 0; i < weaponCount; i++)
		{
			Promotable<SHPStruct*> insignia;
			_snprintf_s(tempBuffer, sizeof(tempBuffer), "Insignia.Weapon%d.%s", i + 1, "%s");
			insignia.Read(exINI, pSection, tempBuffer);
			this->Insignia_Weapon[i] = insignia;

			Promotable<int> frame = Promotable<int>(-1);
			_snprintf_s(tempBuffer, sizeof(tempBuffer), "InsigniaFrame.Weapon%d.%s", i + 1, "%s");
			frame.Read(exINI, pSection, tempBuffer);
			this->InsigniaFrame_Weapon[i] = frame;

			Valueable<Vector3D<int>> frames;
			frames = Vector3D<int>(-1, -1, -1);
			_snprintf_s(tempBuffer, sizeof(tempBuffer), "InsigniaFrames.Weapon%d", i + 1);
			frames.Read(exINI, pSection, tempBuffer);
			this->InsigniaFrames_Weapon[i] = frames;
		}
	}

	// Art tags
	INI_EX exArtINI(CCINIClass::INI_Art);
	auto pArtSection = pThis->ImageFile;

	this->CameoPCX.Read(&CCINIClass::INI_Art, pArtSection, "CameoPCX");
	this->GreyCameoPCX.Read(&CCINIClass::INI_Art, pArtSection, "GreyCameoPCX");
	this->CameoPal.LoadFromINI(&CCINIClass::INI_Art, pArtSection, "CameoPalette");

	this->TurretOffset.Read(exArtINI, pArtSection, "TurretOffset");
	this->TurretShadow.Read(exArtINI, pArtSection, "TurretShadow");
	ValueableVector<int> shadow_indices;
	shadow_indices.Read(exArtINI, pArtSection, "ShadowIndices");
	ValueableVector<int> shadow_indices_frame;
	shadow_indices_frame.Read(exArtINI, pArtSection, "ShadowIndices.Frame");
	if (shadow_indices_frame.size() != shadow_indices.size())
	{
		if (!shadow_indices_frame.empty())
			Debug::LogGame("[Developer warning] %s ShadowIndices.Frame size (%d) does not match ShadowIndices size (%d) \n"
				, pSection, shadow_indices_frame.size(), shadow_indices.size());
		shadow_indices_frame.resize(shadow_indices.size(), -1);
	}
	for (size_t i = 0; i < shadow_indices.size(); i++)
		this->ShadowIndices[shadow_indices[i]] = shadow_indices_frame[i];

	this->ShadowIndex_Frame.Read(exArtINI, pArtSection, "ShadowIndex.Frame");

	this->LaserTrailData.clear();
	for (size_t i = 0; ; ++i)
	{
		NullableIdx<LaserTrailTypeClass> trail;
		_snprintf_s(tempBuffer, sizeof(tempBuffer), "LaserTrail%d.Type", i);
		trail.Read(exArtINI, pArtSection, tempBuffer);

		if (!trail.isset())
			break;

		Valueable<CoordStruct> flh;
		_snprintf_s(tempBuffer, sizeof(tempBuffer), "LaserTrail%d.FLH", i);
		flh.Read(exArtINI, pArtSection, tempBuffer);

		Valueable<bool> isOnTurret;
		_snprintf_s(tempBuffer, sizeof(tempBuffer), "LaserTrail%d.IsOnTurret", i);
		isOnTurret.Read(exArtINI, pArtSection, tempBuffer);

		this->LaserTrailData.push_back({ ValueableIdx<LaserTrailTypeClass>(trail), flh, isOnTurret });
	}

	this->ParseBurstFLHs(exArtINI, pArtSection, this->WeaponBurstFLHs, this->EliteWeaponBurstFLHs, "");
	this->ParseBurstFLHs(exArtINI, pArtSection, this->DeployedWeaponBurstFLHs, this->EliteDeployedWeaponBurstFLHs, "Deployed");
	this->ParseBurstFLHs(exArtINI, pArtSection, this->CrouchedWeaponBurstFLHs, this->EliteCrouchedWeaponBurstFLHs, "Prone");

	this->OnlyUseLandSequences.Read(exArtINI, pArtSection, "OnlyUseLandSequences");

	this->PronePrimaryFireFLH.Read(exArtINI, pArtSection, "PronePrimaryFireFLH");
	this->ProneSecondaryFireFLH.Read(exArtINI, pArtSection, "ProneSecondaryFireFLH");
	this->DeployedPrimaryFireFLH.Read(exArtINI, pArtSection, "DeployedPrimaryFireFLH");
	this->DeployedSecondaryFireFLH.Read(exArtINI, pArtSection, "DeployedSecondaryFireFLH");

	for (size_t i = 0; ; i++)
	{
		Nullable<CoordStruct> alternateFLH;
		_snprintf_s(tempBuffer, sizeof(tempBuffer), "AlternateFLH%u", i);
		alternateFLH.Read(exArtINI, pArtSection, tempBuffer);

		// ww always read all of AlternateFLH0-5
		if (i >= 5U && !alternateFLH.isset())
			break;
		else if (!alternateFLH.isset())
			alternateFLH = this->OwnerObject()->Weapon[0].FLH; // Game defaults to this for AlternateFLH, not 0,0,0

		if (this->AlternateFLHs.size() < i)
			this->AlternateFLHs[i] = alternateFLH;
		else
			this->AlternateFLHs.push_back(alternateFLH);
	}

	// Parasitic types
	this->AttachEffects.LoadFromINI(pINI, pSection);

	auto [canParse, resetValue] = PassengerDeletionTypeClass::CanParse(exINI, pSection);

	if (canParse && !this->PassengerDeletionType)
		this->PassengerDeletionType = std::make_unique<PassengerDeletionTypeClass>(this->OwnerObject());

	if (this->PassengerDeletionType)
	{
		if (resetValue)
			this->PassengerDeletionType.reset();
		else
			this->PassengerDeletionType->LoadFromINI(pINI, pSection);
	}

	Nullable<bool> isInterceptor;
	isInterceptor.Read(exINI, pSection, "Interceptor");

	if (isInterceptor)
	{
		if (this->InterceptorType == nullptr)
			this->InterceptorType = std::make_unique<InterceptorTypeClass>(this->OwnerObject());

		this->InterceptorType->LoadFromINI(pINI, pSection);
	}
	else if (isInterceptor.isset())
	{
		this->InterceptorType.reset();
	}

	if (this->OwnerObject()->WhatAmI() == AbstractType::InfantryType)
	{
		if (this->DroppodType == nullptr)
			this->DroppodType = std::make_unique<DroppodTypeClass>();
		this->DroppodType->LoadFromINI(pINI, pSection);
	}
	else
	{
		this->DroppodType.reset();
	}

	if (GeneralUtils::IsValidString(pThis->PaletteFile) && !pThis->Palette)
		Debug::Log("[Developer warning] [%s] has Palette=%s set but no palette file was loaded (missing file or wrong filename). Missing palettes cause issues with lighting recalculations.\n", pArtSection, pThis->PaletteFile);
}

template <typename T>
void TechnoTypeExt::ExtData::Serialize(T& Stm)
{
	Stm
		.Process(this->HealthBar_Hide)
		.Process(this->UIDescription)
		.Process(this->LowSelectionPriority)
		.Process(this->MindControlRangeLimit)
		.Process(this->FactoryPlant_Multiplier)

		.Process(this->InterceptorType)

		.Process(this->GroupAs)
		.Process(this->RadarJamRadius)
		.Process(this->InhibitorRange)
		.Process(this->DesignatorRange)
		.Process(this->TurretOffset)
		.Process(this->TurretShadow)
		.Process(this->ShadowIndices)
		.Process(this->ShadowIndex_Frame)
		.Process(this->Spawner_LimitRange)
		.Process(this->Spawner_ExtraLimitRange)
		.Process(this->Spawner_DelayFrames)
		.Process(this->Spawner_AttackImmediately)
		.Process(this->Harvester_Counted)
		.Process(this->Promote_IncludeSpawns)
		.Process(this->ImmuneToCrit)
		.Process(this->MultiMindControl_ReleaseVictim)
		.Process(this->CameoPriority)
		.Process(this->NoManualMove)
		.Process(this->InitialStrength)
		.Process(this->ReloadInTransport)
		.Process(this->ForbidParallelAIQueues)
		.Process(this->ShieldType)
		.Process(this->PassengerDeletionType)

		.Process(this->Ammo_AddOnDeploy)
		.Process(this->Ammo_AutoDeployMinimumAmount)
		.Process(this->Ammo_AutoDeployMaximumAmount)
		.Process(this->Ammo_DeployUnlockMinimumAmount)
		.Process(this->Ammo_DeployUnlockMaximumAmount)

		.Process(this->AutoDeath_Behavior)
		.Process(this->AutoDeath_VanishAnimation)
		.Process(this->AutoDeath_OnAmmoDepletion)
		.Process(this->AutoDeath_AfterDelay)
		.Process(this->AutoDeath_TechnosDontExist)
		.Process(this->AutoDeath_TechnosDontExist_Any)
		.Process(this->AutoDeath_TechnosDontExist_AllowLimboed)
		.Process(this->AutoDeath_TechnosDontExist_Houses)
		.Process(this->AutoDeath_TechnosExist)
		.Process(this->AutoDeath_TechnosExist_Any)
		.Process(this->AutoDeath_TechnosExist_AllowLimboed)
		.Process(this->AutoDeath_TechnosExist_Houses)

		.Process(this->Slaved_OwnerWhenMasterKilled)
		.Process(this->SlavesFreeSound)
		.Process(this->SellSound)
		.Process(this->EVA_Sold)

		.Process(this->VoiceCreated)
		.Process(this->VoicePickup)

		.Process(this->WarpOut)
		.Process(this->WarpIn)
		.Process(this->WarpAway)
		.Process(this->ChronoTrigger)
		.Process(this->ChronoDistanceFactor)
		.Process(this->ChronoMinimumDelay)
		.Process(this->ChronoRangeMinimum)
		.Process(this->ChronoDelay)
		.Process(this->ChronoSpherePreDelay)
		.Process(this->ChronoSphereDelay)
		.Process(this->WarpInWeapon)
		.Process(this->WarpInMinRangeWeapon)
		.Process(this->WarpOutWeapon)
		.Process(this->WarpInWeapon_UseDistanceAsDamage)

		.Process(this->SubterraneanSpeed)
		.Process(this->SubterraneanHeight)

		.Process(this->OreGathering_Anims)
		.Process(this->OreGathering_Tiberiums)
		.Process(this->OreGathering_FramesPerDir)
		.Process(this->LaserTrailData)
		.Process(this->DestroyAnim_Random)
		.Process(this->NotHuman_RandomDeathSequence)
		.Process(this->DefaultDisguise)
		.Process(this->UseDisguiseMovementSpeed)
		.Process(this->WeaponBurstFLHs)
		.Process(this->EliteWeaponBurstFLHs)
		.Process(this->AlternateFLHs)

		.Process(this->OpenTopped_RangeBonus)
		.Process(this->OpenTopped_DamageMultiplier)
		.Process(this->OpenTopped_WarpDistance)
		.Process(this->OpenTopped_IgnoreRangefinding)
		.Process(this->OpenTopped_AllowFiringIfDeactivated)
		.Process(this->OpenTopped_ShareTransportTarget)
		.Process(this->OpenTopped_UseTransportRangeModifiers)
		.Process(this->OpenTopped_CheckTransportDisableWeapons)

		.Process(this->AutoFire)
		.Process(this->AutoFire_TargetSelf)

		.Process(this->AggressiveStance)
		.Process(this->AggressiveStance_Togglable)
		.Process(this->VoiceEnterAggressiveStance)
		.Process(this->VoiceExitAggressiveStance)

		.Process(this->NoSecondaryWeaponFallback)
		.Process(this->NoSecondaryWeaponFallback_AllowAA)
		.Process(this->NoAmmoWeapon)
		.Process(this->NoAmmoAmount)
		.Process(this->JumpjetRotateOnCrash)
		.Process(this->ShadowSizeCharacteristicHeight)
		.Process(this->DeployingAnim_AllowAnyDirection)
		.Process(this->DeployingAnim_KeepUnitVisible)
		.Process(this->DeployingAnim_ReverseForUndeploy)
		.Process(this->DeployingAnim_UseUnitDrawer)

		.Process(this->EnemyUIName)
		.Process(this->ForceWeapon_Naval_Decloaked)
		.Process(this->ForceWeapon_Cloaked)
		.Process(this->ForceWeapon_Disguised)
		.Process(this->Ammo_Shared)
		.Process(this->Ammo_Shared_Group)
		.Process(this->SelfHealGainType)
		.Process(this->Passengers_SyncOwner)
		.Process(this->Passengers_SyncOwner_RevertOnExit)

		.Process(this->OnlyUseLandSequences)

		.Process(this->PronePrimaryFireFLH)
		.Process(this->ProneSecondaryFireFLH)
		.Process(this->DeployedPrimaryFireFLH)
		.Process(this->DeployedSecondaryFireFLH)
		.Process(this->CrouchedWeaponBurstFLHs)
		.Process(this->EliteCrouchedWeaponBurstFLHs)
		.Process(this->DeployedWeaponBurstFLHs)
		.Process(this->EliteDeployedWeaponBurstFLHs)

		.Process(this->IronCurtain_KeptOnDeploy)
		.Process(this->IronCurtain_Effect)
		.Process(this->IronCurtain_KillWarhead)
		.Process(this->ForceShield_KeptOnDeploy)
		.Process(this->ForceShield_Effect)
		.Process(this->ForceShield_KillWarhead)

		.Process(this->Explodes_KillPassengers)
		.Process(this->Explodes_DuringBuildup)
		.Process(this->DeployFireWeapon)
		.Process(this->TargetZoneScanType)

		.Process(this->Insignia)
		.Process(this->InsigniaFrames)
		.Process(this->InsigniaFrame)
		.Process(this->Insignia_ShowEnemy)
		.Process(this->Insignia_Weapon)
		.Process(this->InsigniaFrame_Weapon)
		.Process(this->InsigniaFrames_Weapon)

		.Process(this->TiltsWhenCrushes_Vehicles)
		.Process(this->TiltsWhenCrushes_Overlays)
		.Process(this->CrushForwardTiltPerFrame)
		.Process(this->CrushOverlayExtraForwardTilt)
		.Process(this->CrushSlowdownMultiplier)

		.Process(this->DigitalDisplay_Disable)
		.Process(this->DigitalDisplayTypes)

		.Process(this->AmmoPipFrame)
		.Process(this->EmptyAmmoPipFrame)
		.Process(this->AmmoPipWrapStartFrame)
		.Process(this->AmmoPipSize)
		.Process(this->AmmoPipOffset)

		.Process(this->ShowSpawnsPips)
		.Process(this->SpawnsPipFrame)
		.Process(this->EmptySpawnsPipFrame)
		.Process(this->SpawnsPipSize)
		.Process(this->SpawnsPipOffset)

		.Process(this->SpawnDistanceFromTarget)
		.Process(this->SpawnHeight)
		.Process(this->LandingDir)
		.Process(this->DroppodType)

		.Process(this->Convert_HumanToComputer)
		.Process(this->Convert_ComputerToHuman)

		.Process(this->CrateGoodie_RerollChance)

		.Process(this->Tint_Color)
		.Process(this->Tint_Intensity)
		.Process(this->Tint_VisibleToHouses)

		.Process(this->RevengeWeapon)
		.Process(this->RevengeWeapon_AffectsHouses)

		.Process(this->AttachEffects)

		.Process(this->BuildLimitGroup_Types)
		.Process(this->BuildLimitGroup_Nums)
		.Process(this->BuildLimitGroup_Factor)
		.Process(this->BuildLimitGroup_ContentIfAnyMatch)
		.Process(this->BuildLimitGroup_NotBuildableIfQueueMatch)
		.Process(this->BuildLimitGroup_ExtraLimit_Types)
		.Process(this->BuildLimitGroup_ExtraLimit_Nums)
		.Process(this->BuildLimitGroup_ExtraLimit_MaxCount)
		.Process(this->BuildLimitGroup_ExtraLimit_MaxNum)

		.Process(this->RecountBurst)
		.Process(this->Turret_IdleRotate)
		.Process(this->Turret_PointToMouse)
		.Process(this->TurretROT)
		.Process(this->Turret_Restriction)
		.Process(this->Turret_ExtraAngle)
		.Process(this->Turret_BodyOrientation)
		.Process(this->Turret_BodyOrientationAngle)
		.Process(this->Turret_BodyOrientationSymmetric)
		.Process(this->CanBeBuiltOn)
		.Process(this->UnitBaseNormal)
		.Process(this->UnitBaseForAllyBuilding)
		.Process(this->Cameo_AlwaysExist)
		.Process(this->Cameo_AuxTechnos)
		.Process(this->Cameo_NegTechnos)
		.Process(this->CameoCheckMutex)
		.Process(this->UIDescription_Unbuildable)
		.Process(this->CameoPCX)
		.Process(this->GreyCameoPCX)
		.Process(this->SelectedInfo_UpperType)
		.Process(this->SelectedInfo_UpperColor)
		.Process(this->SelectedInfo_BelowType)
		.Process(this->SelectedInfo_BelowColor)
		.Process(this->FakeOf)
		.Process(this->CameoPal)
		.Process(this->NoRearm_UnderEMP)
		.Process(this->NoRearm_Temporal)
		.Process(this->NoReload_UnderEMP)
		.Process(this->NoReload_Temporal)
		.Process(this->RateDown_Ammo)
		.Process(this->RateDown_Delay)
		.Process(this->RateDown_Cover)
		.Process(this->RateDown_Reset)
		.Process(this->UniqueTechno)
		.Process(this->NoQueueUpToEnter)
		.Process(this->NoQueueUpToUnload)
		.Process(this->CanManualReload)

		.Process(this->CombatAlert)
		.Process(this->CombatAlert_NotBuilding)
		.Process(this->CombatAlert_UseFeedbackVoice)
		.Process(this->CombatAlert_UseAttackVoice)
		.Process(this->CombatAlert_UseEVA)
		.Process(this->CombatAlert_EVA)
		.Process(this->Spawner_RecycleRange)
		.Process(this->Spawner_RecycleAnim)
		.Process(this->AINormalTargetingDelay)
		.Process(this->PlayerNormalTargetingDelay)
		.Process(this->AIGuardAreaTargetingDelay)
		.Process(this->PlayerGuardAreaTargetingDelay)
		.Process(this->KeepWarping)
		.Process(this->KeepWarping_Distance)
		.Process(this->FiringByPassMovingCheck)
		.Process(this->SkipCrushSlowdown)
		.Process(this->PlayerGuardModePursuit)
		.Process(this->PlayerGuardModeStray)
		.Process(this->PlayerGuardModeGuardRangeMultiplier)
		.Process(this->PlayerGuardModeGuardRangeAddend)
		.Process(this->PlayerGuardStationaryStray)
		.Process(this->AIGuardModePursuit)
		.Process(this->AIGuardModeStray)
		.Process(this->AIGuardModeGuardRangeMultiplier)
		.Process(this->AIGuardModeGuardRangeAddend)
		.Process(this->AIGuardStationaryStray)
		.Process(this->Engineer_CanAutoFire)
		.Process(this->BunkerableAnyWay)
		.Process(this->Harvester_CanGuardArea)
		.Process(this->DigStartROT)
		.Process(this->DigInSpeed)
		.Process(this->DigOutSpeed)
		.Process(this->DigEndROT)
		.Process(this->FlightClimb)
		.Process(this->FlightCrash)
		.Process(this->ExplodeOnDestroy)
		.Process(this->FireDeathWeaponOnCrushed)
		.Process(this->ExitCoord)
		.Process(this->MissileSpawnUseOtherFLHs)
		.Process(this->HarvesterQuickUnloader)
		.Process(this->DistributeTargetingFrame)
		.Process(this->AttackMove_Follow)
		.Process(this->AttackMove_Follow_IncludeAir)
		.Process(this->AttackMove_StopWhenTargetAcquired)
		.Process(this->AttackMove_PursuitTarget)
		.Process(this->ThisIsAJumpjet)
		.Process(this->ImAJumpjetFromAirport)
		.Process(this->IgnoreRallyPoint)
		.Process(this->Sinkable)
		.Process(this->SinkSpeed)
		.Process(this->JumpjetSpeedType)

		.Process(this->KeepAlive)

		.Process(this->FallingDownDamage)
		.Process(this->FallingDownDamage_Water)

		.Process(this->KeepTargetOnMove)
		.Process(this->KeepTargetOnMove_ExtraDistance)

		.Process(this->Wake)
		.Process(this->Wake_Grapple)
		.Process(this->Wake_Sinking)
		;
}
void TechnoTypeExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<TechnoTypeClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void TechnoTypeExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<TechnoTypeClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

// =============================
// container

TechnoTypeExt::ExtContainer::ExtContainer() : Container("TechnoTypeClass") { }
TechnoTypeExt::ExtContainer::~ExtContainer() = default;

// =============================
// container hooks

DEFINE_HOOK(0x711835, TechnoTypeClass_CTOR, 0x5)
{
	GET(TechnoTypeClass*, pItem, ESI);

	TechnoTypeExt::ExtMap.TryAllocate(pItem);

	return 0;
}

DEFINE_HOOK(0x711AE0, TechnoTypeClass_DTOR, 0x5)
{
	GET(TechnoTypeClass*, pItem, ECX);

	TechnoTypeExt::ExtMap.Remove(pItem);

	return 0;
}

DEFINE_HOOK_AGAIN(0x716DC0, TechnoTypeClass_SaveLoad_Prefix, 0x5)
DEFINE_HOOK(0x7162F0, TechnoTypeClass_SaveLoad_Prefix, 0x6)
{
	GET_STACK(TechnoTypeClass*, pItem, 0x4);
	GET_STACK(IStream*, pStm, 0x8);

	TechnoTypeExt::ExtMap.PrepareStream(pItem, pStm);

	return 0;
}

DEFINE_HOOK(0x716DAC, TechnoTypeClass_Load_Suffix, 0xA)
{
	TechnoTypeExt::ExtMap.LoadStatic();

	return 0;
}

DEFINE_HOOK(0x717094, TechnoTypeClass_Save_Suffix, 0x5)
{
	TechnoTypeExt::ExtMap.SaveStatic();

	return 0;
}

DEFINE_HOOK_AGAIN(0x716132, TechnoTypeClass_LoadFromINI, 0x5)
DEFINE_HOOK(0x716123, TechnoTypeClass_LoadFromINI, 0x5)
{
	GET(TechnoTypeClass*, pItem, EBP);
	GET_STACK(CCINIClass*, pINI, 0x380);

	TechnoTypeExt::ExtMap.LoadFromINI(pItem, pINI);

	return 0;
}

#if ANYONE_ACTUALLY_USE_THIS
DEFINE_HOOK(0x679CAF, RulesClass_LoadAfterTypeData_CompleteInitialization, 0x5)
{
	//GET(CCINIClass*, pINI, ESI);

	for (auto const& [pType, pExt] : BuildingTypeExt::ExtMap)
	{
		pExt->CompleteInitialization();
	}

	return 0;
}
#endif

DEFINE_HOOK(0x747E90, UnitTypeClass_LoadFromINI, 0x5)
{
	GET(UnitTypeClass*, pItem, ESI);

	if (auto pTypeExt = TechnoTypeExt::ExtMap.Find(pItem))
	{
		if (!pTypeExt->Harvester_Counted.isset() && pItem->Harvester)
		{
			pTypeExt->Harvester_Counted = true;
			RulesExt::Global()->HarvesterTypes.AddUnique(pItem);
		}
	}

	return 0;
}
