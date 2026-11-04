#pragma once

/*
 * Kabuto API
 *
 * Usage:
 *
 *     #include "KabutoAPI.h"
 *     auto* api = RequestKabutoHolstersAPI();
 *     if (api && api->GetVersion() >= 1) {
 *     bool IsHandHelmetZone = api->IsHandHelmetZone();
 */

#include <cstdint>
#include <windows.h>

#ifndef KAB_CALL
#define KAB_CALL __cdecl
#endif

class KabutoAPI
{
public:

    virtual std::uint32_t KAB_CALL GetVersion() const = 0;

    virtual bool KAB_CALL IsHandHelmetZone() const = 0;

    virtual bool KAB_CALL IsHoldingHelmetinPrimary() const = 0;

    virtual bool KAB_CALL IsEquippingHelmet() const = 0;

};

inline KabutoAPI* RequestKabutoAPI()
{
    typedef KabutoAPI* (*GetKABAPIFuncPtr_t)();


    HMODULE KABModule = GetModuleHandleA("KabutoVR.dll");



    if (KABModule != nullptr)
    {
        GetKABAPIFuncPtr_t getApiFunc = (GetKABAPIFuncPtr_t)GetProcAddress(KABModule, "KABAPI_GetApi");
        if (getApiFunc)
        {
            return getApiFunc();
        }
    }

    return nullptr;
}
