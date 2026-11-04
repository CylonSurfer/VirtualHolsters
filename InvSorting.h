#pragma once
#include "f4se/GameRTTI.h"
#include "f4se\GameExtraData.h"
#include "f4se\GameFormComponents.h"
#include "f4se\GameForms.h"
#include "utils.h"

namespace Holsters {
	struct InventorySnapshot {
		bool isWeapEquipped = false;
		bool TaggedWeapFound = false;
		int TaggedWeapStackID = -1;
		int EquippedWeapStackID = -1;
		BGSInventoryItem TaggedWeaponInvItem{};
		BGSInventoryItem EquippedWeaponInvItem{};
		BGSInventoryItem HolsterInvItem{};
		TESForm* Taggedform = nullptr;
		TESForm* Equippedform = nullptr;
		TESObjectWEAP* Taggedweapon = nullptr;
		TESObjectWEAP* Equippedweapon = nullptr;
		ExtraTextDisplayData* TaggedweapETDD = nullptr;
		ExtraTextDisplayData* EquippedweapETDD = nullptr;
		TBO_InstanceData* TaggedweapTBOID = nullptr;
		TBO_InstanceData* EquippedweapTBOID = nullptr;
		UInt16 EquippedNameSize = 0;
		std::string EquippedFallbackName = "null";
		std::string TaggedFallbackName = "null";
	};
	void WeapCheck(int i);
	void WeapSort(int handle);
	void resetHolsterName(int handle);
	void addHolster(int handle, TESObjectREFR* object);
	InventorySnapshot GetINVSnapShot(int handle, BGSInventoryList* inventory, std::string holstername);
	ExtraTextDisplayData* FindHolster(BGSInventoryList* inventory);
	BGSInventoryItem::Stack* GetStackByStackID(BGSInventoryItem* item, UInt16 stackID);
	TBO_InstanceData* GetTBO_InstanceDataFromInventoryItem(BGSInventoryItem* item, UInt16 stackID);
	ExtraTextDisplayData* GetExtraTextDisplayData(BGSInventoryItem* item, UInt16 stackID);
	void registerWeaponInMain(TESForm* weapForm, TESObjectWEAP* weapon, TESObjectREFR* object, int handle);
	void GameLoadInvSort();
	void WaitforObject3D(TESForm* weapForm, TESObjectWEAP* weapon, TESObjectREFR* object, TESObjectREFR* container, int handle, int retryCount);
	TESObjectREFR* createObjectFromInvItem(TESObjectWEAP* weapon, BGSInventoryItem item, int stackID);
	extern bool settingUpHolsters;
	void RemoveThrowableHolster(int holster);
}