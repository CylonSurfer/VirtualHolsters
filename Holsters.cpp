
// Thanks to RollingRock for orginal code from which this was adapted

#include "Holsters.h"
#include "utils.h"
#include "Offsets.h"
#include "weaponOffset.h"
#include "VRHookAPI.h"
#include "VRHook.h"
#include "VirtualHolstersAPI.h"
#include "f4se/GameMenus.h"
#include "ConfigModeMenu.h"
#include "MenuChecker.h"
#include <iostream>
#include <fstream>
#include "f4se/NiNodes.h"
#include "f4se/GameReferences.h"
#include "f4se/BSCollision.h"

PluginHandle g_pluginHandle = kPluginHandle_Invalid;
F4SEPapyrusInterface* g_papyrus = NULL;
F4SEMessagingInterface* g_messaging = NULL;
OpenVRHookManagerAPI* vrHook;

bool gPlayerisSneaking = false;
bool gInPowerArmor = false;
bool _isTouchEnabled;
bool ghapticsEnabled = false;
bool gshowSpheres = false;
bool gShowWeapons = false;
bool fistsPressed = false;
bool infoPressed = false;
bool saveloaded = false;
bool MenuCheck = false;
int doublepress = 0;
bool gIsRightHanded;
std::map<std::string, int> boneTreeMap;
std::vector<std::string> boneTreeVec;
std::string holsteredWeapNames[8] = { "Empty" ,"Empty", "Empty", "Empty", "Empty", "Empty", "Empty", "Empty" };
TESObjectREFR* HolsterContainers[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
UInt32 gCurHolster;

namespace Holsters {

	RelocAddr<_IsSneaking> IsSneaking(0x24d20);

	typedef int(*_loadNif)(uint64_t path, uint64_t mem, uint64_t flags);
	RelocAddr<_loadNif> loadNif(0x1d0dee0);

	typedef NiNode* (*_cloneNode)(NiNode* node, NiCloneProcess* obj);
	RelocAddr<_cloneNode> cloneNode(0x1c13ff0);

	typedef NiNode* (*_addNode)(uint64_t attachNode, NiAVObject* node);
	RelocAddr<_addNode> addNode(0xada20);

	RelocAddr<uint64_t> g_frameCounter(0x65a2b48);
	RelocAddr<UInt64*> cloneAddr1(0x36ff560);
	RelocAddr<UInt64*> cloneAddr2(0x36ff564);

	const char* HolsterEventName = "HolsterEvent";
	RegistrationSetHolder<NullParameters>      g_HolsterEventRegs;
	std::map<UInt32, MyHolster*> HolsterRegisteredObjects;
	UInt32 nextHolsterHandle;
	std::map<UInt32, MyWeapon*> WeaponRegisteredObjects;
	UInt32 nextWeaponHandle;
	UInt32 curDevice;

	int c_holsterButtonID = vr::EVRButtonId::k_EButton_A;
	int c_configButtonID = vr::EVRButtonId::k_EButton_SteamVR_Touchpad;
	int c_configExitButtonID = vr::EVRButtonId::k_EButton_SteamVR_Trigger;
	int c_configModeShiftButton = vr::EVRButtonId::k_EButton_Grip;
	int c_configModeRotateButton = vr::EVRButtonId::k_EButton_A;
	int c_configChangeHolButton = vr::EVRButtonId::k_EButton_Grip;
	int c_selfieModeButtonID = vr::EVRButtonId::k_EButton_SteamVR_Touchpad;
	int c_movecamButtonID = vr::EVRButtonId::k_EButton_SteamVR_Touchpad;
	int c_equipButtonID = vr::EVRButtonId::k_EButton_SteamVR_Trigger;
	int c_ArtWorkButtonID = vr::EVRButtonId::k_EButton_ApplicationMenu;
	bool c_VATSDisabled = false;
	bool c_ActivationDisabled = false;
	bool menuOpenSticky = false;
	int holsterMode = 0;
	int weaponMode = 0;
	int c_mode = 0;
	int camDistancePA = 0;
	int camDistance = 0;
	int dist = 0;
	
	int holsterArtWorkPresets[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int PAholsterArtWorkPresets[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int PAholsterMaxValue[8];
	int holsterMaxValue[8];
	float pGrabDistance = 96.0;
	float sGrabDistance = 96.0;
	float grabMulti = 1.5000;
	std::string HolsterBones[8] = { "nout", "HmdNode", "HmdNode", "LLeg_Thigh", "RLeg_Thigh", "SPINE1", "Chest", "Chest" };
	std::string HolsterNames[8] = { "nout", "LShoulderHolster", "RShoulderHolster", "LHipHolster", "RHipHolster", "LowerBackHolster", "LeftChestHolster", "RightChestHolster" };
	std::string WeaponBones[8] = { "nout", "LArm_Collarbone", "RArm_Collarbone", "LLeg_Thigh", "RLeg_Thigh", "SPINE1", "Chest", "Chest" };
	std::string PAWeaponBones[8] = { "nout", "Chest", "Chest", "LLeg_Thigh", "RLeg_Thigh", "SPINE1", "Chest", "Chest" };
	std::string VisualArt[8];
	const char* hArt[8] = { "nout", "HolsterArt1", "HolsterArt2", "HolsterArt3", "HolsterArt4", "HolsterArt5", "HolsterArt6", "HolsterArt7" };
	UInt32 CurrentEquippedArmor[3][3];
	UInt32 LastEquippedArmor[3][3];
	UInt32 lastInHolster[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	std::vector<std::string> c_WeaponBlacklist[8]{};
	std::vector<std::string> c_WeaponBlacklistPA[8]{};
	bool testclonefix = false;
	std::vector<BSTriShape*> c_OriginalNodes{};
	std::vector<BSTriShape*> c_CloneNodes{};
	std::vector<std::string> c_ExcludedMagazines{};
	std::vector<std::string> c_SwordDrawSFX{};
	char* weaponRemovalStrings[5]{ "Mag", "ullet", "Cartridge", "Casing", "Object043:0" };
	char* meleeRemovalStrings[2]{ "Blood", "blood" };
	std::vector<char*> c_MeleeRemovalStrings{};
	bool _isHolsterButtonPressed;
	bool _isConfigModeLShiftButtonPressed;
	bool _isConfigModeRShiftButtonPressed;
	bool _isConfigModeSaveButtonPressed;
	bool _isConfigButtonPressed;
	bool _isInfoButtonPressed;
	bool _isChangeConfigModeButtonPressed;
	bool _isChangeHolsterButtonPressed;
	bool _isSelfieModeButtonButtonPressed;
	bool _isMovecamButtonPressed;
	bool _isEquipButtonPressed;
	bool _isArtWorkButtonPressed;
	bool _WeaponHolsterInProgress;
	bool _isPipboyOpen;
	bool _selfieModeActive;
	bool _camMovedBack;
	bool _fistRun = true;
	bool gamesaveloaded = false;
	bool artSwitched = false;
	bool isInMasterMode = false;
	float rAxisOffsetX;
	float rAxisOffsetY;
	float lAxisOffsetX;
	float lAxisOffsetY;
	float RXOffset;
	float RYOffset;
	float RZOffset;
	bool equipsticky = false;
	bool fistssticky = false;
	bool isHolsterIntersected = false;
	bool _isConfigModeActive = false;
	bool initBoneTreeFlag = true;

	// Main Functions

	bool initBoneTree() {
		BSFlattenedBoneTree* rt = (BSFlattenedBoneTree*)(*g_player)->unkF0->rootNode->m_children.m_data[0]->GetAsNiNode();
		if (!rt) {
			return false;
		}
		boneTreeMap.clear();
		boneTreeVec.clear();
		_MESSAGE("BoneTree Init -> Num Transforms = %d", rt->numTransforms);
		if (rt->numTransforms <= 0) {
			return false;
		}
		for (auto i = 0; i < rt->numTransforms; i++) {
			//_MESSAGE("BoneTree Init -> Push %s into position %d", rt->transforms[i].name.c_str(), i);
			boneTreeMap.insert({ rt->transforms[i].name.c_str(), i });
			boneTreeVec.push_back(rt->transforms[i].name.c_str());
		}
		_MESSAGE("BoneTree Init Completed");
		return true;
	}

	void MainLoop() {
		if (*g_ui) {  // Fix for if player dies and then loads a save with a missing .esp (player is kicked to main menu and mod needs to start like fresh boot).
			BSFixedString menuName("MainMenu");
			if (((*g_ui)->IsMenuOpen(menuName)) && (!_fistRun)) {
				_fistRun = true;
				_MESSAGE("Main Menu Detected - RESETTING TO FIRST RUN");
			}
		}
		if ((*g_player)->unkF0 && (*g_player)->unkF0->rootNode && (*g_player)->firstPersonSkeleton != nullptr) {
			if (initBoneTreeFlag) {
				if (!initBoneTree()) {
					return;
				}
				initBoneTreeFlag = false;
				PlayerLoaded();
			}
			detectHolsterSphere();
			VRButtonsMain();
			closeMenus();
			handlePowerArmor();
			playerIsSneaking();
			if (_isConfigModeActive) {
				updateSneakUI();
				updatePAUI();
				updateUIInfo();
				updateUITiles();
			}
			if (!gPlayerisSneaking && gInPowerArmor) {
				holsterMode = 1;
			}
			else if (gPlayerisSneaking && !gInPowerArmor) {
				holsterMode = 2;
			}
			else if (gPlayerisSneaking && gInPowerArmor) {
				holsterMode = 3;
			}
			else {
				holsterMode = 0;
			}
			gInPowerArmor ? weaponMode = 1 : weaponMode = 0;
			checkArmor(); //checks for any changes to armor and repositions weapons if needed
			checkfists();
			if (isVRHMenuOpen() && !menuOpenSticky) {
				menuOpenSticky = true;
			}
			else if (!isVRHMenuOpen() && menuOpenSticky) {
				menuOpenSticky = false;
				for (int i = 1; i < 8; i++) {
					WeapCheck(i);
				}
			}
			BSFixedString pipboyMenu("PipboyMenu");
			IMenu* menu = (*g_ui)->GetMenu(pipboyMenu);
			if (menu) {
				_isPipboyOpen = true;
			}
			else {
				_isPipboyOpen = false;
			}
			//int SitState = Offsets::GetSitState((*g_gameVM)->m_virtualMachine, 0, *g_player);
			//g_messaging->Dispatch(g_pluginHandle, 17, nullptr, 0, "VirtualReloads");
		}
	}

	void checkArmor() {
		int ArmorIndex[3][3]{ {3, 6 ,11 }, {3, 9 ,14 }, { 3, 10 ,15 } };
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				TESForm* equippedItem = (*g_player)->equipData->slots[ArmorIndex[i][j]].item;
				if (equippedItem) {
					CurrentEquippedArmor[i][j] = equippedItem->formID;
				}
				else {
					CurrentEquippedArmor[i][j] = 0;
				}
			}
			if ((CurrentEquippedArmor[i][0] != LastEquippedArmor[i][0]) || (CurrentEquippedArmor[i][1] != LastEquippedArmor[i][1]) || (CurrentEquippedArmor[i][2] != LastEquippedArmor[i][2])) {
				for (int j = 0; j < 3; j++) {
					LastEquippedArmor[i][j] = CurrentEquippedArmor[i][j];
				}
				if (i == 0) {
					int holNum[5] = { 1, 2, 5, 6, 7 };
					for (int i = 0; i < 5; i++) {
						moveWeapon(holNum[i]);
					}
				}
				else if (i == 1) {
					moveWeapon(3);
				}
				else {
					moveWeapon(4);
				}
			}
		}
	}

	void checkfists() {
		bool isWeaponDrawn = (*g_player)->actorState.IsWeaponDrawn();
		if (isWeaponDrawn && !equipsticky) {
			equipsticky = true;
			TESObjectWEAP* currentWeap = Offsets::GetEquippedWeapon((*g_gameVM)->m_virtualMachine, 0, *g_player, 0);
			if (!currentWeap && !fistssticky) {
				fistssticky = true;
				setFingerPositionScalar(true, 0.0, 0.0, 0.0, 0.0, 0.0);
			}
		}
		else if (!isWeaponDrawn && equipsticky) {
			equipsticky = false;
			if (fistssticky) {
				fistssticky = false;
				//_MESSAGE("Fists UnEquipped");
				restoreFingerPoseControl(true);
			}
		}
	}

	void handlePowerArmor() {
		if (detectInPowerArmor() && !gInPowerArmor) {
			std::string itemName;
			int slot[3] = { 11, 14, 15 };
			for (int i = 0; i < 3; i++) {
				int ii = slot[i];
				TESForm* equippedItem = (*g_player)->equipData->slots[ii].item;
				if (equippedItem) {
					itemName = equippedItem->GetFullName();
				}
				else {
					itemName = "None";
				}
			}
			gInPowerArmor = true;
			weaponsPAChange();
		}
		else if (!detectInPowerArmor() && gInPowerArmor) {
			gInPowerArmor = false;
			weaponsPAChange();
		}
	}

	bool playerIsSneaking() {
		bool isSneaking = IsSneaking(*g_player);
		if (isSneaking && !gPlayerisSneaking) {
			gPlayerisSneaking = true;
			sphereMoveSneak();
			return playerIsSneaking;
		}
		else if (!isSneaking && gPlayerisSneaking) {
			gPlayerisSneaking = false;
			sphereMoveStand();
			return playerIsSneaking;
		}
		else {
			return playerIsSneaking;
		}
	}

	void weaponsPAChange() {
		destroyAllHolsterSpheres();
		bool t = initBoneTree();
		for (int i = 1; i < 8; i++) {
			nDestoryWeapon(i, true, false);	
		}
		GameLoadInvSort();
		updateSpheresForPAEvent();
	}

	void VRButtonsMain() {
		if (VRHook::g_vrHook != nullptr) {
			VRHook::g_vrHook->setVRControllerState();
			// Gameplay Controls 
			if (!_isConfigModeActive) {
				uint64_t dominantHand = (gIsRightHanded ? VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Right).ulButtonPressed : VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Left).ulButtonPressed);
				const auto HolsterWButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_holsterButtonID);
				const auto ConfigButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configButtonID);
				const auto EquipButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_equipButtonID);
				if (!EquipButtonPressed && _isEquipButtonPressed && !_isPipboyOpen) {
					_isEquipButtonPressed = false;
					if ((*g_player)->actorState.IsWeaponDrawn()) {
						TESObjectWEAP* currentWeap = Offsets::GetEquippedWeapon((*g_gameVM)->m_virtualMachine, 0, *g_player, 0);
						if (!currentWeap) {
							if (fistsPressed) {
								Offsets::DrawWeaponMagicHands(*g_player, false);
								fistsPressed = false;
							}
						}
					}
				}
				else if (EquipButtonPressed && !_isEquipButtonPressed && !_isPipboyOpen) {
					_isEquipButtonPressed = true;
					std::thread t3(FistsTimer, 500);
					t3.detach();
					fistsPressed = true;
				}

				if (HolsterWButtonPressed && !_isHolsterButtonPressed && isHolsterIntersected && !isGameStopped() && !_WeaponHolsterInProgress) {
					_isHolsterButtonPressed = true;
					WeapSort(gCurHolster);
					if (c_ActivationDisabled) {
						MenuCheck = true;
						std::thread t2(CheckMenusTimer);
						t2.detach();
					}
				}
				else if (!HolsterWButtonPressed) {
					_isHolsterButtonPressed = false;
				}
				if (ConfigButtonPressed && !_isConfigButtonPressed && isHolsterIntersected && !(*g_player)->actorState.IsWeaponDrawn()) {
					_isConfigButtonPressed = true;
					BSFixedString pipboyMenu("PipboyMenu");
					IMenu* menu = (*g_ui)->GetMenu(pipboyMenu);
					if (!menu) {
						setupConfigMode();
						_isConfigModeActive = true;
					}
				}
				else if (!ConfigButtonPressed) {
					_isConfigButtonPressed = false;
				}

				if (ConfigButtonPressed && !_isInfoButtonPressed && _isPipboyOpen) {
					_isInfoButtonPressed = true;
					std::thread t4(infoTimer, 350);
					t4.detach();
					infoPressed = true;
					if (doublepress < 2) {
						doublepress++;
					}
				}
				else if (!ConfigButtonPressed && _isInfoButtonPressed) {
					_isInfoButtonPressed = false;
					if (infoPressed && doublepress == 2) {
						infoPressed = false;
						doublepress = 0;
						std::string mes = ("<br>Left Shoulder: " + holsteredWeapNames[1] + "</br>" + "<br>Right Shoulder: " + holsteredWeapNames[2] + "</br>" + "<br>Left Hip: " + holsteredWeapNames[3] + "</br>" + "<br>Right Hip: " + holsteredWeapNames[4] + "</br>" + "<br>Lower Back: " + holsteredWeapNames[5] + "</br>" + "<br>Left Chest: " + holsteredWeapNames[6] + "</br>"+ "<br>Right Chest: " + holsteredWeapNames[7] + "</br>");
						ShowMessagebox(mes.c_str());
						BSFixedString menuName("FavoritesMenu");
						if ((*g_ui)->IsMenuOpen(menuName)) {
							if ((*g_ui)->IsMenuRegistered(menuName)) {
								CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Close);
							}
						}
					}
					
				}






			}
			// ConfigMode Controls
			if (_isConfigModeActive) {
				uint64_t dominantHand = (gIsRightHanded ? VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Right).ulButtonPressed : VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Left).ulButtonPressed);
				uint64_t offHand = (gIsRightHanded ? VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Left).ulButtonPressed : VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Right).ulButtonPressed);
				vr::VRControllerAxis_t doinantHandStick = (gIsRightHanded ? VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Right).rAxis[0] : VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Left).rAxis[0]);
				vr::VRControllerAxis_t offHandStick = (gIsRightHanded ? VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Left).rAxis[0] : VRHook::g_vrHook->getControllerState(VRHook::VRSystem::TrackerType::Right).rAxis[0]);
				const auto ConfigExitLButtonPressed = offHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configExitButtonID);
				const auto ConfigExitRButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configExitButtonID);
				const auto ConfigShiftLButtonPressed = offHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configModeShiftButton);
				const auto ConfigShiftRButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configModeRotateButton);
				//const auto ConfigSaveButtonPressed = offHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configModeSaveButton);
				//const auto ChangeConfigModeButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configChangeModeButton);
				const auto ChangeHolsterButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_configChangeHolButton);
				const auto SelfieButtonPressed = dominantHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_selfieModeButtonID);
				const auto CamButtonPressed = offHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_movecamButtonID);
				const auto ArtWorkButtonPressed = offHand & vr::ButtonMaskFromId((vr::EVRButtonId)c_ArtWorkButtonID);
				if (SelfieButtonPressed && !_isSelfieModeButtonButtonPressed && _isConfigModeRShiftButtonPressed) {
					_isSelfieModeButtonButtonPressed = true;
					if (!_selfieModeActive) {
						_selfieModeActive = true;
					}
					else {
						_selfieModeActive = false;
					}
					SInt32 evt = HolsterEvent_Selfie;
					if (g_HolsterEventRegs.m_data.size() > 0) {
						g_HolsterEventRegs.ForEach(
							[&evt](const EventRegistration<NullParameters>& reg) {
								SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
							}
						);
					}
				}
				else if (!SelfieButtonPressed) {
					_isSelfieModeButtonButtonPressed = false;
				}
				if (CamButtonPressed && !_isMovecamButtonPressed) {
					_isMovecamButtonPressed = true;
					if (!ConfigShiftLButtonPressed) {
						std::string a = std::to_string(_isTouchEnabled);
						_isTouchEnabled ? _isTouchEnabled = false : _isTouchEnabled = true;

					}
					else {
						if (!_camMovedBack) {
							_camMovedBack = true;
							SInt32 evt;
							if (gInPowerArmor) {
								evt = HolsterEvent_CamBack;
								dist = camDistancePA / 10;
							}
							else {
								evt = HolsterEvent_CamForward;
								dist = camDistance / 10;
							}
							if (g_HolsterEventRegs.m_data.size() > 0) {
								g_HolsterEventRegs.ForEach(
									[&evt](const EventRegistration<NullParameters>& reg) {
										for (int i = 0; i < dist; i++) {
											SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
										}
									}
								);
							}
						}
						else {
							_isMovecamButtonPressed = true;
							_camMovedBack = false;
							SInt32 evt;
							if (gInPowerArmor) {
								evt = HolsterEvent_CamForward;
								dist = camDistancePA / 10;
							}
							else {
								evt = HolsterEvent_CamBack;
								dist = camDistance / 10;
							}
							if (g_HolsterEventRegs.m_data.size() > 0) {
								g_HolsterEventRegs.ForEach(
									[&evt](const EventRegistration<NullParameters>& reg) {
										for (int i = 0; i < dist; i++) {
											SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
										}
									}
								);
							}
						}
					}
				}
				else if (!CamButtonPressed) {
					_isMovecamButtonPressed = false;
				}
				if (ArtWorkButtonPressed && !_isArtWorkButtonPressed && !_isConfigModeLShiftButtonPressed) {
					_isArtWorkButtonPressed = true;
					pressArtWorkChange(gCurHolster);
				}
				else if (!ArtWorkButtonPressed) {
					_isArtWorkButtonPressed = false;
				}
				if (ArtWorkButtonPressed && !_isArtWorkButtonPressed && _isConfigModeLShiftButtonPressed) {
					_isArtWorkButtonPressed = true;
					PressChangeAllArtWork();
				}
				if (ConfigShiftLButtonPressed && !_isConfigModeLShiftButtonPressed) {
					_isConfigModeLShiftButtonPressed = true;
				}
				else if (!ConfigShiftLButtonPressed) {
					_isConfigModeLShiftButtonPressed = false;
				}
				if (ConfigShiftRButtonPressed && !_isConfigModeRShiftButtonPressed) {
					_isConfigModeRShiftButtonPressed = true;
				}
				else if (!ConfigShiftRButtonPressed) {
					_isConfigModeRShiftButtonPressed = false;
				}
				//if (ChangeConfigModeButtonPressed && !_isChangeConfigModeButtonPressed && !_isConfigModeRShiftButtonPressed) {
					//_isChangeConfigModeButtonPressed = true;
				//}
				//else if (!ChangeConfigModeButtonPressed) {
					//_isChangeConfigModeButtonPressed = false;
				//}
				if (ChangeHolsterButtonPressed && !_isChangeHolsterButtonPressed && !_isConfigModeRShiftButtonPressed) {
					_isChangeHolsterButtonPressed = true;
					switchHolster();
				}
				else if (!ChangeHolsterButtonPressed) {
					_isChangeHolsterButtonPressed = false;
				}
				//if (ConfigSaveButtonPressed && !_isConfigModeSaveButtonPressed && !_isConfigModeLShiftButtonPressed) {
					//_isConfigModeSaveButtonPressed = true;
					//saveconfig();
				//}
				//else if (!ConfigSaveButtonPressed) {
					//_isConfigModeSaveButtonPressed = false;
				//}
				if (doinantHandStick.y > 0.50 || doinantHandStick.y < -0.50) {
					if (!_isConfigModeLShiftButtonPressed) {
						rAxisOffsetY = doinantHandStick.y / 10;
						if (!_isConfigModeRShiftButtonPressed) {
							moveholsterY();
						}
						else {
							if (c_mode == 2) {
								rotateholsterY();
							}
						}
					}
				}
				if (doinantHandStick.x > 0.50 || doinantHandStick.x < -0.50) {
					if (!_isConfigModeLShiftButtonPressed) {
						rAxisOffsetX = doinantHandStick.x / 10;
						if (!_isConfigModeRShiftButtonPressed) {
							moveholsterX();
						}
						else {
							if (c_mode == 2) {
								rotateholsterX();
							}
						}
					}
				}
				if (offHandStick.x > 0.50 || offHandStick.x < -0.50) {
					if (!_isConfigModeLShiftButtonPressed) {
						lAxisOffsetX = offHandStick.x / 10;
						if (!_isConfigModeRShiftButtonPressed) {
							moveholsterZ();
						}
						else {
							if (c_mode == 2) {
								rotateholsterZ();
							}
						}
					}
				}
				if (offHandStick.y > 0.50 || offHandStick.y < -0.50) {
					if (_isConfigModeLShiftButtonPressed) {
						lAxisOffsetY = offHandStick.y / 10;
						scaleHolster();
					}
				}
				if (ConfigExitLButtonPressed || ConfigExitRButtonPressed) {
					exitConfigMode();
					_isConfigModeActive = false;
				}
			}
		}
		else {
			VRHook::InitVRSystem();
		}
	}

	void mainStartup() {
		_MESSAGE("Starting up Virtual Holsters");
		nextHolsterHandle = 1;
		nextWeaponHandle = 1;
		vrHook = RequestOpenVRHookManagerObject();
		if (VRHook::g_vrHook != nullptr) {
			VRHook::g_vrHook->setVRControllerState();
		}
		else {
			VRHook::InitVRSystem();
			VRHook::g_vrHook->setVRControllerState();
		}
		loadConfigFiles();
		_fistRun = true;
	}

	void saveGameLoaded() {
		initBoneTreeFlag = true;
	}

	void PlayerLoaded() {
		_MESSAGE("Game Event:: Game Loaded");
		isHolsterIntersected = false;
		createAllHolsterSpheres();
		_MESSAGE("---- Holster Spheres Recreated");
		if (!_fistRun) {
			_MESSAGE("---- Destorying All Weapons....");
			for (int i = 1; i < 8; i++) {
				nDestoryWeapon(i, true, false);
			}
			_MESSAGE("---- All Weapons Destoryed");
			saveloaded = true;
		}
		else {
			Setting* set = GetINISetting("fPrimaryWandMaxGrabDistance:VRWand");
			pGrabDistance = set->data.f32;
			set = GetINISetting("fSecondaryWandMaxGrabDistance:VRWand");
			sGrabDistance = set->data.f32;
			set = GetINISetting("fDownwardGrabFloorDistanceMultiplier:VRWand");
			grabMulti = set->data.f32;
			_fistRun = false;
			saveloaded = true;
		}
		_MESSAGE("---- Holster Container Links:: Setup Start");
		UInt32 HolsterID[8] = { 0x001022, 0x001022, 0x001021, 0x001020, 0x00101F, 0x001023, 0x001024, 0x006B86 };
		//UInt32 HolsterID[8] = { 0x01C1FC, 0x01C1FC, 0x01C1FD, 0x01C1F8, 0x01C1FB, 0x01C1F9, 0x01C1FA, 0x01C1FE };
		for (int i = 1; i < 8; i++) {
			TESObjectREFR* ref = DYNAMIC_CAST(GetFormFromFile(HolsterID[i], "VirtualHolsters.esp"), TESForm, TESObjectREFR);
			if (ref) {
				std::string mes = "-------- Holster0" + std::to_string(i) + " Linked to Container0" + std::to_string(i);
				HolsterContainers[i] = ref;
				_MESSAGE(mes.c_str());
			}
			else {
				std::string mes = "-------- Error: Unable to find Container0" + std::to_string(i);
				_MESSAGE(mes.c_str());
			}
		}
		_MESSAGE("---- Holster Container Links:: Setup Complete");
		_MESSAGE("---- Holster Config:: Update");
		//for (int i = 1; i < 8; i++) {
			GameLoadInvSort();
		//}
		_MESSAGE("---- Holster Config:: Update Completed");
		_MESSAGE("Game Event:: Game Load Completed");
	}

	bool loadConfigFiles() {
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");

		if (rc < 0) {
			_MESSAGE("ERROR: cannot read VirtualHolsters.ini");
			return false;
		}
		c_holsterButtonID = (int)ini.GetLongValue("Button Config", "HolsterButtonID", vr::EVRButtonId::k_EButton_A);
		c_configButtonID = (int)ini.GetLongValue("Button Config", "ConfigModeButtonID", vr::EVRButtonId::k_EButton_SteamVR_Touchpad);
		c_configModeShiftButton = (int)ini.GetLongValue("Button Config", "ConfigModeShiftButtonID", vr::EVRButtonId::k_EButton_Grip);
		c_configModeRotateButton = (int)ini.GetLongValue("Button Config", "ConfigRotateButtonID", vr::EVRButtonId::k_EButton_A);
		//c_configModeSaveButton = (int)ini.GetLongValue("Button Config", "SaveProfileButtonID", vr::EVRButtonId::k_EButton_A);
		//c_configChangeModeButton = (int)ini.GetLongValue("Button Config", "ModeSwitchButtonID", vr::EVRButtonId::k_EButton_ApplicationMenu);
		c_configChangeHolButton = (int)ini.GetLongValue("Button Config", "CycleHolstersButtonID", vr::EVRButtonId::k_EButton_Grip);
		c_selfieModeButtonID = (int)ini.GetLongValue("Button Config", "ToggleSelfieModeButtonID", vr::EVRButtonId::k_EButton_SteamVR_Touchpad);
		gshowSpheres = ini.GetBoolValue("Main Options", "DisplaySpheres", true);
		c_ActivationDisabled = ini.GetBoolValue("Main Options", "bDisableActivation", false);
		c_VATSDisabled = ini.GetBoolValue("Main Options", "bDisableVATS", false);
		ghapticsEnabled = ini.GetBoolValue("Main Options", "EnableHaptics", true);
		gShowWeapons = ini.GetBoolValue("Main Options", "DisplayWeapons", true);
		gIsRightHanded = ini.GetBoolValue("Main Options", "isRightHanded", true);
		testclonefix = ini.GetBoolValue("Main Options", "bTestCloneFix", false);
		camDistance = (int)ini.GetLongValue("Config Mode", "CameraMoveBackDist", 120);
		camDistancePA = (int)ini.GetLongValue("Config Mode", "CameraMoveBackDistPA", 240);
		RXOffset = (float)ini.GetDoubleValue("Config Mode", "RotateXMultiplier", 0.0);
		RYOffset = (float)ini.GetDoubleValue("Config Mode", "RotateYMultiplier", 0.0);
		RZOffset = (float)ini.GetDoubleValue("Config Mode", "RotateZMultiplier", 0.0);
		//grabDistance = (double*)ini.GetDoubleValue("Main Options", "fPrimaryWandMaxGrabDistance", 96.0);
		isInMasterMode = ini.GetBoolValue("Config Mode", "MasterMode", false);
		holsterArtWorkPresets[0] = 0;
		holsterArtWorkPresets[1] = (int)ini.GetLongValue("Config Mode", "Holster01CustomArtSlot", 0);
		holsterArtWorkPresets[2] = (int)ini.GetLongValue("Config Mode", "Holster02CustomArtSlot", 0);
		holsterArtWorkPresets[3] = (int)ini.GetLongValue("Config Mode", "Holster03CustomArtSlot", 0);
		holsterArtWorkPresets[4] = (int)ini.GetLongValue("Config Mode", "Holster04CustomArtSlot", 0);
		holsterArtWorkPresets[5] = (int)ini.GetLongValue("Config Mode", "Holster05CustomArtSlot", 0);
		holsterArtWorkPresets[6] = (int)ini.GetLongValue("Config Mode", "Holster06CustomArtSlot", 0);
		holsterArtWorkPresets[7] = (int)ini.GetLongValue("Config Mode", "Holster07CustomArtSlot", 0);
		PAholsterArtWorkPresets[0] = 0;
		PAholsterArtWorkPresets[1] = (int)ini.GetLongValue("Config Mode", "PAHolster01CustomArtSlot", 0);
		PAholsterArtWorkPresets[2] = (int)ini.GetLongValue("Config Mode", "PAHolster02CustomArtSlot", 0);
		PAholsterArtWorkPresets[3] = (int)ini.GetLongValue("Config Mode", "PAHolster03CustomArtSlot", 0);
		PAholsterArtWorkPresets[4] = (int)ini.GetLongValue("Config Mode", "PAHolster04CustomArtSlot", 0);
		PAholsterArtWorkPresets[5] = (int)ini.GetLongValue("Config Mode", "PAHolster05CustomArtSlot", 0);
		PAholsterArtWorkPresets[6] = (int)ini.GetLongValue("Config Mode", "PAHolster06CustomArtSlot", 0);
		PAholsterArtWorkPresets[7] = (int)ini.GetLongValue("Config Mode", "PAHolster07CustomArtSlot", 0);
		readOffsetJson();
		std::string fileName[8] = { "","LeftShoulder.ini", "RightShoulder.ini", "LeftHip.ini", "RightHip.ini", "LowerBack.ini", "LeftChest.ini", "RightChest.ini", };
		std::string fileName2[8] = { "","LeftShoulderPA.ini", "RightShoulderPA.ini", "LeftHiPA.ini", "RightHipPA.ini", "LowerBackPA.ini", "LeftChestPA.ini", "RightChestPA.ini", };
		for (int i = 1; i < 8; i++) {
			const std::string path = { ".\\Data\\F4SE\\plugins\\VRH_WeaponBlackList\\" + fileName[i] };
			std::string str;
			std::fstream file;
			file.open(path, std::ios::in);
			while (getline(file, str))
			{
				c_WeaponBlacklist[i].push_back(str);
			}
		}
		for (int i = 1; i < 8; i++) {
			const std::string path = { ".\\Data\\F4SE\\plugins\\VRH_WeaponBlackList\\" + fileName2[i] };
			//_MESSAGE(path.c_str());
			std::string str;
			std::fstream file;
			file.open(path, std::ios::in);
			while (getline(file, str))
			{
				c_WeaponBlacklistPA[i].push_back(str);
			}

		}
		const std::string path = { ".\\Data\\F4SE\\plugins\\VRH_MagazineBlackList\\MagazineBlackList.ini" };
		//_MESSAGE(path.c_str());
		std::string str;
		std::fstream file;
		file.open(path, std::ios::in);
		while (getline(file, str))
		{
			c_ExcludedMagazines.push_back(str);
		}
		const std::string path2 = { ".\\Data\\F4SE\\plugins\\VRH_weapon_offsets\\SwordDrawSFXList.ini" };
		//_MESSAGE(path.c_str());
		std::string str2;
		std::fstream file2;
		file2.open(path2, std::ios::in);
		while (getline(file2, str2))
		{
			c_SwordDrawSFX.push_back(str2);
		}
		return true;
	}

	// Config Mode Functions

	void ActivationToggle() {
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetBoolValue("Main Options", "bDisableActivation", c_ActivationDisabled);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void ChangeVATS() {
		if (c_VATSDisabled) {
			SInt32 evt2 = HolsterEvent_EnableVATS;
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt2](const EventRegistration<NullParameters>& reg) {
						SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt2, gCurHolster);
					}
				);
			}
			c_VATSDisabled = false;
			if (c_holsterButtonID == 1) {
				c_holsterButtonID = 7;
			}
		}
		else {
			SInt32 evt2 = HolsterEvent_DisableVATS;
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt2](const EventRegistration<NullParameters>& reg) {
						SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt2, gCurHolster);
					}
				);
			}
			c_VATSDisabled = true;
		}
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetBoolValue("Main Options", "bDisableVATS", c_VATSDisabled);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void swapHolsterButton() {
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetLongValue("Button Config", "HolsterButtonID", c_holsterButtonID);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void configmodeupdate(int mode) {
		c_mode = mode;
	}

	void closeSelfieMode() {
		if (_selfieModeActive) {
			SInt32 evt = HolsterEvent_Selfie;
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt](const EventRegistration<NullParameters>& reg) {
						SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
					}
				);
			}
			_selfieModeActive = false;
		}
	}

	void retorecam() {
		if (_camMovedBack) {
			SInt32 evt;
			if (gInPowerArmor) {
				evt = HolsterEvent_CamForward;
				dist = camDistancePA / 10;
			}
			else {
				evt = HolsterEvent_CamBack;
				dist = camDistance / 10;
			}
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt](const EventRegistration<NullParameters>& reg) {
						for (int i = 0; i < dist; i++) {
							SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
						}
					}
				);
			}
			_camMovedBack = false;
		}
	}

	void toggleHaptics() {
		ghapticsEnabled ? ghapticsEnabled = false : ghapticsEnabled = true;
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetBoolValue("Main Options", "EnableHaptics", ghapticsEnabled);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void toggleWeapVis() {
		if (gShowWeapons) {
			//destoryAllWeapons();
			for (int i = 1; i < 8; i++) {
				nDestoryWeapon(i, true, false);
			}
			gShowWeapons = false;
		}
		else {
			gShowWeapons = true;
			//for (int i = 1; i < 8; i++) {
			GameLoadInvSort();
			//}
			/*SInt32 evt = HolsterEvent_Recreate;
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt](const EventRegistration<NullParameters>& reg) {
						SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
					}
				);
			}*/


		}
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetBoolValue("Main Options", "DisplayWeapons", gShowWeapons);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void toggleSphereVis() {
		gshowSpheres ? gshowSpheres = false : gshowSpheres = true;
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetBoolValue("Main Options", "DisplaySpheres", gshowSpheres);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void toggleSwitchRGA() {
		if (c_configModeRotateButton == vr::EVRButtonId::k_EButton_A) {
			c_configModeRotateButton = vr::EVRButtonId::k_EButton_Grip;
		}
		else if (c_configModeRotateButton == vr::EVRButtonId::k_EButton_Grip) {
			c_configModeRotateButton = vr::EVRButtonId::k_EButton_A;
		}
		else {
			_MESSAGE("IRREGULAR BUTTON CONFIG NO CHANGES MADE");
		}
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetLongValue("Button Config", "ConfigRotateButtonID", c_configModeRotateButton);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void toggleSwitchLGA() {
		if (c_configModeShiftButton == vr::EVRButtonId::k_EButton_Grip) {
			c_configModeShiftButton = vr::EVRButtonId::k_EButton_A;

		}
		else if (c_configModeShiftButton == vr::EVRButtonId::k_EButton_A) {
			c_configModeShiftButton = vr::EVRButtonId::k_EButton_Grip;
		}
		else {
			_MESSAGE("IRREGULAR BUTTON CONFIG NO CHANGES MADE");
		}
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetLongValue("Button Config", "ConfigModeShiftButtonID", c_configModeShiftButton);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void toggleSouthPaw() {
		gIsRightHanded ? gIsRightHanded = false : gIsRightHanded = true;
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		rc = ini.SetBoolValue("Main Options", "isRightHanded", gIsRightHanded);
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void PressChangeAllArtWork() {
		//_MESSAGE("UI: CHANGE ALL ARTWORK");
		int x = gInPowerArmor ? PAholsterArtWorkPresets[gCurHolster] : holsterArtWorkPresets[gCurHolster];
		int mx = gInPowerArmor ? PAholsterMaxValue[gCurHolster] : holsterMaxValue[gCurHolster];
		std::string y = (std::to_string(gCurHolster) + " " + std::to_string(x));
		//_MESSAGE(y.c_str());
		if (x == 0) {
			x = mx;
		}
		else {
			x = x - 1;
		}
		for (int i = 1; i < 8; i++) {
			if (i != gCurHolster) {
				gInPowerArmor ? PAholsterArtWorkPresets[i] = x : holsterArtWorkPresets[i] = x;
				std::string p = std::to_string(i) + " " + std::to_string(x);
				//_MESSAGE(p.c_str());
				artSwitched = false;
				pressArtWorkChange(i);
				if (artSwitched == false) {
					gInPowerArmor ? PAholsterArtWorkPresets[i] = x + 1 : holsterArtWorkPresets[i] = x + 1;
					CSimpleIniA ini;
					SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
					std::string slot;
					gInPowerArmor ? slot = "PAHolster0" + std::to_string(i) + "CustomArtSlot" : slot = "Holster0" + std::to_string(i) + "CustomArtSlot";
					//_MESSAGE(slot.c_str());
					gInPowerArmor ? rc = ini.SetLongValue("Config Mode", slot.c_str(), PAholsterArtWorkPresets[i]) : rc = ini.SetLongValue("Config Mode", slot.c_str(), holsterArtWorkPresets[i]);
					rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
				}
			}
		}
	}

	void pressArtWorkChange(int holster) {
		std::string artName = "HolsterArt" + std::to_string(gCurHolster);
		NiNode* artToChange = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
		if (artToChange) {
			//_MESSAGE("UI: CHANGING ART");
			NiNode* bone = nullptr;
			gInPowerArmor ? bone = FindNode(PAWeaponBones[holster].c_str()) : bone = FindNode(WeaponBones[holster].c_str());
			std::vector<std::string> listOfFiles;
			std::string currentWeapon = VisualArt[holster];
			std::string searchpath;
			//_MESSAGE("UI: SEARCHING FOR ART");
			gInPowerArmor ? searchpath = "Data\\Meshes\\VRH\\Holsters\\_PA\\" + currentWeapon + "\\HolsterArt" + std::to_string(holster) + "*.nif" : searchpath = "Data\\Meshes\\VRH\\Holsters\\" + currentWeapon + "\\HolsterArt" + std::to_string(holster) + "*.nif";
			listOfFiles = getFileNames(searchpath);
			if (listOfFiles.size() == 0) {
				//_MESSAGE("UI: NO CUSTOM ART FOUND FOR THIS SLOT / WEAPON");
				return;
			}
			//_MESSAGE("UI: CUSTOM ART FOUND, SWITCHING TO NEXT ART");
			gInPowerArmor ? PAholsterArtWorkPresets[holster] += 1 : holsterArtWorkPresets[holster] += 1;
			std::string finalpath;
			int x = gInPowerArmor ? PAholsterArtWorkPresets[holster] : holsterArtWorkPresets[holster];
			int xx = listOfFiles.size();
			gInPowerArmor ? PAholsterMaxValue[holster] = (xx - 1) : holsterMaxValue[holster] = (xx - 1);
			std::string yy;
			yy = "UI: NUMBER OF ART FOUND " + std::to_string(xx);
			//_MESSAGE(yy.c_str());
			std::string yyy = "UI: CURRENT SLOT NUMBER " + std::to_string(x);
			//_MESSAGE(yyy.c_str());
			if (x > xx) {
				return;
			}
			if (x == xx) {
				gInPowerArmor ? PAholsterArtWorkPresets[holster] = 0 : holsterArtWorkPresets[holster] = 0;
			}
			x = gInPowerArmor ? PAholsterArtWorkPresets[holster] : holsterArtWorkPresets[holster];
			std::string zzz = "UI: CURRENT SLOT NUMBER2 " + std::to_string(x);
			gInPowerArmor ? finalpath = "Data/Meshes/VRH/Holsters/_PA/" + currentWeapon + "/" + listOfFiles[x] : finalpath = "Data/Meshes/VRH/Holsters/" + currentWeapon + "/" + listOfFiles[x];
			//_MESSAGE(finalpath.c_str());
			if (!exists_file(finalpath)) {
				return;
			}
			char* hP;
			hP = new char[finalpath.size() + 1];
			std::copy(finalpath.begin(), finalpath.end(), hP);
			hP[finalpath.size()] = '\0';
			//gInPowerArmor ? _MESSAGE("UI MODE: APPLYING PA HOLSTER ART FOUND AT") : _MESSAGE("UI : APPLYING HOLSTER ART FOUND AT");
			//_MESSAGE(finalpath.c_str());
			NiNode* retNode = loadNifFromFile(hP);
			NiCloneProcess proc;
			proc.unk18 = Offsets::cloneAddr1;
			proc.unk48 = Offsets::cloneAddr2;
			NiNode* holArt = Offsets::cloneNode(retNode, &proc);
			holArt->m_name = artName.c_str();
			bone->AttachChild(holArt, true);
			holArt->m_localTransform = artToChange->m_localTransform;
			artToChange->m_localTransform.scale = 0;
			artToChange->m_parent->RemoveChild(artToChange);
			//_MESSAGE("UI: HOLSTER ART SUCCESSFULLY CHANGED");
			CSimpleIniA ini;
			SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
			std::string slot;
			gInPowerArmor ? slot = "PAHolster0" + std::to_string(holster) + "CustomArtSlot" : slot = "Holster0" + std::to_string(holster) + "CustomArtSlot";
			//_MESSAGE(slot.c_str());
			gInPowerArmor ? rc = ini.SetLongValue("Config Mode", slot.c_str(), PAholsterArtWorkPresets[holster]) : rc = ini.SetLongValue("Config Mode", slot.c_str(), holsterArtWorkPresets[holster]);
			rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
			artSwitched = true;
		}
	}

	void closeMenus() {
		BSFixedString menuName("FavoritesMenu");
		if (((*g_ui)->IsMenuOpen(menuName)) && (isHolsterIntersected)) {
			if ((*g_ui)->IsMenuRegistered(menuName)) {
				CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Close);
			}
		}
		BSFixedString menuName2("ScopeMenu");
		if ((*g_ui)->IsMenuOpen(menuName2)) {
			if ((*g_ui)->IsMenuRegistered(menuName2)) {
				//NiNode* sphere = FindNode("BlackSphere");
				//if (sphere) {
					//auto& pHashSet = (*g_ui)->menuTable;
					//auto* pItem = pHashSet.Find(&menuName2);
					//RemoveImagespaceModifier(g_Data.PipboyMenuImod);
					//sphere->m_localTransform.scale = 0.0;
					//pItem->menuInstance->flags &= ~IMenu::kFlag_BlurBackground;
					//pItem->menuInstance->flags &= ~IMenu::kFlag_ShaderdWorld;
				//}
			}
		}
		if (MenuCheck) {
			const char* Menus[5] = { "PowerArmorModMenu", "CookingMenu", "ExamineMenu", "ContainerMenu", "TerminalMenu" };
			for (int i = 0; i < 5; i++) {
				BSFixedString M = Menus[i];
				if ((*g_ui)->IsMenuOpen(M)) {
					if ((*g_ui)->IsMenuRegistered(M)) {
						CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(M, kMessage_Close);
						MenuCheck = false;
						i = 6;
						return;
					}
				}
			}
		}
	}

	void scaleHolster() {
		if (configMode == 1) {
			if (gCurHolster == 1 || gCurHolster == 2) {
				return;
			}
			if (HolsterRegisteredObjects[gCurHolster]) {
				NiNode* hol = HolsterRegisteredObjects[gCurHolster]->debugSphere;
				float nScale(lAxisOffsetY);
				float origScale = hol->m_localTransform.scale;
				if ((origScale + nScale) < 1) {
					//_MESSAGE("SCALE RETURN");
					return;
				}
				hol->m_localTransform.scale = (origScale + nScale);
				float radiusA = hol->m_localTransform.scale;
				radiusA = radiusA / 2;
				HolsterRegisteredObjects[gCurHolster]->radius = radiusA;
			}
		}
	}

	void moveholsterX() {
		NiNode* hol = nullptr;
		bool isMelee = WeaponRegisteredObjects[gCurHolster]->isMelee;
		if (configMode == 1) {
			if (gCurHolster == 1 || gCurHolster == 2) {
				return;
			}
			if (HolsterRegisteredObjects[gCurHolster]) {
				hol = HolsterRegisteredObjects[gCurHolster]->debugSphere;
			}
			else {
				return;
			}
			isMelee = false;
		}
		else {
			if (WeaponRegisteredObjects[gCurHolster]) {
				hol = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
			}
			else {
				return;
			}
		}
		if (hol) {
			float nLoc;
			float origLoc;
			if (gCurHolster == 1) {
				bool isInPA = detectInPowerArmor();
				isInPA ? nLoc = rAxisOffsetX : nLoc = rAxisOffsetX * -1;
				isInPA ? origLoc = hol->m_localTransform.pos.z : origLoc = hol->m_localTransform.pos.x;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				isInPA ? hol->m_localTransform.pos.z = (origLoc + nLoc) : hol->m_localTransform.pos.x = (origLoc + nLoc);
			}
			else if (gCurHolster == 2) {
				bool isInPA = detectInPowerArmor();
				isInPA ? nLoc = rAxisOffsetX : nLoc = rAxisOffsetX;
				isInPA ? origLoc = hol->m_localTransform.pos.z : origLoc = hol->m_localTransform.pos.x;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				isInPA ? hol->m_localTransform.pos.z = (origLoc + nLoc) : hol->m_localTransform.pos.x = (origLoc + nLoc);
			}

			else if (gCurHolster == 3) {
				nLoc = rAxisOffsetX;
				isMelee ? origLoc = hol->m_localTransform.pos.z : origLoc = hol->m_localTransform.pos.z;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				isMelee ? hol->m_localTransform.pos.z = (origLoc + nLoc) : hol->m_localTransform.pos.z = (origLoc + nLoc);
			}
			else if (gCurHolster == 4) {
				nLoc = rAxisOffsetX;
				//origLoc = hol->m_localTransform.pos.z;
				isMelee ? origLoc = hol->m_localTransform.pos.z : origLoc = hol->m_localTransform.pos.z;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				//hol->m_localTransform.pos.z = (origLoc + nLoc);
				isMelee ? hol->m_localTransform.pos.z = (origLoc + nLoc) : hol->m_localTransform.pos.z = (origLoc + nLoc);
			}

			else if (gCurHolster == 5) {
				nLoc = rAxisOffsetX * -1;
				origLoc = hol->m_localTransform.pos.z;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				hol->m_localTransform.pos.z = (origLoc + nLoc);
			}
			else if (gCurHolster == 6) {
				nLoc = rAxisOffsetX * -1;
				origLoc = hol->m_localTransform.pos.z;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				hol->m_localTransform.pos.z = (origLoc + nLoc);
			}
			else if (gCurHolster == 7) {
				nLoc = rAxisOffsetX * -1;
				origLoc = hol->m_localTransform.pos.z;
				if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
					return;
				}
				hol->m_localTransform.pos.z = (origLoc + nLoc);
			}
			if (configMode == 2) {
				std::string artName = "HolsterArt" + std::to_string(gCurHolster);
				NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode); //Remove any Holster Art applied this slot
				if (artToMove) {
					artToMove->m_localTransform.pos = hol->m_localTransform.pos;
				}
			}
		}
		if (configMode == 1) {
			HolsterRegisteredObjects[gCurHolster]->offset = hol->m_localTransform.pos;
		}
	}

	void moveholsterY() {
		NiNode* hol = nullptr;
		bool isMelee;
		if (configMode == 1) {
			if (gCurHolster == 1 || gCurHolster == 2) {
				return;
			}
			if (HolsterRegisteredObjects[gCurHolster]) {
				hol = HolsterRegisteredObjects[gCurHolster]->debugSphere;
			}
			else {
				return;
			}
			isMelee = false;
		}
		else {
			if (WeaponRegisteredObjects[gCurHolster]) {
				hol = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
				isMelee = WeaponRegisteredObjects[gCurHolster]->isMelee;
			}
			else {
				return;
			}
		}

		float nLoc(rAxisOffsetY);

		if (hol) {
			float origLoc;
			if (gCurHolster > 4 || gCurHolster < 4) {
				isMelee ? origLoc = hol->m_localTransform.pos.y : origLoc = hol->m_localTransform.pos.y;
			}
			else {
				origLoc = hol->m_localTransform.pos.y;
			}
			//origLoc = hol->m_localTransform.pos.x
			//origLoc = hol->m_localTransform.pos;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			isMelee ? hol->m_localTransform.pos.y = (origLoc + nLoc) : hol->m_localTransform.pos.y = (origLoc + nLoc);
			//hol->m_localTransform.pos = (origLoc + nLoc);
			if (configMode == 2) {
				std::string artName = "HolsterArt" + std::to_string(gCurHolster);
				
				NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
				if (artToMove) {
					artToMove->m_localTransform.pos = hol->m_localTransform.pos;
				}
			}
		}
		if (configMode == 1) {
			HolsterRegisteredObjects[gCurHolster]->offset = hol->m_localTransform.pos;
		}
	}

	void moveholsterZ() {
		NiNode* hol = nullptr;
		if (configMode == 1) {
			if (gCurHolster == 1 || gCurHolster == 2) {
				return;
			}
			if (HolsterRegisteredObjects[gCurHolster]) {
				hol = HolsterRegisteredObjects[gCurHolster]->debugSphere;
			}
			else {
				return;
			}
		}
		else {
			if (WeaponRegisteredObjects[gCurHolster]) {
				hol = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
			}
			else {
				return;
			}
		}
		float nLoc;
		float origLoc;
		if (gCurHolster == 1) {
			bool isInPA = detectInPowerArmor();
			isInPA ? nLoc = lAxisOffsetX : nLoc = lAxisOffsetX;
			isInPA ? origLoc = hol->m_localTransform.pos.x : origLoc = hol->m_localTransform.pos.z;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			isInPA ? hol->m_localTransform.pos.x = (origLoc + nLoc) : hol->m_localTransform.pos.z = (origLoc + nLoc);
		}
		else if (gCurHolster == 2) {
			bool isInPA = detectInPowerArmor();
			isInPA ? nLoc = lAxisOffsetX : nLoc = lAxisOffsetX * -1;
			isInPA ? origLoc = hol->m_localTransform.pos.x : origLoc = hol->m_localTransform.pos.z;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			isInPA ? hol->m_localTransform.pos.x = (origLoc + nLoc) : hol->m_localTransform.pos.z = (origLoc + nLoc);
		}
		else if (gCurHolster == 3) {
			nLoc = lAxisOffsetX;
			origLoc = hol->m_localTransform.pos.x;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			hol->m_localTransform.pos.x = (origLoc + nLoc);
		}
		else if (gCurHolster == 4) {
			nLoc = lAxisOffsetX;
			origLoc = hol->m_localTransform.pos.x;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			hol->m_localTransform.pos.x = (origLoc + nLoc);
		}
		else if (gCurHolster == 5) {
			nLoc = lAxisOffsetX * -1;
			origLoc = hol->m_localTransform.pos.x;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			hol->m_localTransform.pos.x = (origLoc + nLoc);
		}
		else if (gCurHolster == 6) {
			nLoc = lAxisOffsetX * -1;
			origLoc = hol->m_localTransform.pos.x;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			hol->m_localTransform.pos.x = (origLoc + nLoc);
		}
		else if (gCurHolster == 7) {
			nLoc = lAxisOffsetX * -1;
			origLoc = hol->m_localTransform.pos.x;
			if ((origLoc + nLoc) > 50 || (origLoc + nLoc) < -50) {
				return;
			}
			hol->m_localTransform.pos.x = (origLoc + nLoc);
		}
		if (configMode == 2) {
			std::string artName = "HolsterArt" + std::to_string(gCurHolster);
			NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
			if (artToMove) {
				artToMove->m_localTransform.pos = hol->m_localTransform.pos;
			}
		}
		if (configMode == 1) {
			HolsterRegisteredObjects[gCurHolster]->offset = hol->m_localTransform.pos;
		}
	}

	void rotateholsterX() {
		NiNode* hol = nullptr;
		Matrix44 rot;
		bool MeleeWeapon = (WeaponRegisteredObjects[gCurHolster]->isMelee);
		float tAxisOffset;
		//_MESSAGE(std::to_string(RXOffset).c_str());
		if (RXOffset != 0.0) {
			tAxisOffset = rAxisOffsetX * RXOffset;
		}
		else {
			tAxisOffset = rAxisOffsetX;
		}
		MeleeWeapon ? rot.setEulerAngles((degrees_to_rads(tAxisOffset)), 0, 0) : rot.setEulerAngles(0, 0, (degrees_to_rads(tAxisOffset)));
		if (configMode == 2) {
			if (WeaponRegisteredObjects[gCurHolster]) {
				hol = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
				if (hol) {
					hol->m_localTransform.rot = rot.multiply43Left(hol->m_localTransform.rot);
					rot.multiply43Left(hol->m_localTransform.rot);
					std::string artName = "HolsterArt" + std::to_string(gCurHolster);
					
					NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
					if (artToMove) {
						artToMove->m_localTransform.rot = hol->m_localTransform.rot;
					}
				}
			}
			else {
				return;
			}
		}
	}

	void rotateholsterY() {
		Matrix44 rot;
		NiNode* hol = nullptr;
		bool MeleeWeapon = (WeaponRegisteredObjects[gCurHolster]->isMelee);
		float tAxisOffset;
		//_MESSAGE(std::to_string(RYOffset).c_str());
		if (RYOffset != 0.0) {
			tAxisOffset = rAxisOffsetY * RYOffset;
		}
		else {
			tAxisOffset = rAxisOffsetY;
		}
		MeleeWeapon ? rot.setEulerAngles(0, 0, (degrees_to_rads(tAxisOffset))) : rot.setEulerAngles((degrees_to_rads(tAxisOffset)), 0, 0);
		if (configMode == 2) {
			if (WeaponRegisteredObjects[gCurHolster]) {
				hol = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
				if (hol) {
					hol->m_localTransform.rot = rot.multiply43Left(hol->m_localTransform.rot);
					rot.multiply43Left(hol->m_localTransform.rot);
					std::string artName = "HolsterArt" + std::to_string(gCurHolster);
					NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
					if (artToMove) {
						artToMove->m_localTransform.rot = hol->m_localTransform.rot;
					}

				}
			}
			else {
				return;
			}
		}
	}

	void rotateholsterZ() {
		NiNode* hol = nullptr;
		Matrix44 rot;
		float tAxisOffset;
		//_MESSAGE(std::to_string(RZOffset).c_str());
		if (RZOffset != 0.0) {
			tAxisOffset = lAxisOffsetX * RZOffset;
		}
		else {
			tAxisOffset = lAxisOffsetX;
		}
		rot.setEulerAngles(0, (degrees_to_rads(tAxisOffset)), 0);
		if (configMode == 2) {
			if (WeaponRegisteredObjects[gCurHolster]) {
				hol = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
				if (hol) {
					hol->m_localTransform.rot = rot.multiply43Left(hol->m_localTransform.rot);
					rot.multiply43Left(hol->m_localTransform.rot);
					std::string artName = "HolsterArt" + std::to_string(gCurHolster);
					NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
					if (artToMove) {
						artToMove->m_localTransform.rot = hol->m_localTransform.rot;
					}
				}
			}
			else {
				return;
			}
		}
	}

	void saveconfig() {
		BSFixedString storedName;
		BSFixedString itemName;
		std::string name;
		NiNode* mesh;
		NiNode* mesh2;
		if (c_mode == 1) {
			itemName = HolsterRegisteredObjects[gCurHolster]->MeshName;
			storedName = HolsterRegisteredObjects[gCurHolster]->MeshName;
			mesh = HolsterRegisteredObjects[gCurHolster]->debugSphere;
			name = (storedName);
		}


		else {
			if (WeaponRegisteredObjects[gCurHolster]) {
				itemName = WeaponRegisteredObjects[gCurHolster]->HolsterName;
				storedName = WeaponRegisteredObjects[gCurHolster]->WeaponName;
				mesh2 = WeaponRegisteredObjects[gCurHolster]->WeapMesh;
				if (!isInMasterMode) {
					UInt32 WeapForm = WeaponRegisteredObjects[gCurHolster]->ThisForm;
					name = FileNameCreation(gCurHolster, WeapForm);
				}
				else {
					std::string ArmorName;
					if (gInPowerArmor) {
						if (gCurHolster > 4 || gCurHolster < 3) {
							TESForm* equippedItem = (*g_player)->equipData->slots[11].item;
							if (equippedItem) {
								ArmorName = equippedItem->GetFullName();
							}
						}
						else if (gCurHolster == 3) {
							TESForm* equippedItem = (*g_player)->equipData->slots[14].item;
							if (equippedItem) {
								ArmorName = equippedItem->GetFullName();
							}
						}
						else {
							TESForm* equippedItem = (*g_player)->equipData->slots[15].item;
							if (equippedItem) {
								ArmorName = equippedItem->GetFullName();
							}
						}
					}
					if (!WeaponRegisteredObjects[gCurHolster]->isMelee) {
						if (gInPowerArmor) {
							name = "Weapon" + std::to_string(gCurHolster) + ArmorName + "-PowerArmor";
						}
						else {
							name = "Weapon" + std::to_string(gCurHolster) + "-Normal";
						}
					}
					else {
						if (gInPowerArmor) {
							name = "Melee" + std::to_string(gCurHolster) + ArmorName + "-PowerArmor";
						}
						else {
							name = "Melee" + std::to_string(gCurHolster) + "-Normal";
						}
					}
				}
			}
			else {
				return;
			}
		}



		NiNode* hol = FindNode(itemName);
		if (!hol) {
			return;
		}
		std::string wNaa = "ConfigMODE: ";
		wNaa += (name);
		//_MESSAGE(wNaa.c_str());
		std::string wN = "ConfigMODE: ";
		wN += std::to_string(c_mode);
		//_MESSAGE(wN.c_str());
		NiTransform _customTransform1;
		_customTransform1.rot = hol->m_localTransform.rot;
		_customTransform1.pos = hol->m_localTransform.pos;
		if (c_mode == 1) {
			_customTransform1.scale = (hol->m_localTransform.scale / 2);
		}
		else {
			_customTransform1.scale = (hol->m_localTransform.scale);
		}

		if (c_mode == 1) {
			if (holsterMode == 1) {
				g_weaponOffsets->addOffset(name, _customTransform1, powerArmor);
			}
			else if (holsterMode == 2) {
				g_weaponOffsets->addOffset(name, _customTransform1, noPASneak);
			}
			else if (holsterMode == 3) {
				g_weaponOffsets->addOffset(name, _customTransform1, sneakinpowerArmor);
			}
			else {
				g_weaponOffsets->addOffset(name, _customTransform1, noPA);
			}
		}
		else {
			/*if (weaponMode == 1) {
				g_weaponOffsets->addOffset(name, _customTransform1, powerArmor);
			}
			else {
				g_weaponOffsets->addOffset(name, _customTransform1, noPA);
			}*/
			g_weaponOffsets->addOffset(name, _customTransform1, normal);
		}
		if (configMode == 1) {
			HolsterRegisteredObjects[gCurHolster]->radius = _customTransform1.scale;
		}
		CSimpleIniA ini;
		SI_Error rc = ini.LoadFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
		for (int i = 1; i < 8; i++) {
			std::string val;
			gInPowerArmor ? val = ("PAHolster0" + std::to_string(i) + "CustomArtSlot") : val = ("Holster0" + std::to_string(i) + "CustomArtSlot");
			gInPowerArmor ? rc = ini.SetLongValue("Config Mode", val.c_str(), PAholsterArtWorkPresets[i]) : rc = ini.SetLongValue("Config Mode", val.c_str(), holsterArtWorkPresets[i]);
		}
		rc = ini.SaveFile(".\\Data\\F4SE\\plugins\\VirtualHolsters.ini");
	}

	void restoreAllWeapons() {
		for (int i = 1; i < 8; i++) {
			nDestoryWeapon(i, true, false);
			resetHolsterName(i);
		}
	}

	// Holster Sphere Functions

	void detectHolsterSphere() {
		if ((*g_player)->firstPersonSkeleton == nullptr) {
			return;
		}
		if ((BSFadeNode*)(*g_player)->unkF0->rootNode->m_children.m_data[0]->GetAsNiNode()) {
			BSFlattenedBoneTree* rt = (BSFlattenedBoneTree*)(*g_player)->unkF0->rootNode->m_children.m_data[0]->GetAsNiNode();
			NiPoint3 rFinger;
			gIsRightHanded ? rFinger = rt->transforms[boneTreeMap["RArm_Finger23"]].world.pos : rFinger = rt->transforms[boneTreeMap["LArm_Finger23"]].world.pos;
			NiPoint3 offset;
			for (auto const& element : HolsterRegisteredObjects) {
				offset = element.second->bone->m_worldTransform.rot * element.second->offset;
				offset = element.second->bone->m_worldTransform.pos + offset;
				double dist = (double)vec3_len(rFinger - offset);
				if ((dist <= ((double)element.second->radius - 0.1)) && (isHolsterIntersected == false)) {
					if ((*g_player)->actorState.IsWeaponDrawn() && (WeaponRegisteredObjects[element.first])) {
						TESObjectWEAP* currentWeap = Offsets::GetEquippedWeapon((*g_gameVM)->m_virtualMachine, 0, *g_player, 0);
						if (currentWeap) {   //prevents sphere block if "fists" are equipped as no weapon is returned.
							//_MESSAGE("NO WEAPON");
							return;
						}
					}
					if (!element.second->stickyRight) {
						element.second->stickyRight = true;
						UInt32 handle = element.first;
						UInt32 device = 1;
						curDevice = device;
						if (ghapticsEnabled) {
							gIsRightHanded ? vrHook->StartHaptics(2, 0.05, 0.3) : vrHook->StartHaptics(1, 0.05, 0.3);
						}
						isHolsterIntersected = true;
						if (c_ActivationDisabled) {
							Setting* set = GetINISetting("fPrimaryWandMaxGrabDistance:VRWand");
							set->SetDouble(0.0);
							set = GetINISetting("fSecondaryWandMaxGrabDistance:VRWand");
							set->SetDouble(0.0);
							set = GetINISetting("fDownwardGrabFloorDistanceMultiplier:VRWand");
							set->SetDouble(0.0);
							SInt32 evt0 = HolsterEvent_DisableActivate;
							if (g_HolsterEventRegs.m_data.size() > 0) {
								g_HolsterEventRegs.ForEach(
									[&evt0](const EventRegistration<NullParameters>& reg) {
										SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt0, gCurHolster);
									}
								);
							}
						}
						if (!_isConfigModeActive) {
							gCurHolster = handle;
							BSFixedString NameofMesh = element.second->MeshName;
							NiNode* holster = element.second->debugSphere;
							if (!holster) {
								return;
							}
							if (gshowSpheres) {
								holster->flags &= 0xfffffffffffffffe;
								float f = 12.0;
								_isConfigModeActive ? holster->m_localTransform.scale = (HolsterRegisteredObjects[handle]->radius * 2) : holster->m_localTransform.scale = f;
							}
						}
					}
				}
				else if (dist >= ((double)element.second->radius + 0.1)) {
					if (element.second->stickyRight) {
						element.second->stickyRight = false;
						BSFixedString NameofMesh = element.second->MeshName;
						NiNode* holster = element.second->debugSphere;
						if (!holster) {
							return;
						}
						UInt32 handle = element.first;
						UInt32 device = 1;
						curDevice = 0;
						if (!_isConfigModeActive) {
							holster->flags |= 0x1;
							holster->m_localTransform.scale = 0;

						}
						isHolsterIntersected = false;
						if (c_ActivationDisabled) {
							Setting* set = GetINISetting("fPrimaryWandMaxGrabDistance:VRWand");
							set->SetDouble(pGrabDistance);
							set = GetINISetting("fSecondaryWandMaxGrabDistance:VRWand");
							set->SetDouble(sGrabDistance);
							set = GetINISetting("fDownwardGrabFloorDistanceMultiplier:VRWand");
							set->SetDouble(grabMulti);
							SInt32 evt = HolsterEvent_EnableActivate;
							if (g_HolsterEventRegs.m_data.size() > 0) {
								g_HolsterEventRegs.ForEach(
									[&evt](const EventRegistration<NullParameters>& reg) {
										SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt, gCurHolster);
									}
								);
							}
						}
					}
				}
			}
		}
	}

	void displayHolstersSphere(UInt32 handle) {
		if (HolsterRegisteredObjects[handle]) {
			NiNode* bone = HolsterRegisteredObjects[handle]->bone;
			NiNode* hol;
			BSFixedString NameofMesh = HolsterRegisteredObjects[handle]->MeshName;
			NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/1x1Sphere.nif");
			NiCloneProcess proc;
			proc.unk18 = Offsets::cloneAddr1;
			proc.unk48 = Offsets::cloneAddr2;
			hol = Offsets::cloneNode(retNode, &proc);
			if (!hol) {
				_MESSAGE("NIF NOT LOADED");
			}
			if (hol) {
				hol->m_name = BSFixedString(NameofMesh);
				if (hol->m_children.m_emptyRunStart > 0) {
					for (auto i = 0; i < hol->m_children.m_emptyRunStart; ++i) {
						auto nextNode = hol->m_children.m_data[i];
						if (nextNode) {
							BSTriShape* NodeTri = nextNode->GetAsBSTriShape();
							if (NodeTri) {
								std::string n = "OrbMesh" + handle + i;
								nextNode->m_name = n.c_str();
								//_MESSAGE("RENAMED ORB");
							}
						}
					}
				}
				NiNode* Weap2 = getChildNode(NameofMesh, (*g_player)->unkF0->rootNode); //Check for accidental spawned clones & delete
				if (Weap2) {
					Weap2->flags |= 0x1;
					Weap2->m_localTransform.scale = 0;
					Weap2->m_parent->RemoveChild(Weap2);
				}
				bone->AttachChild((NiAVObject*)hol, true);
				hol->m_localTransform.scale = (HolsterRegisteredObjects[handle]->radius * 2);
				HolsterRegisteredObjects[handle]->debugSphere = hol;
				hol->m_localTransform.pos.x = (hol->m_localTransform.pos.x + HolsterRegisteredObjects[handle]->offset.x);
				hol->m_localTransform.pos.y = (hol->m_localTransform.pos.y + HolsterRegisteredObjects[handle]->offset.y);
				hol->m_localTransform.pos.z = (hol->m_localTransform.pos.z + HolsterRegisteredObjects[handle]->offset.z);
				hol->flags |= 0x1;
				hol->m_localTransform.scale = 0;
				//_MESSAGE("SPHERE ATTACHED");
			}
		}

	}

	void registerHolsterSphere(float radius, BSFixedString bone, NiPoint3 pos, BSFixedString MeshPath, BSFixedString MeshName, UInt32 handle) {
		if (radius == 0.0) {
			return;
		}
		if (!(*g_player)->unkF0) {
			_MESSAGE("can't register yet as new game");
			return;
		}
		NiNode* boneNode = FindNode(bone);
		if (!boneNode) {
			return;
		}
		NiPoint3 offsetVec;
		MyHolster* hol = new MyHolster(radius, boneNode, pos, MeshPath, MeshName);
		HolsterRegisteredObjects[handle] = hol;
		//_MESSAGE("HOLSTER SPHERE EVENT: SPHERE REGISTERED");
		displayHolstersSphere(handle);
		return;
	}

	void createAllHolsterSpheres() {
		for (int i = 1; i < 8; i++) {
			std::string c1 = HolsterBones[i];
			std::string hN = "Holster" + std::to_string(i);
			NiTransform _customTransform;
			auto lookup = g_weaponOffsets->getOffset(hN, noPA);
			if (holsterMode == 1) {
				lookup = g_weaponOffsets->getOffset(hN, powerArmor);
			}
			else if (holsterMode == 2) {
				lookup = g_weaponOffsets->getOffset(hN, noPASneak);
			}
			else if (holsterMode == 3) {
				lookup = g_weaponOffsets->getOffset(hN, sneakinpowerArmor);
			}
			else {
				lookup = g_weaponOffsets->getOffset(hN, noPA);
			}
			if (lookup.has_value()) {
				_customTransform = lookup.value();
			}
			else {
				lookup = g_weaponOffsets->getOffset(hN, noPA);
				if (lookup.has_value()) {
					_customTransform = lookup.value();
				}
				else {
					_MESSAGE("UNABLE TO FIND HOLSTER JSON");
					return;
				}
			}
			BSFixedString bone = c1.c_str();
			BSFixedString MeshPath = hN.c_str();
			BSFixedString MeshName = hN.c_str();
			NiPoint3 pos = _customTransform.pos;
			float radius = _customTransform.scale;
			NiNode* boneNode = FindNode(bone);
			if (!boneNode) {
				return;
			}
			registerHolsterSphere(radius, bone, pos, MeshPath, MeshName, i);
		}
	}

	void updateSpheresForPAEvent() {
		detectInPowerArmor() ? _MESSAGE("HOLSTER SPHERE EVENT: STARTING SPHERES CREATION FOR PA") : _MESSAGE("HOLSTER SPHERE EVENT: STARTING SPHERES CREATION FOR NON-POWER ARMOR MODE");
		for (int i = 1; i < 8; i++) {
			std::string c1 = HolsterBones[i];
			std::string hN = "Holster" + std::to_string(i);
			NiTransform _customTransform;
			auto lookup = detectInPowerArmor() ? g_weaponOffsets->getOffset(hN, powerArmor) : g_weaponOffsets->getOffset(hN, noPA);
			if (lookup.has_value()) {
				_customTransform = lookup.value();
			}
			else {
				return;
			}
			BSFixedString bone = c1.c_str();
			BSFixedString MeshPath = hN.c_str();
			BSFixedString MeshName = hN.c_str();
			NiPoint3 pos = _customTransform.pos;
			float radius = _customTransform.scale;
			NiNode* boneNode = FindNode(bone);
			if (!boneNode) {
				return;
			}
			registerHolsterSphere(radius, bone, pos, MeshPath, MeshName, i);
		}
		detectInPowerArmor() ? _MESSAGE("HOLSTER SPHERE EVENT: ALL SPHERES CREATED FOR POWER ARMOR MODE") : _MESSAGE("HOLSTER SPHERE EVENT: ALL SPHERES CREATED FOR NON-POWER ARMOR MODE");
	}

	void destroyAllHolsterSpheres() {
		//_MESSAGE("POWER ARMOR ENTER / EXIT EVENT:: REMOVING REDUNDANT HOLSTER SPHERES");
		NiNode* hol = FindNode("Holster1");
		if (!hol) {
			return;
		}
		else {
			hol->flags |= 0x1;
			hol->m_localTransform.scale = 0;
			hol->m_parent->RemoveChild(hol);
		}
		hol = FindNode("Holster2");
		if (!hol) {
			return;
		}
		else {
			hol->flags |= 0x1;
			hol->m_localTransform.scale = 0;
			hol->m_parent->RemoveChild(hol);
		}
		for (int i = 1; i < 8; i++) {
			delete HolsterRegisteredObjects[i];
			HolsterRegisteredObjects.erase(i);
		}
		isHolsterIntersected = false;
		//_MESSAGE("POWER ARMOR ENTER / EXIT EVENT:: ALL HOLSTER SPHERES REMOVED");
	}

	void sphereMoveSneak() {
		for (int i = 1; i < 8; i++) {
			if (HolsterRegisteredObjects[i]) {
				BSFixedString NameofMesh = HolsterRegisteredObjects[i]->MeshName;
				NiTransform _customTransform;
				NiNode* hol = HolsterRegisteredObjects[i]->debugSphere;
				auto lookup = gInPowerArmor ? g_weaponOffsets->getOffset(NameofMesh.c_str(), sneakinpowerArmor) : g_weaponOffsets->getOffset(NameofMesh.c_str(), noPASneak);
				if (lookup.has_value()) {
					_customTransform = lookup.value();
					hol->m_localTransform.pos = _customTransform.pos;
					float f = 12.0;
					_isConfigModeActive ? hol->m_localTransform.scale = (_customTransform.scale * 2) : hol->m_localTransform.scale = f;
					HolsterRegisteredObjects[i]->offset = _customTransform.pos;
					HolsterRegisteredObjects[i]->radius = _customTransform.scale;
				}
			}
		}
	}

	void sphereMoveStand() {
		for (int i = 1; i < 8; i++) {
			if (HolsterRegisteredObjects[i]) {
				NiTransform _customTransform;
				BSFixedString NameofMesh = HolsterRegisteredObjects[i]->MeshName;
				NiNode* hol = HolsterRegisteredObjects[i]->debugSphere;
				auto lookup = gInPowerArmor ? g_weaponOffsets->getOffset(NameofMesh.c_str(), powerArmor) : g_weaponOffsets->getOffset(NameofMesh.c_str(), noPA);
				if (lookup.has_value()) {
					_customTransform = lookup.value();
					hol->m_localTransform.pos = _customTransform.pos;
					float f = 12.0;
					_isConfigModeActive ? hol->m_localTransform.scale = (_customTransform.scale * 2) : hol->m_localTransform.scale = f;
					HolsterRegisteredObjects[i]->offset = _customTransform.pos;
					HolsterRegisteredObjects[i]->radius = _customTransform.scale;

				}
			}
		}
	}

	float getRadius(int handle) {
		float radius = (HolsterRegisteredObjects[handle]->radius * 2);
		return radius;
	}

	// Visible Weapon Functions

	void hideMagazine(NiNode* original, int handle) {
		original->m_name.c_str();
		original->m_children.m_emptyRunStart;
		if (original->m_children.m_emptyRunStart != 0) {
			for (auto i = 0; i < original->m_children.m_emptyRunStart; ++i) {
				auto nextNode = original->m_children.m_data[i];
				if (nextNode) {
					BSTriShape* NodeTri = nextNode->GetAsBSTriShape();
					if (NodeTri) {
						BSFixedString TriName = NodeTri->m_name;
						bool IsWMelee = WeaponRegisteredObjects[handle]->isMelee;
						if (IsWMelee) {
							for (int i = 0; i < 2; i++) {
								if (matchSubString(TriName.c_str(), meleeRemovalStrings[i])) {
									NodeTri->m_localTransform.scale = 0.0;
								}
							}
						}
						else {
							for (int i = 0; i < 5; i++) {
								if (matchSubString(TriName.c_str(), weaponRemovalStrings[i])) {
									BSFixedString PName = NodeTri->m_parent->m_name;
									if (matchSubString(PName.c_str(), "P-Mag")) {
										NodeTri->m_localTransform.scale = 0.0;
									}
								}
							}
						}
					}
					else {
						NiNode* nextNodeToCheck = nextNode->GetAsNiNode();
						if (nextNodeToCheck != nullptr) {
							if (nextNodeToCheck->m_children.m_emptyRunStart > 0) {
								hideMagazine(nextNodeToCheck, handle);
							}
						}
					}
				}
			}
		}
	}

	// Test Function to Fix clones - introduces crashes.
	void clonemeshfix(NiNode* original, NiNode* clone) {
		_MESSAGE("STARTING CLONE FIX");
		original->m_name.c_str();
		original->m_children.m_emptyRunStart;
		if (original->m_children.m_emptyRunStart != 0) {
			for (auto i = 0; i < original->m_children.m_emptyRunStart; ++i) {
				auto nextNode = original->m_children.m_data[i];
				auto nextCloneNode = clone->m_children.m_data[i];
				if (nextNode && nextCloneNode) {
					nextCloneNode->m_name = nextNode->m_name;
					BSTriShape* NodeTri = nextNode->GetAsBSTriShape();
					BSTriShape* CloneTri = nextCloneNode->GetAsBSTriShape();
					NiNode* nextCloneNodeToCheck = nextCloneNode->GetAsNiNode();
					if (NodeTri && CloneTri) {
						c_OriginalNodes.push_back(NodeTri);
						c_CloneNodes.push_back(CloneTri);
					}
					else {
						NiNode* nextNodeToCheck = nextNode->GetAsNiNode();
						NiNode* nextCloneNodeToCheck = nextCloneNode->GetAsNiNode();
						if (nextNode->GetAsNiNode() != nullptr && nextCloneNode != nullptr) {
							if (nextNodeToCheck->m_children.m_emptyRunStart > 0) {
								clonemeshfix(nextNodeToCheck, nextCloneNodeToCheck);
							}
						}
					}
				}
			}
		}
	}

	void displayWeapon(TESObjectREFR* object, UInt32 handle) {
		if (WeaponRegisteredObjects[handle]) {
			_WeaponHolsterInProgress = true;
			bool isInPowerArmor = detectInPowerArmor();
			NiNode* bone;
			NiNode* holsteredW;
			BSFixedString weapName;
			weapName = WeaponRegisteredObjects[handle]->WeaponName;
			BSFixedString NameofHolster;
			NiNode* Weap = nullptr;
			std::string artName = "HolsterArt" + std::to_string(handle);
			NiNode* artToRemove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode); //Remove any Holster Art applied this slot
			if (artToRemove) {
				artToRemove->flags |= 0x1;
				artToRemove->m_localTransform.scale = 0;
				artToRemove->m_parent->RemoveChild(artToRemove);
			}
			if (!isInPowerArmor) {
				if (std::any_of(c_WeaponBlacklist[handle].begin(), c_WeaponBlacklist[handle].end(), [weapName](std::string cbl) {return !cbl.compare(weapName); })) { //Check if the current weapon is black listed for this slot and do not display it if it is.
					std::string yy = weapName;
					yy = yy + " : IS IN SLOT " + std::to_string(handle) + " BLACKLIST FILE, IT WILL NOT BE DISPLAYED";
					//_MESSAGE(yy.c_str());
					_WeaponHolsterInProgress = false;
					return;
				}
			}
			else {
				if (std::any_of(c_WeaponBlacklistPA[handle].begin(), c_WeaponBlacklistPA[handle].end(), [weapName](std::string cbl) {return !cbl.compare(weapName); })) { //Check if the current weapon is black listed for this slot and do not display it if it is.
					std::string yy = weapName;
					yy = yy + " : IS IN POWER ARMOR SLOT " + std::to_string(handle) + " BLACKLIST FILE, IT WILL NOT BE DISPLAYED";
					//_MESSAGE(yy.c_str());
					_WeaponHolsterInProgress = false;
					return;
				}
			}
			bone = WeaponRegisteredObjects[handle]->wbone;
			holsteredW = WeaponRegisteredObjects[handle]->WeapMesh;
			std::string wN = (weapName.c_str());
			//lastInHolster[handle] = wN;
			wN += std::to_string(handle);
			NameofHolster = WeaponRegisteredObjects[handle]->HolsterName;
			//_MESSAGE("ABOUT TO CHECK WEAPON NODE");
			//Weap = (ImportHolster)->GetObjectRootNode()->GetAsNiNode();
			if (object != nullptr) {
				Weap = object->GetObjectRootNode()->GetAsNiNode();
				//_MESSAGE("OBJECT REF USED TO CLONE");
			}
			else {
				return;
			}
			if (!Weap) {
				_WeaponHolsterInProgress = false;
				return;
			}
			bool IsWMelee = WeaponRegisteredObjects[handle]->isMelee;
			const char* ripper = "Ripper";
			if (IsWMelee && matchSubString(weapName, ripper) == false) {
				hideMagazine(Weap, handle);
			}
			else {
				if (!IsWMelee) {
					for (int i = 0; i < c_ExcludedMagazines.size(); i++) {
						std::string str = c_ExcludedMagazines[i];
						BSFixedString ch = weapName;
						if (matchSubString(ch, str.c_str())) {
							hideMagazine(Weap, handle);
						}
					}
				}
			}
			NiCloneProcess proc;
			proc.unk18 = Offsets::cloneAddr1;
			proc.unk48 = Offsets::cloneAddr2;
			holsteredW = Offsets::cloneNode(Weap, &proc);
			if (testclonefix) {
				clonemeshfix(Weap, holsteredW);
				std::string aaa = "Original " + std::to_string(c_OriginalNodes.size()) + " Clone " + std::to_string(c_CloneNodes.size());
				//_MESSAGE(aaa.c_str());
				for (int i = 0; i < c_OriginalNodes.size(); i++) {
					if (c_OriginalNodes[i] != nullptr && c_CloneNodes[i] != nullptr) {
						//_MESSAGE("Getting Parents");
						NiNode* P = c_OriginalNodes[i]->m_parent;
						NiNode* CP = c_CloneNodes[i]->m_parent;
						if (P != nullptr && CP != nullptr) {
							//_MESSAGE("Parent Found");
							c_CloneNodes[i]->m_name = c_OriginalNodes[i]->m_name;
							P->RemoveChild(c_OriginalNodes[i]);
							CP->RemoveChild(c_CloneNodes[i]);
							CP->AttachChild(c_OriginalNodes[i], true);
							Weap->kFlagForceUpdate;
							holsteredW->kFlagForceUpdate;
							P = nullptr;
							CP = nullptr;
							std::string X = "MESH " + std::to_string(i) + " SWAPPED";
							//_MESSAGE(X.c_str());
						}
					}
				}
				c_OriginalNodes.clear();
				c_CloneNodes.clear();
				Weap = nullptr;
				//ImportHolster = nullptr;
			}
			if (holsteredW) {
				holsteredW->m_name = BSFixedString(NameofHolster);
				NiNode* Weap2 = getChildNode(NameofHolster, (*g_player)->unkF0->rootNode); //Check for accidental spawned clones
				if (Weap2) {
					Weap2->m_localTransform.scale = 0;
					Weap2->m_parent->RemoveChild(Weap2);
				}
				holsteredW->m_spCollisionObject = nullptr;  // Remove weapon collision so that holstered weapons do not break when the player uses furniture.
				bone->AttachChild(holsteredW, true);
				WeaponRegisteredObjects[handle]->WeapMesh = holsteredW;
				NiTransform _customTransform;
				UInt32 WeapForm = WeaponRegisteredObjects[handle]->ThisForm;
				std::string searchname = FileNameCreation(handle, WeapForm);
				lastInHolster[handle] = WeapForm;
				//_MESSAGE(searchname.c_str());
				auto lookup = g_weaponOffsets->getOffset(searchname, normal);
				if (lookup.has_value()) {
					_customTransform = lookup.value();
					holsteredW->m_localTransform = _customTransform;
					//_MESSAGE("REGISTER HOLSTERED WEAPON: WEAPON SPECIFIC CONFIG FOUND...");
					//isInPowerArmor ? _MESSAGE("...FOR POWER ARMOR MODE") : _MESSAGE("...FOR NONE POWER ARMOR MODE");
				}
				else {
					//_MESSAGE("REGISTER HOLSTERED WEAPON: NO CONFIG FOR THIS ITEM FOUND...");
					//_MESSAGE("APPLYING A DEFAULT PROFILE......");
					std::string wN2 = IsWMelee ? "Melee" : "Weapon";
					wN2 += std::to_string(handle);
					std::string ArmorName;
					if (isInPowerArmor) {
						if (handle > 4 || handle < 3) {
							TESForm* equippedItem = (*g_player)->equipData->slots[11].item;
							if (equippedItem) {
								//CurrentEquippedArmor[i][j] = equippedItem->formID;
								ArmorName = equippedItem->GetFullName();
								//_MESSAGE(ArmorName.c_str());
							}
						}
						else if (handle == 3) {
							TESForm* equippedItem = (*g_player)->equipData->slots[14].item;
							if (equippedItem) {
								ArmorName = equippedItem->GetFullName();
							}
						}
						else {
							TESForm* equippedItem = (*g_player)->equipData->slots[15].item;
							if (equippedItem) {
								ArmorName = equippedItem->GetFullName();
							}
						}
					}
					lookup = isInPowerArmor ? g_weaponOffsets->getOffset(wN2 + ArmorName, powerArmor) : g_weaponOffsets->getOffset(wN2, noPA);
					if (lookup.has_value()) {
						_customTransform = lookup.value();
						holsteredW->m_localTransform = _customTransform;
						//IsWMelee ? _MESSAGE("REGISTER HOLSTERED WEAPON: DEFAULT MELEE CONFIG APPLIED") : _MESSAGE("REGISTER HOLSTERED WEAPON: DEFAULT NON-MELEE CONFIG APPLIED");
						//detectInPowerArmor() ? _MESSAGE("...FOR POWER ARMOR MODE") : _MESSAGE("...FOR NONE POWER ARMOR MODE");
						_customTransform = lookup.value();
						holsteredW->m_localTransform = _customTransform;
					}
					else {
						lookup = isInPowerArmor ? g_weaponOffsets->getOffset(wN2, powerArmor) : g_weaponOffsets->getOffset(wN2, noPA);
						if (lookup.has_value()) {
							_customTransform = lookup.value();
							holsteredW->m_localTransform = _customTransform;
							//IsWMelee ? _MESSAGE("REGISTER HOLSTERED WEAPON: DEFAULT MELEE CONFIG APPLIED") : _MESSAGE("REGISTER HOLSTERED WEAPON: DEFAULT NON-MELEE CONFIG APPLIED");
							//detectInPowerArmor() ? _MESSAGE("...FOR POWER ARMOR MODE") : _MESSAGE("...FOR NONE POWER ARMOR MODE");
							_customTransform = lookup.value();
							holsteredW->m_localTransform = _customTransform;
						}
					}
				}
				//Add Customer Holster Art if Found
				std::string currentWeapon = weapName;
				std::vector<std::string> listOfFiles;
				std::string noPASearchPathPath = "Data\\Meshes\\VRH\\Holsters\\" + currentWeapon + "\\HolsterArt" + std::to_string(handle) + "*.nif";
				std::string PASearchPath = "Data\\Meshes\\VRH\\Holsters\\_PA\\" + currentWeapon + "\\HolsterArt" + std::to_string(handle) + "*.nif";
				std::string finalpath;
				if (gInPowerArmor) {
					if (getFileNames(PASearchPath).size() == 0) {
						if (getFileNames(noPASearchPathPath).size() == 0) {
							//_MESSAGE("No Holster Art located For this Weapon");
							_WeaponHolsterInProgress = false;
							return; // No Artwork found - Nothing else to do
						}
						else {
							listOfFiles = getFileNames(noPASearchPathPath);
							int maxNumOfArtwork = listOfFiles.size() - 1;
							PAholsterMaxValue[gCurHolster] = maxNumOfArtwork - 1;
							std::string filePath;
							if (holsterArtWorkPresets[handle] > maxNumOfArtwork) {
								//_MESSAGE("Holster Art Preset not found:: Resetting to default");
								holsterArtWorkPresets[handle] = 0;
								std::string fallBack = ("Falling back to file:: " + listOfFiles[holsterArtWorkPresets[handle]]);
								//_MESSAGE(fallBack.c_str());
								filePath = "Data/Meshes/VRH/Holsters/" + currentWeapon + "/" + listOfFiles[holsterArtWorkPresets[handle]];
							}
							else {
								filePath = "Data/Meshes/VRH/Holsters/" + currentWeapon + "/" + listOfFiles[holsterArtWorkPresets[handle]];
							}
							if (!exists_file(filePath)) {
								std::string mess = "Loading Failed:: " + filePath;
								_WeaponHolsterInProgress = false;
								return; // No Artwork found - Nothing else to do
							}
							else {
								finalpath = filePath;
							}
						}
					}
					else if (getFileNames(PASearchPath).size() > 0) {
						listOfFiles = getFileNames(PASearchPath);
						int maxArtwork = listOfFiles.size() - 1;
						PAholsterMaxValue[gCurHolster] = (maxArtwork - 1);
						std::string l = ("Applying PA Holster Art Preset " + std::to_string(PAholsterArtWorkPresets[handle]));
						//_MESSAGE(l.c_str());
						std::string filePath;
						std::string mess = "Max Installed Artwork: " + std::to_string(maxArtwork);
						//_MESSAGE(mess.c_str());
						if (PAholsterArtWorkPresets[handle] > maxArtwork) {
							//_MESSAGE("Holster Art Preset not found:: Resetting to default");
							PAholsterArtWorkPresets[handle] = 0;
							std::string fallBack = ("Falling back to file:: " + listOfFiles[PAholsterArtWorkPresets[handle]]);
							//_MESSAGE(fallBack.c_str());
							filePath = "Data/Meshes/VRH/Holsters/_PA/" + currentWeapon + "/" + listOfFiles[PAholsterArtWorkPresets[handle]];

						}
						else {
							filePath = "Data/Meshes/VRH/Holsters/_PA/" + currentWeapon + "/" + listOfFiles[PAholsterArtWorkPresets[handle]];
						}
						if (!exists_file(filePath)) {
							std::string mess = "Loading Failed:: " + filePath;
							_WeaponHolsterInProgress = false;
							return; // No Artwork found - Nothing else to do
						}
						else {
							finalpath = filePath;
						}
					}
				}
				else if (getFileNames(noPASearchPathPath).size() == 0) {
					//_MESSAGE("No Holster Art located For this Weapon");
					_WeaponHolsterInProgress = false;
					return;
				}
				else {
					std::string mess = "Searching for Files:: " + noPASearchPathPath;
					//	_MESSAGE(mess.c_str());
					listOfFiles = getFileNames(noPASearchPathPath);
					int maxArtwork = listOfFiles.size() - 1;
					holsterMaxValue[gCurHolster] = (maxArtwork - 1);
					std::string filePath;
					if (holsterArtWorkPresets[handle] > maxArtwork) {
						//_MESSAGE("Holster Art Preset not found::Resetting to default");
						holsterArtWorkPresets[handle] = 0;
						std::string fallBack = ("Falling back to file:: " + listOfFiles[holsterArtWorkPresets[handle]]);
						//_MESSAGE(fallBack.c_str());
						filePath = "Data/Meshes/VRH/Holsters/" + currentWeapon + "/" + listOfFiles[holsterArtWorkPresets[handle]];
					}
					else {
						filePath = "Data/Meshes/VRH/Holsters/" + currentWeapon + "/" + listOfFiles[holsterArtWorkPresets[handle]];
					}
					if (!exists_file(filePath)) {
						std::string mess = "Loading Failed:: " + filePath;
						//_MESSAGE(mess.c_str());
						_WeaponHolsterInProgress = false;
						return; // No Artwork found - Nothing else to do
					}
					else {
						finalpath = filePath;
					}
				}
				char* path;
				path = new char[finalpath.size() + 1];
				std::copy(finalpath.begin(), finalpath.end(), path);
				path[finalpath.size()] = '\0';
				std::string mess = "Loading File:: " + finalpath;
				//_MESSAGE(mess.c_str());
				NiNode* retNode = loadNifFromFile(path);
				NiNode* holArt = Offsets::cloneNode(retNode, &proc);
				holArt->m_name = artName.c_str();
				bone->AttachChild(holArt, true);
				holArt->m_localTransform = holsteredW->m_localTransform;
				WeaponRegisteredObjects[handle]->ArtMesh = holArt;
				//_MESSAGE("REGISTER HOLSTERED WEAPON : CUSTOM HOLSTER ART APPLIED");
				_WeaponHolsterInProgress = false;
			}
			_WeaponHolsterInProgress = false;
		}
		_WeaponHolsterInProgress = false;
	}

	void nDestoryWeapon(UInt32 handle, bool destroyArt, bool playsound) {
		if (gShowWeapons) {
			if (WeaponRegisteredObjects[handle]) {
				if (playsound) {
					playSwordSFX(handle);
				}
				NiNode* weap = WeaponRegisteredObjects[handle]->WeapMesh;
				if (weap) {
						weap->flags |= 0x1;
						weap->m_localTransform.scale = 0;
						NiNode* pNode = nullptr;
						gInPowerArmor ? pNode = getChildNode(PAWeaponBones[handle].c_str(), (*g_player)->unkF0->rootNode) : pNode = getChildNode(WeaponBones[handle].c_str(), (*g_player)->unkF0->rootNode);
						if (pNode != nullptr) {
							NiNode* wTBD = getChildNode(HolsterNames[handle].c_str(), (*g_player)->unkF0->rootNode);
							if (wTBD != nullptr) {
								_MESSAGE("REMOVING WEAPON");
								pNode->RemoveChild(wTBD);
								pNode = nullptr;
								_MESSAGE("WEAPON REMOVED");
							}
						}
				}
				delete WeaponRegisteredObjects[handle];
				WeaponRegisteredObjects.erase(handle);
				//_MESSAGE("destroyed Weapon");
			}
			if (destroyArt) {
				if (*g_player) {
					if ((*g_player)->unkF0) {
						if ((*g_player)->unkF0->rootNode) {
							//std::string artName = "HolsterArt" + std::to_string(handle);
							NiNode* artToRemove = getChildNode(hArt[handle], (*g_player)->unkF0->rootNode); //Remove any Holster Art applied this slot
							//NiNode* artToRemove = WeaponRegisteredObjects[handle]->ArtMesh;
							if (artToRemove) {
								//_MESSAGE("Found Art");
								artToRemove->flags |= 0x1;
								artToRemove->m_localTransform.scale = 0;
								NiNode* parentNode = nullptr;
								gInPowerArmor ? parentNode = FindNode(PAWeaponBones[handle].c_str()) : parentNode = FindNode(WeaponBones[handle].c_str());
								if (parentNode) {
									parentNode->RemoveChild(artToRemove);
								}
								//_MESSAGE("destroyed art");
							}
						}
					}
				}
			}
		}
	}

	void moveWeapon(int handle) {
		if (gShowWeapons) {
			if (WeaponRegisteredObjects[handle]) {
				NiNode* weap = WeaponRegisteredObjects[handle]->WeapMesh;
				std::string wN = WeaponRegisteredObjects[handle]->WeaponName;
				bool IsWMelee = WeaponRegisteredObjects[handle]->isMelee;
				NiTransform _customTransform;
				if (weap) {
					UInt32 WeapForm = WeaponRegisteredObjects[handle]->ThisForm;
					std::string searchname = FileNameCreation(handle, WeapForm);
					auto lookup = g_weaponOffsets->getOffset(searchname, normal);
					if (lookup.has_value()) {
						_customTransform = lookup.value();
						weap->m_localTransform = _customTransform;
						_MESSAGE("EQUIP EVENT MOVE: WEAPON SPECIFIC CONFIG FOUND...");
						std::string artName = "HolsterArt" + std::to_string(handle);
						NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
						if (artToMove) {
							artToMove->m_localTransform = weap->m_localTransform;
						}
					}
					else {
						bool isInPowerArmor = detectInPowerArmor();
						//_MESSAGE("EQUIP EVENT MOVE: NO CONFIG FOR THIS ITEM FOUND...");
						//_MESSAGE("APPLYING A DEFAULT PROFILE......");

						std::string ArmorName;
						if (isInPowerArmor) {
							if (handle > 4 || handle < 3) {
								TESForm* equippedItem = (*g_player)->equipData->slots[11].item;
								if (equippedItem) {
									ArmorName = equippedItem->GetFullName();
								}
							}
							else if (handle == 3) {
								TESForm* equippedItem = (*g_player)->equipData->slots[14].item;
								if (equippedItem) {
									ArmorName = equippedItem->GetFullName();
								}
							}
							else {
								TESForm* equippedItem = (*g_player)->equipData->slots[15].item;
								if (equippedItem) {
									ArmorName = equippedItem->GetFullName();
								}
							}
						}
						std::string wN2 = IsWMelee ? "Melee" : "Weapon";
						wN2 += std::to_string(handle);
						lookup = isInPowerArmor ? g_weaponOffsets->getOffset(wN2 + ArmorName, powerArmor) : g_weaponOffsets->getOffset(wN2, noPA);
						if (lookup.has_value()) {
							_customTransform = lookup.value();
							weap->m_localTransform = _customTransform;
							//IsWMelee ? _MESSAGE("EQUIP EVENT MOVE: DEFAULT MELEE CONFIG APPLIED") : _MESSAGE("EQUIP EVENT MOVE: DEFAULT NON-MELEE CONFIG APPLIED");
							std::string artName = "HolsterArt" + std::to_string(handle);
							NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
							if (artToMove) {
								artToMove->m_localTransform = weap->m_localTransform;
							}
						}
						else {
							lookup = isInPowerArmor ? g_weaponOffsets->getOffset(wN2, powerArmor) : g_weaponOffsets->getOffset(wN2, noPA);
							if (lookup.has_value()) {
								_customTransform = lookup.value();
								weap->m_localTransform = _customTransform;
								//IsWMelee ? _MESSAGE("EQUIP EVENT MOVE: DEFAULT MELEE CONFIG APPLIED") : _MESSAGE("EQUIP EVENT MOVE: DEFAULT NON-MELEE CONFIG APPLIED");
								std::string artName = "HolsterArt" + std::to_string(handle);
								NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
								if (artToMove) {
									artToMove->m_localTransform = weap->m_localTransform;
								}
							}
						}
					}
				}
			}
			else {
				std::string artName = "HolsterArt" + std::to_string(handle);
				NiNode* artToMove = getChildNode(artName.c_str(), (*g_player)->unkF0->rootNode);
				if (artToMove) {
					NiTransform _customTransform;
					std::string searchname = FileNameCreation(handle, lastInHolster[handle]);
					//_MESSAGE(searchname.c_str());
					auto lookup = g_weaponOffsets->getOffset(searchname, normal);
					if (lookup.has_value()) {
						_customTransform = lookup.value();
						artToMove->m_localTransform = _customTransform;
						//_MESSAGE("EQUIP EVENT MOVE: HOLSTER SPECIFIC CONFIG FOUND...MOVING");
					}
					else {
						artToMove->flags |= 0x1;
						artToMove->m_localTransform.scale = 0;
						artToMove->m_parent->RemoveChild(artToMove);
						//_MESSAGE("EQUIP EVENT MOVE: NO HOLSTER SPECIFIC CONFIG FOUND...DESTROYING");
					}
				}
			}
		}
	}

	void nRegisterWeapon(BSFixedString weapName, TESForm* baseForm, TESObjectREFR* object, UInt32 handle, bool isMeleeW) {
		if (gShowWeapons) {
			NiNode* bone = nullptr;
			if (weapName == ".44") {
				weapName = "44cal";
			}
			gInPowerArmor ? bone = FindNode(PAWeaponBones[handle].c_str()) : bone = FindNode(WeaponBones[handle].c_str());
			if (bone != nullptr) {
				UInt32 formID = baseForm->formID;
				BSFixedString holstername = HolsterNames[handle].c_str();
				MyWeapon* weap1 = new MyWeapon(bone, holstername, isMeleeW, weapName, formID);
				VisualArt[handle] = weapName;
				WeaponRegisteredObjects[handle] = weap1;
				displayWeapon(object, handle);
			}
		}
	}

	// Papyrus}

	void SetPointingHand() {
		setFingerPositionScalar((gIsRightHanded ? true : false), 0.0, 1.0, 0.0, 0.0, 0.0);
	}

	void RestorePointingHand() {
		restoreFingerPoseControl((gIsRightHanded ? true : false));
	}

	void playSwordSFX(int handle) {
		BSFixedString weapname = WeaponRegisteredObjects[handle]->WeaponName;
		bool isMelee = WeaponRegisteredObjects[handle]->isMelee;
		if (isMelee) {
			for (int i = 0; i < c_SwordDrawSFX.size(); i++) {
				if (matchSubString(weapname, c_SwordDrawSFX[i].c_str())) {
					SInt32 evt0 = HolsterEvent_DrawSword;
					if (g_HolsterEventRegs.m_data.size() > 0) {
						g_HolsterEventRegs.ForEach(
							[&evt0](const EventRegistration<NullParameters>& reg) {
								SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt0, gCurHolster);
							}
						);
					}
				}
			}
		}
	}

	void RegisterForHolsterEvents(StaticFunctionTag* base, VMObject* thisObject) {
		if (!thisObject) {
			return;
		}
		g_HolsterEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType());
		_MESSAGE("Papyrus Event:: 'VirtualHolstersQuestScript.pex' Registered to Receive Events");
		//Disable VATS if user has chosen to.
		if (c_VATSDisabled) {
			//_MESSAGE("VATS DISABLED");
			SInt32 evt2 = HolsterEvent_DisableVATS;
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt2](const EventRegistration<NullParameters>& reg) {
						SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt2, gCurHolster);
					}
				);
			}
		}

		//Re-enable Activate 
		if (c_ActivationDisabled) {
			SInt32 evt2 = HolsterEvent_EnableActivate;
			if (g_HolsterEventRegs.m_data.size() > 0) {
				g_HolsterEventRegs.ForEach(
					[&evt2](const EventRegistration<NullParameters>& reg) {
						SendPapyrusEvent2<SInt32, UInt32>(reg.handle, reg.scriptName, HolsterEventName, evt2, gCurHolster);
					}
				);
			}
		}
	}

	void UnRegisterForHolsterEvents(StaticFunctionTag* base, VMObject* thisObject) {
		if (!thisObject) {
			return;
		}
		g_HolsterEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());
		_MESSAGE("PAPYRUS EVENT: VIRTUAL HOLSTERS SCRIPT UN-REGISTERED FOR HOLSTER EVENTS");
	}

	bool RegisterFuncs(VirtualMachine* vm) {
		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, VMObject*>("RegisterForHolsterEvents", "HolsterFunctions", Holsters::RegisterForHolsterEvents, vm));
		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, void, VMObject*>("UnRegisterForHolsterEvents", "HolsterFunctions", Holsters::UnRegisterForHolsterEvents, vm));
		return true;
	}
}

// ============================================================================
// VirtualHolsters API Implementation 
// 
// Credit: With thanks to Asciimov.
// ============================================================================

class VirtualHolstersAPIImpl : public VirtualHolstersAPI
{
public:
	static VirtualHolstersAPIImpl& GetSingleton()
	{
		static VirtualHolstersAPIImpl instance;
		return instance;
	}

	std::uint32_t VH_CALL GetVersion() const override
	{
		return 1;
	}

	bool VH_CALL IsHandInHolsterZone(bool isLeft) const override
	{
		// Virtual Holsters currently only tracks "primary" hand (based on handedness)
		// isHolsterIntersected is global - it's true if ANY hand is in a holster
		// For now, we just return the global state
		// TODO: If Virtual Holsters adds per-hand tracking, update this
		return Holsters::isHolsterIntersected;
	}

	std::uint32_t VH_CALL GetCurrentHolster() const override
	{
		return gCurHolster;
	}

	bool VH_CALL IsHolsterFree(std::uint32_t holsterIndex) const override
	{
		if (holsterIndex < 1 || holsterIndex > 7) {
			return false;
		}
		return (holsteredWeapNames[holsterIndex] == "Empty");
	}

	const char* VH_CALL GetHolsteredWeaponName(std::uint32_t holsterIndex) const override
	{
		if (holsterIndex < 1 || holsterIndex > 7) {
			return "";
		}
		return holsteredWeapNames[holsterIndex].c_str();
	}

	bool VH_CALL IsWeaponAlreadyHolstered(const char* weaponName) const override
	{
		if (!weaponName || weaponName[0] == '\0') {
			return false;
		}
		std::string name(weaponName);
		for (int i = 1; i <= 7; i++) {
			if (holsteredWeapNames[i] == name) {
				return true;
			}
		}
		return false;
	}

	bool VH_CALL GetHolsterPosition(std::uint32_t holsterIndex, float& outX, float& outY, float& outZ) const override
	{
		if (holsterIndex < 1 || holsterIndex > 7) {
			return false;
		}
		auto it = Holsters::HolsterRegisteredObjects.find(holsterIndex);
		if (it == Holsters::HolsterRegisteredObjects.end() || !it->second || !it->second->bone) {
			return false;
		}
		// Calculate world position: bone world pos + rotated offset
		NiPoint3 offset = it->second->bone->m_worldTransform.rot * it->second->offset;
		NiPoint3 worldPos = it->second->bone->m_worldTransform.pos + offset;
		outX = worldPos.x;
		outY = worldPos.y;
		outZ = worldPos.z;
		return true;
	}

	float VH_CALL GetHolsterRadius(std::uint32_t holsterIndex) const override
	{
		if (holsterIndex < 1 || holsterIndex > 7) {
			return 0.0f;
		}
		auto it = Holsters::HolsterRegisteredObjects.find(holsterIndex);
		if (it == Holsters::HolsterRegisteredObjects.end() || !it->second) {
			return 0.0f;
		}
		return it->second->radius;
	}

	bool VH_CALL IsInitialized() const override
	{
		// Check if bone tree is initialized (set false after MainLoop processes)
		// and that holsters are registered
		return !Holsters::initBoneTreeFlag && !Holsters::HolsterRegisteredObjects.empty();
	}

private:
	VirtualHolstersAPIImpl() = default;
};

// Exported function - DO NOT CHANGE SIGNATURE
extern "C" __declspec(dllexport) VirtualHolstersAPI * VH_CALL VHAPI_GetApi()
{
	return &VirtualHolstersAPIImpl::GetSingleton();
}



