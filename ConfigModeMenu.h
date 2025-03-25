#pragma once
#include "f4se/GameReferences.h"
#include "f4se/GameObjects.h"
#include "f4se/NiNodes.h"
#include "f4se/NiObjects.h"
#include "f4se/BSGeometry.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "Holsters.h"


namespace Holsters {
	extern int configMode;
	void setupConfigMode();
	void exitConfigMode();
	void updateSneakUI();
	void updatePAUI();
	void changeConfigMode();
	void switchHolster();
	void updateUITiles();
	void updateUIInfo();
	void UITile01Function();
	void UITile02Function();
	void UITile03Function();
	void UITile04Function();
	void UITile05Function();
	void UITile06Function();
	void UITile07Function();
	void UITile08Function();
	void UITile09Function();
	void UITile10Function();
	void UITile11Function();
	void UITile12Function();
	void UISaveMSGTimer();
}
