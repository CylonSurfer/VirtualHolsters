#include "Offsets.h"
#include "f4se\GameReferences.h"
#include "f4se/GameEvents.h"

namespace Offsets {
	RelocPtr<SettingCollectionList*> g_iniPrefSettings_Fixed(0x5BBEC18);
	Setting* GetINIPrefSettingSafe(const char* name) {
		SettingCollectionList** collectionPtr = g_iniPrefSettings_Fixed.GetPtr();
		if (collectionPtr && *collectionPtr) {
			return (*collectionPtr)->Get(name);
		}
		return nullptr;
	}

	RelocAddr<_cloneNode2> cloneNode2(0x1c13f10);
	RelocAddr<uint32_t> g_equipIndex(0x3706d2c);
	RelocAddr<_GetEquippedWeapon> GetEquippedWeapon(0x140d790);
	RelocAddr<_DropFirstObject> DropFirstObject(0x148a390);
	RelocAddr < _Additem> Additem(0x1487c90);
	RelocAddr<_GetBaseObject> GetBaseObject(0x148b8d0);
	RelocAddr<_MoveTo> MoveTo(0x04e6e00);
	RelocAddr<_GetFormCount> GetFormCount(0x148bd50);
	RelocAddr <_PlayIdleAction > PlayIdleAction(0x140b190);
	// loadNif native func
	RelocAddr<_loadNif> loadNif(0x1d0dee0);
	RelocAddr<_cloneNode> cloneNode(0x1c13ff0);
	RelocAddr<_addNode> addNode(0xada20);
	RelocAddr<uint64_t> g_frameCounter(0x65a2b48);
	RelocAddr<UInt64*> cloneAddr1(0x36ff560);
	RelocAddr<UInt64*> cloneAddr2(0x36ff564);
	RelocAddr<_Actor_GetCurrentWeapon> Actor_GetCurrentWeapon(0xe50da0);
	RelocAddr<_Actor_GetWeaponEquipIndex> Actor_GetWeaponEquipIndex(0xe50e70);
	RelocAddr<_EquipObject> EquipObject(0xe6fea0);
	RelocAddr<_UnEquipObject> UnEquipObject(0xe70280);
	RelocAddr<_UnEquipItem1> UnEquipItem1(0xe707b0);
	RelocAddr<_TESObjectREFR_GetWorldSpace> TESObjectREFR_GetWorldSpace(0x3f75a0);
	RelocAddr<_ExtraDataList_setCount> ExtraDataList_setCount(0x88fe0);
	RelocAddr<_ExtraDataList_ExtraDataList> ExtraDataList_ExtraDataList(0x81360);
	RelocAddr<_MemoryManager_Allocate> MemoryManager_Allocate(0x1b91950);
	RelocAddr<_TESDataHandler_CreateReferenceAtLocation> TESDataHandler_CreateReferenceAtLocation(0x11bd80);
	RelocAddr<_BSPointerHandleManagerInterface_GetSmartPointer> BSPointerHandleManagerInterface_GetSmartPointer(0xab60);
	RelocAddr<_DeleteRef> DeleteRef(0x14808f0); 
	RelocAddr<_AddObjecttoContainer> AddObjecttoContainer(0x03e9e90);
	RelocAddr<_AddObjecttoActor> AddObjecttoActor(0x0de0c60);
	RelocAddr<_GetItemCount> GetItemCount(0x3e38c0);
	RelocAddr<_RemoveAllItems> RemoveAllItems(0x3e69d0);	
	RelocAddr<_CopyExtraDataList> CopyExtraDataList(0x082e50);
	RelocAddr<_GetSitState> GetSitState(0x40e410);
	RelocAddr <_DrawWeaponMagicHands> DrawWeaponMagicHands(0xdc4b40);
	RelocAddr <_moveToRef> moveToRef(0x4e6e00);
	RelocAddr <_ActivateRef> ActivateRef(0x32a200);
	RelocAddr<_ActivateRef2> ActivateRef2(0x3f4a60);
	RelocAddr <_DisableRef> DisableRef(0x3f63c0);
	//RelocAddr <_ProvidebHapticFeedBack> ProvidebHapticFeedBack(0x47180);
	struct BGSObjectInstance {
		TESForm* item;         // The Base Form (e.g., TESObjectWEAP)
		void* instanceData; // Attachment/Mod data (TESScript::ObjectInstance::InstanceData)
	};

	// Define the function pointer with the exact signature for the x64 stack
	
	RelocAddr<_DropObject> DropObject(0xf07e90);



}