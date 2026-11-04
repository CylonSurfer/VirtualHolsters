#include "InvSorting.h"
#include "RockFunctionHelpers.h"


namespace Holsters {
	RelocAddr<std::uint64_t*> g_ActorEquipManager(0x5a38bf8);
	RelocAddr<_ActorEquipItem> ActorEquipItem(0x140CD10);

	void WeapCheck(int i) {
		std::string holsterName;
		if (i < 1 || i > 7 || !HolsterContainers[i]) return;
		BGSInventoryList* holsterINV = HolsterContainers[i]->inventoryList;
		BGSInventoryList* PlayerINV = (*g_player)->inventoryList;
		ExtraTextDisplayData* Currentholster = nullptr;
		if (holsterINV) {
			//holsterINV->inventoryLock.LockForReadAndWrite();
			ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
			if (CurrentETDD) {
				holsterName = CurrentETDD->name;
				Currentholster = CurrentETDD;
			}
			//holsterINV->inventoryLock.Unlock();
		}
		if (PlayerINV && holsterINV) {
			//PlayerINV->inventoryLock.LockForRead();
			InventorySnapshot INVSnapShot = GetINVSnapShot(i, PlayerINV, holsterName);
			if (holsterName != "Empty") {
				if (INVSnapShot.Taggedform == nullptr) {    //unable to find holstered weapon in players inventory - destroy holster
					nDestoryWeapon(i, true, false);
					if (holsterINV) {
						//holsterINV->inventoryLock.LockForReadAndWrite();
						ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
						if (CurrentETDD) {
							CurrentETDD->name = "Empty";
							holsteredWeapNames[i] = "Empty";
							CurrentETDD->nameLength = 5;
							CurrentETDD->type = -2;
						}
						//holsterINV->inventoryLock.Unlock();
					}
				}
				else if ((*g_player)->actorState.IsWeaponDrawn() && INVSnapShot.isWeapEquipped) {
					std::string drawnname = "null";
					if (INVSnapShot.EquippedweapETDD) {
						drawnname = INVSnapShot.EquippedweapETDD->name;
					}
					else {
						drawnname = INVSnapShot.EquippedFallbackName;
					}
					if (drawnname == holsteredWeapNames[i]) {   // drawn weapon matches a holstered weapon - destroy holster
						nDestoryWeapon(i, true, false);
						if (holsterINV) {
							//holsterINV->inventoryLock.LockForReadAndWrite();
							ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
							if (CurrentETDD) {
								CurrentETDD->name = "Empty";
								holsteredWeapNames[i] = "Empty";
								CurrentETDD->nameLength = 5;
								CurrentETDD->type = -2;
							}
							//holsterINV->inventoryLock.Unlock();
						}
					}
				}
			}
			//PlayerINV->inventoryLock.Unlock();
			return;
		}
		return;
	}

	void WeapSort(int handle) {
		ExtraTextDisplayData* swapETDD = nullptr;
		ExtraTextDisplayData* Currentholster = nullptr;
		std::string holsterName;
		if (handle < 1 || handle > 7 || !HolsterContainers[handle]) return;
		BGSInventoryList* holsterINV = HolsterContainers[handle]->inventoryList;
		BGSInventoryList* playerINV = (*g_player)->inventoryList;
		if (holsterINV) {
			//holsterINV->inventoryLock.LockForReadAndWrite();
			ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
			if (CurrentETDD) {
				holsterName = CurrentETDD->name;
			}
			//holsterINV->inventoryLock.Unlock();
		}
		else {
			return; // No point in continuing - no holster inventory found.
		}
		if (playerINV && holsterINV) {
			//playerINV->inventoryLock.LockForRead();
			InventorySnapshot INVSnapShot = GetINVSnapShot(handle, playerINV, holsterName);
			bool isWeapEquipped = false;
			if ((*g_player)->actorState.IsWeaponDrawn()) {
				TESObjectWEAP* currentWeap = Offsets::GetEquippedWeapon((*g_gameVM)->m_virtualMachine, 0, *g_player, 0);
				if (currentWeap) {
					isWeapEquipped = true;
				}
			}
			bool taggedWeapFound = INVSnapShot.TaggedWeapFound;
			//-----------------------------------------------
			// determine what holster event needs to happen.
			//-----------------------------------------------
			if (!isWeapEquipped && !taggedWeapFound) {  //No Weapon Equipped & No Tagged Weapon Found - If Holster Name wasnt empty remove the holster and fix its name.
				nDestoryWeapon(handle, true, false);
				if (holsterINV) {
					//holsterINV->inventoryLock.LockForReadAndWrite();
					ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
					if (CurrentETDD) {
						CurrentETDD->name = "Empty";
						holsteredWeapNames[handle] = "Empty";
						CurrentETDD->nameLength = 5;
						CurrentETDD->type = -2;
					}
					//holsterINV->inventoryLock.Unlock();
				}
				//playerINV->inventoryLock.Unlock();
				return;
			}
			else if (isWeapEquipped && !taggedWeapFound) { // Weapon Equipped & No Tagged Weapon Found for Holster - Weapon needs to be holstered.
				if (INVSnapShot.Equippedform && INVSnapShot.Equippedweapon) {
					std::string weapName = "null";
					if (INVSnapShot.EquippedweapETDD) {
						weapName = INVSnapShot.EquippedweapETDD->name;
					}
					else {
						weapName = INVSnapShot.EquippedFallbackName;
					}
					for (int i = 1; i < 8; i++) {     //check to see if this weapon is already holstered in another slot.
						if (holsteredWeapNames[i] == weapName) {
							//playerINV->inventoryLock.Unlock();
							return;
						}
					}
					BGSObjectInstance* instance = new BGSObjectInstance(nullptr, nullptr);
					if (INVSnapShot.EquippedweapTBOID) {
						instance->instanceData = INVSnapShot.EquippedweapTBOID;
					}
					instance->object = INVSnapShot.Equippedform;
					BGSEquipSlot* equipSlot = INVSnapShot.Equippedweapon->equipType.GetEquipSlot();
					TESObjectREFR* currentRefr = createObjectFromInvItem(INVSnapShot.Equippedweapon, INVSnapShot.EquippedWeaponInvItem, INVSnapShot.EquippedWeapStackID);
					BGSObjectInstance* instance2 = new BGSObjectInstance(nullptr, nullptr);
					if (INVSnapShot.EquippedweapTBOID) {
						instance2->instanceData = INVSnapShot.EquippedweapTBOID;
					}
					instance2->object = INVSnapShot.Equippedform;
					BGSEquipSlot* equipSlot2 = INVSnapShot.Equippedweapon->equipType.GetEquipSlot();
					Offsets::UnEquipObject(*g_ActorEquipManager, *g_player, instance2, 1, nullptr, -1, false, true, true, false, 0);  //<----- works now
					if (currentRefr) {
						if (holsterINV) {
							//holsterINV->inventoryLock.LockForReadAndWrite();
							ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
							if (CurrentETDD) {
								CurrentETDD->nameLength = INVSnapShot.EquippedNameSize;
								CurrentETDD->name = weapName.c_str();
								CurrentETDD->type = -2;
								holsteredWeapNames[handle] = weapName;
							}
							//holsterINV->inventoryLock.Unlock();
						}
						uintptr_t moduleBase = (uintptr_t)GetModuleHandleA("tactsuit_vr_plugin.dll");
						if (moduleBase) {
							std::string keys[8] = { "Holster01", "Holster01", "Holster02", "Holster03", "Holster04", "Holster05", "Holster06", "Holster07" };
							//std::thread haptics(playbHapticsFile, keys[handle]);
							//haptics.detach();
							//playbHapticsFile(keys[handle]);
							if (g_tactsuitInterface) {
								//_MESSAGE(keys[handle].c_str());
								g_tactsuitInterface->PlayHapticEffect(keys[handle], 0.0, 0.0, bHapticsHolsterStrength, false);
							}

							//gIsRightHanded ? vrHook->StartHaptics(2, 0.15, 1.0) : vrHook->StartHaptics(1, 0.15, 1.0);
							std::thread haptics2(matchHapticsOnController);
							haptics2.detach();
						}
						WaitforObject3D(INVSnapShot.Equippedform, INVSnapShot.Equippedweapon, currentRefr, HolsterContainers[handle], handle, 0);
						//playerINV->inventoryLock.LockForReadAndWrite();
						//INVSnapShot.TaggedWeaponInvItem.stack->count = INVSnapShot.TaggedWeaponInvItem.stack->count - 1;
						//playerINV->inventoryLock.Unlock();
						//playerINV->inventoryLock.Unlock();
						return;
					}
					else {
						//playerINV->inventoryLock.Unlock();
						return;
					}
				}
				else {
					//playerINV->inventoryLock.Unlock();
					return;
				}
				//playerINV->inventoryLock.Unlock();
				return;
			}
			else if (!isWeapEquipped && taggedWeapFound) { // No Weapon Equipped & Tagged Weapon Found for Holster - Weapon needs to be drawn.
				if (INVSnapShot.Taggedform && INVSnapShot.Taggedweapon) {
					BGSObjectInstance* instance = new BGSObjectInstance(nullptr, nullptr);
					if (INVSnapShot.TaggedweapTBOID) {
						instance->instanceData = INVSnapShot.TaggedweapTBOID;
					}
					instance->object = INVSnapShot.Taggedform;
					BGSEquipSlot* equipSlot = INVSnapShot.Taggedweapon->equipType.GetEquipSlot();
					std::string n = INVSnapShot.Taggedweapon->GetFullName();
					//if (n == "BALISONG") {
						//EquipItem((*g_player), INVSnapShot.Taggedform, false, true);
					//}
					//else {
					if (Kabuto) {
						if (Kabuto->IsHandHelmetZone()) {
							return;
						}
						if (Kabuto->IsHoldingHelmetinPrimary()) {
							return;
						}
						if (Kabuto->IsEquippingHelmet()) {
							return;
						}
					}
					UInt32 equipSlotID = equipSlot->formID;
					if (g_rockRegistered && equipSlotID == 289452) {
						BSFixedString offHandName = gIsRightHanded ? "RArm_Hand" : "LArm_Hand";
						NiNode* HandNode = getChildNode(offHandName, (*g_player)->unkF0->rootNode);
						if (HandNode) {
							BSFlattenedBoneTree* rt = (BSFlattenedBoneTree*)(*g_player)->unkF0->rootNode->m_children.m_data[0]->GetAsNiNode();
							NiPoint3 offset(5.0f, -2.0f, 0.0f);
							offset = gIsRightHanded ? rt->transforms[boneTreeMap["RArm_Hand"]].world.rot * offset : rt->transforms[boneTreeMap["LArm_Hand"]].world.rot * offset;
							offset = gIsRightHanded ? rt->transforms[boneTreeMap["RArm_Hand"]].world.pos + offset: rt->transforms[boneTreeMap["LArm_Hand"]].world.pos + offset;
							
							NiPoint3 spawnPos = offset;
							NiPoint3 spawnRot = { 0.0f, 0.0f, 0.0f };
							if (INVSnapShot.Taggedform) {
								uint32_t outHandle = DropItemFromActor(*g_player, INVSnapShot.Taggedform, nullptr , 1, &spawnPos, nullptr);
								UInt32 out = outHandle;
								TESObjectREFR* droppedWorldRef = nullptr;
								LookupREFRByHandle(&out, &droppedWorldRef);
								RockGrabItem(gIsRightHanded ? false : true, droppedWorldRef);

							}
							//isWaitingForDrop3D = true;
						}
					}
					else {
						Offsets::EquipObject(*g_ActorEquipManager, *g_player, instance, INVSnapShot.TaggedWeapStackID, 1, equipSlot, true, false, true, true, false);
					}
					//}
					uintptr_t moduleBase = (uintptr_t)GetModuleHandleA("tactsuit_vr_plugin.dll");
					if (moduleBase) {
						std::string keys[8] = { "Holster01", "UnHolster01", "UnHolster02", "UnHolster03", "UnHolster04", "UnHolster05", "UnHolster06", "UnHolster07" };
						//std::thread haptics(playbHapticsFile, keys[handle]);
						//haptics.detach();
						//playbHapticsFile(keys[handle]);
						if (g_tactsuitInterface) {
							//_MESSAGE(keys[handle].c_str());
							g_tactsuitInterface->PlayHapticEffect(keys[handle], 0.0, 0.0, bHapticsHolsterStrength, false);
						}
						//gIsRightHanded ? vrHook->StartHaptics(2, 0.15, 1.0) : vrHook->StartHaptics(1, 0.15, 1.0);
						std::thread haptics2(matchHapticsOnController2);
						haptics2.detach();
					}
					if (g_rockRegistered && equipSlotID == 289452) {
						WeapCheck(gCurHolster);
						return;
					}
					else {
						nDestoryWeapon(handle, false, true);
						if (holsterINV) {
							//holsterINV->inventoryLock.LockForReadAndWrite();
							ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
							if (CurrentETDD) {
								CurrentETDD->name = "Empty";
								holsteredWeapNames[handle] = "Empty";
								CurrentETDD->nameLength = 5;
								CurrentETDD->type = -2;
							}
							//holsterINV->inventoryLock.Unlock();
						}
						//playerINV->inventoryLock.Unlock();
						return;
					}
				}
				else {
					//playerINV->inventoryLock.Unlock();
					return;
				}
				//playerINV->inventoryLock.Unlock();
				return;
			}
			else { //Weapon Equipped & Tagged Weapon Found for Holster - Nothing to do, Holster is Full.  
				//playerINV->inventoryLock.Unlock();
				return;
			}
			//playerINV->inventoryLock.Unlock();
			return;
		}
		else {
			return;
		}
	}

	void resetHolsterName(int handle) {
		std::string holsterName;
		if (handle < 1 || handle > 7 || !HolsterContainers[handle]) return;
		BGSInventoryList* holsterINV = HolsterContainers[handle]->inventoryList;
		if (holsterINV) {
			//holsterINV->inventoryLock.LockForReadAndWrite();
			ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
			if (CurrentETDD) {
				CurrentETDD->name = "Empty";
				holsteredWeapNames[handle] = "Empty";
				CurrentETDD->nameLength = 5;
				CurrentETDD->type = -2;
			}
			//holsterINV->inventoryLock.Unlock();
		}
		return;
	}

	void addHolster(int handle, TESObjectREFR* object) {
		std::string ETTDName;
		if (!object) {
			_MESSAGE("API WARNING: NO OBJECT REF PASSED TO ADDHOLSTER FUNCTION - ABORT");
			return;
		}

		else {
			//_MESSAGE("ADDING WEAPON TO HOLSTER");
			auto EDL = object->extraDataList;
			if (EDL) {
				ExtraTextDisplayData* currentETDD = DYNAMIC_CAST(EDL->GetByType(kExtraData_TextDisplayData), BSExtraData, ExtraTextDisplayData);
				if (currentETDD) {
					ETTDName = currentETDD->name;
				}
				else {
					ETTDName = object->baseForm->GetFullName();
				}
			}
			for (int i = 1; i < 8; i++) {     //check to see if this weapon is already holstered in another slot.
				if (holsteredWeapNames[i] == ETTDName) {
					_MESSAGE("API WARNING: THIS WEAPON IS ALREADY HOLSTERED - ABORT");
					return;
				}
			}
			TESObjectWEAP* weapon = static_cast<TESObjectWEAP*>(object->baseForm);
			UInt32 equipSlot = weapon->weapData.equipSlot->formID;
			if (!g_rockRegistered) {
				if (equipSlot == 289452) return;
			}
			UInt32 meleeKWFormId[2] = { 0x4A0A4, 0x4A0A45 };
			bool isMelee = false;
			for (int index = 0; index < 2; index++) {
				for (UInt32 i = 0; i < weapon->keyword.numKeywords; i++)
				{
					if (weapon->keyword.keywords[i])
					{
						if (weapon->keyword.keywords[i]->formID == meleeKWFormId[index])
						{
							isMelee = true;
						}
					}
				}
			}
			//std::string mes = "handle " + std::to_string(handle) + " ETTD: " + ETTDName;
			//_MESSAGE(mes.c_str());
			AddedViaAPI = true;
			nRegisterWeapon(object->baseForm->GetFullName(), object->baseForm, object, handle, isMelee);
			if (handle < 1 || handle > 7 || !HolsterContainers[handle]) return;
			BGSInventoryList* holsterINV = HolsterContainers[handle]->inventoryList;
			if (holsterINV) {
				//holsterINV->inventoryLock.LockForReadAndWrite();
				ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
				if (CurrentETDD) {
					CurrentETDD->name = ETTDName.c_str();
					holsteredWeapNames[handle] = ETTDName;
					CurrentETDD->nameLength = std::strlen(ETTDName.c_str());
					CurrentETDD->type = -2;
				}
				//holsterINV->inventoryLock.Unlock();
			}
		}
		return;
	}

	InventorySnapshot GetINVSnapShot(int handle, BGSInventoryList* inventory, std::string holstername) {

		InventorySnapshot CurrentSnapShot;
		if (inventory) {
			for (size_t invPos = 0; invPos < inventory->items.count; invPos++) {
				BGSInventoryItem item = inventory->items[invPos];
				TESForm* form = item.form;
				BGSInventoryItem::Stack* stack = item.stack;
				if (form) {  //Only Visit FORMS 
					if (form->formType == kFormType_WEAP) {  //Only Visit WEAPON Items
						TESObjectWEAP* weapon = static_cast<TESObjectWEAP*>(form);
						if (stack) {
							int stackPos = 0;
							stack->Visit([&](BGSInventoryItem::Stack* element) {  //Start of Sub Stacks For this Weapon
								if (element) {
									ExtraDataList* stackDataList = element->extraData;
									if (stackDataList) {
										ExtraInstanceData* currentEID = DYNAMIC_CAST(stackDataList->GetByType(kExtraData_InstanceData), BSExtraData, ExtraInstanceData);
										ExtraTextDisplayData* currentETDD = DYNAMIC_CAST(stackDataList->GetByType(kExtraData_TextDisplayData), BSExtraData, ExtraTextDisplayData);
										if (element->flags && element->kFlagEquipped && !CurrentSnapShot.isWeapEquipped) {
											if (currentEID && currentETDD) {
												CurrentSnapShot.isWeapEquipped = true;
												CurrentSnapShot.EquippedWeaponInvItem = item;
												CurrentSnapShot.Equippedform = form;
												CurrentSnapShot.Equippedweapon = weapon;
												CurrentSnapShot.EquippedweapETDD = currentETDD;
												CurrentSnapShot.EquippedWeapStackID = stackPos;
												CurrentSnapShot.EquippedweapTBOID = GetTBO_InstanceDataFromInventoryItem(&item, stackPos);
												CurrentSnapShot.EquippedNameSize = currentETDD->nameLength;
											}
											else { // No ETDD or EID - Use fallback.
												bool isThrowable = true;
												for (int slotIndex = 32; slotIndex < 44; slotIndex++) {
													TESForm* equippedItem = (*g_player)->equipData->slots[slotIndex].item;
													if (equippedItem && isThrowable) {
														UInt32 equipSlot = weapon->weapData.equipSlot->formID;
														if (equipSlot != 289452) { // ensure equipped weapon is not using 'Grenade' slot. After dropping items equipped throwables sometimes appear in slot index 41 as well as the main drawn weapon.
															isThrowable = false;
															break;
														}
													}
												}
												if (!isThrowable) {
													std::string weapName = form->GetFullName();
													CurrentSnapShot.isWeapEquipped = true;
													CurrentSnapShot.EquippedWeaponInvItem = item;
													CurrentSnapShot.Equippedform = form;
													CurrentSnapShot.Equippedweapon = weapon;
													CurrentSnapShot.EquippedweapETDD = nullptr;
													CurrentSnapShot.EquippedWeapStackID = stackPos;
													CurrentSnapShot.EquippedweapTBOID = nullptr;
													CurrentSnapShot.EquippedNameSize = std::strlen(weapName.c_str());
													CurrentSnapShot.EquippedFallbackName = weapName;
												}
											}
										}
										if (currentEID && currentETDD) {
											std::string weapName = currentETDD->name;
											if (weapName == holstername) {
												if (!CurrentSnapShot.TaggedWeapFound) {
													CurrentSnapShot.TaggedWeaponInvItem = item;
													CurrentSnapShot.TaggedWeapFound = true;
													CurrentSnapShot.TaggedWeapStackID = stackPos;
													CurrentSnapShot.Taggedform = form;
													CurrentSnapShot.Taggedweapon = weapon;
													CurrentSnapShot.TaggedweapETDD = currentETDD;
													CurrentSnapShot.TaggedweapTBOID = GetTBO_InstanceDataFromInventoryItem(&item, stackPos);
												}
											}
										}
										else {  // No ETDD or EID - Use fallback.
											std::string weapName = form->GetFullName();
											if (weapName == holstername) {
												if (!CurrentSnapShot.TaggedWeapFound) {
													CurrentSnapShot.TaggedWeapFound = true;
													CurrentSnapShot.TaggedWeaponInvItem = item;
													CurrentSnapShot.TaggedWeapStackID = stackPos;
													CurrentSnapShot.Taggedform = form;
													CurrentSnapShot.Taggedweapon = weapon;
													CurrentSnapShot.TaggedweapETDD = nullptr;
													CurrentSnapShot.TaggedweapTBOID = nullptr;
													CurrentSnapShot.TaggedFallbackName = weapName;
												}
											}
										}
									}
								}
								stackPos++;
								return true;
								}); // End of Sub Stacks
						}
					}
				}
			}
			return CurrentSnapShot;
		}
	}

	ExtraTextDisplayData* FindHolster(BGSInventoryList* inventory) {
		for (size_t invPos = 0; invPos < inventory->items.count; invPos++) {
			BGSInventoryItem item = inventory->items[invPos];
			TESForm* form = item.form;
			BGSInventoryItem::Stack* stack = item.stack;
			if (form) {  //Only Visit FORMS 
				if (form->formType == kFormType_WEAP) {
					//TESObjectWEAP* weapItem = static_cast<TESObjectWEAP*>(form);
					if (stack) {
						ExtraDataList* stackDataList = stack->extraData;
						if (stackDataList) {
							auto ExtraData = stackDataList->GetByType(kExtraData_TextDisplayData);
							if (ExtraData) {
								ExtraTextDisplayData* currentETDD = DYNAMIC_CAST(stackDataList->GetByType(kExtraData_TextDisplayData), BSExtraData, ExtraTextDisplayData);
								if (currentETDD) {
									return currentETDD;
								}
								else {
									return nullptr;
								}
							}
							else {
								return nullptr;
								_MESSAGE("ERROR EXTRA DATA NOT FOUND ON LOOKUP");
							}
						}
					}
				}
			}
		}
		return nullptr;
	}

	BGSInventoryItem::Stack* GetStackByStackID(BGSInventoryItem* item, UInt16 stackID)
	{
		BGSInventoryItem::Stack* stack = item->stack;
		if (!stack) return NULL;
		while (stackID != 0)
		{
			stack = stack->next;
			if (!stack) return NULL;
			stackID--;
		}
		return stack;
	}

	TBO_InstanceData* GetTBO_InstanceDataFromInventoryItem(BGSInventoryItem* item, UInt16 stackID)
	{
		BGSInventoryItem::Stack* stack = item->stack;
		if (!stack) return NULL;
		while (stackID != 0)
		{
			stack = stack->next;
			if (!stack) return NULL;
			stackID--;
		}
		if (stack)
		{
			ExtraInstanceData* eid = DYNAMIC_CAST(stack->extraData->GetByType(kExtraData_InstanceData), BSExtraData, ExtraInstanceData);
			if (eid) return eid->instanceData;
		}
		return NULL;
	}

	ExtraTextDisplayData* GetExtraTextDisplayData(BGSInventoryItem* item, UInt16 stackID)
	{
		BGSInventoryItem::Stack* stack = item->stack;
		if (!stack) return NULL;
		while (stackID != 0)
		{
			stack = stack->next;
			if (!stack) return NULL;
			stackID--;
		}
		if (stack)
		{
			ExtraTextDisplayData* etdd = DYNAMIC_CAST(stack->extraData->GetByType(kExtraData_InstanceData), BSExtraData, ExtraTextDisplayData);
			if (etdd) return etdd;
		}
		return NULL;
	}

	void registerWeaponInMain(TESForm* weapForm, TESObjectWEAP* weapon, TESObjectREFR* object, int handle) {
		BSFixedString FormName = weapForm->GetFullName();
		TESObjectARMO* i = nullptr;
		UInt32 meleeKWFormId[2] = { 0x4A0A4, 0x4A0A45 };
		bool isMelee = false;
		for (int index = 0; index < 2; index++) {
			for (UInt32 i = 0; i < weapon->keyword.numKeywords; i++)
			{
				if (weapon->keyword.keywords[i])
				{
					if (weapon->keyword.keywords[i]->formID == meleeKWFormId[index])
					{
						isMelee = true;
					}
				}
			}
		}
		nRegisterWeapon(FormName, weapForm, object, handle, isMelee);
	}

	void GameLoadInvSort() {
		_MESSAGE("---- Holster Config:: Update Completed");
		_MESSAGE("Game Event:: Game Load Completed");
		bool isFirstRun = true;
		BGSInventoryList* playerINV = (*g_player)->inventoryList;
		for (int i = 1; i < 8; i++) {
			if (i < 1 || i > 7 || !HolsterContainers[i]) return;
			BGSInventoryList* holsterINV = HolsterContainers[i]->inventoryList;
			if (holsterINV) { // Holster Inventory Located - Proceed As Normal.
				//holsterINV->inventoryLock.LockForReadAndWrite();
				ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
				if (CurrentETDD) {
					std::string name = CurrentETDD->name;
					std::string mess = "-------- ETDD Link Established with Holster0" + std::to_string(i);
					std::string mess2 = "------------ Holster Status: " + name;
					_MESSAGE(mess.c_str());
					_MESSAGE(mess2.c_str());
					holsteredWeapNames[i] = CurrentETDD->name;
				}
				else {
					std::string mess = "-------- ETDD Link Failed For Holster0" + std::to_string(i);
					_MESSAGE("------------ Holster Status: Fail - Something Went Wrong!");
				}
				//holsterINV->inventoryLock.Unlock();
				isFirstRun = false;
			}
			else {  // No Inventory List Found - This Signifies a Fist Run Scenario. Create VRH Template in Holsters Inventory.
				std::string mess = "-------- Unable To Establish ETDD Link with Holster0" + std::to_string(i);
				std::string mess2 = "------------ Holster0" + std::to_string(i) + ":: ETDD Config Start";
				_MESSAGE(mess.c_str());
				_MESSAGE(mess2.c_str());
				NEW_REFR_DATA* refrData = new NEW_REFR_DATA();
				refrData->location = (*g_player)->pos;
				refrData->direction = (*g_player)->rot;
				refrData->interior = (*g_player)->parentCell;
				refrData->world = Offsets::TESObjectREFR_GetWorldSpace(*g_player);
				ExtraDataList* extraData = (ExtraDataList*)Offsets::MemoryManager_Allocate(g_mainHeap, 0x28, 0, false);
				Offsets::ExtraDataList_ExtraDataList(extraData);
				extraData->m_refCount += 1;
				Offsets::ExtraDataList_setCount(extraData, 10);
				refrData->extra = extraData;
				TESObjectWEAP* holsterBase = DYNAMIC_CAST(GetFormFromFile(0x01C1F7, "VirtualHolsters.esp"), TESForm, TESObjectWEAP);  //Get The Holster Template Form
				refrData->object = holsterBase;
				void* weapon = new std::size_t;
				void* newHandle = Offsets::TESDataHandler_CreateReferenceAtLocation(*g_dataHandler, weapon, refrData);
				std::uintptr_t newRefr = 0x0;
				Offsets::BSPointerHandleManagerInterface_GetSmartPointer(newHandle, &newRefr);
				TESObjectREFR* currentRefr = (TESObjectREFR*)newRefr;                                                                 //New ObjectRef Created From Holster Template Form
				if (currentRefr) {
					TESBoundObject* boundObject = DYNAMIC_CAST(holsterBase, TESForm, TESBoundObject);
					ExtraDataList* list = currentRefr->extraDataList;
					if (boundObject && list) {
						Offsets::AddObjecttoContainer(HolsterContainers[i], boundObject, reinterpret_cast<ExtraDataList*>(&list), 1, nullptr, ITEM_REMOVE_REASON::kNone);  //ObjectRef added to Holsters Container
						holsterINV = HolsterContainers[i]->inventoryList;
						if (holsterINV) {
							Offsets::DeleteRef(currentRefr);                                                                                                               //Delete ObjectRef from gameworld - no longer needed
							//holsterINV->inventoryLock.LockForReadAndWrite();
							ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
							if (CurrentETDD) {
								std::string mess3 = "---------------- ETDD Link Established with Holster0" + std::to_string(i);
								std::string mess4 = "-------------------- Holster Status: Empty";
								std::string mess5 = "------------ Holster0" + std::to_string(i) + ":: ETDD Config Complete";
								CurrentETDD->name = "Empty";
								CurrentETDD->nameLength = 5;
								CurrentETDD->type = -2;
								holsteredWeapNames[i] = "Empty";
								_MESSAGE(mess3.c_str());
								_MESSAGE(mess4.c_str());
								_MESSAGE(mess5.c_str());

							}
							//holsterINV->inventoryLock.Unlock();
						}
					}
				}
			}
		}

		if (isFirstRun) {
			return;
		}
		settingUpHolsters = true;
		/*for (int i = 1; i < 8; i++) {
			if (playerINV) {
				//playerINV->inventoryLock.LockForReadAndWrite();
				InventorySnapshot INVSnapShot = GetINVSnapShot(i, playerINV, holsteredWeapNames[i]);
				if (INVSnapShot.Taggedform && INVSnapShot.Taggedweapon) {
					TESObjectREFR* currentRefr = createObjectFromInvItem(INVSnapShot.Taggedweapon, INVSnapShot.TaggedWeaponInvItem, INVSnapShot.TaggedWeapStackID);
					if (currentRefr) {
						//playerINV->inventoryLock.LockForReadAndWrite();
						//INVSnapShot.TaggedWeaponInvItem.stack->count = INVSnapShot.TaggedWeaponInvItem.stack->count - 1;
						//playerINV->inventoryLock.Unlock();
						WaitforObject3D(INVSnapShot.Taggedform, INVSnapShot.Taggedweapon, currentRefr, HolsterContainers[i], i, 0);
					}
				}
				//playerINV->inventoryLock.Unlock();
			}
		}*/
		return;
	}

	void RemoveThrowableHolster(int holster) {
		BGSInventoryList* holsterINV = HolsterContainers[holster]->inventoryList;
		nDestoryWeapon(holster, false, true);
		if (holsterINV) {
			//holsterINV->inventoryLock.LockForReadAndWrite();
			ExtraTextDisplayData* CurrentETDD = FindHolster(holsterINV);
			if (CurrentETDD) {
				CurrentETDD->name = "Empty";
				holsteredWeapNames[holster] = "Empty";
				CurrentETDD->nameLength = 5;
				CurrentETDD->type = -2;
			}
			//holsterINV->inventoryLock.Unlock();
		}
	}

	TESObjectREFR* createObjectFromInvItem(TESObjectWEAP* weapon, BGSInventoryItem item, int stackID) {
		BGSInventoryItem::Stack* cloneStack = GetStackByStackID(&item, stackID);
		if (cloneStack && weapon) {
			NEW_REFR_DATA* refrData = new NEW_REFR_DATA();
			refrData->location = (*g_player)->pos;
			refrData->location.z = refrData->location.z + 1000;
			refrData->direction = (*g_player)->rot;
			refrData->interior = (*g_player)->parentCell;
			refrData->world = Offsets::TESObjectREFR_GetWorldSpace(*g_player);
			refrData->extra = cloneStack->extraData;
			refrData->object = weapon;
			void* weapDrop = new std::size_t;
			void* newHandle = Offsets::TESDataHandler_CreateReferenceAtLocation(*g_dataHandler, weapDrop, refrData);
			std::uintptr_t newRefr = 0x0;
			Offsets::BSPointerHandleManagerInterface_GetSmartPointer(newHandle, &newRefr);
			TESObjectREFR* currentRefr = (TESObjectREFR*)newRefr;
			return currentRefr;
		}
		else {
			return nullptr;
		}
	}

	void WaitforObject3D(TESForm* weapForm, TESObjectWEAP* weapon, TESObjectREFR* object, TESObjectREFR* container, int handle, int retryCount = 0) {
		const int MAX_RETRIES = 100;      // ~5 seconds at 50ms per retry
		const int RETRY_DELAY_MS = 50;
		if (weapon == nullptr || object == nullptr) {
			return;
		}
		if (retryCount >= MAX_RETRIES) {
			_MESSAGE("WaitforObject3D: Gave up waiting for 3D data on handle %d after %d retries", handle, retryCount);
			return;
		}

		auto loadedData = object->unkF0;
		if (loadedData == nullptr || loadedData->rootNode == nullptr) {
			if (retryCount > 0) {
				Sleep(RETRY_DELAY_MS);
			}
			std::thread t6(WaitforObject3D, weapForm, weapon, object, container, handle, retryCount + 1);
			t6.detach();
			return;
		}
		registerWeaponInMain(weapForm, weapon, object, handle);
		Offsets::DisableRef(object);
		Offsets::DeleteRef(object);
	}
}