#include "Body.h"

#include <TacticalClass.h>
#include "LocomotionClass.h"

#pragma region MarkScatterState

// Unmark Flag
DEFINE_HOOK(0x4DF0D0, FootClass_AbortMotion_ScatterClear, 0x8)
{
	GET(FootClass* const, pThis, ECX);

	TechnoExt::ExtMap.Find(pThis)->IsScattering = false;

	return 0;
}

DEFINE_HOOK(0x4D82B0, FootClass_EnterIdleMode_ScatterClear, 0x5)
{
	GET(FootClass* const, pThis, ECX);

	TechnoExt::ExtMap.Find(pThis)->IsScattering = false;

	return 0;
}

DEFINE_HOOK(0x51AA45, InfantryClass_SetDestination_ScatterClear, 0x7)
{
	GET(InfantryClass* const, pThis, ECX);

	TechnoExt::ExtMap.Find(pThis)->IsScattering = false;

	return 0;
}

DEFINE_HOOK(0x741978, UnitClass_SetDestination_ScatterClear, 0x9)
{
	GET(UnitClass* const, pThis, ECX);

	TechnoExt::ExtMap.Find(pThis)->IsScattering = false;

	return 0;
}

// Mark Flag
DEFINE_HOOK(0x51D43F, InfantryClass_Scatter_ScatterRecord1, 0x6)
{
	enum { SkipGameCode = 0x51D45B };

	GET(InfantryClass* const, pThis, ESI);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x50, -0x38));

	pThis->SetDestination(MapClass::Instance->GetCellAt(cell), true);

	if (RulesExt::Global()->ExtendedScatterAction)
		TechnoExt::ExtMap.Find(pThis)->IsScattering = true;

	return SkipGameCode;
}

DEFINE_HOOK(0x743C91, UnitClass_Scatter_ScatterRecord1, 0x7)
{
	enum { SkipGameCode = 0x744076 };

	GET(UnitClass* const, pThis, EBP);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x58, 0x4));

	pThis->SetDestination(MapClass::Instance->GetCellAt(cell), true);

	if (RulesExt::Global()->ExtendedScatterAction)
		TechnoExt::ExtMap.Find(pThis)->IsScattering = true;

	return SkipGameCode;
}

#pragma endregion

#pragma region EnhancedScatterContent

static inline void EnhancedScatterContent(CellClass* pCell, TechnoClass* pCaller, const CoordStruct& coords, bool alt)
{
	for (auto pObject = (alt ? pCell->AltObject : pCell->FirstObject); pObject; pObject = pObject->NextObject)
	{
		const auto pFoot = abstract_cast<FootClass*>(pObject);

		if (!pFoot || pFoot == pCaller || pFoot->IsTether || !pFoot->Owner->IsAlliedWith(pCaller))
			continue;

		const auto pFootExt = TechnoExt::ExtMap.Find(pFoot);

		if (pFootExt->IsScattering || pFoot->NavQueue.Count > 0)
			pFootExt->IsScattering = false;
		else if (const auto pFootDestination = pFoot->Destination)
			pFoot->NavQueue.AddItem(pFootDestination);

		pFoot->Scatter(coords, true, true);
	}
}

static void __fastcall CallEnhancedScatterContent(CellClass* pCell, TechnoClass* pCaller, const CoordStruct& coords, bool alt)
{
	if (const auto pFoot = abstract_cast<FootClass*>(pCaller))
	{
		if (RulesExt::Global()->ExtendedScatterAction)
			EnhancedScatterContent(pCell, pFoot, (pFoot->WhatAmI() == AbstractType::Infantry ? CoordStruct::Empty : coords), alt);
		else
			pCell->ScatterContent(CoordStruct::Empty, true, true, alt);
	}
	else // Building
	{
		pCell->ScatterContent(CoordStruct::Empty, true, true, alt);

		if (RulesExt::Global()->ExtendedScatterAction)
		{
			for (int i = 0; i < 8; ++i)
			{
				const auto pNearCell = pCell->GetNeighbourCell(static_cast<FacingType>(i));
				EnhancedScatterContent(pNearCell, pCaller, coords, alt);
			}
		}
		else
		{
			for (int i = 0; i < 8; ++i)
			{
				const auto pNearCell = pCell->GetNeighbourCell(static_cast<FacingType>(i));

				if (pNearCell->FindTechnoNearestTo(Point2D::Empty, false, pCaller))
					pNearCell->ScatterContent(coords, true, true, alt);
			}
		}
	}
}

static inline void CallEnhancedScatterContent(CellClass* pCell, FootClass* pFoot, bool alt)
{
	CallEnhancedScatterContent(pCell, pFoot, pFoot->Location, alt);
}

// Factory Entrance
DEFINE_HOOK(0x4495DF, BuildingClass_CheckWeaponFactoryOutsideBusy_ScatterEntranceContent, 0x5)
{
	enum { Busy = 0x449691, NotBusy = 0x44969B };

	GET(BuildingClass* const, pThis, ESI);
	GET(CellClass* const, pCell, EAX);
	GET_STACK(const CoordStruct, coords, STACK_OFFSET(0x30, -0xC));

	const auto pTechno = pCell->FindTechnoNearestTo(Point2D::Empty, false, pThis);

	if (!pTechno)
		return NotBusy;

	if (RulesExt::Global()->ExtendedScatterAction && !pTechno->Owner->IsAlliedWith(pThis))
		return Busy;

	CallEnhancedScatterContent(pCell, pThis, coords, false);

	return Busy;
}

// Locomotion Process
DEFINE_HOOK(0x4B1F2C, DriveLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x4B1F48 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(Point2D* const, pCoords, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(MapClass::Instance->GetTargetCell(*pCoords), pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B2DB4, DriveLocomotionClass_MovingProcess2_ScatterForwardContent1, 0x5)
{
	enum { SkipGameCode = 0x4B2DC5 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, EDI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B3271, DriveLocomotionClass_MovingProcess2_ScatterForwardContent2, 0x5)
{
	enum { SkipGameCode = 0x4B3282 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B391F, DriveLocomotionClass_MovingProcess2_ScatterForwardContent3, 0x5)
{
	enum { SkipGameCode = 0x4B3607 };

	GET(LocomotionClass* const, pThis, EBP);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x5C, -0x48));
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(MapClass::Instance->GetCellAt(cell), pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B442D, DriveLocomotionClass_MovingProcess2_ScatterForwardContent4, 0x5)
{
	enum { SkipGameCode = 0x4B41B3 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ECX);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x515966, HoverLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x515982 };

	GET(LocomotionClass* const, pThis, EBX);
	GET(Point2D* const, pCoords, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(MapClass::Instance->GetTargetCell(*pCoords), pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x516BB9, HoverLocomotionClass_MovingProcess2_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x516BCA };

	GET(LocomotionClass* const, pThis, ESI);
	GET(CellClass* const, pCell, EBX);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x5B0BA4, MechLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x5B0BB5 };

	GET(LocomotionClass* const, pThis, EBX);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A156F, ShipLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x6A158B };

	GET(LocomotionClass* const, pThis, EBP);
	GET(Point2D* const, pCoords, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(MapClass::Instance->GetTargetCell(*pCoords), pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A2404, ShipLocomotionClass_MovingProcess2_ScatterForwardContent1, 0x5)
{
	enum { SkipGameCode = 0x6A2415 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, EDI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A28C1, ShipLocomotionClass_MovingProcess2_ScatterForwardContent2, 0x5)
{
	enum { SkipGameCode = 0x6A28D2 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A2F6E, ShipLocomotionClass_MovingProcess2_ScatterForwardContent3, 0x5)
{
	enum { SkipGameCode = 0x6A2C56 };

	GET(LocomotionClass* const, pThis, EBP);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x5C, -0x48));
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(MapClass::Instance->GetCellAt(cell), pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A3A59, ShipLocomotionClass_MovingProcess2_ScatterForwardContent4, 0x5)
{
	enum { SkipGameCode = 0x6A37DF };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ECX);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

DEFINE_HOOK(0x75B885, WalkLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x75B896 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	CallEnhancedScatterContent(pCell, pThis->LinkedTo, alt);

	return SkipGameCode;
}

#pragma endregion

#pragma region EnhancedScatterExecute

static inline void ScatterPathCellContent(FootClass* pThis, CellClass* pCell)
{
	for (auto pObject = pCell->GetContent(); pObject; pObject = pObject->NextObject)
	{
		const auto pFoot = abstract_cast<FootClass*>(pObject);

		if (!pFoot || pFoot == pThis || pFoot->IsTether || !pFoot->Owner->IsAlliedWith(pThis))
			continue;

		if (std::abs(static_cast<short>(static_cast<short>(pFoot->PrimaryFacing.Desired().Raw) - static_cast<short>(pThis->PrimaryFacing.Current().Raw))) < 24576)
			continue;

		const auto pFootExt = TechnoExt::ExtMap.Find(pFoot);

		if (pFootExt->IsScattering || pFoot->NavQueue.Count > 0)
			pFootExt->IsScattering = false;
		else if (const auto pFootDestination = pFoot->Destination)
			pFoot->NavQueue.AddItem(pFootDestination);

		pFoot->Scatter(CoordStruct::Empty, true, true);
	}
}

static inline CellStruct GetScatterCell(FootClass* pThis, int face)
{
	const auto thisCoord = pThis->GetDestination();
	const auto thisCell = CellClass::Coord2Cell(thisCoord);
	const auto pThisCell = MapClass::Instance->GetCellAt(thisCell);
	const auto height = (pThisCell->Level + (pThis->IsOnBridge() ? 4 : 0)) * Unsorted::LevelHeight;
	auto alternativeCell = CellStruct::Empty;
	auto alternativeMove = Move::No;

	for (int i = 0; i < 8; ++i)
	{
		const auto facing = static_cast<size_t>(face + i) & 7u;
		const auto cell = thisCell + CellSpread::GetNeighbourOffset(facing);
		const auto pCell = MapClass::Instance->GetCellAt(cell);

		if (!MapClass::Instance->IsWithinUsableArea(cell, true))
			continue;

		const auto move = pThis->IsCellOccupied(pCell, static_cast<FacingType>(facing), pThis->GetCellLevel(), nullptr, true);

		if (move != Move::OK) // More selects
		{
			if (move == Move::Temp)
			{
				if (alternativeMove != Move::Temp)
				{
					alternativeCell = cell;
					alternativeMove = move;
				}
			}
			else if (move == Move::ClosedGate)
			{
				if (alternativeMove != Move::ClosedGate && alternativeMove != Move::Temp)
				{
					alternativeCell = cell;
					alternativeMove = move;
				}
			}
			else if (move == Move::Destroyable)
			{
				if (alternativeMove != Move::Destroyable && alternativeMove != Move::ClosedGate && alternativeMove != Move::Temp)
				{
					alternativeCell = cell;
					alternativeMove = move;
				}
			}
			else if (move == Move::Cloak)
			{
				if (alternativeMove == Move::No || alternativeMove == Move::FriendlyDestroyable || alternativeMove == Move::MovingBlock)
				{
					alternativeCell = cell;
					alternativeMove = move;
				}
			}
			else if (move == Move::MovingBlock)
			{
				if (alternativeMove == Move::No || alternativeMove == Move::FriendlyDestroyable)
				{
					alternativeCell = cell;
					alternativeMove = move;
				}
			}
			else if (move == Move::FriendlyDestroyable)
			{
				if (alternativeMove == Move::No)
				{
					alternativeCell = cell;
					alternativeMove = move;
				}
			}

			continue;
		}

		auto coords = CellClass::Cell2Coord(cell, height);
		auto buffer = CellStruct::Empty;
		const auto mapCell = *reinterpret_cast<CellStruct*(__thiscall*)(TacticalClass*, CellStruct*, CoordStruct*)>(0x6D6410)(TacticalClass::Instance(), &buffer, &coords);

		if (cell != mapCell) // || pCell->ContainsBridge()
			continue;

		return cell;
	}

	return alternativeCell;
}

static inline int GetTechnoCloseEnoughRange(TechnoClass* pCaller)
{
	if (TechnoExt::ExtMap.Find(pCaller)->IsScattering)
		return pCaller->WhatAmI() == AbstractType::Infantry ? 128 : 0;

	return RulesClass::Instance->CloseEnough;
}

// Check Clear
DEFINE_JUMP(LJMP, 0x73F8E0, 0x73FA2C) // Skip face to face check. Why check this?

// Override Mission
DEFINE_HOOK(0x4B3659, DriveLocomotionClass_MovingProcess2_CheckOverrideMission, 0x6)
{
	GET(FootClass* const, pThis, EAX);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x5C, -0x48));

	ScatterPathCellContent(pThis, MapClass::Instance->GetCellAt(cell));

	return 0;
}

DEFINE_HOOK(0x515D30, HoverLocomotionClass_MovingProcess_CheckOverrideMission, 0x6)
{
	GET(FootClass* const, pThis, EAX);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x68, -0x5C));

	ScatterPathCellContent(pThis, MapClass::Instance->GetCellAt(cell));

	return 0;
}

DEFINE_HOOK(0x5B0BCA, MechLocomotionClass_MovingProcess_CheckOverrideMission, 0x6)
{
	GET(FootClass* const, pThis, EAX);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x68, -0x54));

	ScatterPathCellContent(pThis, MapClass::Instance->GetCellAt(cell));

	return 0;
}

DEFINE_HOOK(0x6A2CA8, ShipLocomotionClass_MovingProcess2_CheckOverrideMission, 0x6)
{
	GET(FootClass* const, pThis, EAX);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x5C, -0x48));

	ScatterPathCellContent(pThis, MapClass::Instance->GetCellAt(cell));

	return 0;
}

DEFINE_HOOK(0x75B8AC, WalkLocomotionClass_MovingProcess_CheckOverrideMission, 0x6)
{
	GET(FootClass* const, pThis, EAX);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x48, -0x38));

	ScatterPathCellContent(pThis, MapClass::Instance->GetCellAt(cell));

	return 0;
}

// Execute Scatter
DEFINE_HOOK(0x51D487, InfantryClass_Scatter_EnhancedScatter, 0x6)
{
	if (!RulesExt::Global()->ExtendedScatterAction)
		return 0;

	enum { SkipGameCode = 0x51D6E6 };

	GET(InfantryClass* const, pThis, ESI);
	GET_STACK(const int, face, STACK_OFFSET(0x50, -0x34));

	const auto cell = GetScatterCell(pThis, face);

	if (cell != CellStruct::Empty)
	{
		pThis->QueueMission(Mission::Move, false);
		pThis->SetDestination(MapClass::Instance->GetCellAt(cell), true);
		TechnoExt::ExtMap.Find(pThis)->IsScattering = true;
	}

	return SkipGameCode;
}

DEFINE_HOOK(0x743E08, UnitClass_Scatter_EnhancedScatter, 0x7)
{
	if (!RulesExt::Global()->ExtendedScatterAction)
		return 0;

	enum { SkipGameCode = 0x744076 };

	GET(UnitClass* const, pThis, EBP);
	GET(const int, face, EDI);

	const auto cell = GetScatterCell(pThis, face);

	if (cell != CellStruct::Empty)
	{
		pThis->QueueMission(Mission::Move, false);
		pThis->SetDestination(MapClass::Instance->GetCellAt(cell), true);
		TechnoExt::ExtMap.Find(pThis)->IsScattering = true;
	}

	return SkipGameCode;
}

// Range Check
DEFINE_HOOK(0x4B2979, DriveLocomotionClass_MovingProcess2_GetCloseEnoughRange1, 0x6)
{
	enum { Greater = 0x4B2A47, Lower = 0x4B298B };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x4B2C56, DriveLocomotionClass_MovingProcess2_GetCloseEnoughRange2, 0x6)
{
	enum { Greater = 0x4B2D68, Lower = 0x4B2C68 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x4B313B, DriveLocomotionClass_MovingProcess2_GetCloseEnoughRange3, 0x6)
{
	enum { Greater = 0x4B3225, Lower = 0x4B314D };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x4B37B8, DriveLocomotionClass_MovingProcess2_GetCloseEnoughRange4, 0x6)
{
	enum { Greater = 0x4B38B3, Lower = 0x4B37CA };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x4B42D5, DriveLocomotionClass_MovingProcess2_GetCloseEnoughRange5, 0x6)
{
	enum { Greater = 0x4B43D0, Lower = 0x4B42E7 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x51586A, HoverLocomotionClass_MovingProcess_GetCloseEnoughRange, 0x6)
{
	enum { Greater = 0x515902, Lower = 0x51587C };

	GET(LocomotionClass* const, pThis, EBX);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x51676F, HoverLocomotionClass_MovingProcess2_GetCloseEnoughRange1, 0x6)
{
	enum { Greater = 0x5167BF, Lower = 0x51677D };

	GET(LocomotionClass* const, pThis, ESI);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x516AC9, HoverLocomotionClass_MovingProcess2_GetCloseEnoughRange2, 0x6)
{
	enum { Greater = 0x516B6D, Lower = 0x516ADB };

	GET(LocomotionClass* const, pThis, ESI);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x5B0349, MechLocomotionClass_MovingProcess_GetCloseEnoughRange1, 0x6)
{
	enum { Greater = 0x5B0375, Lower = 0x5B0357 };

	GET(LocomotionClass* const, pThis, EBX);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x5B0A99, MechLocomotionClass_MovingProcess_GetCloseEnoughRange2, 0x6)
{
	enum { Greater = 0x5B0B59, Lower = 0x5B0AAB };

	GET(LocomotionClass* const, pThis, EBX);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x6A1FC9, ShipLocomotionClass_MovingProcess2_GetCloseEnoughRange1, 0x6)
{
	enum { Greater = 0x6A2097, Lower = 0x6A1FDB };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x6A22A6, ShipLocomotionClass_MovingProcess2_GetCloseEnoughRange2, 0x6)
{
	enum { Greater = 0x6A23B8, Lower = 0x6A22B8 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x6A278B, ShipLocomotionClass_MovingProcess2_GetCloseEnoughRange3, 0x6)
{
	enum { Greater = 0x6A2875, Lower = 0x6A279D };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x6A2E07, ShipLocomotionClass_MovingProcess2_GetCloseEnoughRange4, 0x6)
{
	enum { Greater = 0x6A2F02, Lower = 0x6A2E19 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x6A3901, ShipLocomotionClass_MovingProcess2_GetCloseEnoughRange5, 0x6)
{
	enum { Greater = 0x6A39FC, Lower = 0x6A3913 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x75B040, WalkLocomotionClass_MovingProcess_GetCloseEnoughRange1, 0x6)
{
	enum { Greater = 0x75B06C, Lower = 0x75B04E };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

DEFINE_HOOK(0x75B75E, WalkLocomotionClass_MovingProcess_GetCloseEnoughRange2, 0x6)
{
	enum { Greater = 0x75B839, Lower = 0x75B770 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(const int, distance, EAX);

	return distance > GetTechnoCloseEnoughRange(pThis->LinkedTo) ? Greater : Lower;
}

#pragma endregion
