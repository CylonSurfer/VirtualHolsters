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
