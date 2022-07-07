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

namespace
{
    static bool IsWindows10Version20H1OrLater()
    {
        static bool CachedResult = ([]() -> bool
        {
            OSVERSIONINFOEXW OSVersionInfoEx = { 0 };
            OSVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
            OSVersionInfoEx.dwMajorVersion = 10;
            OSVersionInfoEx.dwMinorVersion = 0;
            OSVersionInfoEx.dwBuildNumber = 19041;
            return ::VerifyVersionInfoW(
                &OSVersionInfoEx,
                VER_BUILDNUMBER,
                ::VerSetConditionMask(
                    ::VerSetConditionMask(
                        ::VerSetConditionMask(
                            0,
                            VER_MAJORVERSION,
                            VER_GREATER_EQUAL),
                        VER_MINORVERSION,
                        VER_GREATER_EQUAL),
                    VER_BUILDNUMBER,
                    VER_GREATER_EQUAL));
        }());

        return CachedResult;
    }
}

HRESULT MileSetUseImmersiveDarkModeAttribute(
    _In_ HWND WindowHandle,
    _In_ BOOL Value)
{
    const DWORD DwmWindowUseImmersiveDarkModeBefore20H1Attribute = 19;
    const DWORD DwmWindowUseImmersiveDarkModeAttribute = 20;
    return ::DwmSetWindowAttribute(
        WindowHandle,
        (::IsWindows10Version20H1OrLater()
            ? DwmWindowUseImmersiveDarkModeAttribute
            : DwmWindowUseImmersiveDarkModeBefore20H1Attribute),
        &Value,
        sizeof(BOOL));
}
