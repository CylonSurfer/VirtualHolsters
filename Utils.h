// Thanks to RollingRock for orginal code from which this was adapted

#pragma once
#include "f4se/GameSettings.h"
#include "matrix.h"

namespace Holsters {
	struct InventoryState;
	Setting* GetINISettingNative(const char* name);
	float vec3_len(NiPoint3 v1);
	NiPoint3 vec3_norm(NiPoint3 v1);
	void SetINIBool(BSFixedString name, bool value);
	void setFingerPositionScalar(bool isLeft, float thumb, float index, float middle, float ring, float pinky);
	void restoreFingerPoseControl(bool isLeft);
	void SetINIFloat(BSFixedString name, float value);
	float vec3_dot(NiPoint3 v1, NiPoint3 v2);
	NiPoint3 vec3_cross(NiPoint3 v1, NiPoint3 v2);
	float degrees_to_rads(float deg);
	float rads_to_degrees(float deg);
	void ShowMessagebox(std::string asText);
	void EquipItem(Actor* actor, TESForm* akItem, bool abPreventEquip, bool abSilent);
	typedef void(*_ActorEquipItem)(VirtualMachine* registry, UInt64 stackID, Actor* actor, TESForm* akItem, bool abPreventRemoval, bool abSilent);
	extern RelocAddr <_ActorEquipItem> ActorEquipItem;
	bool getLeftHandedMode();
	bool exists_file(std::string);
	void CheckMenusTimer();
	void FistsTimer(int time);
	void infoTimer(int time);
	void checkingloadingcompleted();
	NiNode* getChildNode(const char* nodeName, NiNode* nde);
	NiNode* get1stChildNode(const char* nodeName, NiNode* nde);
	NiNode* FindNode(BSFixedString nodename);
	bool HasKeyword(TESObjectARMO* armor, UInt32 keywordFormId);
	bool detectInPowerArmor();
	std::vector<std::string> getFileNames(const std::string&);
	bool matchSubString(const char* w1, const char* w2);
	std::string FileNameCreation(int handle, UInt32 weaponformID);
	TESForm* GetFormFromFile(UInt32 formID, const char* pluginName);
}
