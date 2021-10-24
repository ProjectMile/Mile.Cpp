/*
 * PROJECT:   Mile.SocketBroker
 * FILE:      Mile.SocketBroker.cpp
 * PURPOSE:   Implementation for Mile.SocketBroker
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    ::MessageBoxW(
        nullptr,
        L"Hello, Mile.SocketBroker!\n",
        L"Mile.SocketBroker",
        0);

    return 0;
}
