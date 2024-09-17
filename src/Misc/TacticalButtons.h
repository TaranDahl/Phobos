#include <Utilities/Macro.h>
#include <Ext/SWType/Body.h>
#include <ControlClass.h>

// Test function for extra pressable buttons above tactical map

// Note:
// If some of the functions are intended for some multiplayer gamemodes,
// I think they should be triggered through the EventClass. But in fact,
// there are too few existing events can be used that I don't know what
// universal functionality can be achieved. - CrimRecya

// It would be great if it could be modularized
// TODO TacticalButtonClass::TacticalButtonGroup::TacticalButton

class TacticalButtonClass
{
public:
	static TacticalButtonClass Instance;

private:
	static int CheckMouseOverButtons(const Point2D* pMousePosition);
	static bool CheckMouseOverBackground(const Point2D* pMousePosition);

public:
	inline bool MouseOverButtons();
	inline bool MouseOverTactical();

	int GetButtonIndex();
	void SetMouseButtonIndex(const Point2D* pMousePosition);
	void PressDesignatedButton(int triggerIndex);

	// Button index 1-9 : Super weapons buttons
	static void DrawButtonForSW();
	static void RecheckButtonForSW();
	static bool InsertButtonForSW(int& superIndex);
	static bool MoveButtonForSW(SuperWeaponTypeClass* pDataType, SuperWeaponTypeClass* pAddType, SWTypeExt::ExtData* pAddTypeExt, unsigned int ownerBits);
	static void TriggerButtonForSW(int buttonIndex);

	struct DummySelectClass
	{
		char _[0x2C] {};
		StripClass *LinkTo { nullptr };
		int unknown_int_30 { 0 };
		bool MouseEntered { false };
		int SWIndex { -1 }; // New
	};

public:
	bool PressedInButtonsLayer { false }; // Check press

	// Button index 1-9 : Super weapons buttons
	bool DummyAction { false };

private:
	int ButtonIndex { -1 }; // -1 -> above no buttons, 0 -> above buttons background, POSITIVE -> above button who have this index
	Point2D LastPosition { Point2D::Empty }; // Check moving

	// Button index 1-9 : Super weapons buttons
	std::vector<int> SWButtonData;
	SuperClass* RecordSuper = nullptr; // Cannot be used, only for comparison purposes
};
