// Thanks to RollingRock for orginal code from which this was adapted

#pragma once

#include "f4sE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "f4se/GameReferences.h"
#include "f4se/GameObjects.h"
#include "f4se/GameForms.h"
#include "f4se/NiNodes.h"
#include "f4se/NiObjects.h"
#include "f4se/BSGeometry.h"
#include "f4se/GameSettings.h"
#include "f4se/GameMenus.h"
#include "f4se/GameData.h"
#include "NiCloneProcess.h"
//#include "MiscStructs.h"
#include "f4se/PapyrusArgs.h"
#include "MiscStructs.h"

class BSAnimationManager;

template <class T>
class StackPtr {
public:
	StackPtr() { p = nullptr; }

	T p;
};

enum class ITEM_REMOVE_REASON
{
	kNone = 0x0,
	kStealing = 0x1,
	kSelling = 0x2,
	KDropping = 0x3,
	kStoreContainer = 0x4,
	kStoreTeammate = 0x5
};

namespace Offsets {
	typedef bool(*_PlayIdleAction)(Actor* actor, void* action, TESObjectREFR target, VirtualMachine* vm, UInt32 stackId);
	extern RelocAddr <_PlayIdleAction> PlayIdleAction;

	typedef TESObjectWEAP* (*_GetEquippedWeapon)(VirtualMachine* registry, UInt64 stackID, Actor* actor, UInt64 aiEquipIndex);
	extern RelocAddr <_GetEquippedWeapon> GetEquippedWeapon;

	typedef TESObjectREFR* (*_DropFirstObject)(VirtualMachine* registry, UInt64 stackID, TESObjectREFR* akObjectRef, bool abInitiallyDisabled);
	extern RelocAddr <_DropFirstObject> DropFirstObject;

	typedef TESObjectREFR* (*_Additem)(VirtualMachine* registry, UInt64 stackID, TESObjectREFR* akObjectRef, TESObjectWEAP* item, UInt64 ammount, bool absilent);
	extern RelocAddr <_Additem> Additem;
	

	typedef TESForm* (*_GetBaseObject)(VirtualMachine* registry, UInt64 stackID, VMRefOrInventoryObj* akObjectRef);
	extern RelocAddr <_GetBaseObject> GetBaseObject;

	typedef void(*_MoveTo)(TESObjectREFR* akObjectRef, TESObjectREFR* akTarget, float afXOffset, float afYOffset, float afZOffset);
	extern RelocAddr <_MoveTo> MoveTo;

	typedef UInt64(*_GetFormCount)(VirtualMachine* registry, UInt64 stackID, TESObjectREFR* actorRefr, TESForm* akItem);
	extern RelocAddr <_GetFormCount> GetFormCount;

	// loadNif native func
	typedef int(*_loadNif)(uint64_t path, uint64_t mem, uint64_t flags);
	extern RelocAddr<_loadNif> loadNif;

	typedef NiNode* (*_cloneNode)(NiNode* node, NiCloneProcess* obj);
	extern RelocAddr<_cloneNode> cloneNode;

	typedef NiNode* (*_addNode)(uint64_t attachNode, NiAVObject* node);
	extern RelocAddr<_addNode> addNode;

	extern RelocAddr<uint64_t> g_frameCounter;
	extern RelocAddr<UInt64*> cloneAddr1;
	extern RelocAddr<UInt64*> cloneAddr2;

	typedef TESObjectWEAP* (*_Actor_GetCurrentWeapon)(Actor* a_actor, TESObjectWEAP* weap, Holsters::BGSEquipIndex idx);
	extern RelocAddr<_Actor_GetCurrentWeapon> Actor_GetCurrentWeapon;

	typedef void(*_Actor_GetWeaponEquipIndex)(Actor* a_actor, Holsters::BGSEquipIndex* idx, Holsters::BGSObjectInstance* instance);
	extern RelocAddr<_Actor_GetWeaponEquipIndex> Actor_GetWeaponEquipIndex;

	typedef void(*_EquipObject)(std::uint64_t a_actorequipmanager, Actor* a_actor, Holsters::BGSObjectInstance* a_instance, std::uint32_t a_stackID, std::uint32_t a_number, BGSEquipSlot* a_slot,
		bool a_queueEquip, bool a_forceEquip, bool a_playSounds, bool a_applynow, bool a_locked);
	extern RelocAddr<_EquipObject>EquipObject;

	typedef void(*_UnEquipObject)(std::uint64_t, Actor* a_actor, Holsters::BGSObjectInstance* a_instance, std::uint32_t a_number, BGSEquipSlot* a_slot, std::uint32_t a_stackID,
		bool a_queueEquip, bool a_playSounds, bool a_applynow, bool a_locked, BGSEquipSlot* a_slotBeingReplaced);
	extern RelocAddr<_UnEquipObject>UnEquipObject;

	typedef bool(*_UnEquipItem1)(Actor* a_actor, TESForm* a_form);
	extern RelocAddr<_UnEquipItem1>UnEquipItem1;

	typedef TESWorldSpace* (*_TESObjectREFR_GetWorldSpace)(TESObjectREFR* a_refr);
	extern RelocAddr<_TESObjectREFR_GetWorldSpace> TESObjectREFR_GetWorldSpace;

	typedef void(*_ExtraDataList_setCount)(ExtraDataList* a_list, int a_count);
	extern RelocAddr<_ExtraDataList_setCount> ExtraDataList_setCount;

	typedef void(*_ExtraDataList_ExtraDataList)(ExtraDataList* a_list);
	extern RelocAddr<_ExtraDataList_ExtraDataList> ExtraDataList_ExtraDataList;

	typedef void* (*_MemoryManager_Allocate)(Heap* manager, uint64_t size, uint32_t someint, bool somebool);
	extern RelocAddr<_MemoryManager_Allocate> MemoryManager_Allocate;

	typedef void* (*_TESDataHandler_CreateReferenceAtLocation)(DataHandler* dataHandler, void* newRefr, Holsters::NEW_REFR_DATA* refrData);
	extern RelocAddr<_TESDataHandler_CreateReferenceAtLocation> TESDataHandler_CreateReferenceAtLocation;

	typedef void(*_BSPointerHandleManagerInterface_GetSmartPointer)(void* a_handle, void* a_refr);
	extern RelocAddr<_BSPointerHandleManagerInterface_GetSmartPointer> BSPointerHandleManagerInterface_GetSmartPointer;

	typedef void(*_DeleteRef)(TESObjectREFR* object);
	extern RelocAddr<_DeleteRef> DeleteRef;

	typedef void(*_AddObjecttoContainer)(TESObjectREFR* container, TESBoundObject* a_obj, ExtraDataList* a_extra, std::int32_t a_count, TESObjectREFR* a_oldContainer, ITEM_REMOVE_REASON a_reason);
	extern RelocAddr<_AddObjecttoContainer> AddObjecttoContainer;

	typedef void(*_AddObjecttoActor)(Actor* container, TESBoundObject* a_obj, ExtraDataList* a_extra, std::int32_t a_count, TESObjectREFR* a_oldContainer, ITEM_REMOVE_REASON a_reason);
	extern RelocAddr<_AddObjecttoActor> AddObjecttoActor;

	typedef int (*_GetItemCount)(TESObjectREFR* ref);
	extern RelocAddr<_GetItemCount> GetItemCount;

	typedef void(*_RemoveAllItems)(TESObjectREFR* container, ITEM_REMOVE_REASON a_reason);
	extern RelocAddr<_RemoveAllItems> RemoveAllItems;

	typedef ExtraDataList*(*_CopyExtraDataList)(ExtraDataList* source);
	extern RelocAddr<_CopyExtraDataList> CopyExtraDataList;

	typedef void(*_TESObjectREFR_UpdateAnimation)(TESObjectREFR* obj, float a_delta);
	extern RelocAddr< _TESObjectREFR_UpdateAnimation> TESObjectREFR_UpdateAnimation;

	typedef void(*_ScopeExitFade)();
	extern RelocAddr< _ScopeExitFade> ScopeExitFade;

	typedef int(*_GetSitState)(VirtualMachine* registry, UInt64 stackID, Actor* actor);
	extern RelocAddr< _GetSitState> GetSitState;

	typedef void (*_DrawWeaponMagicHands)(Actor* Actor, bool isTrue);
	extern RelocAddr<_DrawWeaponMagicHands> DrawWeaponMagicHands;
}
