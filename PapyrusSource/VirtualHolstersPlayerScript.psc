Scriptname VirtualHolstersPlayerScript extends ReferenceAlias

Quest Property VirtualHolsterQuest Auto Const Mandatory

Event OnPlayerLoadGame()
    Utility.Wait(1)
    If ((VirtualHolsterQuest as virtualholstersquestscript).FirstRun != True)
        (VirtualHolsterQuest as virtualholstersquestscript).SetupHolsters()
    endif	
EndEvent

Event OnInit()
	(VirtualHolsterQuest as virtualholstersquestscript).SetupHolsters()
EndEvent






