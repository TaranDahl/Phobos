#include "Body.h"

#include <EventClass.h>
#include <HouseClass.h>
#include <SuperClass.h>

#include <Ext/Sidebar/SWSidebar/SWSidebarClass.h>

std::unique_ptr<SidebarExt::ExtData> SidebarExt::Data = nullptr;

SHPStruct* SidebarExt::TabProducingProgress[4];

void SidebarExt::Allocate(SidebarClass* pThis)
{
	Data = std::make_unique<SidebarExt::ExtData>(pThis);
}

void SidebarExt::Remove(SidebarClass* pThis)
{
	Data = nullptr;
}

bool __stdcall SidebarExt::AresTabCameo_RemoveCameo(BuildType* pItem)
{
	const auto pTechnoType = TechnoTypeClass::GetByTypeAndIndex(pItem->ItemType, pItem->ItemIndex);
	const auto pCurrent = HouseClass::CurrentPlayer();

	if (pTechnoType)
	{
		const auto pFactory = pTechnoType->FindFactory(true, false, false, pCurrent);

		if (pFactory && pFactory->Owner->CanBuild(pTechnoType, false, true) != CanBuildResult::Unbuildable)
			return false;
	}
	else
	{
		const auto& supers = pCurrent->Supers;

		if (supers.ValidIndex(pItem->ItemIndex) && supers[pItem->ItemIndex]->IsPresent && !SWSidebarClass::Instance.AddButton(pItem->ItemIndex))
			return false;
	}

	if (pItem->CurrentFactory)
	{
		EventClass event = EventClass(pCurrent->ArrayIndex, EventType::Abandon, static_cast<int>(pItem->ItemType), pItem->ItemIndex, pTechnoType && pTechnoType->Naval);
		EventClass::AddEvent(event);
	}

	if (pItem->ItemType == AbstractType::BuildingType || pItem->ItemType == AbstractType::Building)
	{
		DisplayClass::Instance->CurrentBuilding = nullptr;
		DisplayClass::Instance->CurrentBuildingType = nullptr;
		DisplayClass::Instance->CurrentBuildingOwnerArrayIndex = -1;
		DisplayClass::Instance->SetActiveFoundation(nullptr);
	}

	if (pTechnoType)
	{
		const auto absType = pTechnoType->WhatAmI();
		const auto buildCat = absType == AbstractType::BuildingType ? static_cast<BuildingTypeClass*>(pTechnoType)->BuildCat : BuildCat::DontCare;
		// EVERYONE likes hardcoded BuildCat::DontCare, so why is this function still designed like this? why pass this parameter???

		if (pCurrent->GetPrimaryFactory(absType, pTechnoType->Naval, buildCat))
		{
			EventClass event = EventClass(pCurrent->ArrayIndex, EventType::AbandonAll, static_cast<int>(pItem->ItemType), pItem->ItemIndex, pTechnoType->Naval);
			EventClass::AddEvent(event);
		}
	}

	return true;
}

// =============================
// load / save

template <typename T>
void SidebarExt::ExtData::Serialize(T& Stm)
{
	Stm
		.Process(this->SWSidebar_Enable)
		.Process(this->SWSidebar_Indices)
		;
}

void SidebarExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<SidebarClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void SidebarExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<SidebarClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}


// =============================
// container hooks

DEFINE_HOOK(0x6A4F0B, SidebarClass_CTOR, 0x5)
{
	GET(SidebarClass*, pItem, EAX);

	SidebarExt::Allocate(pItem);

	return 0;
}

DEFINE_HOOK(0x6AC82F, SidebarClass_DTOR, 0x5)
{
	GET(SidebarClass*, pItem, EBX);

	SidebarExt::Remove(pItem);
	return 0;
}

IStream* SidebarExt::g_pStm = nullptr;

DEFINE_HOOK_AGAIN(0x6AC5D0, SidebarClass_SaveLoad_Prefix, 0x5)
DEFINE_HOOK(0x6AC5E0, SidebarClass_SaveLoad_Prefix, 0x5)
{
	GET_STACK(IStream*, pStm, 0x4);

	SidebarExt::g_pStm = pStm;

	return 0;
}

DEFINE_HOOK(0x6AC5DA, SidebarClass_Load_Suffix, 0x6)
{
	auto buffer = SidebarExt::Global();

	PhobosByteStream Stm(0);
	if (Stm.ReadBlockFromStream(SidebarExt::g_pStm))
	{
		PhobosStreamReader Reader(Stm);

		if (Reader.Expect(SidebarExt::Canary) && Reader.RegisterChange(buffer))
			buffer->LoadFromStream(Reader);
	}

	return 0;
}

DEFINE_HOOK(0x6AC5EA, SidebarClass_Save_Suffix, 0x6)
{
	auto buffer = SidebarExt::Global();
	PhobosByteStream saver(sizeof(*buffer));
	PhobosStreamWriter writer(saver);

	writer.Expect(SidebarExt::Canary);
	writer.RegisterChange(buffer);

	buffer->SaveToStream(writer);
	saver.WriteBlockToStream(SidebarExt::g_pStm);

	return 0;
}
