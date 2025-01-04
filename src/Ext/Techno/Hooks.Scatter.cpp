#include "Body.h"

#include "LocomotionClass.h"

static void __fastcall CallEnhancedScatterContent(CellClass* pCell, TechnoClass* pCaller, const CoordStruct& coords)
{
	pCell->ScatterContent(CoordStruct::Empty, true, true, false);

	for (int i = 0; i < 8; ++i)
	{
		const auto pNearCell = pCell->GetNeighbourCell(static_cast<FacingType>(i));
		const auto pNearTechno = pNearCell->FindTechnoNearestTo(Point2D::Empty, false, pCaller);

		if (!pNearTechno)
			continue;

		const auto pNearOwner = pNearTechno->Owner;

		if (!pNearOwner || !pNearOwner->IsAlliedWith(pCaller))
			continue;

		pNearCell->ScatterContent(coords, true, true, false);
	}

/*
	TODO
*/
}

DEFINE_HOOK(0x4495DF, BuildingClass_CheckWeaponFactoryOutsideBusy_ScatterEntranceContent, 0x5)
{
	enum { Busy = 0x449691, NotBusy = 0x44969B };

	GET(BuildingClass* const, pThis, ESI);
	GET(CellClass* const, pCell, EAX);
	GET_STACK(const CoordStruct, coords, STACK_OFFSET(0x30, -0xC));

	const auto pTechno = pCell->FindTechnoNearestTo(Point2D::Empty, false, pThis);

	if (!pTechno)
		return NotBusy;

	const auto pOwner = pTechno->Owner;

	if (!pOwner || !pOwner->IsAlliedWith(pThis))
		return Busy;

	CallEnhancedScatterContent(pCell, pThis, coords);

	return Busy;
}

DEFINE_HOOK(0x4B1F2C, DriveLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x4B1F48 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(Point2D* const, pCoords, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	const auto pCell = MapClass::Instance->GetTargetCell(*pCoords);
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B2DB4, DriveLocomotionClass_MovingProcess2_ScatterForwardContent1, 0x5)
{
	enum { SkipGameCode = 0x4B2DC5 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, EDI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B3271, DriveLocomotionClass_MovingProcess2_ScatterForwardContent2, 0x5)
{
	enum { SkipGameCode = 0x4B3282 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B391F, DriveLocomotionClass_MovingProcess2_ScatterForwardContent3, 0x5)
{
	enum { SkipGameCode = 0x4B3607 };

	GET(LocomotionClass* const, pThis, EBP);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x5C, -0x48));
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	const auto pCell = MapClass::Instance->GetCellAt(cell);
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x4B442D, DriveLocomotionClass_MovingProcess2_ScatterForwardContent4, 0x5)
{
	enum { SkipGameCode = 0x4B41B3 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ECX);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x515966, HoverLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x515982 };

	GET(LocomotionClass* const, pThis, EBX);
	GET(Point2D* const, pCoords, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	const auto pCell = MapClass::Instance->GetTargetCell(*pCoords);
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x516BB9, HoverLocomotionClass_MovingProcess2_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x516BCA };

	GET(LocomotionClass* const, pThis, ESI);
	GET(CellClass* const, pCell, EBX);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x5B0BA4, MechLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x5B0BB5 };

	GET(LocomotionClass* const, pThis, EBX);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A156F, ShipLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x6A158B };

	GET(LocomotionClass* const, pThis, EBP);
	GET(Point2D* const, pCoords, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	const auto pCell = MapClass::Instance->GetTargetCell(*pCoords);
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A2404, ShipLocomotionClass_MovingProcess2_ScatterForwardContent1, 0x5)
{
	enum { SkipGameCode = 0x6A2415 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, EDI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A28C1, ShipLocomotionClass_MovingProcess2_ScatterForwardContent2, 0x5)
{
	enum { SkipGameCode = 0x6A28D2 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A2F6E, ShipLocomotionClass_MovingProcess2_ScatterForwardContent3, 0x5)
{
	enum { SkipGameCode = 0x6A2C56 };

	GET(LocomotionClass* const, pThis, EBP);
	GET_STACK(const CellStruct, cell, STACK_OFFSET(0x5C, -0x48));
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	const auto pCell = MapClass::Instance->GetCellAt(cell);
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x6A3A59, ShipLocomotionClass_MovingProcess2_ScatterForwardContent4, 0x5)
{
	enum { SkipGameCode = 0x6A37DF };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ECX);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}

DEFINE_HOOK(0x75B885, WalkLocomotionClass_MovingProcess_ScatterForwardContent, 0x5)
{
	enum { SkipGameCode = 0x75B896 };

	GET(LocomotionClass* const, pThis, EBP);
	GET(CellClass* const, pCell, ESI);
	GET(const bool, alt, EDX);

	const auto pLinkTo = pThis->LinkedTo;
	CallEnhancedScatterContent(pCell, pLinkTo, pLinkTo->Location);

	return SkipGameCode;
}
