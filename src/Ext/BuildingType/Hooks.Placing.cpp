#include "Body.h"
#include <EventClass.h>
#include <TerrainClass.h>
#include <TacticalClass.h>
#include <IsometricTileTypeClass.h>
#include <Ext/House/Body.h>
#include <Ext/TechnoType/Body.h>
#include <Ext/TerrainType/Body.h>
#include <Utilities/EnumFunctions.h>
#include <Utilities/AresHelper.h>

// Draw placement preview Hook
DEFINE_HOOK(0x6D504C, TacticalClass_DrawPlacement_PlacementPreview, 0x6)
{
	auto pRules = RulesExt::Global();

	if (!pRules->PlacementPreview || !Phobos::Config::ShowPlacementPreview)
		return 0;

	BuildingClass* pBuilding = specific_cast<BuildingClass*>(DisplayClass::Instance->CurrentBuilding);
	CellStruct displayCell = DisplayClass::Instance->CurrentFoundation_CenterCell + DisplayClass::Instance->CurrentFoundation_TopLeftOffset;

	if (!pBuilding) // Just for displaying auto place again
	{
		HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(HouseClass::CurrentPlayer);
		pBuilding = pHouseExt->CurrentBuilding;
		displayCell = pHouseExt->CurrentBuildingTopLeft;
	}

	BuildingTypeClass* pType = pBuilding ? pBuilding->Type : nullptr;
	auto pTypeExt = pType ? BuildingTypeExt::ExtMap.Find(pType) : nullptr;
	bool isShow = pTypeExt && pTypeExt->PlacementPreview;

	if (isShow)
	{
		CellClass* pCell = MapClass::Instance->TryGetCellAt(displayCell);

		if (!pCell)
			return 0;

		int imageFrame = 0;
		SHPStruct* pImage = pTypeExt->PlacementPreview_Shape.GetSHP();

		if (!pImage)
		{
			pImage = pType->LoadBuildup();
			if (pImage)
				imageFrame = ((pImage->Frames / 2) - 1);
			else
				pImage = pType->GetImage();

			if (!pImage)
				return 0;
		}

		imageFrame = Math::clamp(pTypeExt->PlacementPreview_ShapeFrame.Get(imageFrame), 0, (int)pImage->Frames);

		CoordStruct offset = pTypeExt->PlacementPreview_Offset;
		int height = offset.Z + pCell->GetFloorHeight({ 0, 0 });
		Point2D point = TacticalClass::Instance->CoordsToClient(CellClass::Cell2Coord(pCell->MapCoords, height)).first;
		point.X += offset.X;
		point.Y += offset.Y;

		BlitterFlags blitFlags = pTypeExt->PlacementPreview_Translucency.Get(pRules->PlacementPreview_Translucency) |
			BlitterFlags::Centered | BlitterFlags::Nonzero | BlitterFlags::MultiPass;

		ConvertClass* pPalette = pTypeExt->PlacementPreview_Remap.Get()
			? pBuilding->GetDrawer()
			: pTypeExt->PlacementPreview_Palette.GetOrDefaultConvert(FileSystem::UNITx_PAL());

		DSurface* pSurface = DSurface::Temp;
		RectangleStruct rect = pSurface->GetRect();
		rect.Height -= 32; // account for bottom bar

		CC_Draw_Shape(pSurface, pPalette, pImage, imageFrame, &point, &rect, blitFlags,
			0, 0, ZGradient::Ground, 1000, 0, nullptr, 0, 0, 0);
	}

	return 0;
}

// Set placement grid translucency Hook
DEFINE_HOOK(0x47EFAE, CellClass_Draw_It_SetPlacementGridTranslucency, 0x6)
{
	auto pRules = RulesExt::Global();
	BlitterFlags translucency = (pRules->PlacementPreview && Phobos::Config::ShowPlacementPreview)
		? pRules->PlacementGrid_TranslucencyWithPreview.Get(pRules->PlacementGrid_Translucency)
		: pRules->PlacementGrid_Translucency;

	if (translucency != BlitterFlags::None)
	{
		LEA_STACK(BlitterFlags*, blitFlags, STACK_OFFSET(0x68, -0x58));
		*blitFlags |= translucency;
	}

	return 0;
}

// Buildable-upon TerrainTypes Hook #2 - Draw laser fence placement even if they are on the way.
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

// Buildable-upon TerrainTypes Hook #3 - Remove them when buildings are placed on them.
DEFINE_HOOK(0x5684B1, MapClass_PlaceDown_BuildableUponTypes, 0x6)
{
	GET(ObjectClass*, pObject, EDI);
	GET(CellClass*, pCell, EAX);

	if (pObject->WhatAmI() == AbstractType::Building)
	{
		if (auto const pTerrain = pCell->GetTerrain(false))
		{
			auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

			if (pTypeExt->CanBeBuiltOn)
			{
				pCell->RemoveContent(pTerrain, false);
				TerrainTypeExt::Remove(pTerrain);
			}
		}
	}

	return 0;
}

/*
DisplayClass:
unknown_1180 -> CurrentFoundation_InAdjacent
When the cell which mouse is pointing at has changed, new command has given or try to click to place the current building

unknown_1181 -> CurrentFoundation_NoShrouded
When the cell which mouse is pointing at has changed or new command has given

CurrentFoundationCopy_CenterCell -> CurrentFoundation_CenterCell_Buffer
When the left mouse button release, move the CurrentFoundation_CenterCell to here and clear the CurrentFoundation_CenterCell, and move itself back to CurrentFoundation_CenterCell if place failed

CurrentFoundationCopy_TopLeftOffset -> CurrentFoundation_TopLeftOffset_Buffer
When the left mouse button release, move the CurrentFoundation_TopLeftOffset to here and clear the CurrentFoundation_TopLeftOffset, and move itself back to CurrentFoundation_TopLeftOffset if place failed

CurrentFoundationCopy_Data -> CurrentFoundation_Data_Buffer
When the left mouse button release, move the CurrentFoundation_Data to here and clear the CurrentFoundation_Data, and move itself back to CurrentFoundation_Data if place failed, otherwise clear itself

unknown_1190 -> CurrentBuilding_Buffer
When the left mouse button release, move the CurrentBuilding to here and clear the CurrentBuilding, and move itself back to CurrentBuilding if place failed, otherwise clear itself

unknown_1194 -> CurrentBuildingType_Buffer
When the left mouse button release, move the CurrentBuildingType to here and clear the CurrentBuildingType, and move itself back to CurrentBuildingType if place failed, otherwise clear itself

unknown_1198 -> CurrentBuildingTypeArrayIndex_Buffer
When the left mouse button release, move the unknown_11AC to here and clear the unknown_11AC, and move itself back to unknown_11AC if place failed

unknown_11AC -> CurrentBuildingTypeArrayIndex
When the building that building type factory create is selected, this record the ArrayIndex of the current building type

CellClass:
AltFlags = AltCellFlags::Unknown_4 -> InBuildingProcess
Vanilla only 1 frame between AddPlaceEvent and RespondToEvent
*/

// BaseNormal for units Hook #1 - Rewrite the algorithm, it will immediately return as long as BaseNormal exists
DEFINE_HOOK(0x4A8F21, MapClass_PassesProximityCheck_BaseNormalExtra, 0x9)
{
	enum { CheckCompleted = 0x4A904E };

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
												return CheckCompleted;
											}
										}

										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return CheckCompleted;
									}
									else if (RulesClass::Instance->BuildOffAlly && pOwner->IsAlliedWith(HouseClass::Array->Items[idxHouse]) && pBuilding->Type->EligibileForAllyBuilding)
									{
										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return CheckCompleted;
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
										return CheckCompleted;
									}
									else if (RulesClass::Instance->BuildOffAlly && pOwner->IsAlliedWith(HouseClass::Array->Items[idxHouse]) && pTypeExt->UnitBaseForAllyBuilding)
									{
										R->Stack<bool>(STACK_OFFSET(0x30, 0xC), true);
										return CheckCompleted;
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

	R->Stack<bool>(STACK_OFFSET(0x30, 0xC), false);
	return CheckCompleted;
}

// BaseNormal for units Hook #2-1 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4AACD9, MapClass_TacticalAction_BaseNormalRecheck, 0x5)
{
	if (RulesExt::Global()->CheckUnitBaseNormal && !(Unsorted::CurrentFrame % 8))
		return 0x4AACF5;

	return 0;
}

// BaseNormal for units Hook #2-2 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4A9361, MapClass_CallBuildingPlaceCheck_BaseNormalRecheck, 0x5)
{
	if (RulesExt::Global()->CheckUnitBaseNormal && !(Unsorted::CurrentFrame % 8))
		return 0x4A9371;

	return 0;
}

// Buildable-upon TechnoTypes Helper
namespace BuildOnOccupiersHelpers
{
	bool Exist = false;
	bool Mouse = false;
}

// Buildable-upon TerrainTypes Hook #1 - Allow placing buildings on top of them
// Buildable-upon TechnoTypes Hook #1 - Allow placing buildings on top of them
DEFINE_HOOK(0x47C640, CellClass_CanThisExistHere_IgnoreSomething, 0x6)
{
	enum { CanNotExistHere = 0x47C6D1, CanExistHere = 0x47C6A0 };

	GET(CellClass*, pCell, EDI);
	GET(BuildingTypeClass*, pBuildingType, EAX);
	GET_STACK(HouseClass*, pOwner, STACK_OFFSET(0x18, 0xC));

	const bool expand = RulesExt::Global()->ExpandBuildingPlace && pOwner && pOwner->IsControlledByHuman();
	bool landFootOnly = false;
	BuildOnOccupiersHelpers::Exist = false;

	if (!Game::IsActive)
		return CanExistHere;

	if (pBuildingType->LaserFence)
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft || absType == AbstractType::Building)
			{
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pObject->GetTechnoType());

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				TechnoClass* const pTechno = static_cast<TechnoClass*>(pObject);
				TechnoTypeClass* const pTechnoType = pTechno->GetTechnoType();
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
				{
					if (pTechno->Owner != pOwner || pTechnoType->Speed <= 0 || !expand)
						return CanNotExistHere;
					else
						landFootOnly = true;
				}
			}
			else if (absType == AbstractType::Terrain)
			{
				if (TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pObject))
				{
					auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

					if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
						return CanNotExistHere;
				}
			}

			pObject = pObject->NextObject;
		}

		if (landFootOnly)
			BuildOnOccupiersHelpers::Exist = true;

		return CanExistHere;
	}
	else if (pBuildingType->LaserFencePost || pBuildingType->Gate)
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft)
			{
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pObject->GetTechnoType());

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pBuilding->Type);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn || !pBuilding->Type->LaserFence || pBuilding->GetOwningHouse() != pOwner)
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				TechnoClass* const pTechno = static_cast<TechnoClass*>(pObject);
				TechnoTypeClass* const pTechnoType = pTechno->GetTechnoType();
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
				{
					if (pTechno->Owner != pOwner || pTechnoType->Speed <= 0 || !expand)
						return CanNotExistHere;
					else
						landFootOnly = true;
				}
			}
			else if (absType == AbstractType::Terrain)
			{
				if (TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pObject))
				{
					auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

					if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
						return CanNotExistHere;
				}
			}

			pObject = pObject->NextObject;
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

				if (absType == AbstractType::Building || absType == AbstractType::Infantry || absType == AbstractType::Unit)
				{
					auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pObject->GetTechnoType());

					if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
						return CanNotExistHere;
				}

				pObject = pObject->NextObject;
			}

			return CanExistHere;
		}

		return CanNotExistHere;
	}
	else
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft || absType == AbstractType::Building)
			{
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pObject->GetTechnoType());

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Infantry || absType == AbstractType::Unit)
			{
				TechnoClass* const pTechno = static_cast<TechnoClass*>(pObject);
				TechnoTypeClass* const pTechnoType = pTechno->GetTechnoType();
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pTechnoType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
				{
					if (pTechno->Owner != pOwner || pTechnoType->Speed <= 0 || !expand)
						return CanNotExistHere;
					else
						landFootOnly = true;
				}
			}
			else if (absType == AbstractType::Terrain)
			{
				if (TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pObject))
				{
					auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

					if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
						return CanNotExistHere;
				}
			}

			pObject = pObject->NextObject;
		}
	}

	if (landFootOnly)
		BuildOnOccupiersHelpers::Exist = true;

	return CanExistHere;
}

// Buildable-upon TechnoTypes Hook #2 - Draw yellow grid if there is only infantries and units on the cell
DEFINE_HOOK(0x47EF52, CellClass_DrawPlaceGrid_DrawExtraYellowGrid, 0x6)
{
	R->EDI(BuildOnOccupiersHelpers::Exist);
	return 0;
}

// Buildable-upon TechnoTypes Hook #3 - Don not draw yellow grid if is placing
DEFINE_JUMP(LJMP, 0x47EED6, 0x47EFB9);

// Buildable-upon TechnoTypes Hook #4 - Hang up place event if there is only infantries and units on the cell
DEFINE_HOOK(0x4FB1EA, HouseClass_UnitFromFactory_HangUpPlaceEvent, 0x5)
{
	enum { CanBuild = 0x4FB23C, TemporarilyCanNotBuild = 0x4FB5BA, CanNotBuild = 0x4FB35F };

	GET(HouseClass*, pHouse, EBP);
	GET(TechnoClass*, pTechno, ESI);
	GET(BuildingClass*, pFactory, EDI);
	GET_STACK(CellStruct, cell, STACK_OFFSET(0x3C, 0x10));

	if (pTechno->WhatAmI() == AbstractType::Building)
	{
		HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(pHouse);
		BuildingClass* const pBuilding = static_cast<BuildingClass*>(pTechno);
		BuildingTypeClass* const pBuildingType = pBuilding->Type;

		if (!pBuildingType->PlaceAnywhere)
		{
			const short foundationWidth = pBuildingType->GetFoundationWidth();
			const short foundationHeight = pBuildingType->GetFoundationHeight(false);
			const short topLeftX = cell.X;
			const short topLeftY = cell.Y;
			const short bottomRightX = topLeftX + foundationWidth;
			const short bottomRightY = topLeftY + foundationHeight;
			bool canBuild = true;
			bool noOccupy = true;

			for (short curX = topLeftX; curX < bottomRightX && canBuild; ++curX)
			{
				for (short curY = topLeftY; curY < bottomRightY && canBuild; ++curY)
				{
					CellClass* const pCell = MapClass::Instance->GetCellAt(CellStruct{curX, curY});

					if (!pCell || !pCell->CanThisExistHere(pBuildingType->SpeedType, pBuildingType, pHouse))
						canBuild = false;
					else if (BuildOnOccupiersHelpers::Exist)
						noOccupy = false;
				}
			}

			if (!canBuild) // Can not build
			{
				if (pHouseExt->CurrentBuildingTimes == 20)
					BuildOnOccupiersHelpers::Mouse = true;

				pHouseExt->CurrentBuilding = nullptr;
				pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
				pHouseExt->CurrentBuildingTimes = 20;
				return CanNotBuild;
			}
			else if (!noOccupy) // Temporarily can not build
			{
				if (!(pHouseExt->CurrentBuildingTimes % 5)) // Force occupiers leave
				{
					CellClass* pTempCell = nullptr;
					int tempOccupiers = 0;
					std::vector<CellClass*> checkedCells;
					checkedCells.reserve(((foundationHeight + foundationWidth) << 3) + 64); // Max size

					for (short curX = topLeftX; curX < bottomRightX; ++curX)
					{
						for (short curY = topLeftY; curY < bottomRightY; ++curY)
						{
							CellClass* const pCell = MapClass::Instance->GetCellAt(CellStruct{curX, curY});
							ObjectClass* pObject = pCell->FirstObject;

							while (pObject)
							{
								AbstractType const absType = pObject->WhatAmI();

								if (absType == AbstractType::Infantry)
								{
									InfantryClass* const pInfantry = static_cast<InfantryClass*>(pObject);
									auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pInfantry->Type);

									if ((!pTypeExt || !pTypeExt->CanBeBuiltOn) && pInfantry->Owner == pHouse)
									{
										CellClass* pCellDestination = nullptr;
										CellStruct location { curX, curY };

										if (pTempCell)
										{
											if (tempOccupiers >= 2)
											{
												pTempCell->AltFlags |= AltCellFlags::Unknown_4;
												pCellDestination = pTempCell;
												checkedCells.push_back(pTempCell);
												pTempCell = nullptr;
												tempOccupiers = 0;
											}
											else
											{
												pCellDestination = pTempCell;
												checkedCells.push_back(pTempCell);
												++tempOccupiers;
											}

											break;
										}
										else
										{
											for (short times = 0, edge = 0, curLength = 1, maxLength = 1, pace = 1; times < 81; ++times)
											{
												if (CellClass* const pCurCell = MapClass::Instance->GetCellAt(location))
												{
													if (!(pCurCell->AltFlags & AltCellFlags::Unknown_4)
														&& pInfantry->IsCellOccupied(pCurCell, FacingType::None, -1, nullptr, false) == Move::OK)
													{
														ObjectClass* pCurObject = pCurCell->FirstObject;
														int occupiers = 0;

														while (pCurObject)
														{
															if (pCurObject->WhatAmI() == AbstractType::Infantry)
																++occupiers;

															pCurObject = pCurObject->NextObject;
														}

														if (occupiers >= 2)
														{
															pCurCell->AltFlags |= AltCellFlags::Unknown_4;
														}
														else
														{
															++occupiers;
															pTempCell = pCurCell;
															tempOccupiers = occupiers;
														}

														pCellDestination = pCurCell;
														checkedCells.push_back(pCurCell);
														break;
													}
												}

												if (edge) // Counter-clockwise
													location.X += pace;
												else
													location.Y += pace;

												if (curLength < maxLength)
												{
													++curLength;
												}
												else if (edge)
												{
													edge = 0;
													curLength = 1;
													++maxLength;
													pace = -pace;
												}
												else
												{
													++edge;
													curLength = 1;
												}
											}
										}

										if (pCellDestination)
										{
											pInfantry->SetTarget(nullptr);
											pInfantry->SetDestination(nullptr, false);
											pInfantry->ForceMission(Mission::None);

											if (pInfantry->IsDeployed())
												pInfantry->QueueMission(Mission::Unload, true);

											pInfantry->QueueMission(Mission::Move, false);
											pInfantry->SetDestination(pCellDestination, true);
										}
										else
										{
											curX = bottomRightX;
											curY = bottomRightY;
											pHouseExt->CurrentBuildingTimes = 0;
											break;
										}
									}
								}
								else if (absType == AbstractType::Unit)
								{
									UnitClass* const pUnit = static_cast<UnitClass*>(pObject);
									auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pUnit->Type);

									if ((!pTypeExt || !pTypeExt->CanBeBuiltOn) && pUnit->Owner == pHouse)
									{
										CellClass* pCellDestination = nullptr;
										CellStruct location { curX, curY };

										for (short times = 0, edge = 0, curLength = 1, maxLength = 1, pace = 1; times < 81; ++times)
										{
											if (CellClass* const pCurCell = MapClass::Instance->GetCellAt(location))
											{
												if (!(pCurCell->AltFlags & AltCellFlags::Unknown_4)
													&& pUnit->IsCellOccupied(pCurCell, FacingType::None, -1, nullptr, false) == Move::OK)
												{
													pCurCell->AltFlags |= AltCellFlags::Unknown_4;
													pCellDestination = pCurCell;
													checkedCells.push_back(pCurCell);
													break;
												}
											}

											if (edge) // Counter-clockwise
												location.X += pace;
											else
												location.Y += pace;

											if (curLength < maxLength)
											{
												++curLength;
											}
											else if (edge)
											{
												edge = 0;
												curLength = 1;
												++maxLength;
												pace = -pace;
											}
											else
											{
												++edge;
												curLength = 1;
											}
										}

										if (pCellDestination)
										{
											pUnit->SetTarget(nullptr);
											pUnit->SetDestination(nullptr, false);
											pUnit->ForceMission(Mission::None);

											if (pUnit->Deployed)
												pUnit->QueueMission(Mission::Unload, true);

											pUnit->QueueMission(Mission::Move, false);
											pUnit->SetDestination(pCellDestination, true);
										}
										else
										{
											curX = bottomRightX;
											curY = bottomRightY;
											pHouseExt->CurrentBuildingTimes = 0;
											break;
										}
									}
								}

								pObject = pObject->NextObject;
							}
						}
					}

					for (auto const& pCheckedCell : checkedCells)
						pCheckedCell->AltFlags &= ~AltCellFlags::Unknown_4;
				}

				if (!pHouseExt->CurrentBuilding) // Start
				{
					pHouseExt->CurrentBuilding = pBuilding;
					pHouseExt->CurrentBuildingTopLeft = cell;
				}
				else // Continue
				{
					--pHouseExt->CurrentBuildingTimes;
				}

				if (pHouseExt->CurrentBuildingTimes > 0)
				{
					pHouseExt->CurrentBuildingTimer.Start(8);
					return TemporarilyCanNotBuild;
				} // Time out

				pHouseExt->CurrentBuilding = nullptr;
				pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
				pHouseExt->CurrentBuildingTimes = 20;
				return CanNotBuild;
			}
		} // Can Build

		pHouseExt->CurrentBuilding = nullptr;
		pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
		pHouseExt->CurrentBuildingTimes = 20;
	}

	pFactory->SendCommand(RadioCommand::RequestLink, pTechno);

	if (pTechno->Unlimbo(CoordStruct{ (cell.X << 8) + 128, (cell.Y << 8) + 128, 0 }, DirType::North))
		return CanBuild;

	return CanNotBuild;
}

// Buildable-upon TechnoTypes Hook #5 - Check whether need to skip the replace command
DEFINE_HOOK(0x4FB395, HouseClass_UnitFromFactory_SkipMouseReturn, 0x6)
{
	if (BuildOnOccupiersHelpers::Mouse)
	{
		BuildOnOccupiersHelpers::Mouse = false;
		return 0;
	}

	R->EBX(0);
	return 0x4FB489;
}

// Buildable-upon TechnoTypes Hook #6 - Restart timer, reset AltFlags and clear buffer when mouse click
DEFINE_HOOK(0x4FB87C, HouseClass_BuildingCameoClick_StopLastEvent, 0x7)
{
	HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(HouseClass::CurrentPlayer);

	if (pHouseExt->CurrentBuildingTimer.IsTicking())
	{
		pHouseExt->CurrentBuilding = nullptr;
		pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
		pHouseExt->CurrentBuildingTimes = 20;
		pHouseExt->CurrentBuildingTimer.Stop();
		reinterpret_cast<void(__thiscall*)(DisplayClass*, CellStruct)>(0x4A8D50)(DisplayClass::Instance, CellStruct::Empty);
		DisplayClass::Instance->unknown_1190 = 0;
		DisplayClass::Instance->unknown_1194 = 0;
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #7 - Check whether can place again in each house
DEFINE_HOOK(0x4F8F87, HouseClass_AI_HangUpBuildingCheck, 0x6)
{
	GET(HouseClass*, pHouse, ESI);

	if (HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(pHouse))
	{
		if (pHouseExt->CurrentBuilding && pHouseExt->CurrentBuildingTimer.Completed())
		{
			pHouseExt->CurrentBuildingTimer.Stop();
			EventClass event
			(
				pHouse->ArrayIndex,
				EventType::Place,
				AbstractType::Building,
				pHouseExt->CurrentBuilding->Type->GetArrayIndex(),
				pHouseExt->CurrentBuilding->Type->Naval,
				pHouseExt->CurrentBuildingTopLeft
			);
			EventClass::AddEvent(event);
		}
	}

	return 0;
}

// TODO overlayWall, firestormWall, laserFence, Scatter, vanishTechno, newSearchAlgorithm
