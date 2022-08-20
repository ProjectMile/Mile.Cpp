/*
 * PROJECT:   Simple Native Project
 * FILE:      SimpleNativeProject.cpp
 * PURPOSE:   Implementation for Simple Native Project
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "MINT.h"

void EntryPoint()
{
    //::DbgBreakPoint();

    BOOLEAN WasEnabled;
    ::RtlAdjustPrivilege(SE_TCB_PRIVILEGE, TRUE, FALSE, &WasEnabled);

    ::NtSerializeBoot();

    ::DbgPrint("The F@cking MSVC 2015/2017/2019/2022 toolset ruined our $1M project!\n");
    ::DbgPrint("The F@cking MSVC 2015/2017/2019/2022 toolset ruined our $1M project!\n");
    ::DbgPrint("The F@cking MSVC 2015/2017/2019/2022 toolset ruined our $1M project!\n");
    ::DbgPrint("The F@cking MSVC 2015/2017/2019/2022 toolset ruined our $1M project!\n");
    ::DbgPrint("The F@cking MSVC 2015/2017/2019/2022 toolset ruined our $1M project!\n");

    UNICODE_STRING OutputString;
    ::RtlInitUnicodeString(
        &OutputString,
        L"The F@cking MSVC 2015/2017/2019/2022 toolset ruined our $1M project!\n");
    ::NtDisplayString(&OutputString);

    ::NtWaitForSingleObject(NtCurrentProcess(), FALSE, nullptr);
}

#pragma comment(linker, "/entry:EntryPoint")
