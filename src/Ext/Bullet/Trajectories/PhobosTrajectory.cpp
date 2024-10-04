#include <Ext/BulletType/Body.h>
#include <Ext/Bullet/Body.h>
#include <Ext/WeaponType/Body.h>

#include <BulletClass.h>
#include <Helpers/Macro.h>

#include "StraightTrajectory.h"
#include "BombardTrajectory.h"
#include "DisperseTrajectory.h"
#include "EngraveTrajectory.h"
#include "ParabolaTrajectory.h"
#include "TracingTrajectory.h"

TrajectoryTypePointer::TrajectoryTypePointer(TrajectoryFlag flag)
{
	switch (flag)
	{
	case TrajectoryFlag::Straight:
		_ptr = std::make_unique<StraightTrajectoryType>();
		return;
	case TrajectoryFlag::Bombard:
		_ptr = std::make_unique<BombardTrajectoryType>();
		return;
	case TrajectoryFlag::Disperse:
		_ptr = std::make_unique<DisperseTrajectoryType>();
		return;
	case TrajectoryFlag::Engrave:
		_ptr = std::make_unique<EngraveTrajectoryType>();
		return;
	case TrajectoryFlag::Parabola:
		_ptr = std::make_unique<ParabolaTrajectoryType>();
		return;
	case TrajectoryFlag::Tracing:
		_ptr = std::make_unique<TracingTrajectoryType>();
		return;
	}
	_ptr.reset();
}

namespace detail
{
	template <>
	inline bool read<TrajectoryFlag>(TrajectoryFlag& value, INI_EX& parser, const char* pSection, const char* pKey)
	{
		if (parser.ReadString(pSection, pKey))
		{
			static std::pair<const char*, TrajectoryFlag> FlagNames[] =
			{
				{"Straight", TrajectoryFlag::Straight},
				{"Bombard" ,TrajectoryFlag::Bombard},
				{"Disperse", TrajectoryFlag::Disperse},
				{"Engrave" ,TrajectoryFlag::Engrave},
				{"Parabola", TrajectoryFlag::Parabola},
				{"Tracing" ,TrajectoryFlag::Tracing},
			};
			for (auto [name, flag] : FlagNames)
			{
				if (_strcmpi(parser.value(), name) == 0)
				{
					value = flag;
					return true;
				}
			}
			Debug::INIParseFailed(pSection, pKey, parser.value(), "Expected a new trajectory type");
		}

		return false;
	}
}

void TrajectoryTypePointer::LoadFromINI(CCINIClass* pINI, const char* pSection)
{
	INI_EX exINI(pINI);
	Nullable<TrajectoryFlag> flag;
	flag.Read(exINI, pSection, "Trajectory");// I assume this shit is parsed once and only once, so I keep the impl here
	if (flag.isset())
	{
		if (!_ptr || _ptr->Flag != flag.Get())
			std::construct_at(this, flag.Get());
	}
	if (_ptr)
	{
		_ptr->Trajectory_Speed.Read(exINI, pSection, "Trajectory.Speed");
		_ptr->Read(pINI, pSection);
	}
}

bool TrajectoryTypePointer::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	PhobosTrajectoryType* PTR = nullptr;
	if (!Stm.Load(PTR))
		return false;
	TrajectoryFlag flag;
	if (PTR && Stm.Load(flag))
	{
		std::construct_at(this, flag);
		PhobosSwizzle::RegisterChange(PTR, _ptr.get());
		return _ptr->Load(Stm, RegisterForChange);
	}
	return true;
}

bool TrajectoryTypePointer::Save(PhobosStreamWriter& Stm) const
{
	auto* raw = get();
	Stm.Process(raw);
	if (raw)
	{
		Stm.Process(raw->Flag);
		return raw->Save(Stm);
	}
	return true;
}

bool PhobosTrajectoryType::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	Stm
		.Process(this->Flag)
		.Process(this->Trajectory_Speed);
	return true;
}

bool PhobosTrajectoryType::Save(PhobosStreamWriter& Stm) const
{
	Stm
		.Process(this->Flag)
		.Process(this->Trajectory_Speed);
	return true;
}

void PhobosTrajectoryType::CreateType(PhobosTrajectoryType*& pType, CCINIClass* const pINI, const char* pSection, const char* pKey)
{
	PhobosTrajectoryType* pNewType = nullptr;
	bool bUpdateType = true;

	pINI->ReadString(pSection, pKey, "", Phobos::readBuffer);
	if (INIClass::IsBlank(Phobos::readBuffer))
		pNewType = nullptr;
	else if (_stricmp(Phobos::readBuffer, "Straight") == 0)
		pNewType = DLLCreate<StraightTrajectoryType>();
	else if (_stricmp(Phobos::readBuffer, "Bombard") == 0)
		pNewType = DLLCreate<BombardTrajectoryType>();
	else if (_stricmp(Phobos::readBuffer, "Disperse") == 0)
		pNewType = DLLCreate<DisperseTrajectoryType>();
	else if (_stricmp(Phobos::readBuffer, "Engrave") == 0)
		pNewType = DLLCreate<EngraveTrajectoryType>();
	else if (_stricmp(Phobos::readBuffer, "Parabola") == 0)
		pNewType = DLLCreate<ParabolaTrajectoryType>();
	else if (_stricmp(Phobos::readBuffer, "Tracing") == 0)
		pNewType = DLLCreate<TracingTrajectoryType>();
	else
		bUpdateType = false;

	if (pNewType)
		pNewType->Read(pINI, pSection);

	if (bUpdateType)
	{
		DLLDelete(pType); // GameDelete already has if(pType) check here.
		pType = pNewType;
	}
}

PhobosTrajectoryType* PhobosTrajectoryType::LoadFromStream(PhobosStreamReader& Stm)
{
	PhobosTrajectoryType* pType = nullptr;
	TrajectoryFlag flag = TrajectoryFlag::Invalid;
	Stm.Process(pType, false);

	if (pType)
	{
		Stm.Process(flag, false);

		switch (flag)
		{
		case TrajectoryFlag::Straight:
			pType = DLLCreate<StraightTrajectoryType>();
			break;
		case TrajectoryFlag::Bombard:
			pType = DLLCreate<BombardTrajectoryType>();
			break;
		case TrajectoryFlag::Disperse:
			pType = DLLCreate<DisperseTrajectoryType>();
			break;
		case TrajectoryFlag::Engrave:
			pType = DLLCreate<EngraveTrajectoryType>();
			break;
		case TrajectoryFlag::Parabola:
			pType = DLLCreate<ParabolaTrajectoryType>();
			break;
		case TrajectoryFlag::Tracing:
			pType = DLLCreate<TracingTrajectoryType>();
			break;
		default:
			return nullptr;
		}

		pType->Flag = flag;
		pType->Load(Stm, false);
	}

	return pType;
}

void PhobosTrajectoryType::WriteToStream(PhobosStreamWriter& Stm, PhobosTrajectoryType* pType)
{
	Stm.Process(pType);
	if (pType)
	{
		Stm.Process(pType->Flag);
		pType->Save(Stm);
	}
}

PhobosTrajectoryType* PhobosTrajectoryType::ProcessFromStream(PhobosStreamReader& Stm, PhobosTrajectoryType* pType)
{
	UNREFERENCED_PARAMETER(pType);
	return LoadFromStream(Stm);
}

PhobosTrajectoryType* PhobosTrajectoryType::ProcessFromStream(PhobosStreamWriter& Stm, PhobosTrajectoryType* pType)
{
	WriteToStream(Stm, pType);
	return pType;
}

bool PhobosTrajectory::Load(PhobosStreamReader& Stm, bool RegisterForChange)
{
	Stm.Process(this->Flag, false);

	return true;
}

bool PhobosTrajectory::Save(PhobosStreamWriter& Stm) const
{
	Stm.Process(this->Flag);
	return true;
}

#pragma region RedoAllThesePleaseItsInFactVerySimpleJustFollowTemplateDef_h
PhobosTrajectory* PhobosTrajectory::LoadFromStream(PhobosStreamReader& Stm)
{
	PhobosTrajectory* pTraj = nullptr;
	TrajectoryFlag flag = TrajectoryFlag::Invalid;
	Stm.Process(pTraj, false);

	if (pTraj)
	{
		Stm.Process(flag, false);
		auto old = pTraj;
		switch (flag)
		{
		case TrajectoryFlag::Straight:
			pTraj = new StraightTrajectory(noinit_t {});
			break;
		case TrajectoryFlag::Bombard:
			pTraj = new BombardTrajectory(noinit_t {});
			break;
		case TrajectoryFlag::Disperse:
			pTraj = new DisperseTrajectory(noinit_t {});
			break;
		case TrajectoryFlag::Engrave:
			pTraj = new EngraveTrajectory(noinit_t {});
			break;
		case TrajectoryFlag::Parabola:
			pTraj = new ParabolaTrajectory(noinit_t {});
			break;
		case TrajectoryFlag::Tracing:
			pTraj = new TracingTrajectory(noinit_t {});
			break;
		default:
			return nullptr;
		}

		pTraj->Flag = flag;
		pTraj->Load(Stm, false);
		PhobosSwizzle::RegisterChange(old, pTraj);
	}

	return pTraj;
}

void PhobosTrajectory::WriteToStream(PhobosStreamWriter& Stm, PhobosTrajectory* pTraj)
{
	Stm.Process(pTraj);
	if (pTraj)
	{
		Stm.Process(pTraj->Flag);
		pTraj->Save(Stm);
	}
}

PhobosTrajectory* PhobosTrajectory::ProcessFromStream(PhobosStreamReader& Stm, PhobosTrajectory* pTraj)
{
	UNREFERENCED_PARAMETER(pTraj);
	return LoadFromStream(Stm);
}

PhobosTrajectory* PhobosTrajectory::ProcessFromStream(PhobosStreamWriter& Stm, PhobosTrajectory* pTraj)
{
	WriteToStream(Stm, pTraj);
	return pTraj;
}
#pragma endregion

DEFINE_HOOK(0x4666F7, BulletClass_AI_Trajectories, 0x6)
{
	enum { Detonate = 0x467E53 };

	GET(BulletClass*, pThis, EBP);

	auto const pExt = BulletExt::ExtMap.Find(pThis);
	bool detonate = false;

	if (auto pTraj = pExt->Trajectory)
		detonate = pTraj->OnAI(pThis);

	if (detonate && !pThis->SpawnNextAnim)
		return Detonate;

	if (pExt->Trajectory && pExt->LaserTrails.size())
	{
		CoordStruct futureCoords
		{
			pThis->Location.X + static_cast<int>(pThis->Velocity.X),
			pThis->Location.Y + static_cast<int>(pThis->Velocity.Y),
			pThis->Location.Z + static_cast<int>(pThis->Velocity.Z)
		};

		for (auto& trail : pExt->LaserTrails)
		{
			if (!trail.LastLocation.isset())
				trail.LastLocation = pThis->Location;

			trail.Update(futureCoords);
		}
	}

	return 0;
}

DEFINE_HOOK(0x467E53, BulletClass_AI_PreDetonation_Trajectories, 0x6)
{
	GET(BulletClass*, pThis, EBP);

	auto const pExt = BulletExt::ExtMap.Find(pThis);

	if (auto pTraj = pExt->Trajectory)
		pTraj->OnAIPreDetonate(pThis);

	return 0;
}

DEFINE_HOOK(0x46745C, BulletClass_AI_Position_Trajectories, 0x7)
{
	GET(BulletClass*, pThis, EBP);
	LEA_STACK(BulletVelocity*, pSpeed, STACK_OFFSET(0x1AC, -0x11C));
	LEA_STACK(BulletVelocity*, pPosition, STACK_OFFSET(0x1AC, -0x144));

	auto const pExt = BulletExt::ExtMap.Find(pThis);

	if (auto pTraj = pExt->Trajectory)
		pTraj->OnAIVelocity(pThis, pSpeed, pPosition);

	return 0;
}

DEFINE_HOOK(0x4677D3, BulletClass_AI_TargetCoordCheck_Trajectories, 0x5)
{
	enum { SkipCheck = 0x4678F8, ContinueAfterCheck = 0x467879, Detonate = 0x467E53 };

	GET(BulletClass*, pThis, EBP);

	auto const pExt = BulletExt::ExtMap.Find(pThis);

	if (auto pTraj = pExt->Trajectory)
	{
		switch (pTraj->OnAITargetCoordCheck(pThis))
		{
		case TrajectoryCheckReturnType::SkipGameCheck:
			return SkipCheck;
			break;
		case TrajectoryCheckReturnType::SatisfyGameCheck:
			return ContinueAfterCheck;
			break;
		case TrajectoryCheckReturnType::Detonate:
			return Detonate;
			break;
		default:
			break;
		}
	}

	return 0;
}

DEFINE_HOOK(0x467927, BulletClass_AI_TechnoCheck_Trajectories, 0x5)
{
	enum { SkipCheck = 0x467A26, ContinueAfterCheck = 0x467514 };

	GET(BulletClass*, pThis, EBP);
	GET(TechnoClass*, pTechno, ESI);

	auto const pExt = BulletExt::ExtMap.Find(pThis);

	if (auto pTraj = pExt->Trajectory)
	{
		switch (pTraj->OnAITechnoCheck(pThis, pTechno))
		{
		case TrajectoryCheckReturnType::SkipGameCheck:
			return SkipCheck;
			break;
		case TrajectoryCheckReturnType::SatisfyGameCheck:
			return ContinueAfterCheck;
			break;
		default:
			break;
		}
	}

	return 0;
}

DEFINE_HOOK(0x468B72, BulletClass_Unlimbo_Trajectories, 0x5)
{
	GET(BulletClass*, pThis, EBX);
	GET_STACK(CoordStruct*, pCoord, STACK_OFFSET(0x54, 0x4));
	GET_STACK(BulletVelocity*, pVelocity, STACK_OFFSET(0x54, 0x8));

	auto const pExt = BulletExt::ExtMap.Find(pThis);
	auto const pTypeExt = pExt->TypeExtData;

	if (pTypeExt && pTypeExt->TrajectoryType)
	{
		pExt->Trajectory = pTypeExt->TrajectoryType->CreateInstance();
		pExt->Trajectory->OnUnlimbo(pThis, pCoord, pVelocity);
	}

	return 0;
}
