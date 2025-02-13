#include "Body.h"

#include <EventClass.h>
#include <TerrainClass.h>
#include <AircraftClass.h>
#include <TacticalClass.h>
#include <IsometricTileTypeClass.h>

#include <Ext/House/Body.h>
#include <Ext/TechnoType/Body.h>
#include <Ext/TerrainType/Body.h>
#include <Ext/Scenario/Body.h>
#include <Utilities/EnumFunctions.h>

// Buildable-upon TerrainTypes Hook #2 -> sub_6D5730 - Draw laser fence placement even if they are on the way.
DEFINE_HOOK(0x6D57C1, TacticalClass_DrawLaserFencePlacement_BuildableTerrain, 0x9)
{
	enum { ContinueChecks = 0x6D57D2, DontDraw = 0x6D59A6 };

	GET(CellClass*, pCell, ESI);

	if (auto const pTerrain = pCell->GetTerrain(false))
	{
		auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

		if (pTypeExt->CanBeBuiltOn)
			return ContinueChecks;

		return DontDraw;
	}

	return ContinueChecks;
}

// Buildable-upon TerrainTypes Hook #3 -> sub_5683C0 - Remove them when buildings are placed on them.
// Buildable-upon TechnoTypes Hook #7 -> sub_5683C0 - Remove some of them when buildings are placed on them.
DEFINE_HOOK(0x5684B1, MapClass_PlaceDown_BuildableUponTypes, 0x6)
{
	GET(ObjectClass*, pObject, EDI);
	GET(CellClass*, pCell, EAX);

	if (pObject->WhatAmI() == AbstractType::Building)
	{
		auto pCellObject = pCell->FirstObject;

		while (pCellObject)
		{
			const auto absType = pCellObject->WhatAmI();

			if (absType == AbstractType::Infantry || absType == AbstractType::Unit || absType == AbstractType::Aircraft || absType == AbstractType::Building)
			{
				const auto pTechno = static_cast<TechnoClass*>(pCellObject);
				const auto pType = pTechno->GetTechnoType();
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					pTechno->KillPassengers(nullptr);
					pTechno->Stun();
					pTechno->Limbo();
					pTechno->UnInit();
				}
			}
			else if (const auto pTerrain = abstract_cast<TerrainClass*>(pCellObject))
			{
				const auto pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					pCell->RemoveContent(pTerrain, false);
					TerrainTypeExt::Remove(pTerrain);
				}
			}

			pCellObject = pCellObject->NextObject;
		}
	}

	return 0;
}

// Buildable-upon TerrainTypes Hook #4 -> sub_5FD270 - Allow placing buildings on top of them
DEFINE_HOOK(0x5FD2B6, OverlayClass_Unlimbo_SkipTerrainCheck, 0x9)
{
	enum { Unlimbo = 0x5FD2CA, NoUnlimbo = 0x5FD2C3 };

	GET(CellClass* const, pCell, EAX);

	if (!Game::IsActive)
		return Unlimbo;

	auto pCellObject = pCell->FirstObject;

	while (pCellObject)
	{
		if (const auto pTerrain = abstract_cast<TerrainClass*>(pCellObject))
		{
			const auto pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

			if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
				return NoUnlimbo;

			pCell->RemoveContent(pTerrain, false);
			TerrainTypeExt::Remove(pTerrain);
		}

		pCellObject = pCellObject->NextObject;
	}

	return Unlimbo;
}

// Buildable Proximity Helper
namespace ProximityTemp
{
	bool Build = false;
	bool Exist = false;
	bool Mouse = false;
	CellClass* CurrentCell = nullptr;
	BuildingTypeClass* BuildType = nullptr;
}

// BaseNormal extra checking Hook #1-1 -> sub_4A8EB0 - Set context and clear up data
DEFINE_HOOK(0x4A8F20, DisplayClass_BuildingProximityCheck_SetContext, 0x5)
{
	GET(BuildingTypeClass*, pType, ESI);

	if (RulesExt::Global()->PlacementGrid_Expand)
		ScenarioExt::Global()->BaseNormalCells.clear();

	ProximityTemp::Build = false;
	ProximityTemp::BuildType = pType;

	return 0;
}

// BaseNormal extra checking Hook #1-2 -> sub_4A8EB0 - Check allowed building
DEFINE_HOOK(0x4A8FD7, DisplayClass_BuildingProximityCheck_BuildArea, 0x6)
{
	enum { SkipBuilding = 0x4A902C };

	GET(BuildingClass*, pCellBuilding, ESI);

	auto const pTypeExt = BuildingTypeExt::ExtMap.Find(pCellBuilding->Type);

	if (pTypeExt->NoBuildAreaOnBuildup && pCellBuilding->CurrentMission == Mission::Construction)
		return SkipBuilding;

	auto const& pBuildingsAllowed = BuildingTypeExt::ExtMap.Find(ProximityTemp::BuildType)->Adjacent_Allowed;

	if (pBuildingsAllowed.size() > 0 && !pBuildingsAllowed.Contains(pCellBuilding->Type))
		return SkipBuilding;

	auto const& pBuildingsDisallowed = BuildingTypeExt::ExtMap.Find(ProximityTemp::BuildType)->Adjacent_Disallowed;

	if (pBuildingsDisallowed.size() > 0 && pBuildingsDisallowed.Contains(pCellBuilding->Type))
		return SkipBuilding;

	return 0;
}

// BaseNormal extra checking Hook #1-3 -> sub_4A8EB0 - Break loop or record cell for drawing
DEFINE_HOOK(0x4A902C, MapClass_PassesProximityCheck_BaseNormalExtra, 0x5)
{
	enum { CheckCompleted = 0x4A904E };

	REF_STACK(bool, canBuild, STACK_OFFSET(0x30, 0xC));

	if (canBuild)
	{
		if (!RulesExt::Global()->PlacementGrid_Expand)
			return CheckCompleted;

		canBuild = false; // Reset to false so that cells with BaseNormal can be correctly identified
		ProximityTemp::Build = true;

		GET_STACK(const CellStruct, currentCell, STACK_OFFSET(0x30, -0x20));
		ScenarioExt::Global()->BaseNormalCells.push_back(currentCell);
	}

	return 0;
}

// BaseNormal extra checking Hook #1-4 -> sub_4A8EB0 - Restore the correct result
DEFINE_HOOK(0x4A904E, MapClass_PassesProximityCheck_RestoreResult, 0x5)
{
	GET_STACK(bool, canBuild, STACK_OFFSET(0x30, 0xC));
	GET_STACK(const int, idxHouse, STACK_OFFSET(0x30, 0x8));

	const bool gridExpand = RulesExt::Global()->PlacementGrid_Expand;

	if (RulesExt::Global()->CheckExtraBaseNormal && (!canBuild || gridExpand))
	{
		const auto& baseNormalTechnos = ScenarioExt::Global()->BaseNormalTechnos;

		if (baseNormalTechnos.size())
		{
			GET(const int, topLeftX, EBP);
			GET_STACK(const int, foundationWidth, STACK_OFFSET(0x30, -0x1C));
			GET_STACK(const int, topLeftY, STACK_OFFSET(0x30, -0xC));
			GET_STACK(const int, foundationHeight, STACK_OFFSET(0x30, 0x4));

			const auto pBuildType = ProximityTemp::BuildType;
			const auto pBuildTypeExt = BuildingTypeExt::ExtMap.Find(ProximityTemp::BuildType);
			const auto range = pBuildType->Adjacent + 1;
			const auto maxX = topLeftX + range + foundationWidth;
			const auto maxY = topLeftY + range + foundationHeight;
			const auto minX = topLeftX - range;
			const auto minY = topLeftY - range;

			for (const auto& pExt : baseNormalTechnos)
			{
				const auto pTechno = pExt->OwnerObject();

				if (!TechnoExt::IsActive(pTechno) || pTechno->Deactivated)
					continue;

				const auto technoMapCell = pTechno->GetMapCoords();

				if (technoMapCell.X < minX || technoMapCell.Y < minY || technoMapCell.X >= maxX || technoMapCell.Y >= maxY)
					continue;

				const auto pOwner = pTechno->Owner;
				const auto pTypeExt = pExt->TypeExtData;

				auto canBeBaseNormal = [&]()
				{
					if (pOwner->ArrayIndex == idxHouse)
						return pTypeExt->ExtraBaseNormal.Get();
					else if (RulesClass::Instance->BuildOffAlly && pOwner->IsAlliedWith(HouseClass::Array->Items[idxHouse]))
						return pTypeExt->ExtraBaseForAllyBuilding.Get();

					return false;
				};

				if (!canBeBaseNormal())
					continue;

				const auto& pExtraAllowed = pBuildTypeExt->Adjacent_AllowedExtra;

				if (pExtraAllowed.size() > 0 && !pExtraAllowed.Contains(pTypeExt->OwnerObject()))
					continue;

				const auto& pExtraDisallowed = pBuildTypeExt->Adjacent_DisallowedExtra;

				if (pExtraDisallowed.size() > 0 && pExtraDisallowed.Contains(pTypeExt->OwnerObject()))
					continue;

				if (gridExpand)
				{
					ProximityTemp::Build = true;
					ScenarioExt::Global()->BaseNormalCells.push_back(technoMapCell);
				}
				else
				{
					R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
					break;
				}
			}
		}
	}

	if (gridExpand)
		R->Stack<bool>(STACK_OFFSET(0x30, 0xC), ProximityTemp::Build);

	return 0;
}

// BaseNormal for units Hook #2-1 -> sub_4AAC10 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4AACD9, MapClass_TacticalAction_BaseNormalRecheck, 0x5)
{
	return (RulesExt::Global()->CheckExtraBaseNormal && !(Unsorted::CurrentFrame % 8)) ? 0x4AACF5 : 0;
}

// BaseNormal for units Hook #2-2 -> sub_4A91B0 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4A9361, MapClass_CallBuildingPlaceCheck_BaseNormalRecheck, 0x5)
{
	return (RulesExt::Global()->CheckExtraBaseNormal && !(Unsorted::CurrentFrame % 8)) ? 0x4A9371 : 0;
}

// Buildable-upon TerrainTypes Hook #1 -> sub_47C620 - Allow placing buildings on top of them
// Buildable-upon TechnoTypes Hook #1 -> sub_47C620 - Rewrite and check whether allow placing buildings on top of them
// Customized Laser Fence Hook #1 -> sub_47C620 - Forbid placing laser fence post on inappropriate laser fence
// Fix DeploysInto Desync core Hook -> sub_47C620 - Exclude the specific unit who want to deploy
DEFINE_HOOK(0x47C640, CellClass_CanThisExistHere_IgnoreSomething, 0x6)
{
	enum { CanNotExistHere = 0x47C6D1, CanExistHere = 0x47C6A0 };

	GET(const CellClass* const, pCell, EDI);
	GET(const BuildingTypeClass* const, pBuildingType, EAX);
	GET_STACK(HouseClass* const, pOwner, STACK_OFFSET(0x18, 0xC));

	ProximityTemp::Exist = false;

	if (!Game::IsActive)
		return CanExistHere;

	const auto expand = RulesExt::Global()->ExtendedBuildingPlacing.Get();
	bool landFootOnly = false;

	if (pBuildingType->LaserFence)
	{
		for (auto pObject = pCell->FirstObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == AbstractType::Building)
			{
				const auto pBuilding = static_cast<BuildingClass*>(pObject);
				const auto pType = pBuilding->Type;
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
			else if (const auto pTerrain = abstract_cast<TerrainClass*>(pObject))
			{
				const auto pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
		}
	}
	else if (pBuildingType->LaserFencePost || pBuildingType->Gate)
	{
		bool skipFlag = TechnoExt::Deployer ? TechnoExt::Deployer->CurrentMapCoords == pCell->MapCoords : false;
		bool builtOnCanBeBuiltOn = false;

		for (auto pObject = pCell->FirstObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft)
			{
				const auto pAircraft = static_cast<AircraftClass*>(pObject);
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pAircraft->Type);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
					builtOnCanBeBuiltOn = true;
				else
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Building)
			{
				const auto pBuilding = static_cast<BuildingClass*>(pObject);
				const auto pType = pBuilding->Type;
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					builtOnCanBeBuiltOn = true;
				}
				else if (pOwner != pBuilding->Owner || !pType->LaserFence)
				{
					return CanNotExistHere;
				}
				else if (pBuildingType->LaserFencePost)
				{
					if (const auto pFenceType = BuildingTypeExt::ExtMap.Find(pBuildingType)->LaserFencePost_Fence.Get())
					{
						if (pFenceType != pType)
							return CanNotExistHere;
					}
					else // Vanilla search
					{
						const auto count = BuildingTypeClass::Array->Count;

						for (int i = 0; i < count; ++i)
						{
							const auto pSearchType = BuildingTypeClass::Array->Items[i];

							if (pSearchType->LaserFence)
							{
								if (pSearchType != pType)
									return CanNotExistHere;
								else
									break;
							}
						}
					}
				}
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				if (pObject == TechnoExt::Deployer)
				{
					skipFlag = true;
					continue;
				}

				const auto pTechno = static_cast<FootClass*>(pObject);
				const auto pTechnoType = pTechno->GetTechnoType();
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					if (pTechno->GetMapCoords() == pTechno->CurrentMapCoords)
						builtOnCanBeBuiltOn = true;
					else if (expand)
						landFootOnly = true;
					else
						return CanNotExistHere;
				}
				else if (!expand || pTechnoType->Speed <= 0 || !BuildingTypeExt::CheckOccupierCanLeave(pOwner, pTechno->Owner))
				{
					return CanNotExistHere;
				}
				else
				{
					landFootOnly = true;
				}
			}
			else if (const auto pTerrain = abstract_cast<TerrainClass*>(pObject))
			{
				const auto pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
					builtOnCanBeBuiltOn = true;
				else
					return CanNotExistHere;
			}
		}

		if (!landFootOnly && !builtOnCanBeBuiltOn && (pCell->OccupationFlags & (skipFlag ? 0x1F : 0x3F)))
		{
			if (expand)
				landFootOnly = true;
			else
				return CanNotExistHere;
		}
	}
	else if (pBuildingType->ToTile)
	{
		const auto isoTileTypeIndex = pCell->IsoTileTypeIndex;

		if (isoTileTypeIndex >= 0 && isoTileTypeIndex < IsometricTileTypeClass::Array->Count && !IsometricTileTypeClass::Array->Items[isoTileTypeIndex]->Morphable)
			return CanNotExistHere;

		for (auto pObject = pCell->FirstObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == AbstractType::Building)
			{
				const auto pBuilding = static_cast<BuildingClass*>(pObject);
				const auto pType = pBuilding->Type;
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
		}
	}
	else
	{
		bool skipFlag = TechnoExt::Deployer ? TechnoExt::Deployer->CurrentMapCoords == pCell->MapCoords : false;
		bool builtOnCanBeBuiltOn = false;

		for (auto pObject = pCell->FirstObject; pObject; pObject = pObject->NextObject)
		{
			const auto absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft || absType == AbstractType::Building)
			{
				const auto pTechno = static_cast<TechnoClass*>(pObject);
				const auto pTechnoType = pTechno->GetTechnoType();
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
					builtOnCanBeBuiltOn = true;
				else
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				if (pObject == TechnoExt::Deployer)
				{
					skipFlag = true;
					continue;
				}

				const auto pTechno = static_cast<FootClass*>(pObject);
				const auto pTechnoType = pTechno->GetTechnoType();
				const auto pTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					if (pTechno->GetMapCoords() == pTechno->CurrentMapCoords)
						builtOnCanBeBuiltOn = true;
					else if (expand)
						landFootOnly = true;
					else
						return CanNotExistHere;
				}
				else if (!expand || pTechnoType->Speed <= 0 || !BuildingTypeExt::CheckOccupierCanLeave(pOwner, pTechno->Owner))
				{
					return CanNotExistHere;
				}
				else
				{
					landFootOnly = true;
				}
			}
			else if (const auto pTerrain = abstract_cast<TerrainClass*>(pObject))
			{
				const auto pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
					builtOnCanBeBuiltOn = true;
				else
					return CanNotExistHere;
			}
		}

		if (!landFootOnly && !builtOnCanBeBuiltOn && (pCell->OccupationFlags & (skipFlag ? 0x1F : 0x3F)))
		{
			if (expand)
				landFootOnly = true;
			else
				return CanNotExistHere;
		}
	}

	if (landFootOnly)
		ProximityTemp::Exist = true;

	return CanExistHere; // Continue check the overlays .etc
}

// Buildable-upon TechnoTypes Hook #2-1 -> sub_47EC90 - Record cell before draw it then skip vanilla AltFlags check
DEFINE_HOOK(0x47EEBC, CellClass_DrawPlaceGrid_RecordCell, 0x6)
{
	enum { DontDrawAlt = 0x47EF1A, DrawVanillaAlt = 0x47EED6 };

	GET(CellClass* const, pCell, ESI);
	GET(const bool, zero, EDX);

	const BlitterFlags flags = BlitterFlags::Centered | BlitterFlags::bf_400;
	ProximityTemp::CurrentCell = pCell;

	if (!(pCell->AltFlags & AltCellFlags::ContainsBuilding))
	{
		if (!RulesExt::Global()->ExtendedBuildingPlacing)
		{
			R->EDX<BlitterFlags>(flags | (zero ? BlitterFlags::Zero : BlitterFlags::Nonzero));
			return DrawVanillaAlt;
		}
		else if (BuildingTypeClass* const pType = abstract_cast<BuildingTypeClass*>(DisplayClass::Instance->CurrentBuildingTypeCopy))
		{
			R->Stack<bool>(STACK_OFFSET(0x30, -0x1D), pCell->CanThisExistHere(pType->SpeedType, pType, HouseClass::CurrentPlayer));
			R->EDX<BlitterFlags>(flags | BlitterFlags::TransLucent75);
			return DontDrawAlt;
		}
	}

	R->EDX<BlitterFlags>(flags | (zero ? BlitterFlags::Zero : BlitterFlags::Nonzero));
	return DontDrawAlt;
}

// Buildable-upon TechnoTypes Hook #2-2 -> sub_47EC90 - Draw different color grid
DEFINE_HOOK(0x47EF52, CellClass_DrawPlaceGrid_DrawGrids, 0x6)
{
	const auto pRules = RulesExt::Global();

	if (!pRules->PlacementGrid_Expand)
		return 0;

	const auto pCell = ProximityTemp::CurrentCell;
	const auto cell = pCell->MapCoords;
	const auto pObj = DisplayClass::Instance->CurrentBuildingType;
	const auto range = static_cast<short>((pObj && pObj->WhatAmI() == AbstractType::BuildingType) ? static_cast<BuildingTypeClass*>(pObj)->Adjacent + 1 : 0);

	const auto maxX = static_cast<short>(cell.X + range);
	const auto maxY = static_cast<short>(cell.Y + range);
	const auto minX = static_cast<short>(cell.X - range);
	const auto minY = static_cast<short>(cell.Y - range);

	bool green = false;
	const auto& cells = ScenarioExt::Global()->BaseNormalCells;

	// Brute force
	for (const auto& baseCell : cells)
	{
		if (baseCell.X >= minX && baseCell.Y >= minY && baseCell.X <= maxX && baseCell.Y <= maxY)
		{
			green = true;
			break;
		}
	}

	const auto foot = ProximityTemp::Exist;
	ProximityTemp::Exist = false;
	const bool land = pCell->LandType != LandType::Water;
	const auto frames = land ? pRules->PlacementGrid_LandFrames.Get() : pRules->PlacementGrid_WaterFrames.Get();
	auto frame = foot ? frames.X : (green ? frames.Z : frames.Y);

	if (frame >= Make_Global<SHPStruct*>(0x8A03FC)->Frames) // place.shp
		frame = 0;

	R->EDI(frame);
	return 0;
}

static inline BuildingTypeClass* GetAnotherPlacingType(BuildingTypeClass* pType, CellStruct checkCell, bool opposite)
{
	if (!pType->PlaceAnywhere)
	{
		const auto pTypeExt = BuildingTypeExt::ExtMap.Find(pType);

		if (!pTypeExt->LimboBuild)
		{
			const auto onWater = MapClass::Instance->GetCellAt(checkCell)->LandType == LandType::Water;
			const auto waterBound = pType->SpeedType == SpeedType::Float;

			if (const auto pAnotherType = (opposite ^ onWater) ? (waterBound ? nullptr : pTypeExt->PlaceBuilding_OnWater) : (waterBound ? pTypeExt->PlaceBuilding_OnLand : nullptr))
			{
				if (pAnotherType->BuildCat == pType->BuildCat && !pAnotherType->PlaceAnywhere && !BuildingTypeExt::ExtMap.Find(pAnotherType)->LimboBuild)
					return pAnotherType;
			}
		}
	}

	return nullptr;
}

static inline BuildingTypeClass* GetAnotherPlacingType(DisplayClass* pDisplay)
{
	if (const auto pCurrentBuilding = abstract_cast<BuildingClass*>(pDisplay->CurrentBuilding))
		return GetAnotherPlacingType(pCurrentBuilding->Type, pDisplay->CurrentFoundation_CenterCell, false);

	return nullptr;
}

// Buildable-upon TechnoTypes Hook #3 -> sub_4FB0E0 - Hang up place event if there is only infantries and units on the cell
DEFINE_HOOK(0x4FB1EA, HouseClass_UnitFromFactory_HangUpPlaceEvent, 0x5)
{
	enum { CanBuild = 0x4FB23C, TemporarilyCanNotBuild = 0x4FB5BA, CanNotBuild = 0x4FB35F, BuildSucceeded = 0x4FB649 };

	GET(HouseClass* const, pHouse, EBP);
	GET(TechnoClass* const, pTechno, ESI);
	GET(FactoryClass* const, pPrimary, EBX);
	GET(BuildingClass* const, pFactory, EDI);
	GET_STACK(const CellStruct, topLeftCell, STACK_OFFSET(0x3C, 0x10));

	if (pTechno->WhatAmI() == AbstractType::Building)
	{
		const auto pDisplay = DisplayClass::Instance();
		auto pBuilding = static_cast<BuildingClass*>(pTechno);
		auto pBuildingType = pBuilding->Type;
		const auto pBufferBuilding = pBuilding;
		const auto pBufferType = pBuildingType;

		auto checkCell = topLeftCell;

		if (pBuildingType->Gate)
		{
			if (pBuildingType->GetFoundationWidth() > 2)
				checkCell.X += 1;
			else if (pBuildingType->GetFoundationHeight(false) > 2)
				checkCell.Y += 1;
		}
		else if (pBuildingType->GetFoundationWidth() > 2 || pBuildingType->GetFoundationHeight(false) > 2)
		{
			checkCell += CellStruct { 1, 1 };
		}

		if (const auto pOtherType = GetAnotherPlacingType(pBuildingType, checkCell, false))
		{
			pBuildingType = pOtherType;
		}
		else if (const auto pAnotherType = GetAnotherPlacingType(pBuildingType, topLeftCell, true)) // Center cell may be different, so make assumptions
		{
			checkCell = topLeftCell;

			if (pAnotherType->Gate)
			{
				if (pAnotherType->GetFoundationWidth() > 2)
					checkCell.X += 1;
				else if (pAnotherType->GetFoundationHeight(false) > 2)
					checkCell.Y += 1;
			}
			else if (pAnotherType->GetFoundationWidth() > 2 || pAnotherType->GetFoundationHeight(false) > 2)
			{
				checkCell += CellStruct { 1, 1 };
			}

			// If the land occupation of the two buildings is different, the larger one will prevail, And the smaller one may not be placed on the shore.
			if ((MapClass::Instance->GetCellAt(checkCell)->LandType == LandType::Water) ^ (pBuildingType->SpeedType == SpeedType::Float))
				pBuildingType = pAnotherType;
		}

		const auto pTypeExt = BuildingTypeExt::ExtMap.Find(pBuildingType);

		if (pTypeExt->LimboBuild)
		{
			BuildingTypeExt::CreateLimboBuilding(pBuilding, pBuildingType, pHouse, pTypeExt->LimboBuildID);

			if (pDisplay->CurrentBuilding == pBuilding && HouseClass::CurrentPlayer == pHouse)
			{
				pDisplay->SetActiveFoundation(nullptr);
				pDisplay->CurrentBuilding = nullptr;
				pDisplay->CurrentBuildingType = nullptr;
				pDisplay->CurrentBuildingOwnerArrayIndexCopy = -1;

				if (!Unsorted::ArmageddonMode)
				{
					reinterpret_cast<void(__thiscall*)(DisplayClass*, CellStruct*)>(0x4A8D50)(pDisplay, nullptr); // Clear CurrentFoundationCopy_Data
					pDisplay->CurrentBuildingCopy = nullptr;
					pDisplay->CurrentBuildingTypeCopy = nullptr;
				}
			}

			const auto pFactoryType = pFactory->Type;

			if (pFactoryType->ConstructionYard)
			{
				VocClass::PlayGlobal(RulesClass::Instance->BuildingSlam, 0x2000, 1.0);

				pFactory->DestroyNthAnim(BuildingAnimSlot::PreProduction);
				pFactory->DestroyNthAnim(BuildingAnimSlot::Idle);

				const bool damaged = pFactory->GetHealthPercentage() <= RulesClass::Instance->ConditionYellow;
				const auto pAnimName = damaged ? pFactoryType->BuildingAnim[8].Damaged : pFactoryType->BuildingAnim[8].Anim;

				if (pAnimName && *pAnimName)
					pFactory->PlayAnim(pAnimName, BuildingAnimSlot::Production, damaged, false);
			}

			return BuildSucceeded;
		}

		if (RulesExt::Global()->ExtendedBuildingPlacing && !pBuildingType->PlaceAnywhere && !pBuildingType->PowersUpBuilding[0])
		{
			const auto pHouseExt = HouseExt::ExtMap.Find(pHouse);
			bool canBuild = true;
			bool noOccupy = true;

			for (auto pFoundation = pBuildingType->GetFoundationData(false); *pFoundation != CellStruct { 0x7FFF, 0x7FFF }; ++pFoundation)
			{
				const auto currentCoord = topLeftCell + *pFoundation;
				const auto pCell = pDisplay->GetCellAt(currentCoord);

				if (!pCell->CanThisExistHere(pBuildingType->SpeedType, pBuildingType, pHouse))
				{
					canBuild = false;
					break;
				}
				else if (ProximityTemp::Exist)
				{
					noOccupy = false;
				}
			}

			ProximityTemp::Exist = false;

			do
			{
				if (canBuild)
				{
					if (noOccupy)
						break; // Can Build

					do
					{
						if (topLeftCell != pHouseExt->CurrentBuildingTopLeft || pBufferType != pHouseExt->CurrentBuildingType) // New command
						{
							pHouseExt->CurrentBuildingType = pBufferType;
							pHouseExt->CurrentBuildingDrawType = pBuildingType;
							pHouseExt->CurrentBuildingTimes = 30;
							pHouseExt->CurrentBuildingTopLeft = topLeftCell;
						}
						else if (pHouseExt->CurrentBuildingTimes <= 0)
						{
							break; // Time out
						}

						if (!(pHouseExt->CurrentBuildingTimes % 5) && BuildingTypeExt::CleanUpBuildingSpace(pBuildingType, topLeftCell, pHouse))
							break; // No place for cleaning

						if (pHouse == HouseClass::CurrentPlayer && pHouseExt->CurrentBuildingTimes == 30)
						{
							pDisplay->SetActiveFoundation(nullptr);
							pDisplay->CurrentBuilding = nullptr;
							pDisplay->CurrentBuildingType = nullptr;
							pDisplay->CurrentBuildingOwnerArrayIndexCopy = -1;

							if (!Unsorted::ArmageddonMode)
							{
								reinterpret_cast<void(__thiscall*)(DisplayClass*, CellStruct*)>(0x4A8D50)(pDisplay, nullptr); // Clear CurrentFoundationCopy_Data
								pDisplay->CurrentBuildingCopy = nullptr;
								pDisplay->CurrentBuildingTypeCopy = nullptr;
							}
						}

						--pHouseExt->CurrentBuildingTimes;
						pHouseExt->CurrentBuildingTimer.Start(8);

						return TemporarilyCanNotBuild;
					}
					while (false);
				}

				if (pHouseExt->CurrentBuildingType == pBufferType)
				{
					const bool noRevert = pHouseExt->CurrentBuildingTimes != 30;

					pHouseExt->CurrentBuildingType = nullptr;
					pHouseExt->CurrentBuildingDrawType = nullptr;
					pHouseExt->CurrentBuildingTimes = 0;
					pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
					pHouseExt->CurrentBuildingTimer.Stop();

					if (noRevert)
						return CanNotBuild;
				}

				ProximityTemp::Mouse = true;
				return CanNotBuild;
			}
			while (false);

			if (pHouseExt->CurrentBuildingType == pBufferType)
			{
				pHouseExt->CurrentBuildingType = nullptr;
				pHouseExt->CurrentBuildingDrawType = nullptr;
				pHouseExt->CurrentBuildingTimes = 0;
				pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
				pHouseExt->CurrentBuildingTimer.Stop();
			}
		}

		if (pBufferType != pBuildingType)
			pBuilding = static_cast<BuildingClass*>(pBuildingType->CreateObject(pHouse));

		pFactory->SendCommand(RadioCommand::RequestLink, pBuilding);

		if (pBuilding->Unlimbo(CoordStruct{ (topLeftCell.X << 8) + 128, (topLeftCell.Y << 8) + 128, 0 }, DirType::North))
		{
			if (pBufferBuilding != pBuilding)
			{
				if (HouseClass::CurrentPlayer == pHouse)
				{
					if (pDisplay->CurrentBuilding == pBufferBuilding)
						pDisplay->CurrentBuilding = pBuilding;

					if (pDisplay->CurrentBuildingType == pBufferType)
						pDisplay->CurrentBuildingType = pBuildingType;

					if (pDisplay->CurrentBuildingCopy == pBufferBuilding)
						pDisplay->CurrentBuildingCopy = pBuilding;

					if (pDisplay->CurrentBuildingTypeCopy == pBufferType)
						pDisplay->CurrentBuildingTypeCopy = pBuildingType;

					if (Make_Global<BuildingClass*>(0xB0FE5C) == pBufferBuilding) // Q
						Make_Global<BuildingClass*>(0xB0FE5C) = pBuilding;

					if (Make_Global<BuildingClass*>(0xB0FE60) == pBufferBuilding) // W
						Make_Global<BuildingClass*>(0xB0FE60) = pBuilding;
				}

				pBufferBuilding->UnInit();
				pPrimary->Object = pBuilding;
				R->ESI(pBuilding);
			}

			return CanBuild;
		}

		if (pBufferBuilding != pBuilding)
			pBuilding->UnInit();

		ProximityTemp::Mouse = true;
		return CanNotBuild;
	}

	pFactory->SendCommand(RadioCommand::RequestLink, pTechno);

	if (pTechno->Unlimbo(CoordStruct{ (topLeftCell.X << 8) + 128, (topLeftCell.Y << 8) + 128, 0 }, DirType::North))
		return CanBuild;

	ProximityTemp::Mouse = true;
	return CanNotBuild;
}

// Buildable-upon TechnoTypes Hook #4-1 -> sub_4FB0E0 - Check whether need to skip the replace command
DEFINE_HOOK(0x4FB395, HouseClass_UnitFromFactory_SkipMouseReturn, 0x6)
{
	enum { SkipGameCode = 0x4FB489, CheckMouseCoords = 0x4FB3E3 };

	if (!RulesExt::Global()->ExtendedBuildingPlacing)
		return 0;

	if (ProximityTemp::Mouse)
	{
		ProximityTemp::Mouse = false;
		return 0;
	}

	R->EBX(0);

	if (!DisplayClass::Instance->CurrentBuildingTypeCopy)
		return SkipGameCode;

	R->ECX(DisplayClass::Instance->CurrentBuildingTypeCopy);
	return CheckMouseCoords;
}

// Buildable-upon TechnoTypes Hook #4-2 -> sub_4FB0E0 - Check whether need to skip the clear command
DEFINE_HOOK(0x4FB339, HouseClass_UnitFromFactory_SkipMouseClear, 0x6)
{
	enum { SkipGameCode = 0x4FB4A0 };

	GET(TechnoClass* const, pTechno, ESI);

	if (RulesExt::Global()->ExtendedBuildingPlacing)
	{
		if (const auto pBuilding = abstract_cast<BuildingClass*>(pTechno))
		{
			if (const auto pCurrentType = abstract_cast<BuildingTypeClass*>(DisplayClass::Instance->CurrentBuildingType))
			{
				if (!BuildingTypeExt::IsSameBuildingType(pBuilding->Type, pCurrentType))
					return SkipGameCode;
			}
		}
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #4-3 -> sub_4FB0E0 - Check whether need to skip the clear command
DEFINE_HOOK(0x4FAB83, HouseClass_AbandonProductionOf_SkipMouseClear, 0x7)
{
	enum { SkipGameCode = 0x4FABA4 };

	GET(const int, index, EBX);

	if (RulesExt::Global()->ExtendedBuildingPlacing && index >= 0)
	{
		if (const auto pCurrentBuildingType = abstract_cast<BuildingTypeClass*>(DisplayClass::Instance->CurrentBuildingType))
		{
			if (!BuildingTypeExt::IsSameBuildingType(BuildingTypeClass::Array->Items[index], pCurrentBuildingType))
				return SkipGameCode;
		}
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #5 -> sub_4C9FF0 - Restart timer and clear buffer when abandon building production
DEFINE_HOOK(0x4CA05B, FactoryClass_AbandonProduction_AbandonCurrentBuilding, 0x5)
{
	GET(FactoryClass*, pFactory, ESI);

	if (RulesExt::Global()->ExtendedBuildingPlacing)
	{
		const auto pHouseExt = HouseExt::ExtMap.Find(pFactory->Owner);

		if (!pHouseExt || !pHouseExt->CurrentBuildingType)
			return 0;

		const auto pBuilding = abstract_cast<BuildingClass*>(pFactory->Object);

		if (!pBuilding || (pFactory->Owner->IsControlledByHuman() && pHouseExt->CurrentBuildingType != pBuilding->Type))
			return 0;

		pHouseExt->CurrentBuildingType = nullptr;
		pHouseExt->CurrentBuildingDrawType = nullptr;
		pHouseExt->CurrentBuildingTimes = 0;
		pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
		pHouseExt->CurrentBuildingTimer.Stop();
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #6 -> sub_443C60 - Try to clean up the building space when AI is building
DEFINE_HOOK(0x4451F8, BuildingClass_KickOutUnit_CleanUpAIBuildingSpace, 0x6)
{
	enum { CanBuild = 0x4452F0, TemporarilyCanNotBuild = 0x445237, CanNotBuild = 0x4454E6, BuildSucceeded = 0x4454D4, BuildFailed = 0x445696 };

	GET(BaseNodeClass* const, pBaseNode, EBX);
	GET(BuildingClass* const, pBuilding, EDI);
	GET(BuildingClass* const, pFactory, ESI);
	GET(const CellStruct, topLeftCell, EDX);

	const auto pBuildingType = pBuilding->Type;

	if (RulesExt::Global()->AIForbidConYard && pBuildingType->ConstructionYard)
	{
		if (pBaseNode)
		{
			pBaseNode->Placed = true;
			pBaseNode->Attempts = 0;
		}

		return BuildFailed;
	}

	const auto pHouse = pFactory->Owner;
	const auto pTypeExt = BuildingTypeExt::ExtMap.Find(pBuildingType);

	if (pTypeExt->LimboBuild)
	{
		BuildingTypeExt::CreateLimboBuilding(pBuilding, pBuildingType, pHouse, pTypeExt->LimboBuildID);

		if (pBaseNode)
		{
			pBaseNode->Placed = true;
			pBaseNode->Attempts = 0;

			if (pHouse->ProducingBuildingTypeIndex == pBuildingType->ArrayIndex)
				pHouse->ProducingBuildingTypeIndex = -1;
		}

		const auto pFactoryType = pFactory->Type;

		if (pFactoryType->ConstructionYard)
		{
			VocClass::PlayGlobal(RulesClass::Instance->BuildingSlam, 0x2000, 1.0);

			pFactory->DestroyNthAnim(BuildingAnimSlot::PreProduction);
			pFactory->DestroyNthAnim(BuildingAnimSlot::Idle);

			const bool damaged = pFactory->GetHealthPercentage() <= RulesClass::Instance->ConditionYellow;
			const auto pAnimName = damaged ? pFactoryType->BuildingAnim[8].Damaged : pFactoryType->BuildingAnim[8].Anim;

			if (pAnimName && *pAnimName)
				pFactory->PlayAnim(pAnimName, BuildingAnimSlot::Production, damaged, false);
		}

		return BuildSucceeded;
	}

	if (!RulesExt::Global()->ExtendedBuildingPlacing)
		return 0;

	if (topLeftCell != CellStruct::Empty && !pBuildingType->PlaceAnywhere)
	{
		if (!pBuildingType->PowersUpBuilding[0])
		{
			const auto pHouseExt = HouseExt::ExtMap.Find(pHouse);
			bool canBuild = true;
			bool noOccupy = true;

			for (auto pFoundation = pBuildingType->GetFoundationData(false); *pFoundation != CellStruct { 0x7FFF, 0x7FFF }; ++pFoundation)
			{
				const auto currentCoord = topLeftCell + *pFoundation;
				const auto pCell = MapClass::Instance->GetCellAt(currentCoord);

				if (!pCell->CanThisExistHere(pBuildingType->SpeedType, pBuildingType, pHouse))
				{
					canBuild = false;
					break;
				}
				else if (ProximityTemp::Exist)
				{
					noOccupy = false;
				}
			}

			ProximityTemp::Exist = false;

			do
			{
				if (canBuild)
				{
					if (noOccupy)
						break; // Can Build

					do
					{
						if (topLeftCell != pHouseExt->CurrentBuildingTopLeft || pBuildingType != pHouseExt->CurrentBuildingType) // New command
						{
							pHouseExt->CurrentBuildingType = pBuildingType;
							pHouseExt->CurrentBuildingDrawType = pBuildingType;
							pHouseExt->CurrentBuildingTopLeft = topLeftCell;
						}

						if (!pHouseExt->CurrentBuildingTimer.HasTimeLeft())
						{
							pHouseExt->CurrentBuildingTimer.Start(40);

							if (BuildingTypeExt::CleanUpBuildingSpace(pBuildingType, topLeftCell, pHouse))
								break; // No place for cleaning
						}

						return TemporarilyCanNotBuild;
					}
					while (false);
				}

				pHouseExt->CurrentBuildingType = nullptr;
				pHouseExt->CurrentBuildingDrawType = nullptr;
				pHouseExt->CurrentBuildingTimes = 0;
				pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
				pHouseExt->CurrentBuildingTimer.Stop();

				return CanNotBuild;
			}
			while (false);

			pHouseExt->CurrentBuildingType = nullptr;
			pHouseExt->CurrentBuildingDrawType = nullptr;
			pHouseExt->CurrentBuildingTimes = 0;
			pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
			pHouseExt->CurrentBuildingTimer.Stop();
		}
		else
		{
			const auto pCell = MapClass::Instance->GetCellAt(topLeftCell);
			const auto pCellBuilding = pCell->GetBuilding();

			if (!pCellBuilding || !reinterpret_cast<bool(__thiscall*)(BuildingClass*, BuildingTypeClass*, HouseClass*)>(0x452670)(pCellBuilding, pBuildingType, pHouse)) // CanUpgradeBuilding
				return CanNotBuild;
		}
	}

	if (pBuilding->Unlimbo(CoordStruct{ (topLeftCell.X << 8) + 128, (topLeftCell.Y << 8) + 128, 0 }, DirType::North))
	{
		const auto pFactoryType = pFactory->Type;

		if (pFactoryType->ConstructionYard)
		{
			pFactory->DestroyNthAnim(BuildingAnimSlot::PreProduction);
			pFactory->DestroyNthAnim(BuildingAnimSlot::Idle);

			const bool damaged = pFactory->GetHealthPercentage() <= RulesClass::Instance->ConditionYellow;
			const auto pAnimName = damaged ? pFactoryType->BuildingAnim[8].Damaged : pFactoryType->BuildingAnim[8].Anim;

			if (pAnimName && *pAnimName)
				pFactory->PlayAnim(pAnimName, BuildingAnimSlot::Production, damaged, false);
		}

		return CanBuild;
	}

	return CanNotBuild;
}

static inline bool CanDrawGrid(bool draw)
{
	if (ProximityTemp::Exist)
	{
		ProximityTemp::Exist = false;
		return false;
	}

	return draw;
}

// Laser fence use GetBuilding to check whether can build and draw, so no need to change
// Buildable-upon TechnoTypes Hook #8-1 -> sub_6D5C50 - Don't draw overlay wall grid when have occupiers
DEFINE_HOOK(0x6D5D38, TacticalClass_DrawOverlayWallGrid_DisableWhenHaveTechnos, 0x8)
{
	enum { Valid = 0x6D5D40, Invalid = 0x6D5F0F };

	GET(bool, valid, EAX);

	return CanDrawGrid(valid) ? Valid : Invalid;
}

// Buildable-upon TechnoTypes Hook #8-2 -> sub_6D59D0 - Don't draw firestorm wall grid when have occupiers
DEFINE_HOOK(0x6D5A9D, TacticalClass_DrawFirestormWallGrid_DisableWhenHaveTechnos, 0x8)
{
	enum { Valid = 0x6D5AA5, Invalid = 0x6D5C2F };

	GET(bool, valid, EAX);

	return CanDrawGrid(valid) ? Valid : Invalid;
}

// Buildable-upon TechnoTypes Hook #8-3 -> sub_588750 - Don't place overlay wall when have occupiers
DEFINE_HOOK(0x588873, MapClass_BuildingToWall_DisableWhenHaveTechnos, 0x8)
{
	enum { Valid = 0x58887B, Invalid = 0x588935 };

	GET(bool, valid, EAX);

	return CanDrawGrid(valid) ? Valid : Invalid;
}

// Buildable-upon TechnoTypes Hook #8-4 -> sub_588570 - Don't place firestorm wall when have occupiers
DEFINE_HOOK(0x588664, MapClass_BuildingToFirestormWall_DisableWhenHaveTechnos, 0x8)
{
	enum { Valid = 0x58866C, Invalid = 0x588730 };

	GET(bool, valid, EAX);

	return CanDrawGrid(valid) ? Valid : Invalid;
}

// Buildable-upon TechnoTypes Hook #9-1 -> sub_7393C0 - Try to clean up the building space when is deploying
DEFINE_HOOK(0x73946C, UnitClass_TryToDeploy_CleanUpDeploySpace, 0x6)
{
	enum { CanDeploy = 0x73958A, TemporarilyCanNotDeploy = 0x73953B, CanNotDeploy = 0x7394E0 };

	GET(UnitClass* const, pUnit, EBP);
	GET(CellStruct, topLeftCell, ESI);

	if (!RulesExt::Global()->ExtendedBuildingPlacing)
		return 0;

	const auto pTechnoExt = TechnoExt::ExtMap.Find(pUnit);
	const auto pBuildingType = pUnit->Type->DeploysInto;
	const auto pHouseExt = HouseExt::ExtMap.Find(pUnit->Owner);
	auto& vec = pHouseExt->OwnedDeployingUnits;

	if (pBuildingType->GetFoundationWidth() > 2 || pBuildingType->GetFoundationHeight(false) > 2)
		topLeftCell -= CellStruct { 1, 1 };

	R->Stack<CellStruct>(STACK_OFFSET(0x28, -0x14), topLeftCell);

	if (!pBuildingType->PlaceAnywhere)
	{
		bool canBuild = true;
		bool noOccupy = true;

		for (auto pFoundation = pBuildingType->GetFoundationData(false); *pFoundation != CellStruct { 0x7FFF, 0x7FFF }; ++pFoundation)
		{
			const auto currentCoord = topLeftCell + *pFoundation;
			const auto pCell = MapClass::Instance->GetCellAt(currentCoord);

			if (!pCell->CanThisExistHere(pBuildingType->SpeedType, pBuildingType, pUnit->Owner))
			{
				canBuild = false;
				break;
			}
			else if (ProximityTemp::Exist)
			{
				noOccupy = false;
			}
		}

		ProximityTemp::Exist = false;

		do
		{
			if (canBuild)
			{
				if (noOccupy)
					break; // Can build

				do
				{
					if (pTechnoExt && !pTechnoExt->UnitAutoDeployTimer.InProgress())
					{
						if (BuildingTypeExt::CleanUpBuildingSpace(pBuildingType, topLeftCell, pUnit->Owner, pUnit))
							break; // No place for cleaning

						if (vec.size() == 0 || std::find(vec.begin(), vec.end(), pUnit) == vec.end())
							vec.push_back(pUnit);

						pTechnoExt->UnitAutoDeployTimer.Start(40);
					}

					return TemporarilyCanNotDeploy;
				}
				while (false);
			}

			if (vec.size() > 0)
				vec.erase(std::remove(vec.begin(), vec.end(), pUnit), vec.end());

			if (pTechnoExt)
				pTechnoExt->UnitAutoDeployTimer.Stop();

			return CanNotDeploy;
		}
		while (false);
	}

	if (vec.size() > 0)
		vec.erase(std::remove(vec.begin(), vec.end(), pUnit), vec.end());

	if (pTechnoExt)
		pTechnoExt->UnitAutoDeployTimer.Stop();

	return CanDeploy;
}

// Buildable-upon TechnoTypes Hook #9-2 -> sub_73FD50 - Push the owner house into deploy check
DEFINE_HOOK(0x73FF8F, UnitClass_MouseOverObject_ShowDeployCursor, 0x6)
{
	if (RulesExt::Global()->ExtendedBuildingPlacing) // This IF check is not so necessary
	{
		GET(const UnitClass* const, pUnit, ESI);
		LEA_STACK(HouseClass**, pHousePtr, STACK_OFFSET(0x20, -0x20));
		*pHousePtr = pUnit->Owner;
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #10 -> sub_4C6CB0 - Stop deploy when get stop command
DEFINE_HOOK(0x4C7665, EventClass_RespondToEvent_StopDeployInIdleEvent, 0x6)
{
	if (RulesExt::Global()->ExtendedBuildingPlacing) // This IF check is not so necessary
	{
		GET(const UnitClass* const, pUnit, ESI);

		if (pUnit->Type->DeploysInto)
		{
			const auto mission = pUnit->CurrentMission;

			if (mission == Mission::Guard || mission == Mission::Unload)
			{
				if (const auto pHouseExt = HouseExt::ExtMap.Find(pUnit->Owner))
				{
					auto& vec = pHouseExt->OwnedDeployingUnits;

					if (vec.size() > 0)
						vec.erase(std::remove(vec.begin(), vec.end(), pUnit), vec.end());
				}
			}
		}
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #11 -> sub_4F8440 - Check whether can place again in each house
DEFINE_HOOK(0x4F8DB1, HouseClass_Update_CheckHangUpBuilding, 0x6)
{
	GET(const HouseClass* const, pHouse, ESI);

	if (!pHouse->IsControlledByHuman())
		return 0;

	if (pHouse == HouseClass::CurrentPlayer && (pHouse->RecheckTechTree || !(Unsorted::CurrentFrame() % 15)))
	{
		if (const auto pFactory = pHouse->Primary_ForBuildings)
		{
			if (pFactory->IsDone())
			{
				if (const auto pBuilding = abstract_cast<BuildingClass*>(pFactory->Object))
					BuildingTypeExt::AutoPlaceBuilding(pBuilding);
			}
		}

		if (const auto pFactory = pHouse->Primary_ForDefenses)
		{
			if (pFactory->IsDone())
			{
				if (const auto pBuilding = abstract_cast<BuildingClass*>(pFactory->Object))
					BuildingTypeExt::AutoPlaceBuilding(pBuilding);
			}
		}
	}

	if (!RulesExt::Global()->ExtendedBuildingPlacing)
		return 0;

	if (const auto pHouseExt = HouseExt::ExtMap.Find(pHouse))
	{
		if (pHouse == HouseClass::CurrentPlayer) // Prevent unexpected wrong event
		{
			const auto pBuildingType = pHouseExt->CurrentBuildingType;

			if (pHouseExt->CurrentBuildingTimer.Completed() && pBuildingType)
			{
				pHouseExt->CurrentBuildingTimer.Stop();
				const EventClass event
				(
					pHouse->ArrayIndex,
					EventType::Place,
					AbstractType::Building,
					pBuildingType->GetArrayIndex(),
					pBuildingType->Naval,
					pHouseExt->CurrentBuildingTopLeft
				);
				EventClass::AddEvent(event);
			}
		}

		if (pHouseExt->OwnedDeployingUnits.size() > 0)
		{
			auto& vec = pHouseExt->OwnedDeployingUnits;

			for (auto it = vec.begin(); it != vec.end(); )
			{
				const auto pUnit = *it;

				if (!pUnit->InLimbo && pUnit->IsOnMap && !pUnit->IsSinking && pUnit->Owner == pHouse && !pUnit->Destination && pUnit->CurrentMission == Mission::Guard && !pUnit->ParasiteEatingMe && !pUnit->TemporalTargetingMe)
				{
					if (const auto pType = pUnit->Type)
					{
						if (pType->DeploysInto)
						{
							if (const auto pExt = TechnoExt::ExtMap.Find(pUnit))
							{
								if (!(pExt->UnitAutoDeployTimer.GetTimeLeft() % 8))
									pUnit->QueueMission(Mission::Unload, true);

								++it;
								continue;
							}
						}
					}
				}

				it = vec.erase(it);
			}
		}
	}

	return 0;
}

DEFINE_HOOK_AGAIN(0x4ABA47, DisplayClass_PreparePassesProximityCheck_ReplaceBuildingType, 0x6)
DEFINE_HOOK(0x4A946E, DisplayClass_PreparePassesProximityCheck_ReplaceBuildingType, 0x6)
{
	const auto pDisplay = DisplayClass::Instance();

	if (const auto pAnotherType = GetAnotherPlacingType(pDisplay))
	{
		if (pDisplay->CurrentBuildingType && pDisplay->CurrentBuildingType != pAnotherType)
		{
			pDisplay->CurrentBuildingType = pAnotherType;
			pDisplay->SetActiveFoundation(pAnotherType->GetFoundationData(true));
		}
	}
	else if (const auto pCurrentBuilding = abstract_cast<BuildingClass*>(pDisplay->CurrentBuilding))
	{
		if (pDisplay->CurrentBuildingType && pDisplay->CurrentBuildingType != pCurrentBuilding->Type)
		{
			pDisplay->CurrentBuildingType = pCurrentBuilding->Type;
			pDisplay->SetActiveFoundation(pCurrentBuilding->Type->GetFoundationData(true));
		}
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #12 -> sub_6D5030 - Draw the placing building preview
DEFINE_HOOK(0x6D504C, TacticalClass_DrawPlacement_DrawPlacingPreview, 0x6)
{
	if (!RulesExt::Global()->ExtendedBuildingPlacing)
		return 0;

	const auto pPlayer = HouseClass::CurrentPlayer();
	const auto pDisplay = DisplayClass::Instance();

	for (const auto& pHouse : *HouseClass::Array)
	{
		if (pPlayer->IsObserver() || pHouse->IsAlliedWith(pPlayer))
		{
			const auto pHouseExt = HouseExt::ExtMap.Find(pHouse);
			const bool isPlayer = pHouse == pPlayer;
			{
				auto pType = pHouseExt->CurrentBuildingDrawType;

				if (pType || (isPlayer && (pType = abstract_cast<BuildingTypeClass*>(pDisplay->CurrentBuildingTypeCopy), pType)))
				{
					auto pCell = pDisplay->TryGetCellAt(pHouseExt->CurrentBuildingTopLeft);

					if (pCell || (isPlayer && (pCell = pDisplay->TryGetCellAt(pDisplay->CurrentFoundationCopy_TopLeftOffset + pDisplay->CurrentFoundationCopy_CenterCell), pCell)))
					{
						auto pImage = pType->LoadBuildup();
						int imageFrame = 0;

						if (pImage)
							imageFrame = ((pImage->Frames / 2) - 1);
						else
							pImage = pType->GetImage();

						if (pImage)
						{
							BlitterFlags blitFlags = BlitterFlags::TransLucent75 | BlitterFlags::Centered | BlitterFlags::Nonzero | BlitterFlags::MultiPass;
							auto rect = DSurface::Temp->GetRect();
							rect.Height -= 32;
							auto point = TacticalClass::Instance->CoordsToClient(CellClass::Cell2Coord(pCell->MapCoords, (1 + pCell->GetFloorHeight(Point2D::Empty)))).first;
							point.Y -= 15;

							const auto ColorSchemeIndex = pHouse->ColorSchemeIndex;
							const auto Palettes = pType->Palette;
							const auto pColor = Palettes ? Palettes->Items[ColorSchemeIndex] : ColorScheme::Array->Items[ColorSchemeIndex];

							DSurface::Temp->DrawSHP(pColor->LightConvert, pImage, imageFrame, &point, &rect, blitFlags, 0, 0, ZGradient::Ground, 1000, 0, nullptr, 0, 0, 0);
						}
					}
				}
			}

			if (pHouseExt->OwnedDeployingUnits.size() > 0)
			{
				for (const auto& pUnit : pHouseExt->OwnedDeployingUnits)
				{
					if (pUnit && pUnit->Type)
					{
						if (const auto pType = pUnit->Type->DeploysInto)
						{
							auto displayCell = CellClass::Coord2Cell(pUnit->GetCoords()); // pUnit->GetMapCoords();

							if (pType->GetFoundationWidth() > 2 || pType->GetFoundationHeight(false) > 2)
								displayCell -= CellStruct { 1, 1 };

							if (const auto pCell = pDisplay->TryGetCellAt(displayCell))
							{
								auto pImage = pType->LoadBuildup();
								int imageFrame = 0;

								if (pImage)
									imageFrame = ((pImage->Frames / 2) - 1);
								else
									pImage = pType->GetImage();

								if (pImage)
								{
									BlitterFlags blitFlags = BlitterFlags::TransLucent75 | BlitterFlags::Centered | BlitterFlags::Nonzero | BlitterFlags::MultiPass;
									auto rect = DSurface::Temp->GetRect();
									rect.Height -= 32;
									auto point = TacticalClass::Instance->CoordsToClient(CellClass::Cell2Coord(pCell->MapCoords, (1 + pCell->GetFloorHeight(Point2D::Empty)))).first;
									point.Y -= 15;

									const auto ColorSchemeIndex = pUnit->Owner->ColorSchemeIndex;
									const auto Palettes = pType->Palette;
									const auto pColor = Palettes ? Palettes->Items[ColorSchemeIndex] : ColorScheme::Array->Items[ColorSchemeIndex];

									DSurface::Temp->DrawSHP(pColor->LightConvert, pImage, imageFrame, &point, &rect, blitFlags, 0, 0, ZGradient::Ground, 1000, 0, nullptr, 0, 0, 0);
								}
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

// Auto Build Hook -> sub_6A8B30 - Auto Build Buildings
DEFINE_HOOK(0x6A8E34, StripClass_Update_AutoBuildBuildings, 0x7)
{
	GET(BuildingClass* const, pBuilding, ESI);

	BuildingTypeExt::BuildLimboBuilding(pBuilding);
	BuildingTypeExt::AutoPlaceBuilding(pBuilding);

	return 0;
}

// Limbo Build Hook -> sub_42EB50 - Check Base Node
DEFINE_HOOK(0x42EB8E, BaseClass_GetBaseNodeIndex_CheckValidBaseNode, 0x6)
{
	enum { Valid = 0x42EBC3, Invalid = 0x42EBAE };

	GET(BaseClass* const, pBase, ESI);
	GET(BaseNodeClass* const, pBaseNode, EAX);

	if (pBaseNode->Placed)
	{
		const auto index = pBaseNode->BuildingTypeIndex;

		if (index >= 0 && index < BuildingTypeClass::Array->Count)
		{
			const auto pType = BuildingTypeClass::Array->Items[index];

			if ((pType->ConstructionYard && RulesExt::Global()->AIForbidConYard) || BuildingTypeExt::ExtMap.Find(pType)->LimboBuild)
				return Invalid;
		}
	}

	return reinterpret_cast<bool(__thiscall*)(HouseClass*, BaseNodeClass*)>(0x50CAD0)(pBase->Owner, pBaseNode) ? Valid : Invalid;
}

// Customized Laser Fence Hook #2 -> sub_453060 - Select the specific laser fence type
DEFINE_HOOK(0x452E2C, BuildingClass_CreateLaserFence_FindSpecificIndex, 0x5)
{
	enum { SkipGameCode = 0x452E50 };

	GET(BuildingClass* const, pThis, EDI);

	if (const auto pExt = BuildingTypeExt::ExtMap.Find(pThis->Type))
	{
		if (const auto pFenceType = pExt->LaserFencePost_Fence.Get())
		{
			if (pFenceType->LaserFence)
			{
				R->EBP(pFenceType->ArrayIndex);
				R->EAX(BuildingTypeClass::Array->Count);
				return SkipGameCode;
			}
		}
	}

	return 0;
}

// Customized Laser Fence Hook #3 -> sub_440580 - Skip uninit inappropriate laser fence
DEFINE_HOOK(0x440AE9, BuildingClass_Unlimbo_SkipUninitFence, 0x7)
{
	enum { SkipGameCode = 0x440B07 };

	GET(BuildingClass* const, pFence, EDI);
	GET(BuildingClass* const, pThis, ESI);

	if (const auto pExt = BuildingTypeExt::ExtMap.Find(pThis->Type))
	{
		if (const auto pFenceType = pExt->LaserFencePost_Fence.Get())
		{
			if (pFenceType != pFence->Type)
				return SkipGameCode;
		}
		else // Vanilla search
		{
			const auto count = BuildingTypeClass::Array->Count;

			for (int i = 0; i < count; ++i)
			{
				const auto pSearchType = BuildingTypeClass::Array->Items[i];

				if (pSearchType->LaserFence)
				{
					if (pSearchType != pFence->Type)
						return SkipGameCode;
					else
						break;
				}
			}
		}
	}

	return 0;
}

// Customized Laser Fence Hook #4 -> sub_452BB0 - Only accept specific fence post
DEFINE_HOOK(0x452CB4, BuildingClass_FindLaserFencePost_CheckLaserFencePost, 0x7)
{
	enum { SkipGameCode = 0x452D2C };

	GET(BuildingClass* const, pPost, ESI);
	GET(BuildingClass* const, pThis, EDI);

	if (const auto pThisTypeExt = BuildingTypeExt::ExtMap.Find(pThis->Type))
	{
		if (const auto pPostTypeExt = BuildingTypeExt::ExtMap.Find(pPost->Type))
		{
			if (pThisTypeExt->LaserFencePost_Fence.Get() != pPostTypeExt->LaserFencePost_Fence.Get())
				return SkipGameCode;
		}
	}

	return 0;
}

// Customized Laser Fence Hook #5 -> sub_6D5730 - Break draw inappropriate laser fence grids
DEFINE_HOOK(0x6D5815, TacticalClass_DrawLaserFenceGrid_SkipDrawLaserFence, 0x6)
{
	enum { SkipGameCode = 0x6D59A6 };

	GET(BuildingTypeClass* const, pPostType, ECX);

	// Have used CurrentBuilding->Type yet, so simply use static_cast
	if (const auto pPlaceTypeExt = BuildingTypeExt::ExtMap.Find(static_cast<BuildingClass*>(DisplayClass::Instance->CurrentBuilding)->Type))
	{
		if (const auto pPostTypeExt = BuildingTypeExt::ExtMap.Find(pPostType))
		{
			if (pPlaceTypeExt->LaserFencePost_Fence.Get() != pPostTypeExt->LaserFencePost_Fence.Get())
				return SkipGameCode;
		}
	}

	return 0;
}
