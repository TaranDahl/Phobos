#include <AircraftClass.h>
#include "Body.h"
#include <ScenarioClass.h>
#include <TunnelLocomotionClass.h>
#include <Ext/BuildingType/Body.h>
#include <Ext/House/Body.h>
#include <Ext/Anim/Body.h>
#include <Ext/WarheadType/Body.h>
#include <Ext/WeaponType/Body.h>
#include <Utilities/EnumFunctions.h>

DEFINE_HOOK(0x73E730, UnitClass_MissionHarvest_HarvesterScanAfterUnload, 0x5)
{
	GET(UnitClass* const, pThis, EBP);
	GET(AbstractClass* const, pFocus, EAX);

	// Focus is set when the harvester is fully loaded and go home.
	if (pFocus && RulesExt::Global()->HarvesterScanAfterUnload)
	{
		CellStruct cellBuffer = { 0,0 };
		CellStruct* pCellStru = (CellStruct*)pThis->ScanForTiberium((DWORD)&cellBuffer, RulesClass::Instance->TiberiumLongScan / 256, 0);

		if (*pCellStru != CellStruct::Empty)
		{
			auto pCell = MapClass::Instance->GetCellAt(*pCellStru);
			int distFromTiberium = pCell ? pThis->DistanceFrom(pCell) : -1;
			int distFromFocus = pThis->DistanceFrom(pFocus);

			// Check if pCell is better than focus.
			if (distFromTiberium > 0 && distFromTiberium < distFromFocus)
				R->EAX(pCell);
		}
	}

	return 0;
}

void __fastcall ArrivingRefineryNearBy(UnitClass* pThis, BuildingClass* pDock)
{
	pDock->UpdateRefinerySmokeSystems();
	reinterpret_cast<int(__thiscall*)(BuildingClass*, int, int, bool, int)>(0x451750)(pDock, 7, !pDock->IsGreenHP(), 0, 0); //BuildingClass::PlayAnimByIdx
	reinterpret_cast<int(__thiscall*)(BuildingClass*, int, int, bool, int)>(0x451750)(pDock, 8, !pDock->IsGreenHP(), 0, 0);

	HouseClass* const pOwner = pDock->GetOwningHouse();

	if (pThis->Type->Weeder)
	{
		bool playAnim = true;

		while (true)
		{
			const int idx = reinterpret_cast<int(__fastcall*)(StorageClass*)>(0x6C9820)(&pThis->Tiberium);

			if (idx == -1)
				break;

			float amount = pThis->Tiberium.GetAmount(idx);
			amount = pThis->Tiberium.RemoveAmount(amount, idx);

			if (amount <= 0.0)
				continue;

			playAnim = false;
			reinterpret_cast<int(__thiscall*)(HouseClass*, int, int)>(0x4F9700)(pOwner, static_cast<int>(amount), idx);
		}

		if (playAnim && pDock->Anims[10])
		{
			pDock->DestroyNthAnim(BuildingAnimSlot::Special);
		}
	}
	else
	{
		int numPurifier = pOwner->NumOrePurifiers;

		if (!pOwner->IsHumanPlayer && SessionClass::Instance->GameMode != GameMode::Campaign)
			numPurifier = RulesClass::Instance->AIVirtualPurifiers.Items[static_cast<int>(pOwner->AIDifficulty)] + numPurifier;

		const float multiplier = numPurifier * RulesClass::Instance->PurifierBonus;
		int money = 0;

		while (true)
		{
			const int idx = reinterpret_cast<int(__fastcall*)(StorageClass*)>(0x6C9820)(&pThis->Tiberium);

			if (idx == -1)
				break;

			float amount = pThis->Tiberium.GetAmount(idx);
			amount = pThis->Tiberium.RemoveAmount(amount, idx);

			if (amount <= 0.0)
				continue;

			money += static_cast<int>(amount * TiberiumClass::Array->Items[idx]->Value);
			const float amountFromPurifier = amount * multiplier;

			if (amountFromPurifier > 0.0)
				money += static_cast<int>(amountFromPurifier * TiberiumClass::Array->Items[idx]->Value);
		}

		if (money)
		{
			pOwner->GiveMoney(money);

			if (BuildingTypeExt::ExtData* const pDockTypeExt = BuildingTypeExt::ExtMap.Find(pDock->Type))
			{
				RulesExt::ExtData* const pRulesExt = RulesExt::Global();

				if ((pRulesExt->DisplayIncome_AllowAI || pDock->Owner->IsControlledByHuman())
					&& pDockTypeExt->DisplayIncome.Get(pRulesExt->DisplayIncome))
				{
					FlyingStrings::AddMoneyString(
						money,
						pDock->Owner,
						pDockTypeExt->DisplayIncome_Houses.Get(pRulesExt->DisplayIncome_Houses.Get()),
						pDock->GetRenderCoords(),
						pDockTypeExt->DisplayIncome_Offset
					);
				}
			}

			// pThis->Animation.Value = 0;
		}
		else if (pDock->Anims[10])
		{
			pDock->DestroyNthAnim(BuildingAnimSlot::Special);
		}
	}

	pThis->MissionStatus = 0;
	pThis->SetDestination(nullptr, false);
}

// MissionStatus == 2 means the unit is returning.
DEFINE_HOOK(0x73EB2C, UnitClass_MissionHarvest_Status2, 0x6)
{
	enum { SkipGameCode = 0x73EE85 };

	GET(UnitClass* const, pThis, EBP);

	UnitTypeClass* const pType = pThis->Type;
	TechnoTypeExt::ExtData* const pTypeExt = TechnoTypeExt::ExtMap.Find(pType);

	if (!pTypeExt || !pTypeExt->HarvesterQuickUnloader)
		return 0;

	std::vector<BuildingTypeClass*> docks;

	if (const int dockCount = pType->Dock.Count)
	{
		docks.reserve(dockCount);

		for (auto const& pBuildingType : pType->Dock)
		{
			if (pBuildingType)
				docks.push_back(pBuildingType);
		}
	}

	if (!docks.size())
		return SkipGameCode;

	// Check arrived
	CellClass* const pThisCell = pThis->GetCell();

	for (int i = 0; i < 4; ++i)
	{
		if (BuildingClass* const pBuilding = pThisCell->GetNeighbourCell(static_cast<FacingType>(i << 1))->GetBuilding())
		{
			BuildingTypeClass* const pCellBuildingType = pBuilding->Type;

			for (auto const& pBuildingType : docks)
			{
				if (pCellBuildingType == pBuildingType)
				{
					ArrivingRefineryNearBy(pThis, pBuilding);
					return SkipGameCode;
				}
			}
		}
	}

	HouseClass* const pHouse = pThis->Owner;

	// Check destination
	if (AbstractClass* const pDestination = pThis->Destination)
	{
		if (!pHouse->RecheckTechTree)
		{
			CellClass* const pDestinationCell = (pDestination->WhatAmI() == AbstractType::Cell) ?
				static_cast<CellClass*>(pDestination) : MapClass::Instance->GetCellAt(pDestination->GetCoords());

			for (int i = 0; i < 4; ++i)
			{
				if (BuildingClass* const pBuilding = pDestinationCell->GetNeighbourCell(static_cast<FacingType>(i << 1))->GetBuilding())
				{
					BuildingTypeClass* const pCellBuildingType = pBuilding->Type;

					for (auto const& pBuildingType : docks)
					{
						if (pCellBuildingType == pBuildingType)
							return SkipGameCode;
					}
				}
			}
		}
	}

	// Find nearest dock
	const CoordStruct thisLocation = pThis->GetCoords();
	const Point2D thisPosition { (thisLocation.X >> 4), (thisLocation.Y >> 4) };

	const CoordStruct destLocation = pThis->GetDestination();
	CellStruct destCell { static_cast<short>(destLocation.X >> 8), static_cast<short>(destLocation.Y >> 8) };

	int distanceSquared = INT_MAX;
	BuildingClass* pDock = nullptr;

	for (auto const& pBuildingType : docks)
	{
		for (auto const& pBuilding : pHouse->Buildings)
		{
			if (pBuilding && pBuilding->Type == pBuildingType && !pBuilding->InLimbo) // Prevent check radio links
			{
				const CoordStruct dockLocation = pBuilding->GetCoords();
				CellStruct dockCell { static_cast<short>(dockLocation.X >> 8), static_cast<short>(dockLocation.Y >> 8) };

				if (reinterpret_cast<bool(__thiscall*)(DisplayClass*, CellStruct*, CellStruct*, MovementZone, bool, bool, bool)>(0x56D100)
					(DisplayClass::Instance, &destCell, &dockCell, pType->MovementZone, pThis->IsOnBridge(), false ,false)) // Prevent send command
				{
					const Point2D difference { (thisPosition.X - (dockLocation.X >> 4)), (thisPosition.Y - (dockLocation.Y >> 4)) };
					const int newDistanceSquared = (difference.X * difference.X) + (difference.Y * difference.Y);

					if (newDistanceSquared < distanceSquared) // No check for primary building
					{
						distanceSquared = newDistanceSquared;
						pDock = pBuilding;
					}
				}
			}
		}
	}

	if (!pDock)
	{
		pThis->SetDestination(nullptr, true);
		return SkipGameCode;
	}

	// Find a final destination
	const CoordStruct dockLocation = pDock->GetCoords();
	destCell = CellClass::Coord2Cell(dockLocation);
	CellStruct closeTo = CellStruct::Empty;

	if (distanceSquared > 6400)
	{
		const CellStruct difference = CellClass::Coord2Cell(thisLocation) - destCell;
		const bool bias = abs(difference.X) >= abs(difference.Y);
		closeTo.X = bias ? static_cast<short>(Math::sgn(difference.X)) : 0;
		closeTo.Y = bias ? 0 : static_cast<short>(Math::sgn(difference.Y));
	}

	destCell = MapClass::Instance->NearByLocation(destCell, pType->SpeedType, -1, pType->MovementZone, false, 1, 1, false, false, false, true, closeTo, false, false);

	if (destCell == CellStruct::Empty)
	{
		pThis->SetDestination(nullptr, true);
		return SkipGameCode;
	}

	CellClass* const pDestCell = MapClass::Instance->TryGetCellAt(destCell);

	if (!pDestCell || !pType->Teleporter)
	{
		pThis->SetDestination(pDestCell, true);
		return SkipGameCode;
	}

	// Teleporters
	if (FootClass* const ParasiteEatingMe = pThis->ParasiteEatingMe)
		ParasiteEatingMe->ParasiteImUsing->ExitUnit();

	pThis->Mark(MarkType::Up);
	int sound = pType->ChronoOutSound;

	if (sound != -1 || (sound = RulesClass::Instance->ChronoOutSound, sound != -1))
		VocClass::PlayAt(sound, thisLocation, 0);

	if (AnimTypeClass* const pWarpIn = pTypeExt->WarpIn.Get(RulesClass::Instance->WarpIn))
		GameCreate<AnimClass>(pWarpIn, pThis->Location, 0, 1)->Owner = pHouse;

	pThis->SetLocation(pDestCell->GetCoords());
	pThis->OnBridge = pDestCell->ContainsBridge();
	pThis->SetHeight(0);
	pThis->Mark(MarkType::Down);
	pThis->UpdatePosition(PCPType::End);
	pThis->Locomotor->Stop_Moving();
	// pThis->SetDestination(nullptr, true);
	pDestCell->CollectCrate(pThis);
	pThis->unknown_280 = 0;

	if ((sound = pType->ChronoInSound, sound != -1) || (sound = RulesClass::Instance->ChronoInSound, sound != -1))
		VocClass::PlayAt(sound, pThis->Location, 0);

	if (AnimTypeClass* const pWarpOut = pTypeExt->WarpOut.Get(RulesClass::Instance->WarpOut))
		GameCreate<AnimClass>(pWarpOut, pThis->Location, 0, 1)->Owner = pHouse;

	return SkipGameCode;
}
