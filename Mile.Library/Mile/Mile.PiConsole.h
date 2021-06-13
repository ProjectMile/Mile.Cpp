/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.PiConsole.h
 * PURPOSE:   Definition for Portable Interactive Console (Pi Console)
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#ifndef MILE_PI_CONSOLE
#define MILE_PI_CONSOLE

#include "Mile.Windows.h"

namespace Mile
{
    HWND CreatePiConsole(
        _In_ HINSTANCE InstanceHandle,
        _In_ HICON WindowIconHandle,
        _In_ LPCWSTR WindowTitle,
        _In_ DWORD ShowWindowMode);

    void PrintMessageToPiConsole(
        _In_ HWND WindowHandle,
        _In_ LPCWSTR Content);

    LPCWSTR GetInputFromPiConsole(
        _In_ HWND WindowHandle,
        _In_ LPCWSTR InputPrompt);
}

#endif // !MILE_PI_CONSOLE
