/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Windows.DwmHelpers.h
 * PURPOSE:   Definition for Windows DWM Helpers
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#ifndef MILE_WINDOWS_DWMHELPERS
#define MILE_WINDOWS_DWMHELPERS

#include <Windows.h>

/**
 * @brief Allows the window frame for this window to be drawn in dark mode
 *        colors when the dark mode system setting is enabled.
 * @param WindowHandle The handle to the window for which the attribute value
 *                     is to be set.
 * @param Value TRUE to honor dark mode for the window, FALSE to always use
 *              light mode.
 * @return If the function succeeds, it returns S_OK. Otherwise, it returns an
 *         HRESULT error code.
*/
HRESULT MileSetUseImmersiveDarkModeAttribute(
    _In_ HWND WindowHandle,
    _In_ BOOL Value);

#endif // !MILE_WINDOWS_DWMHELPERS
