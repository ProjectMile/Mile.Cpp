/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Windows.DwmHelpers.cpp
 * PURPOSE:   Implementation for Windows DWM Helpers
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "Mile.Windows.DwmHelpers.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

EXTERN_C HRESULT WINAPI MileSetCaptionColorAttribute(
    _In_ HWND WindowHandle,
    _In_ COLORREF Value)
{
    const DWORD DwmWindowCaptionColorAttribute = 35;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        DwmWindowCaptionColorAttribute,
        &Value,
        sizeof(COLORREF));
}

EXTERN_C HRESULT WINAPI MileDisableSystemBackdrop(
    _In_ HWND WindowHandle)
{
    const DWORD DwmWindowSystemBackdropTypeAttribute = 38;
    const DWORD DwmWindowSystemBackdropTypeNone = 1;
    DWORD Value = DwmWindowSystemBackdropTypeNone;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        DwmWindowSystemBackdropTypeAttribute,
        &Value,
        sizeof(DWORD));
}

EXTERN_C BOOL WINAPI MileShouldAppsUseImmersiveDarkMode()
{
    DWORD Type = REG_DWORD;
    DWORD Value = 0;
    DWORD ValueLength = sizeof(DWORD);
    DWORD Error = ::RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD | RRF_SUBKEY_WOW6464KEY,
        &Type,
        &Value,
        &ValueLength);
    if (Error == ERROR_SUCCESS)
    {
        if (Type == REG_DWORD && ValueLength == sizeof(DWORD))
        {
            return (Value == 0) ? TRUE : FALSE;
        }
    }
    return FALSE;
}
