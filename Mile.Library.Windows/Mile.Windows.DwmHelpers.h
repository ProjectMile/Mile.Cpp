/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Windows.DwmHelpers.h
 * PURPOSE:   Definition for Windows DWM Helpers
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#pragma once

#ifndef MILE_WINDOWS_DWMHELPERS
#define MILE_WINDOWS_DWMHELPERS

#include <Windows.h>

/**
 * @brief Tests if the dark mode system setting is enabled on the computer.
 * @return True if the dark mode system setting is enabled. Otherwise, this
 *         function returns false.
*/
EXTERN_C BOOL WINAPI MileShouldAppsUseImmersiveDarkMode();

#endif // !MILE_WINDOWS_DWMHELPERS
