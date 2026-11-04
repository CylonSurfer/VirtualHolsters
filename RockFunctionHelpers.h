#pragma once

#include "Holsters.h"

namespace Holsters {

	extern enum class HeldWeapon {
		None,
		inLeft,
		inRight,
		inBoth
	};
	extern enum class EquippedWeapon {
		None,
		equippedLeft,
		equippedRight
	};
	bool InitializeRock();
	bool ShutdownRock();
	void RockGrabItem(bool isLeft, TESObjectREFR* obj);
	void RockForceDropItem(bool isLeft, TESObjectREFR* obj);
	TESObjectREFR* RockGetHeld(bool isLeft);
	EquippedWeapon GetEquippedWeaponHand();
	HeldWeapon GetHeldWeaponHand();
	bool isTwoHandingWeapon();
	bool isPrimaryHandDetached();
}
