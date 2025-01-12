#include "Body.h"

#include "LocomotionClass.h"

#pragma region MarkScatterState

static inline void StopMovingAndRevertMission(FootClass* pFoot)
{
	const auto pExt = TechnoExt::ExtMap.Find(pFoot);

	if (!pExt->IsScattering)
		return;

	pExt->IsScattering = false;

	if (!pFoot->MissionIsOverriden())
		return;

	pFoot->Mission_Revert();
}

// Unmark Flag
DEFINE_HOOK(0x4D82B0, FootClass_EnterIdleMode_ScatterClear, 0x5)
{
	GET(FootClass* const, pThis, ECX);

	StopMovingAndRevertMission(pThis);

	return 0;
}

DEFINE_HOOK(0x51AA45, InfantryClass_SetDestination_ScatterClear, 0x7)
{
	GET(InfantryClass* const, pThis, ECX);

	StopMovingAndRevertMission(pThis);

	return 0;
}

DEFINE_HOOK(0x741978, UnitClass_SetDestination_ScatterClear, 0x9)
{
	GET(UnitClass* const, pThis, ECX);

	StopMovingAndRevertMission(pThis);

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

DEFINE_HOOK(0x51D6CA, InfantryClass_Scatter_ScatterRecord2, 0x6)
{
	enum { SkipGameCode = 0x51D6E6 };

	GET(InfantryClass* const, pThis, ESI);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x50, -0x40));

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

DEFINE_HOOK(0x744059, UnitClass_Scatter_ScatterRecord2, 0x7)
{
	enum { SkipGameCode = 0x744076 };

	GET(UnitClass* const, pThis, EBP);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x58, -0x40));

	pThis->SetDestination(MapClass::Instance->GetCellAt(cell), true);

	if (RulesExt::Global()->ExtendedScatterAction)
		TechnoExt::ExtMap.Find(pThis)->IsScattering = true;

	return SkipGameCode;
}

#pragma endregion

#pragma region EnhancedScatterContent

static inline FootClass* GetFootOnCell(CellClass* pCell, FootClass* pCaller)
{
	for (auto pObject = pCell->GetContent(); pObject; pObject = pObject->NextObject)
	{
		if (const auto pFoot = abstract_cast<FootClass*>(pObject))
			return pFoot;
	}

	return nullptr;
}

static inline bool CanEnhancedScatterContent(CellClass* pCell, TechnoClass* pCaller)
{
	bool scatter = false;

	for (auto pObject = pCell->GetContent(); pObject; pObject = pObject->NextObject)
	{
		const auto pFoot = abstract_cast<FootClass*>(pObject);

		if (!pFoot)
			continue;

		if (pFoot->IsTether || pFoot->Destination)
			return false;

		const auto pOwner = pFoot->Owner;

		if (!pOwner || !pOwner->IsAlliedWith(pCaller))
			return false;

		scatter = true;
	}

	return scatter;
}

static void __fastcall CallEnhancedScatterContent(CellClass* pCell, TechnoClass* pCaller, const CoordStruct& coords, bool alt)
{
	if (const auto pFoot = abstract_cast<FootClass*>(pCaller))
	{
		if (RulesExt::Global()->ExtendedScatterAction)
		{
			pCell->ScatterContent((pFoot->WhatAmI() == AbstractType::Infantry ? CoordStruct::Empty : coords), true, true, alt);

			if (pFoot->CurrentMapCoords != CellStruct::Empty)
			{
				pCell = MapClass::Instance->GetCellAt(pFoot->CurrentMapCoords);

				for (int i = 0; i < 24; ++i)
				{
					const auto face = pFoot->PathDirections[i];

					if (face <= -1 || face >= 8)
						break;

					pCell = pCell->GetNeighbourCell(static_cast<FacingType>(face));

					if (!CanEnhancedScatterContent(pCell, pFoot))
						break;

					pCell->ScatterContent(coords, true, true, alt);
				}
			}
		}
		else
		{
			pCell->ScatterContent(CoordStruct::Empty, true, true, alt);
		}
	}
	else // Building
	{
		pCell->ScatterContent(CoordStruct::Empty, true, true, alt);

		if (RulesExt::Global()->ExtendedScatterAction)
		{
			for (int i = 0; i < 8; ++i)
			{
				const auto pNearCell = pCell->GetNeighbourCell(static_cast<FacingType>(i));

				if (CanEnhancedScatterContent(pNearCell, pCaller))
					pNearCell->ScatterContent(coords, true, true, alt);
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

	if (RulesExt::Global()->ExtendedScatterAction)
	{
		const auto pOwner = pTechno->Owner;

		if (!pOwner || !pOwner->IsAlliedWith(pThis))
			return Busy;
	}

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

static inline int GetTechnoCloseEnoughRange(TechnoClass* pCaller)
{
	if (TechnoExt::ExtMap.Find(pCaller)->IsScattering)
		return pCaller->WhatAmI() == AbstractType::Infantry ? 128 : 0;

	return RulesClass::Instance->CloseEnough;
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
