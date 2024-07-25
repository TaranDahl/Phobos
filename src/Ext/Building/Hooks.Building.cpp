#include "Body.h"
#include <TerrainClass.h>
#include <IsometricTileTypeClass.h>
#include <Ext/TerrainType/Body.h>
#include <Utilities/EnumFunctions.h>
#include <Utilities/AresHelper.h>

/*
unknown_1180 -> CurrentFoundation_InAdjacent
When the cell which mouse is pointing at has changed, new command has given or try to click to place the current building

unknown_1181 -> CurrentFoundation_NoShrouded
When the cell which mouse is pointing at has changed or new command has given

unknown_1190 -> LastBuilding
When the left mouse button release, move the CurrentBuilding to here and clear the CurrentBuilding

unknown_1194 -> LastBuildingType
When the left mouse button release, move the CurrentBuildingType to here and clear the CurrentBuildingType

unknown_1198 -> LastBuildingTypeArrayIndex
When the left mouse button release, move the unknown_11AC to here and clear the unknown_11AC

unknown_11AC -> CurrentBuildingTypeArrayIndex
When the building that building type factory create is selected, this record the ArrayIndex of the current building type
*/
// BaseNormal for units Hook #1 - Rewrite the algorithm, it will immediately return as long as BaseNormal exists
DEFINE_HOOK(0x4A8F21, MapClass_PassesProximityCheck_BaseNormalExtra, 0x9)
{
	enum { SkipGameCode = 0x4A904E };

	GET(CellStruct*, pFoundationTopLeft, EDI);
	GET(BuildingTypeClass*, pBuildingType, ESI);
	GET_STACK(int, idxHouse, STACK_OFFSET(0x30, 0x8));

	if (Game::IsActive)
	{
		const short foundationWidth = pBuildingType->GetFoundationWidth();
		const short foundationHeight = pBuildingType->GetFoundationHeight(false);
		const short topLeftX = pFoundationTopLeft->X;
		const short topLeftY = pFoundationTopLeft->Y;
		const short bottomRightX = topLeftX + foundationWidth;
		const short bottomRightY = topLeftY + foundationHeight;

		const short buildingAdjacent = static_cast<short>(pBuildingType->Adjacent + 1);
		const short leftX = topLeftX - buildingAdjacent;
		const short topY = topLeftY - buildingAdjacent;
		const short rightX = bottomRightX + buildingAdjacent;
		const short bottomY = bottomRightY + buildingAdjacent;

		for (short curX = leftX; curX < rightX; ++curX)
		{
			for (short curY = topY; curY < bottomY; ++curY)
			{
				if (CellClass* const pCell = MapClass::Instance->GetCellAt(CellStruct{curX, curY}))
				{
					ObjectClass* pObject = pCell->FirstObject;

					while (pObject)
					{
						AbstractType const absType = pObject->WhatAmI();

						if (absType == AbstractType::Building)
						{
							if (curX < topLeftX || curX >= bottomRightX || curY < topLeftY || curY >= bottomRightY)
							{
								BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);

								if (HouseClass* const pOwner = pBuilding->Owner)
								{
									if (pOwner->ArrayIndex == idxHouse && pBuilding->Type->BaseNormal)
									{
										if (CAN_USE_ARES && AresHelper::CanUseAres) // Restore Ares MapClass_CanBuildingTypeBePlacedHere_Ignore
										{
											struct DummyAresBuildingExt // Temp Ares Building Ext
											{
												char _[0xE];
												bool unknownExtBool;
											};

											struct DummyBuildingClass // Temp Building Class
											{
												char _[0x71C];
												DummyAresBuildingExt* align_71C;
											};

											if (const DummyAresBuildingExt* pAresBuildingExt = reinterpret_cast<DummyBuildingClass*>(pBuilding)->align_71C)
											{
												R->Stack<bool>(STACK_OFFSET(0x30, 0xC), !pAresBuildingExt->unknownExtBool);
												return SkipGameCode;
											}
										}

										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return SkipGameCode;
									}
									else if (RulesClass::Instance->BuildOffAlly && pOwner->IsAlliedWith(HouseClass::Array->Items[idxHouse]) && pBuilding->Type->EligibileForAllyBuilding)
									{
										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return SkipGameCode;
									}
								}
							}
						}
						else if (RulesExt::Global()->CheckUnitBaseNormal && absType == AbstractType::Unit)
						{
							UnitClass* const pUnit = static_cast<UnitClass*>(pObject);

							if (HouseClass* const pOwner = pUnit->Owner)
							{
								if (TechnoTypeExt::ExtData* const pTypeExt = TechnoTypeExt::ExtMap.Find(static_cast<UnitClass*>(pObject)->Type))
								{
									if (pOwner->ArrayIndex == idxHouse && pTypeExt->UnitBaseNormal)
									{
										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return SkipGameCode;
									}
									else if (RulesClass::Instance->BuildOffAlly && pOwner->IsAlliedWith(HouseClass::Array->Items[idxHouse]) && pTypeExt->UnitBaseForAllyBuilding)
									{
										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return SkipGameCode;
									}
								}
							}
						}

						pObject = pObject->NextObject;
					}
				}
			}
		}
	}

	R->Stack<bool>(STACK_OFFSET(0x30, 0xC), false); // Fit Kratos
	return SkipGameCode;
}

// BaseNormal for units Hook #2-1 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4AACD9, MapClass_TacticalAction_BaseNormalRecheck, 0x5)
{
	enum { NeedRecheck = 0x4AACF5 };

	if (RulesExt::Global()->CheckUnitBaseNormal && Unsorted::CurrentFrame % 8 == 0)
		return NeedRecheck;

	return 0;
}

// BaseNormal for units Hook #2-2 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4A9361, MapClass_CallBuildingPlaceCheck_BaseNormalRecheck, 0x5)
{
	enum { NeedRecheck = 0x4A9371 };

	if (RulesExt::Global()->CheckUnitBaseNormal && Unsorted::CurrentFrame % 8 == 0)
		return NeedRecheck;

	return 0;
}

namespace TechnoOccupyHelpers // Buildable-upon InfantryTypes and UnitTypes
{
	bool Exist = false;
	CellClass* pCurrentCell = nullptr;
}

// Buildable-upon TerrainTypes Hook #1 & #2 - Allow placing buildings and laser fences on top of them
// Buildable-upon InfantryTypes and UnitTypes Hook #1-1 - Draw yellow pips if there is only infantries and units on the cell
DEFINE_HOOK(0x47C640, CellClass_CanThisExistHere_IgnoreSomething, 0x6)
{
	if (!RulesExt::Global()->ExpandBuildingPlace)
		return 0;

	enum { SkipGameCode = 0x47C6D1, ContinueGameCode = 0x47C6A0 };

	GET(CellClass*, pCell, EDI);
	GET(BuildingTypeClass*, pBuildingType, EAX);
	GET_STACK(HouseClass*, pOwner, STACK_OFFSET(0x18, 0xC));

	bool landFootOnly = false;
	TechnoOccupyHelpers::Exist = false;
	TechnoOccupyHelpers::pCurrentCell = pCell;

	if (!Game::IsActive)
		return ContinueGameCode;

	if (pBuildingType->LaserFence)
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Building)
			{
				return SkipGameCode;
			}
			else if (absType == AbstractType::Terrain)
			{
				TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pObject);

				if (auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type))
				{
					if (!pTypeExt->CanBeBuiltOn)
						return SkipGameCode;
				}
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				landFootOnly = true;
			}

			pObject = pObject->NextObject;
		}

		if (landFootOnly)
		{
			TechnoOccupyHelpers::Exist = true;
			return SkipGameCode;
		}

		return ContinueGameCode;
	}
	else if (pBuildingType->LaserFencePost || pBuildingType->Gate)
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft)
			{
				return SkipGameCode;
			}
			else if (absType == AbstractType::Terrain)
			{
				TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pObject);

				if (auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type))
				{
					if (!pTypeExt->CanBeBuiltOn)
						return SkipGameCode;
				}
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				landFootOnly = true;
			}
			else if (absType == AbstractType::Building)
			{
				break;
			}

			pObject = pObject->NextObject;
		}

		if (pObject)
		{
			if (pObject->WhatAmI() == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);

				if (!pBuilding->Type->LaserFence || pBuilding->GetOwningHouse() != pOwner)
					return SkipGameCode;
			}
			else
			{
				return SkipGameCode;
			}
		}
	}
	else if (pBuildingType->ToTile)
	{
		const int isoTileTypeIndex = pCell->IsoTileTypeIndex;

		if (isoTileTypeIndex < 0 || isoTileTypeIndex >= IsometricTileTypeClass::Array->Count || IsometricTileTypeClass::Array->Items[isoTileTypeIndex]->Morphable)
		{
			ObjectClass* pObject = pCell->FirstObject;

			while (pObject)
			{
				const AbstractType absType = pObject->WhatAmI();

				if (absType == AbstractType::Building)
					return SkipGameCode;
				else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
					landFootOnly = true;

				pObject = pObject->NextObject;
			}

			if (landFootOnly)
			{
				TechnoOccupyHelpers::Exist = true;
				return SkipGameCode;
			}

			return ContinueGameCode;
		}

		return SkipGameCode;
	}
	else
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft || absType == AbstractType::Building)
			{
				return SkipGameCode;
			}
			else if (absType == AbstractType::Terrain)
			{
				TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pObject);

				if (auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type))
				{
					if (!pTypeExt->CanBeBuiltOn)
						return SkipGameCode;
				}
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				landFootOnly = true;
			}

			pObject = pObject->NextObject;
		}
	}

	if (landFootOnly)
	{
		TechnoOccupyHelpers::Exist = true;
		return SkipGameCode;
	}

	if ((pCell->OccupationFlags & 0x3F) == 0)
		return ContinueGameCode;

	return SkipGameCode;
}

// Buildable-upon InfantryTypes and UnitTypes Hook #1-2 - Draw yellow pips if there is only infantries and units on the cell
DEFINE_HOOK(0x47EE98, CellClass_DrawPlacePip_DrawYellowPips1, 0xA)
{
	enum { SkipGameCode = 0x47EEB8, ContinueGameCode = 0x47EEA2 };

	GET(bool, valid, EAX);

	if (!valid && !TechnoOccupyHelpers::Exist)
		return SkipGameCode;

	R->EDX(DisplayClass::Instance->CurrentBuildingType);
	return ContinueGameCode;
}

// Buildable-upon InfantryTypes and UnitTypes Hook #1-3 - Draw yellow pips if there is only infantries and units on the cell
DEFINE_HOOK(0x47EF52, CellClass_DrawPlacePip_DrawYellowPips2, 0x6)
{
	R->EDI(TechnoOccupyHelpers::Exist);
	return 0;
}
