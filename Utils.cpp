
// Thanks to RollingRock for orginal code from which this was adapted

#include "utils.h"
#include <math.h>
#define PI 3.14159265358979323846
#include "f4se/GameRTTI.h"
#include "f4se/GameForms.h"
#include "f4se/GameFormComponents.h"
#include <Windows.h>
#include <vector>
#include <regex>
#include<fstream>
#include <sstream>

namespace Holsters {

	template <typename T, std::size_t N>
	struct BSTSmallArray {
		T* data{ inlineBuffer };
		uint32_t  size{ 0 };
		uint32_t  capacity{ 0x80000000 | static_cast<uint32_t>(N) }; // 0x80000000 = inline mode flag
		T         inlineBuffer[N]{};

		BSTSmallArray() = default;
	};

	using BSTSmallArray4 = BSTSmallArray<uint32_t, 4>;
	static_assert(sizeof(BSTSmallArray4) == 0x20, "BSTSmallArray4 size mismatch!");

	NiNode* CloneThisNode(NiNode* node) {
		NiAVObject* result = Offsets::cloneNode2(node);
		if (result) {
			return static_cast<NiNode*>(result);
		}
		else {
			return nullptr;
		}
	}

	void LogNodeChildren(NiNode* a_node, int a_depth = 0) {
		if (!a_node) return;

		// 1. Create a prefix for indentation to make the log readable
		std::string indent = "";
		for (int i = 0; i < a_depth; i++) indent += "  ";

		// 2. Log the current node
		std::string n = a_node->m_name;
		std::string nn = "c_" + n;
		a_node->m_name = nn.c_str();

		const char* name = a_node->m_name.c_str();
		//_MESSAGE("%s[Node] %s", indent.c_str(), (name && name[0]) ? name : "<Blank Name>");

		// 3. Iterate through the NiTArray of children
		// m_children.m_size is the current count
		// m_children.m_data is the pointer to the array of NiAVObject*
		for (UInt32 i = 0; i < a_node->m_children.m_size; i++) {
			NiAVObject* child = a_node->m_children.m_data[i];

			if (child) {
				// Check if the child is another NiNode (so we can recurse)
				NiNode* childNode = child->GetAsNiNode();

				if (childNode) {
					// Recursive call to see the whole tree
					LogNodeChildren(childNode, a_depth + 1);
				}
				else {
					// It's a leaf node (likely a NiTriShape or BSTriShape)
					std::string n = child->m_name;
					std::string nn = "c_" + n;
					child->m_name = nn.c_str();
					const char* childName = child->m_name.c_str();
					//_MESSAGE("%s  [Leaf] %s", indent.c_str(), (childName && childName[0]) ? childName : "<Blank Name>");
				}
			}
			else {
				//_MESSAGE("%s  [Empty Slot]", indent.c_str());
			}
		}
	}


	typedef Setting* (*_SettingCollectionList_GetPtr)(SettingCollectionList* list, const char* name);
	RelocAddr<_SettingCollectionList_GetPtr> SettingCollectionList_GetPtr(0x501500);

	UInt32 KeywordPowerArmor = 0x4D8A1;
	UInt32 KeywordPowerArmorFrame = 0x15503F;
	UInt32 KeywordCantEquip = 0x1CF299;
	bool _inPowerArmor;

	Setting* GetINISettingNative(const char* name)
	{
		Setting* setting = SettingCollectionList_GetPtr(*g_iniSettings, name);
		if (!setting)
			setting = SettingCollectionList_GetPtr(*g_iniPrefSettings, name);

		return setting;
	}

	void setFingerPositionScalar(bool isLeft, float thumb, float index, float middle, float ring, float pinky) {
		CallGlobalFunctionNoWait6<bool, float, float, float, float, float>("FRIK:FRIK", "setFingerPositionScalar", isLeft, thumb, index, middle, ring, pinky);
	}

	void restoreFingerPoseControl(bool isLeft) {
		CallGlobalFunctionNoWait1<bool>("FRIK:FRIK", "restoreFingerPoseControl", isLeft);
	}

	void SetINIFloat(BSFixedString name, float value) {
		CallGlobalFunctionNoWait2<BSFixedString, float>("Utility", "SetINIFloat", BSFixedString(name.c_str()), value);
	}

	void SetINIBool(BSFixedString name, bool value) {
		CallGlobalFunctionNoWait2<BSFixedString, bool>("Utility", "SetINIBool", BSFixedString(name.c_str()), value);
	}

	float vec3_len(NiPoint3 v1) {

		return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
	}

	void ShowMessagebox(std::string asText) {
		CallGlobalFunctionNoWait1<BSFixedString>("Debug", "Messagebox", BSFixedString(asText.c_str()));
	}

	void UnequipItem(Actor* actor, TESForm* akItem, bool abPreventEquip, bool abSilent) {
		CallGlobalFunctionNoWait4<Actor*, TESForm*, bool, bool>("Actor", "UnequipItem", actor, akItem, abPreventEquip, abSilent);
	}

	void EquipItem(Actor* actor, TESForm* akItem, bool abPreventEquip, bool abSilent) {
		CallGlobalFunctionNoWait4<Actor*, TESForm*, bool, bool>("Actor", "EquipItem",actor, akItem, abPreventEquip, abSilent);
	}

	bool getLeftHandedMode() {
		Setting* set = GetINISetting("bLeftHandedMode:VR");

		return set->data.u8;
	}

	bool exists_file(std::string name) {
		struct stat buffer;

		return (stat(name.c_str(), &buffer) == 0);
	}

	void CheckMenusTimer() {
		Sleep(1000);
		MenuCheck = false;
	}

	void FistsTimer(int time) {
		Sleep(time);
		fistsPressed = false;
	}

	void infoTimer(int time) {
		Sleep(time);
		infoPressed = false;
		doublepress = 0;
	}

	void doublePressTimer() {
		gConfigDoublePress = true;
		Sleep(300);
		gConfigDoublePress = false;
	}



	void checkingloadingcompleted() {
		//Sleep(500);
		PlayerLoaded();
	}

	NiNode* FindNode(BSFixedString nodename) {
		NiNode* boneNode = (NiNode*)getChildNode(nodename, (*g_player)->unkF0->rootNode);
		if (!boneNode) {
			auto n = (*g_player)->unkF0->rootNode->GetAsNiNode();
			while (n->m_parent) {
				n = n->m_parent->GetAsNiNode();
			}
			boneNode = getChildNode(nodename, n);
			if (!boneNode) {
				boneNode = nullptr;
				return boneNode;
			}
		}
		return boneNode;
	}

	NiPoint3 vec3_norm(NiPoint3 v1) {
		double mag = vec3_len(v1);
		if (mag < 0.000001) {
			float maxX = abs(v1.x);
			float maxY = abs(v1.y);
			float maxZ = abs(v1.z);
			if (maxX >= maxY && maxX >= maxZ) {
				return (v1.x >= 0 ? NiPoint3(1, 0, 0) : NiPoint3(-1, 0, 0));
			}
			else if (maxY > maxZ) {
				return (v1.y >= 0 ? NiPoint3(0, 1, 0) : NiPoint3(0, -1, 0));
			}
			return (v1.z >= 0 ? NiPoint3(0, 0, 1) : NiPoint3(0, 0, -1));
		}
		v1.x /= mag;
		v1.y /= mag;
		v1.z /= mag;
		return v1;
	}

	float vec3_dot(NiPoint3 v1, NiPoint3 v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	NiPoint3 vec3_cross(NiPoint3 v1, NiPoint3 v2) {
		NiPoint3 crossP;
		crossP.x = v1.y * v2.z - v1.z * v2.y;
		crossP.y = v1.z * v2.x - v1.x * v2.z;
		crossP.z = v1.x * v2.y - v1.y * v2.x;
		return crossP;
	}

	float degrees_to_rads(float deg) {
		return (deg * PI) / 180;
	}

	float rads_to_degrees(float rad) {
		return (rad * 180) / PI;
	}

	NiNode* getChildNode(const char* nodeName, NiNode* nde) {
		if (!nde->m_name) {
			return nullptr;
		}
		if (!_stricmp(nodeName, nde->m_name.c_str())) {
			return nde;
		}
		NiNode* ret = nullptr;
		for (auto i = 0; i < nde->m_children.m_emptyRunStart; ++i) {
			auto nextNode = nde->m_children.m_data[i] ? nde->m_children.m_data[i]->GetAsNiNode() : nullptr;
			if (nextNode) {
				ret = getChildNode(nodeName, nextNode);
				if (ret) {
					return ret;
				}
			}
		}
		return nullptr;
	}

	NiNode* get1stChildNode(const char* nodeName, NiNode* nde) {
		for (auto i = 0; i < nde->m_children.m_emptyRunStart; ++i) {
			auto nextNode = nde->m_children.m_data[i] ? nde->m_children.m_data[i]->GetAsNiNode() : nullptr;
			if (nextNode) {
				if (!_stricmp(nodeName, nextNode->m_name.c_str())) {
					return nextNode;
				}
			}
		}
		return nullptr;
	}

	bool HasKeyword(TESObjectARMO* armor, UInt32 keywordFormId) {
		if (armor)
		{
			for (UInt32 i = 0; i < armor->keywordForm.numKeywords; i++)
			{
				if (armor->keywordForm.keywords[i])
				{
					if (armor->keywordForm.keywords[i]->formID == keywordFormId)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	bool detectInPowerArmor() {
		// Thanks Shizof and SmoothtMovementVR for below code
		if ((*g_player)->equipData) {
			if ((*g_player)->equipData->slots[0x03].item != nullptr)
			{
				TESForm* equippedForm = (*g_player)->equipData->slots[0x03].item;
				if (equippedForm)
				{
					if (equippedForm->formType == TESObjectARMO::kTypeID)
					{
						TESObjectARMO* armor = DYNAMIC_CAST(equippedForm, TESForm, TESObjectARMO);

						if (armor)
						{
							if (HasKeyword(armor, KeywordPowerArmor) || HasKeyword(armor, KeywordPowerArmorFrame))
							{							
								return true;
							}
							else
							{								
								return false;
							}
						}
					}
				}
			}
		}
		return false;
	}

	std::vector<std::string> getFileNames(const std::string& spec) {
		std::vector<std::string> filenames;
		WIN32_FIND_DATA search_data = {};
		HANDLE handle = FindFirstFileA(spec.c_str(), &search_data);
		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				filenames.push_back(search_data.cFileName);
			} while (FindNextFileA(handle, &search_data));
			FindClose(handle);
		}
		return filenames;
	}

	bool matchSubString(const char* w1, const char* w2) {
		const char* result = strstr(w1, w2);
		if (result) {
			return true;
		}
		else {
			return false;
		}
	}

	std::string FileNameCreation(int handle, UInt32 weaponformID) {
		std::string mypath;
		std::string x = std::to_string(weaponformID) + std::to_string(handle);
		if (handle < 3 || handle > 4) {
			int ArmorIndex[3]{ 3, 6, 11 };
			UInt32 ArmorID[3]{ 0, 0, 0 };
			for (int i = 0; i < 3; i++) {
				TESForm* equippedItem = (*g_player)->equipData->slots[ArmorIndex[i]].item;
				if (equippedItem) {
					ArmorID[i] = equippedItem->formID;
				}
			}
			mypath = x + "_" + std::to_string(ArmorID[0]) + "-" + std::to_string(ArmorID[1]) + "-" + std::to_string(ArmorID[2]);
		}
		else if (handle == 3) {
			int ArmorIndex[3]{ 3, 9, 14 };
			UInt32 ArmorID[3]{ 0, 0, 0 };
			for (int i = 0; i < 3; i++) {
				TESForm* equippedItem = (*g_player)->equipData->slots[ArmorIndex[i]].item;
				if (equippedItem) {
					ArmorID[i] = equippedItem->formID;
				}
			}
			mypath = x + "_" + std::to_string(ArmorID[0]) + "-" + std::to_string(ArmorID[1]) + "-" + std::to_string(ArmorID[2]);

		}
		else {
			int ArmorIndex[3]{ 3, 10, 15 };
			UInt32 ArmorID[3]{ 0, 0, 0 };
			for (int i = 0; i < 3; i++) {
				TESForm* equippedItem = (*g_player)->equipData->slots[ArmorIndex[i]].item;
				if (equippedItem) {
					ArmorID[i] = equippedItem->formID;
				}
			}
			mypath = x + "_" + std::to_string(ArmorID[0]) + "-" + std::to_string(ArmorID[1]) + "-" + std::to_string(ArmorID[2]);
		}
		return mypath;
	}

	TESForm* GetFormFromFile(UInt32 formID, const char* pluginName)
	{
		auto mod = (*g_dataHandler)->LookupLoadedModByName(pluginName);
		if (!mod) // No loaded mod by this name
			return nullptr;

		formID |= ((UInt32)mod->modIndex) << 24;

		return LookupFormByID(formID);
	}

	void gameLoadingTimer(int time) {
		Sleep(time);
		GameLoadTimer = true;
	}

	void bHaptics(PositionType location, std::vector<int> index, std::vector<int> intensity, std::vector<int> duration) {
		uintptr_t moduleBase = (uintptr_t)GetModuleHandleA("tactsuit_vr_plugin.dll");

		if (moduleBase) {
			_MESSAGE("Found Module");
			// 3. Calculate the actual address (Base + RVA)
			// Let's say your RVA from IDA/Ghidra is 0x54A20
			uintptr_t targetAddress = moduleBase + 0x44880;

			// 4. Cast the address to your function pointer type
			Offsets::_ProvidebHapticFeedBack func = reinterpret_cast<Offsets::_ProvidebHapticFeedBack>(targetAddress);

			// 5. Call it safely
			// Ensure you are calling this on the correct thread (usually the Main Thread)
			int numOfEffects = index.size() - 1;
			for (int i = 0; i <= numOfEffects; i++) {
				func(location, index[i], intensity[i], duration[i]);
				Sleep(duration[i]);
			}

			//_MESSAGE("External function called. Result: %d", result);
		}
		else {
			//_ERROR("ExternalMod.dll not found in memory.");
		}
	}

	void regbHapticsFile() {
		uintptr_t moduleBase = (uintptr_t)GetModuleHandleA("tactsuit_vr_plugin.dll");
		if (moduleBase) {
			
			//uintptr_t targetAddress = moduleBase + 0x1be70;
			uintptr_t targetAddress = moduleBase + 0x1c4c0; //RegisterFeedbackFromTactFile
			Offsets::_RegFeedBackFile func = reinterpret_cast<Offsets::_RegFeedBackFile>(targetAddress);
			std::string path = ".\\Data\\F4SE\\plugins\\VRH_bHaptics\\";
			std::string files[22] = { "Holster01.tact", "Holster01.tact", "Holster02.tact", "Holster03.tact", "Holster04.tact", "Holster05.tact", "Holster06.tact", "Holster07.tact",
			"UnHolster01.tact", "UnHolster02.tact", "UnHolster03.tact", "UnHolster04.tact", "UnHolster05.tact", "UnHolster06.tact", "UnHolster07.tact",
			"Holster01Buzz.tact", "Holster02Buzz.tact", "Holster03Buzz.tact", "Holster04Buzz.tact", "Holster05Buzz.tact", "Holster06Buzz.tact", "Holster07Buzz.tact" };
			std::string keys[22] = {"Holster00", "Holster01", "Holster02", "Holster03", "Holster04", "Holster05", "Holster06", "Holster07",
			"UnHolster01", "UnHolster02", "UnHolster03", "UnHolster04", "UnHolster05", "UnHolster06", "UnHolster07", 
			"Holster01Buzz", "Holster02Buzz", "Holster03Buzz", "Holster04Buzz", "Holster05Buzz", "Holster06Buzz", "Holster07Buzz" };
			for (int i = 1; i <= 21; i++) {
				std::string path2 = path + files[i];
				std::ostringstream sstream{};
				std::ifstream fs(path2);
				sstream << fs.rdbuf();
				const std::string tactFileStr(sstream.str());
				func(keys[i].c_str(), tactFileStr.c_str());
			}			
			_MESSAGE("TACT FILES FILE REGISTERED");
		}
		
	}

	void playbHapticsFile(std::string key) {
		if (g_tactsuitInterface) {
			_MESSAGE(key.c_str());
			g_tactsuitInterface->PlayHapticEffect(key, 0.0, 0.0, 1.0, true);
		}
		//uintptr_t moduleBase = (uintptr_t)GetModuleHandleA("tactsuit_vr_plugin.dll");
		//if (moduleBase) {
			//uintptr_t targetAddress = moduleBase + 0x1cff0;
			//uintptr_t targetAddress = moduleBase + 0x1d660; //SubmitRegistered
			//Offsets::_PlayFeedBackFile func = reinterpret_cast<Offsets::_PlayFeedBackFile>(targetAddress);
			//func(key.c_str());
		//}
	}

	void matchHapticsOnController() {
		gIsRightHanded ? vrHook->StartHaptics(2, 0.05, 2.0) : vrHook->StartHaptics(1, 0.05, 2.0);
		Sleep(50);
		gIsRightHanded ? vrHook->StartHaptics(2, 0.2, 0.2) : vrHook->StartHaptics(1, 0.2, 0.2);
	}

	void matchHapticsOnController2() {
		gIsRightHanded ? vrHook->StartHaptics(2, 0.2, 0.2) : vrHook->StartHaptics(1, 0.2, 0.2);
		Sleep(200);
		gIsRightHanded ? vrHook->StartHaptics(2, 0.05, 2.0) : vrHook->StartHaptics(1, 0.05, 2.0);
	}


	TESObjectREFR* SpawnHolsterWeapon(PlayerCharacter* player, BGSObjectInstance* weaponInstance, NiPoint3 targetPos) {
		// CHANGE THIS LINE: Use UInt32 exactly as defined in your typedef
		UInt32 resultHandle = 0;

		NiPoint3 targetRot = { 0.0f, 0.0f, 0.0f };

		// 1. Execute the Drop
		// resultHandle is passed as a pointer (RDX), so &resultHandle is correct
		Offsets::DropObject(player, &resultHandle, weaponInstance, nullptr, 1, &targetPos, &targetRot);

		// 2. Validate the handle
		if (resultHandle != 0) {
			TESObjectREFR* spawnedRef = nullptr;

			// 3. Resolve and Call
			// Your RelocAddr class has an implicit conversion 'operator T()', 
			// so you can use the name directly.
			if (LookupREFRByHandle(&resultHandle, &spawnedRef)) {
				return spawnedRef;
			}
		}

		return nullptr;
	}
	
	//---------------------------------------------




	using DropObject_t = uint32_t * (*)(
		Actor* a_actor,
		uint32_t* a_outHandle,
		BGSObjectInstance* a_itemInstance,
		BSTSmallArray4* a_stackData,
		int32_t            a_count,
		NiPoint3* a_pos,
		NiPoint3* a_rot
		);

	uint32_t DropItemFromActor(
		Actor* actor,
		TESForm* baseForm,
		TBO_InstanceData* instanceData = nullptr,
		int32_t count = 1,
		NiPoint3* pos = nullptr,
		NiPoint3* rot = nullptr)
	{
		if (!actor || !baseForm) return 0;
		RelocAddr<DropObject_t> DropObject(0xDDFA60);

		BGSObjectInstance itemInstance{ baseForm, instanceData };
		BSTSmallArray4 stackData{};
		uint32_t outHandle = 0;

		DropObject(actor, &outHandle, &itemInstance, &stackData, count, pos, rot);

		return outHandle;
	}

	void removeHolsterTimer() {
		Sleep(1500);
		if (_isHolsterButtonPressed && isHolsterIntersected) {
			_MESSAGE("DESTROY HOLSTER");
			RemoveThrowableHolster(gCurHolster);
		}
	}




}