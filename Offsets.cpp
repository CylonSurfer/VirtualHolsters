#include "Offsets.h"

namespace Offsets {
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
	RelocAddr<_TESObjectREFR_UpdateAnimation> TESObjectREFR_UpdateAnimation(0x419b50);
	RelocAddr<_ScopeExitFade> ScopeExitFade(0xbc85f0);
	RelocAddr<_GetSitState> GetSitState(0x40e410);
}