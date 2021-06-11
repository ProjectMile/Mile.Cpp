/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.PiConsole.cpp
 * PURPOSE:   Implementation for Portable Interactive Console (Pi Console)
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "Mile.PiConsole.h"

#include <CommCtrl.h>

#include <string>

namespace
{
    const int PiConsoleInputEditControlHeight = 24;

    HWND PiConsoleGetOutputEditControl(
        _In_ HWND WindowHandle)
    {
        HWND Current = ::GetWindow(WindowHandle, GW_CHILD);

        do
        {
            wchar_t ClassName[256] = { 0 };
            if (::GetClassNameW(Current, ClassName, 256))
            {
                if (std::wcscmp(ClassName, L"Edit") == 0)
                {
                    if (ES_READONLY & ::GetWindowLongPtrW(
                        Current, GWL_STYLE))
                    {
                        return Current;
                    }
                }
            }

            Current = ::GetWindow(Current, GW_HWNDNEXT);

        } while (Current);

        return nullptr;
    }

    HWND PiConsoleGetInputEditControl(
        _In_ HWND WindowHandle)
    {
        HWND Current = ::GetWindow(WindowHandle, GW_CHILD);

        do
        {
            wchar_t ClassName[256] = { 0 };
            if (::GetClassNameW(Current, ClassName, 256))
            {
                if (std::wcscmp(ClassName, L"Edit") == 0)
                {
                    if (!(ES_READONLY &::GetWindowLongPtrW(
                        Current, GWL_STYLE)))
                    {
                        return Current;
                    }
                }
            }

            Current = ::GetWindow(Current, GW_HWNDNEXT);

        } while (Current);

        return nullptr;
    }

    void PiConsoleSetTextCursorPositionToEnd(
        _In_ HWND EditControlHandle)
    {
        int TextLength = ::GetWindowTextLengthW(
            EditControlHandle);

        ::SendMessageW(
            EditControlHandle,
            EM_SETSEL,
            static_cast<WPARAM>(TextLength),
            static_cast<LPARAM>(TextLength));
    }

    void PiConsoleAppendString(
        _In_ HWND EditControlHandle,
        _In_ LPCWSTR Content)
    {
        ::PiConsoleSetTextCursorPositionToEnd(
            EditControlHandle);

        ::SendMessageW(
            EditControlHandle,
            EM_REPLACESEL,
            FALSE,
            reinterpret_cast<LPARAM>(Content));

        ::PiConsoleSetTextCursorPositionToEnd(
            EditControlHandle);
    }

    int PiConsoleGetWindowDpi(
        _In_ HWND WindowHandle)
    {
        int xDPI = USER_DEFAULT_SCREEN_DPI;
        int yDPI = USER_DEFAULT_SCREEN_DPI;

        if (S_OK != Mile::GetDpiForMonitor(
            ::MonitorFromWindow(WindowHandle, MONITOR_DEFAULTTONEAREST),
            MDT_EFFECTIVE_DPI,
            reinterpret_cast<UINT*>(&xDPI),
            reinterpret_cast<UINT*>(&yDPI)))
        {
            xDPI = ::GetDeviceCaps(::GetDC(WindowHandle), LOGPIXELSX);
            yDPI = ::GetDeviceCaps(::GetDC(WindowHandle), LOGPIXELSY);
        }

        return xDPI;
    }
}

HWND Mile::CreatePiConsole(
    _In_ HINSTANCE InstanceHandle,
    _In_ HICON WindowIconHandle,
    _In_ LPCWSTR WindowTitle,
    _In_ DWORD ShowWindowMode)
{
    bool Result = false;
    HWND WindowHandle = nullptr;
    HANDLE CompletedEventHandle = nullptr;
    HANDLE InputCompletedEventHandle = nullptr;
    HANDLE WindowThreadHandle = nullptr;
    HWND InputEditControlHandle = nullptr;
    HWND OutputEditControlHandle = nullptr;

    auto ExitHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (CompletedEventHandle)
        {
            ::CloseHandle(CompletedEventHandle);
        }

        if (WindowThreadHandle)
        {
            ::CloseHandle(WindowThreadHandle);
        }

        if (!Result)
        {
            if (InputCompletedEventHandle)
            {
                ::CloseHandle(InputCompletedEventHandle);
            }

            ::DestroyWindow(WindowHandle);
            ::DestroyWindow(InputEditControlHandle);
            ::DestroyWindow(OutputEditControlHandle);         
            WindowHandle = nullptr;
        }
    });

    auto WindowCallback = [](
        _In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam) -> LRESULT
    {
        switch (uMsg)
        {
        case WM_SIZE:
        {
            int WindowDpi = ::PiConsoleGetWindowDpi(hWnd);

            int RealPiConsoleInputEditControlHeight = ::MulDiv(
                PiConsoleInputEditControlHeight,
                WindowDpi,
                USER_DEFAULT_SCREEN_DPI);

            HWND InputEditControlHandle =
                ::PiConsoleGetInputEditControl(hWnd);
            if (InputEditControlHandle)
            {
                ::SetWindowPos(
                    InputEditControlHandle,
                    nullptr,
                    0,
                    HIWORD(lParam) - RealPiConsoleInputEditControlHeight,
                    LOWORD(lParam),
                    PiConsoleInputEditControlHeight,
                    0);
            }

            HWND OutputEditControlHandle =
                ::PiConsoleGetOutputEditControl(hWnd);
            if (OutputEditControlHandle)
            {
                ::SetWindowPos(
                    OutputEditControlHandle,
                    nullptr,
                    0,
                    0,
                    LOWORD(lParam),
                    HIWORD(lParam) - RealPiConsoleInputEditControlHeight,
                    0);
            }

            ::UpdateWindow(hWnd);

            break;
        }
        case WM_DPICHANGED:
        {
            int WindowDpi = HIWORD(wParam);

            HFONT FontHandle = ::CreateFontW(
                -::MulDiv(16, WindowDpi, USER_DEFAULT_SCREEN_DPI),
                0,
                0,
                0,
                FW_NORMAL,
                false,
                false,
                false,
                DEFAULT_CHARSET,
                OUT_CHARACTER_PRECIS,
                CLIP_CHARACTER_PRECIS,
                CLEARTYPE_NATURAL_QUALITY,
                FF_MODERN,
                L"MS Shell Dlg");

            ::SendMessageW(
                hWnd,
                WM_SETFONT,
                reinterpret_cast<WPARAM>(FontHandle),
                TRUE);

            HWND InputEditControlHandle =
                ::PiConsoleGetInputEditControl(hWnd);
            if (InputEditControlHandle)
            {
                ::SendMessageW(
                    InputEditControlHandle,
                    WM_SETFONT,
                    reinterpret_cast<WPARAM>(FontHandle),
                    TRUE);
            }

            HWND OutputEditControlHandle =
                ::PiConsoleGetOutputEditControl(hWnd);
            if (OutputEditControlHandle)
            {
                ::SendMessageW(
                    OutputEditControlHandle,
                    WM_SETFONT,
                    reinterpret_cast<WPARAM>(FontHandle),
                    TRUE);
            }

            auto lprcNewScale = reinterpret_cast<RECT*>(lParam);

            ::SetWindowPos(
                hWnd,
                nullptr,
                lprcNewScale->left,
                lprcNewScale->top,
                lprcNewScale->right - lprcNewScale->left,
                lprcNewScale->bottom - lprcNewScale->top,
                SWP_NOZORDER | SWP_NOACTIVATE);

            ::UpdateWindow(hWnd);

            break;
        }
        case WM_DESTROY:
        {
            ::CloseHandle(reinterpret_cast<HANDLE>(
                ::GetWindowLongPtrW(hWnd, GWL_USERDATA)));

            ::SetWindowLongPtrW(
                hWnd,
                GWL_USERDATA,
                reinterpret_cast<LONG>(nullptr));

            ::PostQuitMessage(0);

            break;
        }
        default:
            return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    };

    CompletedEventHandle = ::CreateEventExW(
        nullptr,
        nullptr,
        0,
        EVENT_ALL_ACCESS);
    if (!CompletedEventHandle)
    {
        return nullptr;
    }

    InputCompletedEventHandle = ::CreateEventExW(
        nullptr,
        nullptr,
        0,
        EVENT_ALL_ACCESS);
    if (!CompletedEventHandle)
    {
        return nullptr;
    }

    WindowThreadHandle = Mile::CreateThread([&]()
    {
        Mile::EnablePerMonitorDialogScaling();

        WNDCLASSEXW WindowClass;
        WindowClass.cbSize = sizeof(WNDCLASSEX);
        WindowClass.style = CS_HREDRAW | CS_VREDRAW;
        WindowClass.lpfnWndProc = WindowCallback;
        WindowClass.cbClsExtra = 0;
        WindowClass.cbWndExtra = 0;
        WindowClass.hInstance = InstanceHandle;
        WindowClass.hIcon = WindowIconHandle;
        WindowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        WindowClass.lpszMenuName = nullptr;
        WindowClass.lpszClassName = L"MilePiConsoleWindow";
        WindowClass.hIconSm = WindowIconHandle;

        if (!::RegisterClassExW(&WindowClass))
        {
            ::SetEvent(CompletedEventHandle);
            return;
        }     

        WindowHandle = ::CreateWindowExW(
            WS_EX_APPWINDOW,
            WindowClass.lpszClassName,
            WindowTitle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            0,
            640,
            400,
            nullptr,
            nullptr,
            WindowClass.hInstance,
            nullptr);
        if (!WindowHandle)
        {
            ::SetEvent(CompletedEventHandle);
            return;
        }

        ::SetWindowLongPtrW(
            WindowHandle,
            GWL_USERDATA,
            reinterpret_cast<LONG>(InputCompletedEventHandle));

        Mile::EnableChildWindowDpiMessage(WindowHandle);

        RECT ClientRectangle;
        if (!::GetClientRect(WindowHandle, &ClientRectangle))
        {
            ::SetEvent(CompletedEventHandle);
            return;
        }

        int WindowDpi = ::PiConsoleGetWindowDpi(WindowHandle);

        int RealPiConsoleInputEditControlHeight = ::MulDiv(
            PiConsoleInputEditControlHeight,
            WindowDpi,
            USER_DEFAULT_SCREEN_DPI);

        OutputEditControlHandle = ::CreateWindowExW(
            0,
            L"Edit",
            L"",
            ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_CHILD | WS_VISIBLE,
            0,
            0,
            ClientRectangle.right,
            ClientRectangle.bottom - RealPiConsoleInputEditControlHeight,
            WindowHandle,
            nullptr,
            WindowClass.hInstance,
            nullptr);
        if (!OutputEditControlHandle)
        {
            ::SetEvent(CompletedEventHandle);
            return;
        }

        InputEditControlHandle = ::CreateWindowExW(
            0,
            L"Edit",
            L"",
            WS_CHILD | WS_VISIBLE,
            0,
            ClientRectangle.bottom - RealPiConsoleInputEditControlHeight,
            ClientRectangle.right,
            RealPiConsoleInputEditControlHeight,
            WindowHandle,
            nullptr,
            WindowClass.hInstance,
            nullptr);
        if (!InputEditControlHandle)
        {
            ::SetEvent(CompletedEventHandle);
            return;
        }

        ::SendMessageW(
            InputEditControlHandle,
            EM_SETCUEBANNER,
            FALSE,
            reinterpret_cast<LPARAM>(L"请在此输入命令并回车"));

        HFONT FontHandle = ::CreateFontW(
            -::MulDiv(16, WindowDpi, USER_DEFAULT_SCREEN_DPI),
            0,
            0,
            0,
            FW_NORMAL,
            false,
            false,
            false,
            DEFAULT_CHARSET,
            OUT_CHARACTER_PRECIS,
            CLIP_CHARACTER_PRECIS,
            CLEARTYPE_NATURAL_QUALITY,
            FF_MODERN,
            L"MS Shell Dlg");

        ::SendMessageW(
            WindowHandle,
            WM_SETFONT,
            reinterpret_cast<WPARAM>(FontHandle),
            TRUE);

        ::SendMessageW(
            InputEditControlHandle,
            WM_SETFONT,
            reinterpret_cast<WPARAM>(FontHandle),
            TRUE);

        ::SendMessageW(
            OutputEditControlHandle,
            WM_SETFONT,
            reinterpret_cast<WPARAM>(FontHandle),
            TRUE);

        ::DeleteObject(FontHandle);

        ::ShowWindow(WindowHandle, ShowWindowMode);
        ::UpdateWindow(WindowHandle);

        Result = true;

        ::SetEvent(CompletedEventHandle);

        MSG Message;
        while (::GetMessageW(&Message, nullptr, 0, 0))
        {
            ::TranslateMessage(&Message);
            ::DispatchMessageW(&Message);
        }

    });
    if (!WindowThreadHandle)
    {
        return nullptr;
    }

    ::WaitForSingleObjectEx(CompletedEventHandle, INFINITE, FALSE);

    return WindowHandle;
}

void Mile::PrintMessageToPiConsole(
    _In_ HWND WindowHandle,
    _In_ LPCWSTR Content)
{
    HWND EditControlHandle = ::PiConsoleGetOutputEditControl(WindowHandle);
    if (EditControlHandle)
    {
        ::PiConsoleAppendString(EditControlHandle, Content);
    }
}
