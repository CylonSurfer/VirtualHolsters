
#include "common/IDebugLog.h"  // IDebugLog
#include "f4se_common/f4se_version.h"  // RUNTIME_VERSION
#include "f4se/PluginAPI.h"  // SKSEInterface, PluginInfo
#include "f4sE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "F4SE_common/BranchTrampoline.h"
#include "hook.h"
#include <ShlObj.h>  // CSIDL_MYDOCUMENTS
#include "VRHook.h"
#include "version.h"
#include "Holsters.h"
#include "MenuChecker.h"

void OnF4SEMessage(F4SEMessagingInterface::Message* msg)
{
	if (msg)
	{
		if (msg->type == F4SEMessagingInterface::kMessage_GameLoaded) {
			Holsters::MenuOpenCloseHandler::Register();
			VRHook::InitVRSystem();
			Holsters::mainStartup();
		}

		if (msg->type == F4SEMessagingInterface::kMessage_PostLoadGame)
		{
			//_MESSAGE("Player Save Loaded");
			Holsters::saveGameLoaded();
		}
	}
}

extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* a_f4se, PluginInfo* a_info)
	{
		Sleep(5000);
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, R"(\\My Games\\Fallout4VR\\F4SE\\VirtualHolsters.log)");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("VirtualHolsters v%s", VirtualHolsters_VERSION_VERSTRING);

		a_info->infoVersion = PluginInfo::kInfoVersion;
		a_info->name = "VirtualHolsters";
		a_info->version = VirtualHolsters_VERSION_MAJOR;

		if (a_f4se->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		}

		a_f4se->runtimeVersion;
		if (a_f4se->runtimeVersion < RUNTIME_VR_VERSION_1_2_72)
		{
			_FATALERROR("Unsupported runtime version %s!\n", a_f4se->runtimeVersion);
			return false;
		}

		return true;
	}


	bool F4SEPlugin_Load(const F4SEInterface* a_f4se)
	{
		_MESSAGE("Virtual Holsters Init");

		g_pluginHandle = a_f4se->GetPluginHandle();

		if (g_pluginHandle == kPluginHandle_Invalid) {
			return false;
		}

		g_messaging = (F4SEMessagingInterface*)a_f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage)) {
			_MESSAGE("FAILED TO REGISTER F4SEMessagingInterface");
		}
		_MESSAGE("F4SEMessagingInterface Loaded");

		if (!g_branchTrampoline.Create(1024 * 128))
		{
			_ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return false;
		}

		g_papyrus = (F4SEPapyrusInterface*)a_f4se->QueryInterface(kInterface_Papyrus);


		_MESSAGE("Registering Papyrus Functions");

		if (!g_papyrus->Register(Holsters ::RegisterFuncs)) {
			_MESSAGE("FAILED TO REGISTER PAPYRUS FUNCTIONS!!");
			return false;
		}

		hookMain();
		_MESSAGE("Virtual Holsters Loaded");	
		return true;
	}
};