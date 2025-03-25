#include "hook.h"
#include "Holsters.h"
#include "xbyak/xbyak.h"

RelocAddr<uintptr_t> hookMainLoopFunc(0xd8187e);

typedef void(*_hookedMainLoop)();
RelocAddr<_hookedMainLoop> hookedMainLoop(0xd83ac0);

void hookMain() {
	g_branchTrampoline.Write5Call(hookMainLoopFunc.GetUIntPtr(), (uintptr_t)updateCounter);
}


void updateCounter() {
	Holsters::MainLoop();
	hookedMainLoop();	
}