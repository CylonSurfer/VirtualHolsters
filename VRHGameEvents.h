#pragma once
#include "f4se/GameEvents.h"
#include "f4se/GameMenus.h"
#include "Holsters.h"

namespace Holsters {


	class OnDeathEventHandler : public BSTEventSink<TESDeathEvent> {
	public:
		virtual EventResult ReceiveEvent(TESDeathEvent* a_event, void* dispatcher) override;


		static void Register()
		{
			static auto* pHandler = new OnDeathEventHandler();
			GetEventDispatcher<TESDeathEvent>()->AddEventSink(pHandler);
		}
	};

	extern OnDeathEventHandler onDeathEvent;

	class MessageBoxEventHandler : public BSTEventSink<MenuOpenCloseEvent> {
	public:
		virtual EventResult ReceiveEvent(MenuOpenCloseEvent* a_event, void* dispatcher) override;


		static void Register()
		{
			static auto* pHandler2 = new MessageBoxEventHandler();
			(*g_ui)->menuOpenCloseEventSource.AddEventSink(pHandler2);
		}
	};

	extern MessageBoxEventHandler MessageBoxMenuEvent;
	bool isInMSGBOXMenu();
}