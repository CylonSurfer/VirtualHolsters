#pragma once
#include "f4se/GameRTTI.h"
#include "f4se\GameExtraData.h"
#include "f4se\GameFormComponents.h"
#include "f4se\GameForms.h"
#include "utils.h"

namespace Holsters {
	struct InventorySnapshot;
	void WeapCheck(int i);
	void WeapSort(int handle);
	void resetHolsterName(int handle);
	InventorySnapshot GetINVSnapShot(int handle, BGSInventoryList* inventory, std::string holstername);
	ExtraTextDisplayData* FindHolster(BGSInventoryList* inventory);
	BGSInventoryItem::Stack* GetStackByStackID(BGSInventoryItem* item, UInt16 stackID);
	TBO_InstanceData* GetTBO_InstanceDataFromInventoryItem(BGSInventoryItem* item, UInt16 stackID);
	ExtraTextDisplayData* GetExtraTextDisplayData(BGSInventoryItem* item, UInt16 stackID);
	void registerWeaponInMain(TESForm* weapForm, TESObjectWEAP* weapon, TESObjectREFR* object, int handle);
	void GameLoadInvSort();
	void WaitforObject3D(TESForm* weapForm, TESObjectWEAP* weapon, TESObjectREFR* object, TESObjectREFR* container, int handle);
	TESObjectREFR* createObjectFromInvItem(TESObjectWEAP* weapon, BGSInventoryItem item, int stackID);
}
