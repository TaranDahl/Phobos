#include "ObjectInfo.h"

#include <Utilities/GeneralUtils.h>
#include <BuildingClass.h>
#include <InfantryClass.h>
#include <FootClass.h>
#include <TeamClass.h>
#include <HouseClass.h>
#include <ScriptClass.h>
#include <AITriggerTypeClass.h>
#include <Helpers/Enumerators.h>
#include <CRT.h>

#include <Ext/TechnoType/Body.h>
#include <Ext/Techno/Body.h>

const char* ObjectInfoCommandClass::GetName() const
{
	return "Dump ObjectInfo";
}

const wchar_t* ObjectInfoCommandClass::GetUIName() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_DUMP_OBJECT_INFO", L"Dump Object Info");
}

const wchar_t* ObjectInfoCommandClass::GetUICategory() const
{
	return CATEGORY_DEVELOPMENT;
}

const wchar_t* ObjectInfoCommandClass::GetUIDescription() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_DUMP_OBJECT_INFO_DESC", L"Dump ObjectInfo to log file and display it.");
}

void ObjectInfoCommandClass::Execute(WWKey eInput) const
{
	char buffer[0x800] = { 0 };
	int count = 0;

	auto append = [&buffer](const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		vsprintf_s(Phobos::readBuffer, pFormat, args);
		va_end(args);
		strcat_s(buffer, Phobos::readBuffer);
	};

	auto display = [&buffer]()
	{
		memset(Phobos::wideBuffer, 0, sizeof Phobos::wideBuffer);
		CRT::mbstowcs(Phobos::wideBuffer, buffer, strlen(buffer));
		MessageListClass::Instance->PrintMessage(Phobos::wideBuffer, 600, 5, true);
		Debug::Log("%s\n", buffer);
		buffer[0] = 0;
	};

	auto getTargetInfo = [&append, &count](TechnoClass* pCurrent, AbstractClass* pTarget, const char* pInfoName)
	{
		if (pTarget)
		{
			if (count)
				append(", ");

			auto mapCoords = CellStruct::Empty;
			auto ID = "N/A";

			if (auto const pObject = abstract_cast<ObjectClass*>(pTarget))
			{
				mapCoords = pObject->GetMapCoords();
				ID = pObject->GetType()->get_ID();
			}
			else if (auto const pCell = abstract_cast<CellClass*>(pTarget))
			{
				mapCoords = pCell->MapCoords;
				ID = "Cell";
			}

			const auto distance = (pCurrent->DistanceFrom(pTarget) / Unsorted::LeptonsPerCell);
			append("%s = %s [Distance: %d, Location: (%d, %d)]", pInfoName, ID, distance, mapCoords.X, mapCoords.Y);

			count = (count + 1) % 3;

			if (!count)
				append("\n");
		}
	};

	auto printFoots = [&append, &display, &count, &getTargetInfo](FootClass* pFoot)
	{
		// Basic Status
		append("[Phobos] Dump ObjectInfo runs.\n");
		auto pType = pFoot->GetTechnoType();
		append("ID = %s, ", pType->ID);
		append("Owner = %s (%s), ", pFoot->Owner->get_ID(), pFoot->Owner->PlainName);
		append("Location = (%d, %d), ", pFoot->GetMapCoords().X, pFoot->GetMapCoords().Y);
		append("Health = (%d/%d)", pFoot->Health, pType->Strength);

		auto pTechnoExt = TechnoExt::ExtMap.Find(pFoot);
		auto pShieldData = pTechnoExt->Shield.get();

		if (pTechnoExt->CurrentShieldType && pShieldData)
			append(", Shield = (%d/%d)", pShieldData->GetHP(), pTechnoExt->CurrentShieldType->Strength);

		if (pType->Ammo > 0)
			append(", Ammo = (%d/%d)", pFoot->Ammo, pType->Ammo);

		append("\n");

		// Mission Status
		append("CurrentMission = %d (%s), Status = %d, MissionStartFrame = %d, ", pFoot->CurrentMission, MissionControlClass::FindName(pFoot->CurrentMission), pFoot->MissionStatus, pFoot->CurrentMissionStartTime);
		append("SuspendedMission = %d (%s), ", pFoot->SuspendedMission, MissionControlClass::FindName(pFoot->SuspendedMission));
		append("QueuedMission = %d (%s), ", pFoot->QueuedMission, MissionControlClass::FindName(pFoot->QueuedMission));
		append("MegaMission = %d (%s)\n", pFoot->unknown_int_5C4, MissionControlClass::FindName(static_cast<Mission>(pFoot->unknown_int_5C4)));

		// Team Status
		if (pFoot->BelongsToATeam())
		{
			auto pTeam = pFoot->Team;
			auto pTeamType = pTeam->Type;
			bool found = false;

			for (int i = 0; i < AITriggerTypeClass::Array->Count && !found; i++)
			{
				auto pTriggerTeam1Type = AITriggerTypeClass::Array->GetItem(i)->Team1;
				auto pTriggerTeam2Type = AITriggerTypeClass::Array->GetItem(i)->Team2;

				if (pTeamType && ((pTriggerTeam1Type && pTriggerTeam1Type == pTeamType) || (pTriggerTeam2Type && pTriggerTeam2Type == pTeamType)))
				{
					found = true;
					auto pTriggerType = AITriggerTypeClass::Array->GetItem(i);
					append("Trigger ID = %s, weights [Current, Min, Max]: %f, %f, %f", pTriggerType->ID, pTriggerType->Weight_Current, pTriggerType->Weight_Minimum, pTriggerType->Weight_Maximum);
				}
			}
			display();

			append("Team ID = %s, Script ID = %s, Taskforce ID = %s", pTeam->Type->ID, pTeam->CurrentScript->Type->get_ID(), pTeam->Type->TaskForce->ID);
			display();

			if (pTeam->CurrentScript->CurrentMission >= 0)
				append("Current Script [Line = Action, Argument]: %d = %d,%d", pTeam->CurrentScript->CurrentMission, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Action, pTeam->CurrentScript->Type->ScriptActions[pTeam->CurrentScript->CurrentMission].Argument);
			else
				append("Current Script [Line = Action, Argument]: %d", pTeam->CurrentScript->CurrentMission);

			display();
		}

		// Passenger Status
		if (pFoot->Passengers.NumPassengers > 0)
		{
			FootClass* pCurrent = pFoot->Passengers.FirstPassenger;
			append("%d Passengers: %s", pFoot->Passengers.NumPassengers, pCurrent->GetTechnoType()->ID);

			for (pCurrent = abstract_cast<FootClass*>(pCurrent->NextObject); pCurrent; pCurrent = abstract_cast<FootClass*>(pCurrent->NextObject))
				append(", %s", pCurrent->GetTechnoType()->ID);

			append("\n");
		}

		// Related Status
		getTargetInfo(pFoot, pFoot->Target, "Target");
		getTargetInfo(pFoot, pFoot->LastTarget, "LastTarget");
		getTargetInfo(pFoot, pFoot->GetNthLink(), "NearestLink");
		getTargetInfo(pFoot, pFoot->ArchiveTarget, "ArchiveTarget");
		getTargetInfo(pFoot, pFoot->Transporter, "Transporter");
		getTargetInfo(pFoot, pFoot->Destination, "Destination");
		getTargetInfo(pFoot, pFoot->LastDestination, "LastDestination");
		getTargetInfo(pFoot, pFoot->unknown_500, "QueueUpToEnter");
		getTargetInfo(pFoot, pFoot->unknown_5A0, "Follow");
		getTargetInfo(pFoot, reinterpret_cast<AbstractClass*>(pFoot->unknown_5CC), "MegaTarget");
		getTargetInfo(pFoot, reinterpret_cast<AbstractClass*>(pFoot->unknown_5C8), "MegaDestination");
		getTargetInfo(pFoot, pFoot->ParasiteEatingMe, "Parasite");

		if (const auto pUnit = abstract_cast<UnitClass*>(pFoot))
			getTargetInfo(pUnit, pUnit->FollowerCar, "Follower");
		else if (const auto pAircraft = abstract_cast<AircraftClass*>(pFoot))
			getTargetInfo(pAircraft, pAircraft->DockNowHeadingTo, "Dock");

		if (count % 3)
			append("\n");

		// End
		display();
	};

	auto printBuilding = [&append, &display, &count, &getTargetInfo](BuildingClass* pBuilding)
	{
		// Basic Status
		append("[Phobos] Dump ObjectInfo runs.\n");
		auto pType = pBuilding->Type;
		append("ID = %s, ", pType->ID);
		append("Owner = %s (%s), ", pBuilding->Owner->get_ID(), pBuilding->Owner->PlainName);
		append("Location = (%d, %d), ", pBuilding->GetMapCoords().X, pBuilding->GetMapCoords().Y);
		append("Health = (%d/%d)", pBuilding->Health, pType->Strength);

		auto pTechnoExt = TechnoExt::ExtMap.Find(pBuilding);
		auto pShieldData = pTechnoExt->Shield.get();

		if (pTechnoExt->CurrentShieldType && pShieldData)
			append(", Shield = (%d/%d)", pShieldData->GetHP(), pTechnoExt->CurrentShieldType->Strength);

		if (pType->Ammo > 0)
			append(", Ammo = (%d/%d)\n", pBuilding->Ammo, pType->Ammo);

		append("\n");

		// Mission Status
		append("CurrentMission = %d (%s), Status = %d, MissionStartFrame = %d\n", pBuilding->CurrentMission, MissionControlClass::FindName(pBuilding->CurrentMission), pBuilding->MissionStatus, pBuilding->CurrentMissionStartTime);
		append("SuspendedMission = %d (%s), ", pBuilding->SuspendedMission, MissionControlClass::FindName(pBuilding->SuspendedMission));
		append("QueuedMission = %d (%s)\n", pBuilding->QueuedMission, MissionControlClass::FindName(pBuilding->QueuedMission));

		// Building Status
		bool nextLine = false;

		if (pBuilding->Factory && pBuilding->Factory->Object)
		{
			append("Production: %s (%d%%)", pBuilding->Factory->Object->GetTechnoType()->ID, (pBuilding->Factory->GetProgress() * 100 / 54));
			nextLine = true;
		}

		if (pType->Refinery || pType->ResourceGatherer)
		{
			if (nextLine)
				append(", ");

			append("Money: %d", pBuilding->Owner->Available_Money());
			nextLine = true;
		}

		if (nextLine)
			append("\n");

		// Occupant Status
		if (pBuilding->Occupants.Count > 0)
		{
			append("Occupants: %s", pBuilding->Occupants.GetItem(0)->Type->ID);
			for (int i = 1; i < pBuilding->Occupants.Count; i++)
			{
				append(", %s", pBuilding->Occupants.GetItem(i)->Type->ID);
			}
			append("\n");
		}

		// Passenger Status
		if (pBuilding->Passengers.NumPassengers > 0)
		{
			FootClass* pCurrent = pBuilding->Passengers.FirstPassenger;
			append("%d Passengers: %s", pBuilding->Passengers.NumPassengers, pCurrent->GetTechnoType()->ID);

			for (pCurrent = abstract_cast<FootClass*>(pCurrent->NextObject); pCurrent; pCurrent = abstract_cast<FootClass*>(pCurrent->NextObject))
				append(", %s", pCurrent->GetTechnoType()->ID);

			append("\n");
		}

		// Upgrade Status
		if (pType->Upgrades)
		{
			append("Upgrades (%d/%d): ", pBuilding->UpgradeLevel, pType->Upgrades);
			for (int i = 0; i < 3; i++)
			{
				if (i != 0)
					append(", ");

				append("Slot %d = %s", i + 1, pBuilding->Upgrades[i] ? pBuilding->Upgrades[i]->get_ID() : "<none>");
			}
			append("\n");
		}

		// Related Status
		getTargetInfo(pBuilding, pBuilding->Target, "Target");
		getTargetInfo(pBuilding, pBuilding->LastTarget, "LastTarget");
		getTargetInfo(pBuilding, pBuilding->GetNthLink(), "NearestLink");
		getTargetInfo(pBuilding, pBuilding->ArchiveTarget, "ArchiveTarget");
		getTargetInfo(pBuilding, pBuilding->Transporter, "Transporter");

		if (count % 3)
			append("\n");

		// End
		display();
	};

	bool dumped = false;
	auto dumpInfo = [&printFoots, &printBuilding, &append, &display, &dumped](ObjectClass* pObject)
	{
		switch (pObject->WhatAmI())
		{
		case AbstractType::Infantry:
		case AbstractType::Unit:
		case AbstractType::Aircraft:
			printFoots(static_cast<FootClass*>(pObject));
			break;
		case AbstractType::Building:
			printBuilding(static_cast<BuildingClass*>(pObject));
			break;
		default:
			append("INVALID ITEM!");
			display();
			break;
		}
		dumped = true;
	};

	for (auto pTechno : *TechnoClass::Array)
	{
		if (dumped) break;
		if (pTechno->IsMouseHovering)
			dumpInfo(pTechno);
	}

	if (!dumped)
	{
		if (ObjectClass::CurrentObjects->Count > 0)
		{
			if (ObjectClass::CurrentObjects->Count != 1)
				MessageListClass::Instance->PrintMessage(L"This command will only dump one of these selected object", 600, 5, true);

			dumpInfo(ObjectClass::CurrentObjects->GetItem(ObjectClass::CurrentObjects->Count - 1));
		}
	}
}
