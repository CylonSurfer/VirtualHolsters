Scriptname VirtualHolstersQuestScript extends Quest
;==============
;Imported Libs
;==============
Import FRIK:FRIK
Import HolsterFunctions
;===========
;Properties
;===========
Actor Property PlayerRef Auto Const
Bool Property FirstRun = True Auto
Bool Startup=False
InputEnableLayer NoActivateLayer
Sound Property WPNSwordChineseEquipUp Auto Const Mandatory


;=================================================================================================================================================================================================================================================================================
;=================================================================================================================================================================================================================================================================================
;Default State (Used for everything except 'Sphere Calibration Mode'
;=================================================================================================================================================================================================================================================================================
;=================================================================================================================================================================================================================================================================================
Event Oninit()
	;SetupHolsters()
EndEvent

Function SetupHolsters()
    FirstRun=False
	RegisterForHolsterEvents(Self)
	NoActivateLayer = InputEnableLayer.Create()	
Endfunction

;========================================================================================================================
; Sphere Overlap Events 
;========================================================================================================================
Function HolsterEvent(int EventType, int HolsterNumber)
	
If EventType == 1   ;toggle FRIK selfie Mode
    toggleSelfieMode()
Endif 

If EventType == 2  ;Move Selfie Mode Back
    int i = 1
    While i < 10
        moveBackward()
		i+=1
	EndWhile
Endif

If EventType == 3  ;Move Selfie Mode Forward;
    int i = 1
    While i < 10
        moveForward()
		i+=1
	EndWhile
Endif

If EventType == 4  ;Fix stuck on fists Melee event
    if NoActivateLayer.IsFightingEnabled()
	    NoActivateLayer.EnableFighting(false)
		NoActivateLayer.EnableFighting()
	endif
Endif

If EventType == 5   ;Set Config Mode left hand
    setFingerPositionScalar(true, 0.0, 1.0, 0.0, 0.0, 0.0) 
Endif

If EventType == 6   ;Set Config Mode Right hand
    setFingerPositionScalar(false, 0.0, 1.0, 0.0, 0.0, 0.0)
Endif

If EventType == 7   ;Restore Left Hand Pose
    restoreFingerPoseControl(true)
Endif

If EventType == 8   ;Restore Right Hand Pose
    restoreFingerPoseControl(false)
Endif

if EventType == 9  ;Disable VATS
    NoActivateLayer.EnableVATS(false)
Endif

if EventType == 10  ;Activate Fix On Event
	NoActivateLayer.EnableMenu(false)
	NoActivateLayer.EnableActivate(false)
Endif

if EventType == 11  ;Activate Fix Off Event
	NoActivateLayer.EnableMenu()
	NoActivateLayer.EnableActivate()
Endif

If EventType == 12   ;Set Left Hand Fist
    setFingerPositionScalar(true, 0.0, 0.0, 0.0, 0.0, 0.0)
Endif

if EventType == 13  ;Enable VATS
    NoActivateLayer.EnableVATS(true)
Endif

if EventType == 14  ;Disable Favorites
    NoActivateLayer.EnableFavorites(false) 
Endif

if EventType == 15  ;Enable Favorites
    NoActivateLayer.EnableFavorites(true) 
Endif

if EventType == 16  ;Play Sword SFX on Equip Event
    int instanceID=WPNSwordChineseEquipUp.play(PlayerRef)
	Sound.SetInstanceVolume(instanceID, 0.1)
Endif

if EventType == 17  ;UnEquip Current Weapon
    Form HeldWeapon = PlayerRef.GetEquippedWeapon() as form
    PlayerRef.UnequipItem(HeldWeapon, False, True)
Endif 
	
EndFunction	


