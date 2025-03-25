#include "Holsters.h"
#include "utils.h"
#include "f4se/NiObjects.h"
#include "f4se/GameMenus.h"
#include "ConfigModeMenu.h"
#include "weaponOffset.h"
#include "f4se/GameReferences.h"

namespace Holsters {
	char* mUI_Nodes[12] = {"Data/Meshes/VRH/UI-NODE01_LSWAP.nif", "Data/Meshes/VRH/UI-NODE02_HAPTICS.nif", "Data/Meshes/VRH/UI-NODE03_MODE.nif", "Data/Meshes/VRH/UI-NODE04_SVIS.nif", "Data/Meshes/VRH/UI-NODE05_WVIS.nif", "Data/Meshes/VRH/UI-NODE06_NEXT.nif", "Data/Meshes/VRH/UI-NODE07_RSWAP.nif", "Data/Meshes/VRH/UI-NODE08_ART.nif", "Data/Meshes/VRH/UI-NODE09_SAVE.nif", "Data/Meshes/VRH/UI-NODE10_RETURN.nif", "Data/Meshes/VRH/UI-NODE11_HELP.nif", "Data/Meshes/VRH/UI-NODE12_ACTIVATE.nif" };
	char* mUI_Tiles[12] = { "Data/Meshes/VRH/UI-TILE01_LSWAP.nif", "Data/Meshes/VRH/UI-TILE02_HAPTICS.nif", "Data/Meshes/VRH/UI-TILE03_MODE.nif", "Data/Meshes/VRH/UI-TILE04_SVIS.nif", "Data/Meshes/VRH/UI-TILE05_WVIS.nif", "Data/Meshes/VRH/UI-TILE06_NEXT.nif", "Data/Meshes/VRH/UI-TILE07_RSWAP.nif", "Data/Meshes/VRH/UI-TILE08_ART.nif", "Data/Meshes/VRH/UI-TILE09_SAVE.nif", "Data/Meshes/VRH/UI-TILE10_RETURN.nif", "Data/Meshes/VRH/UI-TILE11_HELP.nif", "Data/Meshes/VRH/UI-TILE12_ACTIVATE.nif" };
	char* mUI_Info_Holster[8] = { "none", "Data/Meshes/VRH/UI-INFO_HOLSTER01.nif", "Data/Meshes/VRH/UI-INFO_HOLSTER02.nif", "Data/Meshes/VRH/UI-INFO_HOLSTER03.nif", "Data/Meshes/VRH/UI-INFO_HOLSTER04.nif", "Data/Meshes/VRH/UI-INFO_HOLSTER05.nif", "Data/Meshes/VRH/UI-INFO_HOLSTER06.nif", "Data/Meshes/VRH/UI-INFO_HOLSTER07.nif" };
	char* nUI_Nodes[12] = { "UI-NODE01_LSWAP", "UI-NODE02_HAPTICS", "UI-NODE03_MODE", "UI-NODE04_SVIS", "UI-NODE05_WVIS", "UI-NODE06_NEXT", "UI-NODE07_RSWAP", "UI-NODE08_ART", "UI-NODE09_SAVE", "UI-NODE10_RETURN", "UI-NODE11_HELP", "UI-NODE12_ACTIVATE" };
	char* nUI_Tiles[12] = { "UI-TILE01_LSWAP", "UI-TILE02_HAPTICS", "UI-TILE03_MODE", "UI-TILE04_SVIS", "UI-TILE05_WVIS", "UI-TILE06_NEXT", "UI-TILE07_RSWAP", "UI-TILE08_ART", "UI-TILE09_SAVE", "UI-TILE10_RETURN", "UI-TILE11_HELP", "UI-TILE12_ACTIVATE" };
	void(*funcs[12])() = { UITile01Function, UITile02Function, UITile03Function, UITile04Function, UITile05Function, UITile06Function, UITile07Function, UITile08Function, UITile09Function, UITile10Function, UITile11Function, UITile12Function };
	char* nUI_InfoNodes[3] = { "Data/Meshes/VRH/UI-INFO_BUTTON01.nif", "Data/Meshes/VRH/UI-INFO_BUTTON02.nif", "Data/Meshes/VRH/UI-INFO_BUTTON03.nif" };
	char* mUI_InfoTiles[3] = { "UI-INFO_BUTTON01", "UI-INFO_BUTTON02", "UI-INFO_BUTTON03" };
	NiNode* niUIButton_Nodes[3] = {};
	NiNode* niUI_Nodes[12] = {};
	NiNode* niUI_Tiles[12] = {};
	bool bUITilePressed[12] = {};
	int configMode;
	NiNode* UIMain = nullptr;
	NiNode* UIModeLable = nullptr;
	NiNode* UIHolsterLable = nullptr;
	NiNode* UISneakOn = nullptr;
	NiNode* UISneakOff = nullptr;
	NiNode* UIPwrArmorOn = nullptr;
	NiNode* UIPwrArmorOff = nullptr;
	NiNode* UIHapticsOn = nullptr;
	NiNode* UIHapticsOff = nullptr;
	NiNode* UISpheresOn = nullptr;
	NiNode* UISpheresOff = nullptr;
	NiNode* UIWeaponsOn = nullptr;
	NiNode* UIWeaponsOff = nullptr;
	NiNode* UISaveMSG = nullptr;
	NiNode* UIActivateOn = nullptr;
	NiNode* UIActivateOff = nullptr;
	NiNode* UIVATSOn = nullptr;
	NiNode* UIVATSOff = nullptr;
	NiNode* UIBUTTON01 = nullptr;
	NiNode* UIBUTTON02 = nullptr;
	NiNode* UIBUTTON03 = nullptr;
	int lastHolsterButton = 0;
	NiNode* primaryAttachNode = nullptr;

	void setupConfigMode() {
		//Create Main Config UI Components
		BSFixedString nodename = "primaryUIAttachNode";
		NiNode* boneNode = nullptr;
		boneNode = FindNode(nodename);
		if (!boneNode) {
			return;
		}
		primaryAttachNode = boneNode;
		// Load and Display Main UI
		BSFixedString NameofMesh = "VRHHudMain";
		NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-MAIN_TITLE.nif");
		NiCloneProcess proc;
		proc.unk18 = Offsets::cloneAddr1;
		proc.unk48 = Offsets::cloneAddr2;
		UIMain = Offsets::cloneNode(retNode, &proc);
		UIMain->m_name = BSFixedString(NameofMesh);
		boneNode->AttachChild((NiAVObject*)UIMain, true);
		gInPowerArmor ? UIMain->m_localTransform.scale = 0.85 : UIMain->m_localTransform.scale = 0.7;
		UIMain->flags &= 0xfffffffffffffffe;
		//_MESSAGE("MAIN MENU CREATED");

		// Add Nodes & Touch Tiles 
		for (int i = 0; i < 12; i++) {
			std::string tile = "Tile: " + std::to_string(i) +" Starting";
			//_MESSAGE(tile.c_str());
			BSFixedString UI_ITEMNAME = (nUI_Nodes[i]);
			NiNode* retNode = loadNifFromFile(mUI_Nodes[i]);
			NiNode* UIItem = Offsets::cloneNode(retNode, &proc);
			UIItem->m_name = BSFixedString(UI_ITEMNAME);
			UIMain->AttachChild((NiAVObject*)UIItem, true);
			UIItem->flags &= 0xfffffffffffffffe;
			niUI_Nodes[i] = UIItem;
			BSFixedString UI_ITEMNAME2 = (nUI_Tiles[i]);
			retNode = loadNifFromFile(mUI_Tiles[i]);
			NiNode* UIItem2 = Offsets::cloneNode(retNode, &proc);
			UIItem2->m_name = BSFixedString(UI_ITEMNAME2);
			UIItem->AttachChild((NiAVObject*)UIItem2, true);
			UIItem2->flags &= 0xfffffffffffffffe;
			niUI_Tiles[i] = UIItem2;
			std::string tile2 = "Tile: " + std::to_string(i) + " Completed";
			//_MESSAGE(tile2.c_str());		
		}

		// Add Current Holster UI Label
		retNode = loadNifFromFile(mUI_Info_Holster[gCurHolster]);
		UIHolsterLable = Offsets::cloneNode(retNode, &proc);
		UIHolsterLable->m_name = BSFixedString("CurrentHolster");
		UIMain->AttachChild((NiAVObject*)UIHolsterLable, true);
		UIHolsterLable->flags &= 0xfffffffffffffffe;

		// Add Current UI MODE Lable
		retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_MODE02.nif");
		UIModeLable = Offsets::cloneNode(retNode, &proc);
		UIModeLable->m_name = BSFixedString("UI-INFO_MODE");
		UIMain->AttachChild((NiAVObject*)UIModeLable, true);
		UIModeLable->flags &= 0xfffffffffffffffe;

		// Add SNEAK MODE Lable
		gPlayerisSneaking ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SNEAKON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SNEAKOFF.nif");
		gPlayerisSneaking ? UISneakOn = Offsets::cloneNode(retNode, &proc) : UISneakOff = Offsets::cloneNode(retNode, &proc);
		gPlayerisSneaking ? UISneakOn->m_name = BSFixedString("UISneakOn") : UISneakOff->m_name = BSFixedString("UISneakOff");
		gPlayerisSneaking ? UIMain->AttachChild((NiAVObject*)UISneakOn, true) : UIMain->AttachChild((NiAVObject*)UISneakOff, true);
		gPlayerisSneaking ? UISneakOn->flags &= 0xfffffffffffffffe : UISneakOff->flags &= 0xfffffffffffffffe;

		// Add PA MODE Lable
		gInPowerArmor ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_PWRARMORON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_PWRARMOROFF.nif");
		gInPowerArmor ? UIPwrArmorOn = Offsets::cloneNode(retNode, &proc) : UIPwrArmorOff = Offsets::cloneNode(retNode, &proc);
		gInPowerArmor ? UIPwrArmorOn->m_name = BSFixedString("UIPwrArmorOn") : UIPwrArmorOff->m_name = BSFixedString("UIPwrArmorOff");
		gInPowerArmor ? UIMain->AttachChild((NiAVObject*)UIPwrArmorOn, true) : UIMain->AttachChild((NiAVObject*)UIPwrArmorOff, true);
		gInPowerArmor ? UIPwrArmorOn->flags &= 0xfffffffffffffffe : UIPwrArmorOff->flags &= 0xfffffffffffffffe;

		// Add HAPTICS MODE STATUS
		ghapticsEnabled ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_HAPTICSON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_HAPTICSOFF.nif");
		ghapticsEnabled ? UIHapticsOn = Offsets::cloneNode(retNode, &proc) : UIHapticsOff = Offsets::cloneNode(retNode, &proc);
		ghapticsEnabled ? UIHapticsOn->m_name = BSFixedString("UIHapticsOn") : UIHapticsOff->m_name = BSFixedString("UIHapticsOff");
		ghapticsEnabled ? UIMain->AttachChild((NiAVObject*)UIHapticsOn, true) : UIMain->AttachChild((NiAVObject*)UIHapticsOff, true);
		ghapticsEnabled ? UIHapticsOn->flags &= 0xfffffffffffffffe : UIHapticsOff->flags &= 0xfffffffffffffffe;

		// Add SPHERE MODE STATUS
		gshowSpheres ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SPHERESON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SPHERESOFF.nif");
		gshowSpheres ? UISpheresOn = Offsets::cloneNode(retNode, &proc) : UISpheresOff = Offsets::cloneNode(retNode, &proc);
		gshowSpheres ? UISpheresOn->m_name = BSFixedString("UISpheresOn") : UISpheresOff->m_name = BSFixedString("UISpheresOff");
		gshowSpheres ? UIMain->AttachChild((NiAVObject*)UISpheresOn, true) : UIMain->AttachChild((NiAVObject*)UISpheresOff, true);
		gshowSpheres ? UISpheresOn->flags &= 0xfffffffffffffffe : UISpheresOff->flags &= 0xfffffffffffffffe;

		// Add WEAPON MODE STATUS
		gShowWeapons ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_WEAPONSON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_WEAPONSOFF.nif");
		gShowWeapons ? UIWeaponsOn = Offsets::cloneNode(retNode, &proc) : UIWeaponsOff = Offsets::cloneNode(retNode, &proc);
		gShowWeapons ? UIWeaponsOn->m_name = BSFixedString("UIWeaponsOn") : UIWeaponsOff->m_name = BSFixedString("UIWeaponsOff");
		gShowWeapons ? UIMain->AttachChild((NiAVObject*)UIWeaponsOn, true) : UIMain->AttachChild((NiAVObject*)UIWeaponsOff, true);
		gShowWeapons ? UIWeaponsOn->flags &= 0xfffffffffffffffe : UIWeaponsOff->flags &= 0xfffffffffffffffe;

		// Add ACTIVATE MODE STATUS
		c_ActivationDisabled ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_ACTIVATEON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_ACTIVATEOFF.nif");
		c_ActivationDisabled ? UIActivateOn = Offsets::cloneNode(retNode, &proc) : UIActivateOff = Offsets::cloneNode(retNode, &proc);
		c_ActivationDisabled ? UIActivateOn->m_name = BSFixedString("UIACTIVATEOn") : UIActivateOff->m_name = BSFixedString("UIACTIVATEOff");
		c_ActivationDisabled ? UIMain->AttachChild((NiAVObject*)UIActivateOn, true) : UIMain->AttachChild((NiAVObject*)UIActivateOff, true);
		c_ActivationDisabled ? UIActivateOn->flags &= 0xfffffffffffffffe : UIActivateOff->flags &= 0xfffffffffffffffe;
		//_MESSAGE("Activation Message");

		// Add VATS MODE STATUS
		c_VATSDisabled ? retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_VATSON.nif") : retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_VATSOFF.nif");
		c_VATSDisabled ? UIVATSOn = Offsets::cloneNode(retNode, &proc) : UIVATSOff = Offsets::cloneNode(retNode, &proc);
		c_VATSDisabled ? UIVATSOn->m_name = BSFixedString("UIVATSOn") : UIVATSOff->m_name = BSFixedString("UIVATSOff");
		c_VATSDisabled ? UIMain->AttachChild((NiAVObject*)UIVATSOn, true) : UIMain->AttachChild((NiAVObject*)UIVATSOff, true);
		c_VATSDisabled ? UIVATSOn->flags &= 0xfffffffffffffffe : UIVATSOff->flags &= 0xfffffffffffffffe;
		//_MESSAGE("VATS Message");

		// Add CURRENT BUTTON STATUS
		for (int i = 0; i < 3; i++) {
				retNode = loadNifFromFile(nUI_InfoNodes[i]);
				niUIButton_Nodes[i] = Offsets::cloneNode(retNode, &proc);
				niUIButton_Nodes[i]->m_name = BSFixedString(mUI_InfoTiles[i]);
				UIMain->AttachChild((NiAVObject*)niUIButton_Nodes[i], true);
				niUIButton_Nodes[i]->flags &= 0xfffffffffffffffe;	       
		}
		if (c_holsterButtonID == 1) {
			niUIButton_Nodes[1]->m_localTransform.scale = 0.0;
			niUIButton_Nodes[2]->m_localTransform.scale = 0.0;
		}
		else if (c_holsterButtonID == 2) {
			niUIButton_Nodes[0]->m_localTransform.scale = 0.0;
			niUIButton_Nodes[2]->m_localTransform.scale = 0.0;
		}
		else if (c_holsterButtonID == 7) {
			niUIButton_Nodes[0]->m_localTransform.scale = 0.0;
			niUIButton_Nodes[1]->m_localTransform.scale = 0.0;
		}
		lastHolsterButton = c_holsterButtonID;
		//_MESSAGE("Button Message");
		// Hide 'Back of Hand' UI
		nodename = "BackOfHand";
		boneNode = FindNode(nodename);
		if (!boneNode) {
			return;
		}
		boneNode->flags |= 0x1;
		boneNode->m_localTransform.scale = 0;

		std::string hN = "Holster" + std::to_string(gCurHolster);
		NiNode* holster = FindNode(hN.c_str());
		if (holster) {
			holster->flags |= 0x1;
			holster->m_localTransform.scale = 0.0;
		}


		// Show Current Sphere 
		/*std::string hN = "Holster";
		hN += std::to_string(gCurHolster);
		char* c2 = new char[hN.size() + 1];
		std::copy(hN.begin(), hN.end(), c2);
		c2[hN.size()] = '\0';
		NiNode* holster = FindNode(c2);
		if (!holster) {
			return;
		}
		float radius = getRadius(gCurHolster);
		holster->flags &= 0xfffffffffffffffe;
		holster->m_localTransform.scale = (radius);*/

		// Setup & hide Messagebox to pause game / controls
		ShowMessagebox("Virtual Holsters Config Saved");
		nodename = "messageBoxMenuWider";
		boneNode = FindNode(nodename);
		if (!boneNode) {
			return;
		}
		boneNode->flags |= 0x1;
		boneNode->m_localTransform.scale = 0;
		configMode = 2;
		configmodeupdate(2);
		_isTouchEnabled = false;
		SetPointingHand();
	}

	void exitConfigMode() {
		writeOffsetJson();
		BSFixedString nodename;
		NiNode* boneNode = nullptr;
		// Restore Arrays & Variables to nullptr
		for (int i = 0; i <= 9; i++) {
			niUI_Nodes[i] = nullptr;
			niUI_Tiles[i] = nullptr;
			bUITilePressed[i] = false;

		}
		UIModeLable = nullptr;
		UIHolsterLable = nullptr;
		UISneakOn = nullptr;
		UISneakOff = nullptr;
		UIPwrArmorOn = nullptr;
		UIPwrArmorOff = nullptr;
		UIHapticsOn = nullptr;
		UIHapticsOff = nullptr;
		UISpheresOn = nullptr;
		UISpheresOff = nullptr;
		UIWeaponsOn = nullptr;
		UIWeaponsOff = nullptr;
		UISaveMSG = nullptr;
		UIActivateOn = nullptr;
		UIActivateOff = nullptr;
		UIVATSOn = nullptr;
		UIVATSOff = nullptr;
		UIBUTTON01 = nullptr;
		UIBUTTON02 = nullptr;
		UIBUTTON03 = nullptr;
		primaryAttachNode = nullptr;

		// Remove Main Config Mode UI
        if (UIMain != nullptr) {
			UIMain->flags |= 0x1;
			UIMain->m_localTransform.scale = 0;
			UIMain->m_parent->RemoveChild(UIMain);
			UIMain = nullptr;
		}
		// UnHide 'Back of Hand' UI
		nodename = "BackOfHand";
		boneNode = FindNode(nodename);
		if (!boneNode) {
			return;
		}
		boneNode->flags &= 0xfffffffffffffffe;
		boneNode->m_localTransform.scale = 1;
		// Restore Messagebox visibility
		nodename = "messageBoxMenuWider";
		boneNode = FindNode(nodename);
		if (!boneNode) {
			return;
		}
		boneNode->flags &= 0xfffffffffffffffe;
		boneNode->m_localTransform.scale = 1.0;
		std::string hN = "Holster";
		hN += std::to_string(gCurHolster);
		char* c2 = new char[hN.size() + 1];
		std::copy(hN.begin(), hN.end(), c2);
		c2[hN.size()] = '\0';
		boneNode = FindNode(c2);
		if (!boneNode) {
			return;
		}
		boneNode->flags |= 0x1;
		boneNode->m_localTransform.scale = 0.0;
		
		BSFixedString NameofHELP = "VRHHELPMain";
		boneNode = FindNode(NameofHELP);
		if (boneNode) {
			boneNode->flags |= 0x1;
			boneNode->m_localTransform.scale = 0;
			boneNode->m_parent->RemoveChild(boneNode);
		}
		primaryAttachNode = nullptr;
		closeSelfieMode();
		retorecam();
		RestorePointingHand();
	}

	void changeConfigMode() {
		NiNode* boneNode = nullptr;
		NiNode* UIItemMain = nullptr;	
		NiNode* retNode = nullptr;
		if (configMode == 1) {
			retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_MODE02.nif");
			configMode = 2;
			configmodeupdate(2);
			std::string hN = "Holster" + std::to_string(gCurHolster);
			NiNode* holster = FindNode(hN.c_str());
			if (!holster) {
				return;
			}
			holster->flags |= 0x1;
			holster->m_localTransform.scale = 0.0;
		}
		else {
			retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_MODE01.nif");
			configMode = 1;
			configmodeupdate(1);
			std::string hN = "Holster" + std::to_string(gCurHolster);
			NiNode* holster = FindNode(hN.c_str());
			if (!holster) {
				return;
			}
			float radius = getRadius(gCurHolster);
			holster->flags &= 0xfffffffffffffffe;
			holster->m_localTransform.scale = (radius);
		}
		if (UIModeLable != nullptr) {
			UIModeLable->flags |= 0x1;
			UIModeLable->m_localTransform.scale = 0;
			if (UIMain != nullptr) {
				UIMain->RemoveChild(UIModeLable);
			}
			UIModeLable = nullptr;
			NiCloneProcess proc;
			proc.unk18 = Offsets::cloneAddr1;
			proc.unk48 = Offsets::cloneAddr2;
			UIModeLable = Offsets::cloneNode(retNode, &proc);
			UIModeLable->m_name = BSFixedString("UI-INFO_MODE");
			if (UIMain != nullptr) {
				UIMain->AttachChild((NiAVObject*)UIModeLable, true);
				UIModeLable->flags &= 0xfffffffffffffffe;
			}
		}
	}

	void switchHolster() {
		NiNode* holster = nullptr;
		// Hide Previous Holster Sphere
		std::string hN;
		if (configMode == 1) {
			hN = "Holster" + std::to_string(gCurHolster);
			holster = FindNode(hN.c_str());
			if (!holster) {
				return;
			}
			holster->flags |= 0x1;
			holster->m_localTransform.scale = 0.0;
		}
		// iterate current holster number by 1
		gCurHolster += 1;
			if (gCurHolster > 7) {
				gCurHolster = 1;
		}
		// Remove old holster UI lable 
		NiNode* boneNode = nullptr;
		BSFixedString nodename;
		if (UIHolsterLable == nullptr) {
			return;
		}
		else {
			UIHolsterLable->flags |= 0x1;
			UIHolsterLable->m_localTransform.scale = 0;
			if (UIMain != nullptr) {
				UIMain->RemoveChild(UIHolsterLable);
			}
			UIHolsterLable = nullptr;
		}
		nodename = "primaryUIAttachNode";
		NiNode* retNode = loadNifFromFile(mUI_Info_Holster[gCurHolster]);
		NiCloneProcess proc;
		proc.unk18 = Offsets::cloneAddr1;
		proc.unk48 = Offsets::cloneAddr2;
		UIHolsterLable = Offsets::cloneNode(retNode, &proc);
		UIHolsterLable->m_name = BSFixedString("CurrentHolster");
		UIMain->AttachChild((NiAVObject*)UIHolsterLable, true);
		UIHolsterLable->flags &= 0xfffffffffffffffe;
		// Show Next Sphere
		if (configMode == 1) {
			hN = "Holster" + std::to_string(gCurHolster);
			holster = FindNode(hN.c_str());
			if (!holster) {
				return;
			}
			float radius = getRadius(gCurHolster);
			holster->flags &= 0xfffffffffffffffe;
			holster->m_localTransform.scale = (radius);
		}
		//holster->m_localTransform.scale = (Holsters::HolsterRegisteredObjects[gCurHolster]->radius * 2);
	}

	void updateSneakUI() {
		NiCloneProcess proc;
		proc.unk18 = Offsets::cloneAddr1;
		proc.unk48 = Offsets::cloneAddr2;
		if (!gPlayerisSneaking) {
			if (UISneakOn != nullptr) {
				UISneakOn->flags |= 0x1;
				UISneakOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UISneakOn);
				}
				UISneakOn = nullptr;
			}
			if (UISneakOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SNEAKOFF.nif");
				UISneakOff = Offsets::cloneNode(retNode, &proc);
				UISneakOff->m_name = BSFixedString("UISneakOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UISneakOff, true);
					UISneakOff->flags &= 0xfffffffffffffffe;
					UISneakOff->m_localTransform.scale = 1;
				}
			}
		}
		else {
			if (UISneakOff != nullptr) {
				UISneakOff->flags |= 0x1;
				UISneakOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UISneakOff);
				}
				UISneakOff = nullptr;
			}
			if (UISneakOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SNEAKON.nif");
				UISneakOn = Offsets::cloneNode(retNode, &proc);
				UISneakOn->m_name = BSFixedString("UISneakOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UISneakOn, true);
					UISneakOn->flags &= 0xfffffffffffffffe;
					UISneakOn->m_localTransform.scale = 1;
				}
			}
		}
	}

	void updateUITiles() {
		BSFlattenedBoneTree* rt = (BSFlattenedBoneTree*)(*g_player)->unkF0->rootNode->m_children.m_data[0]->GetAsNiNode();
		NiPoint3 lFinger;
		gIsRightHanded ? lFinger = rt->transforms[boneTreeMap["LArm_Finger23"]].world.pos : lFinger = rt->transforms[boneTreeMap["RArm_Finger23"]].world.pos;
		for (int i = 0; i <= 11; i++) {
			//if (i == 9) {
				//continue;
			//}
			if (niUI_Nodes[i] && niUI_Tiles[i]) {
				float distance = vec3_len(lFinger - niUI_Tiles[i]->m_worldTransform.pos);
				if (distance > 2.0) {
					niUI_Nodes[i]->m_localTransform.pos.y = 0.0;
					bUITilePressed[i] = false;
				}
				else if (distance <= 2.0) {
					float fz = (2.0 - distance);
					if (fz > 0.0 && fz < 1.2) {
						niUI_Nodes[i]->m_localTransform.pos.y = (fz);
					}
					if ((niUI_Nodes[i]->m_localTransform.pos.y > 1.0) && !bUITilePressed[i]) {
						bUITilePressed[i] = true;
						gIsRightHanded ? vrHook->StartHaptics(1, 0.05, 0.3) : vrHook->StartHaptics(2, 0.05, 0.3);
						funcs[i]();
					}
				}
			}
		}
	}

	void updatePAUI() {
		NiCloneProcess proc;
		proc.unk18 = Offsets::cloneAddr1;
		proc.unk48 = Offsets::cloneAddr2;
		if (!gInPowerArmor) {
			if (UIPwrArmorOn != nullptr) {
				UIPwrArmorOn->flags |= 0x1;
				UIPwrArmorOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIPwrArmorOn);
				}
				UIPwrArmorOn = nullptr;
			}
			if (UIPwrArmorOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_PWRARMOROFF.nif");
				UIPwrArmorOff = Offsets::cloneNode(retNode, &proc);
				UIPwrArmorOff->m_name = BSFixedString("UIPwrArmorOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIPwrArmorOff, true);
					UIPwrArmorOff->flags &= 0xfffffffffffffffe;
					UIPwrArmorOff->m_localTransform.scale = 1;
				}
			}
		}
		else {
			if (UIPwrArmorOff != nullptr) {
				UIPwrArmorOff->flags |= 0x1;
				UIPwrArmorOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIPwrArmorOff);
				}
				UIPwrArmorOff = nullptr;
			}
			if (UIPwrArmorOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_PWRARMORON.nif");
				UIPwrArmorOn = Offsets::cloneNode(retNode, &proc);
				UIPwrArmorOn->m_name = BSFixedString("UIPwrArmorOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIPwrArmorOn, true);
					UIPwrArmorOn->flags &= 0xfffffffffffffffe;
					UIPwrArmorOn->m_localTransform.scale = 1;
				}
			}
		}
	}

	void updateUIInfo() {
		NiCloneProcess proc;
		proc.unk18 = Offsets::cloneAddr1;
		proc.unk48 = Offsets::cloneAddr2;
		if (!ghapticsEnabled) {
			if (UIHapticsOn != nullptr) {
				UIHapticsOn->flags |= 0x1;
				UIHapticsOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIHapticsOn);
				}
				UIHapticsOn = nullptr;
			}
			if (UIHapticsOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_HAPTICSOFF.nif");
				UIHapticsOff = Offsets::cloneNode(retNode, &proc);
				UIHapticsOff->m_name = BSFixedString("UIHapticsOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIHapticsOff, true);
					UIHapticsOff->flags &= 0xfffffffffffffffe;
					UIHapticsOff->m_localTransform.scale = 1;
				}
			}
		}
		else if (ghapticsEnabled) {
			if (UIHapticsOff != nullptr) {
				UIHapticsOff->flags |= 0x1;
				UIHapticsOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIHapticsOff);
				}
				UIHapticsOff = nullptr;
			}
			if (UIHapticsOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_HAPTICSON.nif");
				UIHapticsOn = Offsets::cloneNode(retNode, &proc);
				UIHapticsOn->m_name = BSFixedString("UIHapticsOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIHapticsOn, true);
					UIHapticsOn->flags &= 0xfffffffffffffffe;
					UIHapticsOn->m_localTransform.scale = 1;
				}
			}
		}
		if (!gshowSpheres) {
			if (UISpheresOn != nullptr) {
				UISpheresOn->flags |= 0x1;
				UISpheresOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UISpheresOn);
				}
				UISpheresOn = nullptr;
			}
			if (UISpheresOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SPHERESOFF.nif");
				UISpheresOff = Offsets::cloneNode(retNode, &proc);
				UISpheresOff->m_name = BSFixedString("UISpheresOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UISpheresOff, true);
					UISpheresOff->flags &= 0xfffffffffffffffe;
					UISpheresOff->m_localTransform.scale = 1;
				}
			}
		}
		else if (gshowSpheres) {
			if (UISpheresOff != nullptr) {
				UISpheresOff->flags |= 0x1;
				UISpheresOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UISpheresOff);
				}
				UISpheresOff = nullptr;
			}
			if (UISpheresOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SPHERESON.nif");
				UISpheresOn = Offsets::cloneNode(retNode, &proc);
				UISpheresOn->m_name = BSFixedString("UISpheresOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UISpheresOn, true);
					UISpheresOn->flags &= 0xfffffffffffffffe;
					UISpheresOn->m_localTransform.scale = 1;
				}
			}
		}
		if (!gShowWeapons) {
			if (UIWeaponsOn != nullptr) {
				UIWeaponsOn->flags |= 0x1;
				UIWeaponsOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIWeaponsOn);
				}
				UIWeaponsOn = nullptr;
			}
			if (UIWeaponsOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_WEAPONSOFF.nif");
				UIWeaponsOff = Offsets::cloneNode(retNode, &proc);
				UIWeaponsOff->m_name = BSFixedString("UIWeaponsOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIWeaponsOff, true);
					UIWeaponsOff->flags &= 0xfffffffffffffffe;
					UIWeaponsOff->m_localTransform.scale = 1;
				}
			}
		}
		else if (gShowWeapons) {
			if (UIWeaponsOff != nullptr) {
				UIWeaponsOff->flags |= 0x1;
				UIWeaponsOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIWeaponsOff);
				}
				UIWeaponsOff = nullptr;
			}
			if (UIWeaponsOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_WEAPONSON.nif");
				UIWeaponsOn = Offsets::cloneNode(retNode, &proc);
				UIWeaponsOn->m_name = BSFixedString("UIWeaponsOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIWeaponsOn, true);
					UIWeaponsOn->flags &= 0xfffffffffffffffe;
					UIWeaponsOn->m_localTransform.scale = 1;
				}
			}
		}
		if (!c_ActivationDisabled) {
			if (UIActivateOn != nullptr) {
				UIActivateOn->flags |= 0x1;
				UIActivateOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIActivateOn);
				}
				UIActivateOn = nullptr;
			}
			if (UIActivateOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_ACTIVATEOFF.nif");
				UIActivateOff = Offsets::cloneNode(retNode, &proc);
				UIActivateOff->m_name = BSFixedString("UIACTIVATEOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIActivateOff, true);
					UIActivateOff->flags &= 0xfffffffffffffffe;
					UIActivateOff->m_localTransform.scale = 1;
				}
			}
		}
		else if (c_ActivationDisabled) {
			if (UIActivateOff != nullptr) {
				UIActivateOff->flags |= 0x1;
				UIActivateOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIActivateOff);
				}
				UIActivateOff = nullptr;
			}
			if (UIActivateOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_ACTIVATEON.nif");
				UIActivateOn = Offsets::cloneNode(retNode, &proc);
				UIActivateOn->m_name = BSFixedString("UIACTIVATEOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIActivateOn, true);
					UIActivateOn->flags &= 0xfffffffffffffffe;
					UIActivateOn->m_localTransform.scale = 1;
				}
			}
		}
		if (!c_VATSDisabled) {
			if (UIVATSOn != nullptr) {
				UIVATSOn->flags |= 0x1;
				UIVATSOn->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIVATSOn);
				}
				UIVATSOn = nullptr;
			}
			if (UIVATSOff == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_VATSOFF.nif");
				UIVATSOff = Offsets::cloneNode(retNode, &proc);
				UIVATSOff->m_name = BSFixedString("UIVATSOff");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIVATSOff, true);
					UIVATSOff->flags &= 0xfffffffffffffffe;
					UIVATSOff->m_localTransform.scale = 1;
				}
			}
		}
		else if (c_VATSDisabled) {
			if (UIVATSOff != nullptr) {
				UIVATSOff->flags |= 0x1;
				UIVATSOff->m_localTransform.scale = 0;
				if (UIMain != nullptr) {
					UIMain->RemoveChild(UIVATSOff);
				}
				UIVATSOff = nullptr;
			}
			if (UIVATSOn == nullptr) {
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_VATSON.nif");
				UIVATSOn = Offsets::cloneNode(retNode, &proc);
				UIVATSOn->m_name = BSFixedString("UIVATSOn");
				if (UIMain != nullptr) {
					UIMain->AttachChild((NiAVObject*)UIVATSOn, true);
					UIVATSOn->flags &= 0xfffffffffffffffe;
					UIVATSOn->m_localTransform.scale = 1;
				}
			}
		}
		if (c_holsterButtonID != lastHolsterButton) {
			lastHolsterButton = c_holsterButtonID;
			if (c_holsterButtonID == 1) {
				niUIButton_Nodes[0]->m_localTransform.scale = 1.0;
				niUIButton_Nodes[1]->m_localTransform.scale = 0.0;
				niUIButton_Nodes[2]->m_localTransform.scale = 0.0;
			}
			else if (c_holsterButtonID == 2) {
				niUIButton_Nodes[0]->m_localTransform.scale = 0.0;
				niUIButton_Nodes[1]->m_localTransform.scale = 1.0;
				niUIButton_Nodes[2]->m_localTransform.scale = 0.0;
			}
			else if (c_holsterButtonID == 7) {
				niUIButton_Nodes[0]->m_localTransform.scale = 0.0;
				niUIButton_Nodes[1]->m_localTransform.scale = 0.0;
				niUIButton_Nodes[2]->m_localTransform.scale = 1.0;
			}
		}
	}

	void UITile01Function() {
		toggleSwitchLGA();
	}

	void UITile02Function() {
		toggleHaptics();
	}

	void UITile03Function() {
		toggleWeapVis();
	}

	void UITile04Function() {
		toggleSphereVis();
	}

	void UITile05Function() {
		changeConfigMode();
	}

	void UITile06Function() {
		ChangeVATS();
	}

	void UITile07Function() {
		toggleSwitchRGA();
	}

	void UITile08Function() {
		//pressArtWorkChange(gCurHolster);
		if (c_VATSDisabled) {
			if (c_holsterButtonID == 1) {
				c_holsterButtonID = 2;
			}
			else if (c_holsterButtonID == 2) {
				c_holsterButtonID = 7;
			}
			else if (c_holsterButtonID == 7) {
				c_holsterButtonID = 1;
			}
			else {
				c_holsterButtonID = 2;
			}
		}
		else {
			if (c_holsterButtonID == 1) {
				c_holsterButtonID = 2;
			}
			else if (c_holsterButtonID == 2) {
				c_holsterButtonID = 7;
			}
			else if (c_holsterButtonID == 7) {
				c_holsterButtonID = 2;
			}
			else {
				c_holsterButtonID = 2;
			}
		}
		swapHolsterButton();
	}

	void UITile09Function() {
		restoreAllWeapons();
	}

	void UITile10Function() {
		saveconfig();
		if (UISaveMSG == nullptr) {
			NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-INFO_SAVEMESSAGE.nif");
			NiCloneProcess proc;
			proc.unk18 = Offsets::cloneAddr1;
			proc.unk48 = Offsets::cloneAddr2;
			UISaveMSG = Offsets::cloneNode(retNode, &proc);
			UISaveMSG->m_name = BSFixedString("UIINFOSAVEMESSAGE");
			if (UIMain != nullptr) {
				UIMain->AttachChild((NiAVObject*)UISaveMSG, true);
				UISaveMSG->flags &= 0xfffffffffffffffe;
			}
			std::thread t1(UISaveMSGTimer);
			t1.detach();
		}
		else {
			UISaveMSG->flags &= 0xfffffffffffffffe;
			UISaveMSG->m_localTransform.scale = 1;
			std::thread t1(UISaveMSGTimer);
			t1.detach();
		}
	}

	void UITile11Function() {
		if (primaryAttachNode) {
			NiNode* NodeToFind;
			BSFixedString NameofMesh = "VRHHELPMain";
			NodeToFind = FindNode(NameofMesh);
			if (!NodeToFind && UIMain) {
				BSFixedString NameofMesh = "VRHHELPMain";
				NiNode* retNode = loadNifFromFile("Data/Meshes/VRH/UI-HELPSCREEN.nif");
				NiCloneProcess proc;
				NiNode* HelpUI;
				proc.unk18 = Offsets::cloneAddr1;
				proc.unk48 = Offsets::cloneAddr2;
				HelpUI = Offsets::cloneNode(retNode, &proc);
				HelpUI->m_name = BSFixedString(NameofMesh);
				primaryAttachNode->AttachChild((NiAVObject*)HelpUI, true);
				gInPowerArmor ? HelpUI->m_localTransform.scale = 1.0 : HelpUI->m_localTransform.scale = 0.9;
				HelpUI->flags &= 0xfffffffffffffffe;
			}
			else if (NodeToFind){
				NodeToFind->flags |= 0x1;
				NodeToFind->m_localTransform.scale = 0;
				NodeToFind->m_parent->RemoveChild(NodeToFind);
			}
		}
	}

	void UITile12Function() {
		if (c_ActivationDisabled) {
			c_ActivationDisabled = false;
		}
		else {
			c_ActivationDisabled = true;
		}
		ActivationToggle();
	}

	void UISaveMSGTimer() {
		Sleep(1500);
		if (UISaveMSG != nullptr) {
			UISaveMSG->flags |= 0x1;
			UISaveMSG->m_localTransform.scale = 0;
		}
	}
}