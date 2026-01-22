
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

namespace Holsters {

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

	//---------------------------------------------

	






}