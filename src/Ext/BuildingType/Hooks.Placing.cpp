#include "Body.h"
#include <EventClass.h>
#include <TerrainClass.h>
#include <AircraftClass.h>
#include <TacticalClass.h>
#include <IsometricTileTypeClass.h>
#include <Ext/House/Body.h>
#include <Ext/TechnoType/Body.h>
#include <Ext/TerrainType/Body.h>
#include <Utilities/EnumFunctions.h>
#include <Utilities/AresHelper.h>

// Draw placement preview Hook -> sub_6D5030
DEFINE_HOOK(0x6D504C, TacticalClass_DrawPlacement_PlacementPreview, 0x6)
{
	auto pRules = RulesExt::Global();
	BuildingClass* pBuilding = specific_cast<BuildingClass*>(DisplayClass::Instance->CurrentBuilding);
	CellStruct displayCell = DisplayClass::Instance->CurrentFoundation_CenterCell + DisplayClass::Instance->CurrentFoundation_TopLeftOffset;

	if (!pBuilding && pRules->ExpandBuildingPlace) // Display auto placing when enabled buildable-upon TechnoTypes
	{
		HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(HouseClass::CurrentPlayer);
		pBuilding = pHouseExt->CurrentBuilding;
		displayCell = pHouseExt->CurrentBuildingTopLeft;
	}
	else if (!pRules->PlacementPreview || !Phobos::Config::ShowPlacementPreview)
	{
		return 0;
	}

	if (!pBuilding)
		return 0;

	BuildingTypeClass* pType = pBuilding->Type;
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

// Set placement grid translucency Hook -> sub_47EC90
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
// Buildable-upon TechnoTypes Hook #8 -> sub_5683C0 - Remove some of them when buildings are placed on them.
DEFINE_HOOK(0x5684B1, MapClass_PlaceDown_BuildableUponTypes, 0x6)
{
	GET(ObjectClass*, pObject, EDI);
	GET(CellClass*, pCell, EAX);

	if (pObject->WhatAmI() == AbstractType::Building)
	{
		ObjectClass* pCellObject = pCell->FirstObject;

		while (pCellObject)
		{
			AbstractType const absType = pCellObject->WhatAmI();

			if (absType == AbstractType::Infantry || absType == AbstractType::Unit || absType == AbstractType::Aircraft)
			{
				TechnoClass* const pTechno = static_cast<TechnoClass*>(pCellObject);
				TechnoTypeClass* const pType = pTechno->GetTechnoType();
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					pTechno->KillPassengers(nullptr);
					pTechno->Stun();
					pTechno->Limbo();
					pTechno->UnInit();
				}
			}
			else if (absType == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pCellObject);
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pBuilding->Type);

				if (pTypeExt && pTypeExt->CanBeBuiltOn)
				{
					pBuilding->KillOccupants(nullptr);
					pBuilding->Stun();
					pBuilding->Limbo();
					pBuilding->UnInit();
				}
			}
			else if (absType == AbstractType::Terrain)
			{
				TerrainClass* const pTerrain = abstract_cast<TerrainClass*>(pCellObject);
				auto const pTypeExt = TerrainTypeExt::ExtMap.Find(pTerrain->Type);

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

// BaseNormal for units Hook #1 -> sub_4A8EB0 - Rewrite the algorithm, it will immediately return as long as BaseNormal exists
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

// BaseNormal for units Hook #2-1 -> sub_4AAC10 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4AACD9, MapClass_TacticalAction_BaseNormalRecheck, 0x5)
{
	return (RulesExt::Global()->CheckUnitBaseNormal && !(Unsorted::CurrentFrame % 8)) ? 0x4AACF5 : 0;
}

// BaseNormal for units Hook #2-2 -> sub_4A91B0 - Let the game do the PassesProximityCheck when the cell which mouse is pointing at has not changed
DEFINE_HOOK(0x4A9361, MapClass_CallBuildingPlaceCheck_BaseNormalRecheck, 0x5)
{
	return (RulesExt::Global()->CheckUnitBaseNormal && !(Unsorted::CurrentFrame % 8)) ? 0x4A9371 : 0;
}

// Buildable-upon TechnoTypes Helper
namespace BuildOnOccupiersHelpers
{
	bool Exist = false;
	bool Mouse = false;
}

// Buildable-upon TerrainTypes Hook #1 -> sub_47C620 - Allow placing buildings on top of them
// Buildable-upon TechnoTypes Hook #1 -> sub_47C620 - Allow placing buildings on top of them
DEFINE_HOOK(0x47C640, CellClass_CanThisExistHere_IgnoreSomething, 0x6)
{
	enum { CanNotExistHere = 0x47C6D1, CanExistHere = 0x47C6A0 };

	GET(CellClass*, pCell, EDI);
	GET(BuildingTypeClass*, pBuildingType, EAX);
	GET_STACK(HouseClass*, pOwner, STACK_OFFSET(0x18, 0xC));

	BuildOnOccupiersHelpers::Exist = false;

	if (!Game::IsActive)
		return CanExistHere;

	const bool expand = RulesExt::Global()->ExpandBuildingPlace && pOwner && pOwner->IsControlledByHuman();
	bool landFootOnly = false;

	if (pBuildingType->LaserFence)
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);
				BuildingTypeClass* const pType = pBuilding->Type;
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
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
	else if (pBuildingType->LaserFencePost || pBuildingType->Gate)
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft)
			{
				AircraftClass* const pAircraft = static_cast<AircraftClass*>(pObject);
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pAircraft->Type);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);
				BuildingTypeClass* const pType = pBuilding->Type;
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if ((!pTypeExt || !pTypeExt->CanBeBuiltOn) && (pBuilding->Owner != pOwner || !pType->LaserFence))
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

		if (isoTileTypeIndex >= 0 && isoTileTypeIndex < IsometricTileTypeClass::Array->Count && !IsometricTileTypeClass::Array->Items[isoTileTypeIndex]->Morphable)
			return CanNotExistHere;

		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);
				BuildingTypeClass* const pType = pBuilding->Type;
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
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
	else
	{
		ObjectClass* pObject = pCell->FirstObject;

		while (pObject)
		{
			const AbstractType absType = pObject->WhatAmI();

			if (absType == AbstractType::Aircraft)
			{
				AircraftClass* const pAircraft = static_cast<AircraftClass*>(pObject);
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pAircraft->Type);

				if (!pTypeExt || !pTypeExt->CanBeBuiltOn)
					return CanNotExistHere;
			}
			else if (absType == AbstractType::Building)
			{
				BuildingClass* const pBuilding = static_cast<BuildingClass*>(pObject);
				BuildingTypeClass* const pType = pBuilding->Type;
				auto const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

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

	return CanExistHere; // Continue check the overlays .etc
}

// Buildable-upon TechnoTypes Hook #2 -> sub_47EC90 - Draw yellow grid if there is only infantries and units on the cell
DEFINE_HOOK(0x47EF52, CellClass_DrawPlaceGrid_DrawExtraYellowGrid, 0x6)
{
	R->EDI(BuildOnOccupiersHelpers::Exist);
	return 0;
}

// Buildable-upon TechnoTypes Hook #3 -> sub_47EC90 - Don not draw yellow grid if is placing
DEFINE_JUMP(LJMP, 0x47EED6, 0x47EFB9);

// Buildable-upon TechnoTypes Hook #4 -> sub_4FB0E0 - Hang up place event if there is only infantries and units on the cell
DEFINE_HOOK(0x4FB1EA, HouseClass_UnitFromFactory_HangUpPlaceEvent, 0x5)
{
	enum { CanBuild = 0x4FB23C, TemporarilyCanNotBuild = 0x4FB5BA, CanNotBuild = 0x4FB35F };

	GET(HouseClass*, pHouse, EBP);
	GET(TechnoClass*, pTechno, ESI);
	GET(BuildingClass*, pFactory, EDI);
	GET_STACK(CellStruct, cell, STACK_OFFSET(0x3C, 0x10));

	if (pTechno->WhatAmI() == AbstractType::Building && RulesExt::Global()->ExpandBuildingPlace)
	{
		BuildingClass* const pBuilding = static_cast<BuildingClass*>(pTechno);
		BuildingTypeClass* const pBuildingType = pBuilding->Type;
		HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(pHouse);

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
				if (!(pHouseExt->CurrentBuildingTimes % 5))
				{
					const CellStruct topLeftCell {topLeftX, topLeftY};
					const CellStruct foundationCell {foundationWidth, foundationHeight};

					if (BuildingTypeExt::CleanUpBuildingSpace(topLeftCell, foundationCell, pHouse))
						pHouseExt->CurrentBuildingTimes = 1;
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
					DisplayClass::Instance->unknown_1190 = 0;
					DisplayClass::Instance->unknown_1194 = 0; // Solve the read saved game error EIP:6D5529
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

// Buildable-upon TechnoTypes Hook #5 -> sub_4FB0E0 - Check whether need to skip the replace command
DEFINE_HOOK(0x4FB395, HouseClass_UnitFromFactory_SkipMouseReturn, 0x6)
{
	if (!RulesExt::Global()->ExpandBuildingPlace)
		return 0;

	if (BuildOnOccupiersHelpers::Mouse)
	{
		BuildOnOccupiersHelpers::Mouse = false;
		return 0;
	}

	R->EBX(0);
	return 0x4FB489;
}

// Buildable-upon TechnoTypes Hook #6 -> sub_4FB840 - Restart timer, reset AltFlags and clear buffer when mouse click
DEFINE_HOOK(0x4FB87C, HouseClass_BuildingCameoClick_StopLastEvent, 0x7)
{
	if (!RulesExt::Global()->ExpandBuildingPlace)
		return 0;

	HouseExt::ExtData* const pHouseExt = HouseExt::ExtMap.Find(HouseClass::CurrentPlayer);

	if (pHouseExt->CurrentBuildingTimer.IsTicking())
	{
		pHouseExt->CurrentBuilding = nullptr;
		pHouseExt->CurrentBuildingTopLeft = CellStruct::Empty;
		pHouseExt->CurrentBuildingTimes = 20;
		pHouseExt->CurrentBuildingTimer.Stop();

		// Reset AltFlags
		reinterpret_cast<void(__thiscall*)(DisplayClass*, CellStruct)>(0x4A8D50)(DisplayClass::Instance, CellStruct::Empty);
	}

	return 0;
}

// Buildable-upon TechnoTypes Hook #7 -> sub_4F8440 - Check whether can place again in each house
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

// Laser fence use GetBuilding to check whether can build and draw, so no need to change
// Buildable-upon TechnoTypes Hook #9-1 -> sub_6D5C50 - Don't draw overlay wall grid when have occupiers
DEFINE_HOOK(0x6D5D38, TacticalClass_DrawOverlayWallGrid_DisableWhenHaveTechnos, 0x8)
{
	GET(bool, valid, EAX);
	return (!valid || BuildOnOccupiersHelpers::Exist) ? 0x6D5F0F : 0x6D5D40;
}

// Buildable-upon TechnoTypes Hook #9-2 -> sub_6D59D0 - Don't draw firestorm wall grid when have occupiers
DEFINE_HOOK(0x6D5A9D, TacticalClass_DrawFirestormWallGrid_DisableWhenHaveTechnos, 0x8)
{
	GET(bool, valid, EAX);
	return (!valid || BuildOnOccupiersHelpers::Exist) ? 0x6D5C2F : 0x6D5AA5;
}

// Buildable-upon TechnoTypes Hook #9-3 -> sub_588750 - Don't place overlay wall when have occupiers
DEFINE_HOOK(0x588873, MapClass_BuildingToWall_DisableWhenHaveTechnos, 0x8)
{
	GET(bool, valid, EAX);
	return (!valid || BuildOnOccupiersHelpers::Exist) ? 0x588935 : 0x58887B;
}

// Buildable-upon TechnoTypes Hook #9-4 -> sub_588570 - Don't place firestorm wall when have occupiers
DEFINE_HOOK(0x588664, MapClass_BuildingToFirestormWall_DisableWhenHaveTechnos, 0x8)
{
	GET(bool, valid, EAX);
	return (!valid || BuildOnOccupiersHelpers::Exist) ? 0x588730 : 0x58866C;
}

// Buildable-upon TechnoTypes Hook #10-1 -> sub_7393C0 - Try to clean up the building space when is deploying
DEFINE_HOOK(0x7394BE, UnitClass_TryToDeploy_CleanUpDeploySpace, 0x6)
{
	if (!RulesExt::Global()->ExpandBuildingPlace)
		return 0;

	enum { CanBuild = 0x73958A, TemporarilyCanNotBuild = 0x73950F, CanNotBuild = 0x7394E0 };

	GET(UnitClass*, pUnit, EBP);
	GET_STACK(CellStruct, cell, STACK_OFFSET(0x28, -0x14));

	BuildingTypeClass* const pBuildingType = pUnit->Type->DeploysInto;

	if (!pBuildingType->PlaceAnywhere)
	{
		const short foundationWidth = pBuildingType->GetFoundationWidth();
		const short foundationHeight = pBuildingType->GetFoundationHeight(false);
		const short topLeftX = cell.X;
		const short topLeftY = cell.Y;
		const short bottomRightX = topLeftX + foundationWidth;
		const short bottomRightY = topLeftY + foundationHeight;

		const int capacity = foundationWidth * foundationHeight;
		std::vector<CellClass*> checkedCells;
		checkedCells.reserve(capacity > 0 ? capacity : 1);
		bool canBuild = true;
		bool noOccupy = true;

		for (short curX = topLeftX; curX < bottomRightX && canBuild; ++curX)
		{
			for (short curY = topLeftY; curY < bottomRightY && canBuild; ++curY)
			{
				CellClass* const pCell = MapClass::Instance->GetCellAt(CellStruct{curX, curY});

				if (!pCell || !pCell->CanThisExistHere(pBuildingType->SpeedType, pBuildingType, pUnit->Owner))
					canBuild = false;
				else if (BuildOnOccupiersHelpers::Exist)
					noOccupy = false;

				checkedCells.push_back(pCell);
			}
		}

		if (!canBuild)
		{
			return CanNotBuild;
		}
		else if (!noOccupy)
		{
			const CellStruct topLeftCell {topLeftX, topLeftY};
			const CellStruct foundationCell {foundationWidth, foundationHeight};

			for (auto const& pCheckedCell : checkedCells)
				pCheckedCell->AltFlags |= AltCellFlags::Unknown_4;

			BuildingTypeExt::CleanUpBuildingSpace(topLeftCell, foundationCell, pUnit->Owner, pUnit);

			for (auto const& pCheckedCell : checkedCells)
				pCheckedCell->AltFlags &= ~AltCellFlags::Unknown_4;

			// TODO Redeploy

			return CanNotBuild;
		}
	}

	return CanBuild;
}

// Buildable-upon TechnoTypes Hook #10-2 -> sub_7393C0 - Skip vanilla scatter shit when is deploying
DEFINE_JUMP(LJMP, 0x73950F, 0x73953B);

// Buildable-upon TechnoTypes Hook #10-3 -> sub_73FD50 - Push the owner house into deploy check
DEFINE_HOOK(0x73FF8F, UnitClass_WhatAction_ShowDeployCursor, 0x6)
{
	if (RulesExt::Global()->ExpandBuildingPlace)
	{
		GET(UnitClass*, pUnit, ESI);
		LEA_STACK(HouseClass**, pHousePtr, STACK_OFFSET(0x20, -0x20));
		*pHousePtr = pUnit->Owner;
	}

	return 0;
}
