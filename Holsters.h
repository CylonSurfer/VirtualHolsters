// Thanks to RollingRock for orginal code from which this was adapted

#pragma once
#include "f4se/PapyrusUtilities.h"
#include "f4se/PapyrusEvents.h"
#include "f4se/PapyrusVM.h"
#include "f4se/GameForms.h"
#include "f4se/GameFormComponents.h"
#include "NiCloneProcess.h"
#include "Offsets.h"
#include "SimpleIni.h"
#include "vrHookAPI.h"
#include "InvSorting.h"
#include "BSFlattenedBoneTree.h"
#include <thread>

extern PluginHandle g_pluginHandle;
extern F4SEPapyrusInterface* g_papyrus;
extern F4SEMessagingInterface* g_messaging;
extern bool gPlayerisSneaking;
extern bool gInPowerArmor;
extern bool gIsRightHanded;
extern bool _isTouchEnabled;
extern bool ghapticsEnabled;
extern bool gshowSpheres;
extern bool gShowWeapons;
extern bool MenuCheck;
extern bool fistsPressed;
extern bool infoPressed;
extern UInt32 gCurHolster;
extern OpenVRHookManagerAPI* vrHook;
extern std::map<std::string, int> boneTreeMap;
extern std::vector<std::string> boneTreeVec;
extern std::string holsteredWeapNames[];
extern TESObjectREFR* HolsterContainers[];
extern int doublepress;


namespace Holsters {

	extern int c_holsterButtonID;
	extern bool c_VATSDisabled;
	extern bool c_ActivationDisabled;
	extern bool isHolsterIntersected;
	extern bool initBoneTreeFlag;
	extern std::map<UInt32, class MyHolster*> HolsterRegisteredObjects;
	typedef bool(*_IsSneaking)(Actor* a_actor);
	extern RelocAddr<_IsSneaking> IsSneaking;

	class MyWeapon {
	public:
		MyWeapon() {
			wbone = nullptr;
			WeapMesh = nullptr;
			ArtMesh = nullptr;
			HolsterName = "name";
			isMelee = false;
			WeaponName ="name";
			ThisWeapon = nullptr;
			ThisForm = 0;
		}

		MyWeapon(NiNode* a_bone, BSFixedString a_holN, bool a_melee, BSFixedString a_weapName, UInt32 a_form) : wbone(a_bone), HolsterName(a_holN), isMelee(a_melee), WeaponName(a_weapName), ThisForm(a_form) {
			WeapMesh = nullptr;
			ArtMesh = nullptr;
			ThisWeapon = nullptr;
		}
		NiNode* wbone;
		NiNode* WeapMesh;
		NiNode* ArtMesh;
		BSFixedString HolsterName;
		bool isMelee;
		BSFixedString WeaponName;
		TESObjectREFR* ThisWeapon;
		UInt32 ThisForm;
	};

	class MyHolster {
	public:		
		MyHolster() {
			radius = 0;
			bone = nullptr;
			stickyRight = false;
			stickyLeft = false;
			turnOnDebugSpheres = false;
			offset.x = 0;
			offset.y = 0;
			offset.z = 0;
			debugSphere = nullptr;
			MeshPath = "path";
			MeshName = "name";	
		}

		MyHolster(float a_radius, NiNode* a_bone, NiPoint3 a_offset, BSFixedString a_mesh, BSFixedString a_meshN) : radius(a_radius), bone(a_bone), offset(a_offset), MeshPath(a_mesh), MeshName(a_meshN) {
			stickyRight = false;
			stickyLeft = false;
			turnOnDebugSpheres = false;
			debugSphere = nullptr;
		}		
		float radius;
		NiNode* bone;
		NiPoint3 offset;
		NiPoint3 offset2;
		bool stickyRight;
		bool stickyLeft;
		bool turnOnDebugSpheres;
		NiNode* debugSphere;
		BSFixedString MeshPath;
		BSFixedString MeshName;
	};

	enum HolsterEvent {
		HolsterEvent_None = 0,
		HolsterEvent_Selfie = 1,
		HolsterEvent_CamBack = 2,
		HolsterEvent_CamForward = 3,
		HolsterEvent_StrayFists = 4,
		HolsterEvent_LPointing = 5,
		HolsterEvent_RPointing = 6,
		HolsterEvent_LHandRelease = 7,
		HolsterEvent_RHandRelease = 8,
		HolsterEvent_DisableVATS = 9,
		HolsterEvent_DisableActivate = 10,
		HolsterEvent_EnableActivate = 11,
		HolsterEvent_Leftfist = 12,
		HolsterEvent_EnableVATS = 13,
		HolsterEvent_DisableFavorites = 14,
		HolsterEvent_EnableFavorites = 15,
		HolsterEvent_DrawSword = 16,
		HolsterEvent_SheathWeapon = 17,
		HolsterEvent_AddInv = 18
	};
	
	NiNode* loadNifFromFile(char* path);
	
	bool initBoneTree();
	void MainLoop();
	void checkArmor();
	void checkfists();
	void handlePowerArmor();
	bool playerIsSneaking();
	void weaponsPAChange();
	void VRButtonsMain();
	void mainStartup();
	void saveGameLoaded();
	bool loadConfigFiles();
	void swapHolsterButton();
	void configmodeupdate(int);
	void closeSelfieMode();
	void retorecam();
	void ActivationToggle();
	void ChangeVATS();
	void toggleHaptics();
	void restoreAllWeapons();
	void toggleWeapVis();
	void toggleSphereVis();
	void toggleSwitchRGA();
	void toggleSwitchLGA();
	void toggleSouthPaw();
	void PressChangeAllArtWork();
	void pressArtWorkChange(int);
	void closeMenus();
	void scaleHolster();
	void moveholsterX();
	void moveholsterY();
	void moveholsterZ();
	void rotateholsterX();
	void rotateholsterY();
	void rotateholsterZ();
	void saveconfig();
	void PlayerLoaded();
	void SetPointingHand();
	void RestorePointingHand();
	void detectHolsterSphere();
	void displayHolstersSphere(UInt32);
	void createAllHolsterSpheres();
	void updateSpheresForPAEvent();
	void registerHolsterSphere(float, BSFixedString, NiPoint3, BSFixedString, BSFixedString, UInt32);
	void destroyAllHolsterSpheres();
	void sphereMoveSneak();
	void sphereMoveStand();
	float getRadius(int);
	void hideMagazine(NiNode* child, int handle);
	void clonemeshfix(NiNode* original, NiNode* clone);
	void displayWeapon(TESObjectREFR* object, UInt32 handle);
	void nDestoryWeapon(UInt32 handle, bool destroyArt, bool playsound);
	void playSwordSFX(int handle);
	void moveWeapon(int);
	void nRegisterWeapon(BSFixedString weapName, TESForm* baseForm, TESObjectREFR* object, UInt32 handle, bool isMeleeW);
	void RegisterForHolsterEvents(StaticFunctionTag* base, VMObject* thisObject);
	void UnRegisterForHolsterEvents(StaticFunctionTag* base, VMObject* thisObject);
	bool RegisterFuncs(VirtualMachine* vm);
	
	inline NiNode* loadNifFromFile(char* path) {
		uint64_t flags[2];
		flags[0] = 0x0;
		flags[1] = 0xed | 0x2d;
		uint64_t mem = 0;
		int ret = Offsets::loadNif((uint64_t) & (*path), (uint64_t)&mem, (uint64_t)&flags);

		return (NiNode*)mem;
	}
}   