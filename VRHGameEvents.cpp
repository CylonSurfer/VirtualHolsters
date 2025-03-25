#include "VRHGameEvents.h"
#include "f4sE_common/Relocation.h"
#include "F4SE_common/SafeWrite.h"
#include "utils.h"

namespace Holsters {

	MessageBoxEventHandler MessageBoxMenuEvent;
	OnDeathEventHandler onDeathEvent;
	bool inMSGBOXMenu = false;

	bool isInMSGBOXMenu() {
		return inMSGBOXMenu;
	}
	
	EventResult OnDeathEventHandler::ReceiveEvent(TESDeathEvent* a_event, void* dispatcher) {
		if (a_event->source == *g_player) {
			_MESSAGE("EVENT:: PLAYER DIED");
			return EventResult::kEvent_Continue;
		}		
	}

	EventResult MessageBoxEventHandler::ReceiveEvent(MenuOpenCloseEvent* a_event, void* dispatcher) {
		const char* name = a_event->menuName.c_str();
		if (!_stricmp(name, "MessageBoxMenu")) {
			if (a_event->isOpen) {
				inMSGBOXMenu = true;
				return EventResult::kEvent_Continue;
			}
			else {
				_MESSAGE("Messagebox closed");
				inMSGBOXMenu = false;
				return EventResult::kEvent_Continue;
			}
		}	
	}

}