#pragma once

// Credit: With thanks to Asciimov.

/**
 * VirtualHolsters API
 *
 * Exposes holster state information for external mods (like HIGGS/Heisenberg)
 * to query holster zone status and weapon storage state.
 *
 * Usage from external mods:
 *
 *     #include "VirtualHolstersAPI.h"
 *
 *     // Get the API singleton
 *     auto* api = RequestVirtualHolstersAPI();
 *     if (api && api->GetVersion() >= 1) {
 *         bool inZone = api->IsHandInHolsterZone(false);  // false = right hand
 *         UInt32 holster = api->GetCurrentHolster();
 *         bool free = api->IsHolsterFree(holster);
 *         // ...
 *     }
 */

#include <cstdint>
#include <windows.h>

 // Define VH_CALL as __cdecl for ABI stability
#ifndef VH_CALL
#define VH_CALL __cdecl
#endif

/**
 * VirtualHolsters API interface
 *
 * Provides access to holster zone detection and weapon storage state.
 */
class VirtualHolstersAPI
{
public:
    /**
     * Get API version number. Currently returns 1.
     * Check this before calling other functions.
     */
    virtual std::uint32_t VH_CALL GetVersion() const = 0;

    /**
     * Check if a hand is currently inside a holster zone.
     *
     * @param isLeft  true = left hand, false = right hand
     * @return true if the hand is in a holster zone
     */
    virtual bool VH_CALL IsHandInHolsterZone(bool isLeft) const = 0;

    /**
     * Get the current/most recent holster index that a hand entered.
     *
     * Holster indices:
     *   0 = None (not in holster)
     *   1 = Left Shoulder
     *   2 = Right Shoulder
     *   3 = Left Hip
     *   4 = Right Hip
     *   5 = Lower Back
     *   6 = Left Chest
     *   7 = Right Chest
     *
     * @return Holster index (0-7)
     */
    virtual std::uint32_t VH_CALL GetCurrentHolster() const = 0;

    /**
     * Check if a specific holster slot is empty (no weapon stored).
     *
     * @param holsterIndex  Holster index (1-7, see GetCurrentHolster for meanings)
     * @return true if the holster is empty/free
     */
    virtual bool VH_CALL IsHolsterFree(std::uint32_t holsterIndex) const = 0;

    /**
     * Get the name of the weapon stored in a holster slot.
     *
     * @param holsterIndex  Holster index (1-7)
     * @return Weapon name string, or "Empty" if no weapon
     */
    virtual const char* VH_CALL GetHolsteredWeaponName(std::uint32_t holsterIndex) const = 0;

    /**
     * Check if a weapon (by name) is already stored in any holster.
     *
     * @param weaponName  Name of the weapon to check
     * @return true if the weapon is already in a holster somewhere
     */
    virtual bool VH_CALL IsWeaponAlreadyHolstered(const char* weaponName) const = 0;

    /**
     * Get holster center position in world space.
     *
     * @param holsterIndex  Holster index (1-7)
     * @param outX, outY, outZ  Output position coordinates
     * @return true if position was retrieved successfully
     */
    virtual bool VH_CALL GetHolsterPosition(std::uint32_t holsterIndex, float& outX, float& outY, float& outZ) const = 0;

    /**
     * Get holster sphere radius.
     *
     * @param holsterIndex  Holster index (1-7)
     * @return Radius of the holster zone sphere
     */
    virtual float VH_CALL GetHolsterRadius(std::uint32_t holsterIndex) const = 0;

    /**
     * Check if Virtual Holsters is fully initialized and ready.
     *
     * @return true if initialized and holsters are set up
     */
    virtual bool VH_CALL IsInitialized() const = 0;
    /**
     * Check if the Grip button is assigned as the holster button.
     * If true, external mods should avoid using grip in holster zones.
     *
     * @return true if grip (k_EButton_Grip = 2) is the holster button
     */
    virtual bool VH_CALL IsGripAssignedToHolster() const = 0;

    /**
     * Get the current holster button ID (OpenVR EVRButtonId).
     *
     * Common values:
     *   1 = k_EButton_ApplicationMenu (menu button)
     *   2 = k_EButton_Grip
     *   7 = k_EButton_A
     *   32 = k_EButton_SteamVR_Touchpad
     *   33 = k_EButton_SteamVR_Trigger
     *
     * @return OpenVR button ID for holstering
     */
    virtual std::uint32_t VH_CALL GetHolsterButtonId() const = 0;

    /**
     * Check if Virtual Holsters is in left-handed mode.
     * This mirrors the game's bLeftHandedMode:VR INI setting.
     *
     * @return true if left-handed mode is enabled
     */
    virtual bool VH_CALL IsLeftHandedMode() const = 0;

    /**
    *Switches to the opposite hand for holster interaction / detection
    */
    virtual void VH_CALL switchHandMode() const = 0;

    /**
     * Add a grabbed weapon directly to a holster slot (Cylon's new API).
     * This combines registration + name saving in one call.
     *
     * @param holsterIndex  Holster index (1-7)
     * @param weaponName    ETDD name, or full weapon name as fallback
     * @param baseForm      TESForm* of the weapon's base object
     * @param weaponRef     TESObjectREFR* of the grabbed weapon instance
     * @param isMelee       true if this is a melee weapon
     * @return true if weapon was successfully added to holster
     */
    virtual bool VH_CALL AddHolster(std::uint32_t holsterIndex, const char* weaponName,
        void* baseForm, void* weaponRef, bool isMelee) = 0;

};


// ============================================================================
// API Request Function
// ============================================================================

/**
 * Request VirtualHolstersAPI object from the DLL.
 * Returns nullptr if VirtualHolsters is not loaded.
 *
 * This is the primary entry point for external mods.
 */
inline VirtualHolstersAPI* RequestVirtualHolstersAPI()
{
    typedef VirtualHolstersAPI* (*GetVHAPIFuncPtr_t)();

    // Try to get handle to VirtualHolsters.dll (might already be loaded)
    HMODULE vhModule = GetModuleHandleA("VirtualHolsters.dll");

    // If not loaded via GetModuleHandle, it might be named differently
    // or loaded via F4SE plugin system - so we don't try LoadLibrary
    // to avoid forcing a load order

    if (vhModule != nullptr)
    {
        GetVHAPIFuncPtr_t getApiFunc = (GetVHAPIFuncPtr_t)GetProcAddress(vhModule, "VHAPI_GetApi");
        if (getApiFunc)
        {
            return getApiFunc();
        }
    }

    return nullptr;
}
