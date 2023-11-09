/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Windows.cpp
 * PURPOSE:   Implementation for Windows
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#include "Mile.Windows.h"

#include <strsafe.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)
#include <VersionHelpers.h>
#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)
#include <WtsApi32.h>
#pragma comment(lib, "WtsApi32.lib")
#endif

#pragma region Implementations for Windows (Win32 Style)

namespace
{
    /**
     * @brief The information about the Windows Overlay Filter file provider.
    */
    typedef struct _WOF_FILE_PROVIDER_EXTERNAL_INFO
    {
        WOF_EXTERNAL_INFO Wof;
        FILE_PROVIDER_EXTERNAL_INFO FileProvider;
    } WOF_FILE_PROVIDER_EXTERNAL_INFO, * PWOF_FILE_PROVIDER_EXTERNAL_INFO;

    static void FillFileEnumerateInformation(
        _In_ PFILE_ID_BOTH_DIR_INFO OriginalInformation,
        _Out_ Mile::PFILE_ENUMERATE_INFORMATION ConvertedInformation)
    {
        ConvertedInformation->CreationTime.dwLowDateTime =
            OriginalInformation->CreationTime.LowPart;
        ConvertedInformation->CreationTime.dwHighDateTime =
            OriginalInformation->CreationTime.HighPart;

        ConvertedInformation->LastAccessTime.dwLowDateTime =
            OriginalInformation->LastAccessTime.LowPart;
        ConvertedInformation->LastAccessTime.dwHighDateTime =
            OriginalInformation->LastAccessTime.HighPart;

        ConvertedInformation->LastWriteTime.dwLowDateTime =
            OriginalInformation->LastWriteTime.LowPart;
        ConvertedInformation->LastWriteTime.dwHighDateTime =
            OriginalInformation->LastWriteTime.HighPart;

        ConvertedInformation->ChangeTime.dwLowDateTime =
            OriginalInformation->ChangeTime.LowPart;
        ConvertedInformation->ChangeTime.dwHighDateTime =
            OriginalInformation->ChangeTime.HighPart;

        ConvertedInformation->FileSize =
            OriginalInformation->EndOfFile.QuadPart;

        ConvertedInformation->AllocationSize =
            OriginalInformation->AllocationSize.QuadPart;

        ConvertedInformation->FileAttributes =
            OriginalInformation->FileAttributes;

        ConvertedInformation->EaSize =
            OriginalInformation->EaSize;

        ConvertedInformation->FileId =
            OriginalInformation->FileId;

        ::StringCbCopyNW(
            ConvertedInformation->ShortName,
            sizeof(ConvertedInformation->ShortName),
            OriginalInformation->ShortName,
            OriginalInformation->ShortNameLength);

        ::StringCbCopyNW(
            ConvertedInformation->FileName,
            sizeof(ConvertedInformation->FileName),
            OriginalInformation->FileName,
            OriginalInformation->FileNameLength);
    }
}

Mile::HResultFromLastError Mile::DeviceIoControl(
    _In_ HANDLE hDevice,
    _In_ DWORD dwIoControlCode,
    _In_opt_ LPVOID lpInBuffer,
    _In_ DWORD nInBufferSize,
    _Out_opt_ LPVOID lpOutBuffer,
    _In_ DWORD nOutBufferSize,
    _Out_opt_ LPDWORD lpBytesReturned)
{
    BOOL Result = FALSE;
    OVERLAPPED Overlapped = { 0 };
    Overlapped.hEvent = ::CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr);
    if (Overlapped.hEvent)
    {
        Result = ::DeviceIoControl(
            hDevice,
            dwIoControlCode,
            lpInBuffer,
            nInBufferSize,
            lpOutBuffer,
            nOutBufferSize,
            lpBytesReturned,
            &Overlapped);
        if (!Result)
        {
            if (::GetLastError() == ERROR_IO_PENDING)
            {
                Result = ::GetOverlappedResult(
                    hDevice,
                    &Overlapped,
                    lpBytesReturned,
                    TRUE);
            }
        }

        ::CloseHandle(Overlapped.hEvent);
    }
    else
    {
        ::SetLastError(ERROR_NO_SYSTEM_RESOURCES);
    }

    return Result;
}

Mile::HResultFromLastError Mile::GetNtfsCompressionAttribute(
    _In_ HANDLE FileHandle,
    _Out_ PUSHORT CompressionAlgorithm)
{
    if (!CompressionAlgorithm)
        return E_INVALIDARG;

    DWORD BytesReturned;
    return Mile::DeviceIoControl(
        FileHandle,
        FSCTL_GET_COMPRESSION,
        nullptr,
        0,
        CompressionAlgorithm,
        sizeof(*CompressionAlgorithm),
        &BytesReturned);
}

Mile::HResultFromLastError Mile::SetNtfsCompressionAttribute(
    _In_ HANDLE FileHandle,
    _In_ USHORT CompressionAlgorithm)
{
    switch (CompressionAlgorithm)
    {
    case COMPRESSION_FORMAT_NONE:
    case COMPRESSION_FORMAT_DEFAULT:
    case COMPRESSION_FORMAT_LZNT1:
        break;
    default:
        return E_INVALIDARG;
    }

    DWORD BytesReturned;
    return Mile::DeviceIoControl(
        FileHandle,
        FSCTL_SET_COMPRESSION,
        &CompressionAlgorithm,
        sizeof(CompressionAlgorithm),
        nullptr,
        0,
        &BytesReturned);
}

Mile::HResult Mile::GetWofCompressionAttribute(
    _In_ HANDLE FileHandle,
    _Out_ PDWORD CompressionAlgorithm)
{
    if (!CompressionAlgorithm)
    {
        return E_INVALIDARG;
    }

    WOF_FILE_PROVIDER_EXTERNAL_INFO WofInfo = { 0 };
    DWORD BytesReturned;
    Mile::HResult hr = Mile::HResultFromLastError(Mile::DeviceIoControl(
        FileHandle,
        FSCTL_GET_EXTERNAL_BACKING,
        nullptr,
        0,
        &WofInfo,
        sizeof(WofInfo),
        &BytesReturned));
    if (hr.IsSucceeded())
    {
        if (WofInfo.Wof.Version == WOF_CURRENT_VERSION &&
            WofInfo.Wof.Provider == WOF_PROVIDER_FILE)
        {
            *CompressionAlgorithm = WofInfo.FileProvider.Algorithm;
        }
    }

    return hr;
}

Mile::HResult Mile::SetWofCompressionAttribute(
    _In_ HANDLE FileHandle,
    _In_ DWORD CompressionAlgorithm)
{
    switch (CompressionAlgorithm)
    {
    case FILE_PROVIDER_COMPRESSION_XPRESS4K:
    case FILE_PROVIDER_COMPRESSION_LZX:
    case FILE_PROVIDER_COMPRESSION_XPRESS8K:
    case FILE_PROVIDER_COMPRESSION_XPRESS16K:
        break;
    default:
        return E_INVALIDARG;
    }

    WOF_FILE_PROVIDER_EXTERNAL_INFO WofInfo = { 0 };

    WofInfo.Wof.Version = WOF_CURRENT_VERSION;
    WofInfo.Wof.Provider = WOF_PROVIDER_FILE;

    WofInfo.FileProvider.Version = FILE_PROVIDER_CURRENT_VERSION;
    WofInfo.FileProvider.Flags = 0;
    WofInfo.FileProvider.Algorithm = CompressionAlgorithm;

    DWORD BytesReturned;
    Mile::HResult hr = Mile::HResultFromLastError(Mile::DeviceIoControl(
        FileHandle,
        FSCTL_SET_EXTERNAL_BACKING,
        &WofInfo,
        sizeof(WofInfo),
        nullptr,
        0,
        &BytesReturned));
    if (hr.GetCode() == ERROR_COMPRESSION_NOT_BENEFICIAL ||
        hr.GetCode() == ERROR_MR_MID_NOT_FOUND)
    {
        hr = S_OK;
    }

    return hr;
}

Mile::HResult Mile::RemoveWofCompressionAttribute(
    _In_ HANDLE FileHandle)
{
    DWORD BytesReturned;
    Mile::HResult hr = Mile::HResultFromLastError(Mile::DeviceIoControl(
        FileHandle,
        FSCTL_DELETE_EXTERNAL_BACKING,
        nullptr,
        0,
        nullptr,
        0,
        &BytesReturned));
    if (hr.GetCode() == ERROR_OBJECT_NOT_EXTERNALLY_BACKED)
    {
        hr = S_OK;
    }

    return hr;
}

Mile::HResult Mile::GetCompactOsDeploymentState(
    _Out_ PDWORD DeploymentState)
{
    if (!DeploymentState)
    {
        return E_INVALIDARG;
    }

    HKEY hKey = nullptr;

    Mile::HResult hr = Mile::HResult::FromWin32(::RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        L"System\\Setup",
        0,
        KEY_READ | KEY_WOW64_64KEY,
        &hKey));
    if (hr.IsSucceeded())
    {
        DWORD Type = 0;
        DWORD Data = FALSE;
        DWORD Length = sizeof(DWORD);

        hr = Mile::HResult::FromWin32(::RegQueryValueExW(
            hKey,
            L"Compact",
            nullptr,
            &Type,
            reinterpret_cast<LPBYTE>(&Data),
            &Length));
        if (hr.IsSucceeded() && Type == REG_DWORD)
        {
            *DeploymentState = Data;
        }

        ::RegCloseKey(hKey);
    }

    return hr;
}

Mile::HResult Mile::SetCompactOsDeploymentState(
    _In_ DWORD DeploymentState)
{
    HKEY hKey = nullptr;

    Mile::HResult hr = Mile::HResult::FromWin32(::RegCreateKeyExW(
        HKEY_LOCAL_MACHINE,
        L"System\\Setup",
        0,
        nullptr,
        0,
        KEY_WRITE | KEY_WOW64_64KEY,
        nullptr,
        &hKey,
        nullptr));
    if (hr.IsSucceeded())
    {
        hr = Mile::HResult::FromWin32(::RegSetValueExW(
            hKey,
            L"Compact",
            0,
            REG_DWORD,
            reinterpret_cast<CONST BYTE*>(&DeploymentState),
            sizeof(DWORD)));

        ::RegCloseKey(hKey);
    }

    return hr;
}

Mile::HResult Mile::EnumerateFile(
    _In_ HANDLE FileHandle,
    _In_ Mile::ENUMERATE_FILE_CALLBACK_TYPE Callback,
    _In_opt_ LPVOID Context)
{
    if (!FileHandle || FileHandle == INVALID_HANDLE_VALUE)
    {
        return E_INVALIDARG;
    }

    if (!Callback)
    {
        return E_INVALIDARG;
    }

    const SIZE_T BufferSize = 32768;
    PBYTE Buffer = nullptr;
    PFILE_ID_BOTH_DIR_INFO OriginalInformation = nullptr;
    Mile::FILE_ENUMERATE_INFORMATION ConvertedInformation = { 0 };

    auto ExitHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (Buffer)
        {
            ::MileFreeMemory(Buffer);
        }
    });

    Buffer = reinterpret_cast<PBYTE>(::MileAllocateMemory(BufferSize));
    if (!Buffer)
    {
        return E_OUTOFMEMORY;
    }

    OriginalInformation =
        reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(Buffer);

    if (::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileIdBothDirectoryRestartInfo,
        OriginalInformation,
        BufferSize))
    {
        for (;;)
        {
            ::FillFileEnumerateInformation(
                OriginalInformation,
                &ConvertedInformation);

            if (!Callback(&ConvertedInformation, Context))
            {
                return S_OK;
            }

            if (!OriginalInformation->NextEntryOffset)
            {
                OriginalInformation =
                    reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(Buffer);
                break;
            }

            OriginalInformation = reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(
                reinterpret_cast<ULONG_PTR>(OriginalInformation)
                + OriginalInformation->NextEntryOffset);
        }

        while (::GetFileInformationByHandleEx(
            FileHandle,
            FILE_INFO_BY_HANDLE_CLASS::FileIdBothDirectoryInfo,
            OriginalInformation,
            BufferSize))
        {
            for (;;)
            {
                ::FillFileEnumerateInformation(
                    OriginalInformation,
                    &ConvertedInformation);

                if (!Callback(&ConvertedInformation, Context))
                {
                    return S_OK;
                }

                if (!OriginalInformation->NextEntryOffset)
                {
                    OriginalInformation =
                        reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(Buffer);
                    break;
                }

                OriginalInformation = reinterpret_cast<PFILE_ID_BOTH_DIR_INFO>(
                    reinterpret_cast<ULONG_PTR>(OriginalInformation)
                    + OriginalInformation->NextEntryOffset);
            }
        }
    }

    Mile::HResult hr = Mile::HResultFromLastError();
    if (hr.GetCode() == ERROR_NO_MORE_FILES)
    {
        hr = S_OK;
    }

    return hr;
}

Mile::HResultFromLastError Mile::GetFileSize(
    _In_ HANDLE FileHandle,
    _Out_ PULONGLONG FileSize)
{
    FILE_STANDARD_INFO StandardInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
        &StandardInfo,
        sizeof(FILE_STANDARD_INFO));

    *FileSize = Result
        ? static_cast<ULONGLONG>(StandardInfo.EndOfFile.QuadPart)
        : 0;

    return Result;
}

Mile::HResultFromLastError Mile::GetFileAllocationSize(
    _In_ HANDLE FileHandle,
    _Out_ PULONGLONG AllocationSize)
{
    FILE_STANDARD_INFO StandardInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
        &StandardInfo,
        sizeof(FILE_STANDARD_INFO));

    *AllocationSize = Result
        ? static_cast<ULONGLONG>(StandardInfo.AllocationSize.QuadPart)
        : 0;

    return Result;
}

Mile::HResultFromLastError Mile::GetCompressedFileSizeByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PULONGLONG CompressedFileSize)
{
    FILE_COMPRESSION_INFO FileCompressionInfo;

    if (::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileCompressionInfo,
        &FileCompressionInfo,
        sizeof(FILE_COMPRESSION_INFO)))
    {
        *CompressedFileSize = static_cast<ULONGLONG>(
            FileCompressionInfo.CompressedFileSize.QuadPart);

        return TRUE;
    }

    return Mile::GetFileSize(FileHandle, CompressedFileSize);
}

Mile::HResultFromLastError Mile::GetFileAttributesByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PDWORD FileAttributes)
{
    FILE_BASIC_INFO BasicInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo,
        &BasicInfo,
        sizeof(FILE_BASIC_INFO));

    *FileAttributes = Result
        ? BasicInfo.FileAttributes
        : INVALID_FILE_ATTRIBUTES;

    return Result;
}

Mile::HResultFromLastError Mile::SetFileAttributesByHandle(
    _In_ HANDLE FileHandle,
    _In_ DWORD FileAttributes)
{
    FILE_BASIC_INFO BasicInfo = { 0 };
    BasicInfo.FileAttributes =
        FileAttributes & (
            FILE_SHARE_READ |
            FILE_SHARE_WRITE |
            FILE_SHARE_DELETE |
            FILE_ATTRIBUTE_ARCHIVE |
            FILE_ATTRIBUTE_TEMPORARY |
            FILE_ATTRIBUTE_OFFLINE |
            FILE_ATTRIBUTE_NOT_CONTENT_INDEXED |
            FILE_ATTRIBUTE_NO_SCRUB_DATA) |
        FILE_ATTRIBUTE_NORMAL;

    return ::SetFileInformationByHandle(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileBasicInfo,
        &BasicInfo,
        sizeof(FILE_BASIC_INFO));
}

Mile::HResultFromLastError Mile::GetFileHardlinkCountByHandle(
    _In_ HANDLE FileHandle,
    _Out_ PDWORD HardlinkCount)
{
    FILE_STANDARD_INFO StandardInfo;

    BOOL Result = ::GetFileInformationByHandleEx(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileStandardInfo,
        &StandardInfo,
        sizeof(FILE_STANDARD_INFO));

    *HardlinkCount = Result
        ? StandardInfo.NumberOfLinks
        : static_cast<DWORD>(-1);

    return Result;
}

Mile::HResultFromLastError Mile::DeleteFileByHandle(
    _In_ HANDLE FileHandle)
{
    FILE_DISPOSITION_INFO DispostionInfo;
    DispostionInfo.DeleteFile = TRUE;
    if (::SetFileInformationByHandle(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileDispositionInfo,
        &DispostionInfo,
        sizeof(FILE_DISPOSITION_INFO)))
    {
        return TRUE;
    }

    FILE_DISPOSITION_INFO_EX DispostionInfoEx;
    DispostionInfoEx.Flags = FILE_DISPOSITION_FLAG_DELETE;
    return ::SetFileInformationByHandle(
        FileHandle,
        FILE_INFO_BY_HANDLE_CLASS::FileDispositionInfoEx,
        &DispostionInfoEx,
        sizeof(FILE_DISPOSITION_INFO_EX));
}

Mile::HResult Mile::DeleteFileByHandleIgnoreReadonlyAttribute(
    _In_ HANDLE FileHandle)
{
    DWORD OldAttribute = 0;

    // Save old attributes.
    Mile::HResult hr = Mile::GetFileAttributesByHandle(
        FileHandle,
        &OldAttribute);
    if (hr.IsSucceeded())
    {
        DWORD HardlinkCount = 0;

        // Get hardlink count.
        hr = Mile::GetFileHardlinkCountByHandle(
            FileHandle,
            &HardlinkCount);
        if (hr.IsSucceeded())
        {
            // Remove readonly attribute.
            hr = Mile::SetFileAttributesByHandle(
                FileHandle,
                OldAttribute & (-1 ^ FILE_ATTRIBUTE_READONLY));
            if (hr.IsSucceeded())
            {
                // Delete the file.
                hr = Mile::DeleteFileByHandle(FileHandle);
                if (hr.IsFailed() || HardlinkCount > 1)
                {
                    // Restore attributes if failed or had another hard links.
                    Mile::SetFileAttributesByHandle(
                        FileHandle,
                        OldAttribute);
                }
            }
        }
    }

    return hr;
}

BOOL Mile::IsDotsName(
    _In_ LPCWSTR Name)
{
    return Name[0] == L'.' && (!Name[1] || (Name[1] == L'.' && !Name[2]));
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

BOOL Mile::EnableChildWindowDpiMessage(
    _In_ HWND WindowHandle)
{
    // This hack is only for Windows 10 only.
    if (!::IsWindowsVersionOrGreater(10, 0, 0))
    {
        return FALSE;
    }

    // We don't need this hack if the Per Monitor Aware V2 is existed.
    OSVERSIONINFOEXW OSVersionInfoEx = { 0 };
    OSVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    OSVersionInfoEx.dwBuildNumber = 14393;
    if (::VerifyVersionInfoW(
        &OSVersionInfoEx,
        VER_BUILDNUMBER,
        ::VerSetConditionMask(0, VER_BUILDNUMBER, VER_GREATER_EQUAL)))
    {
        return FALSE;
    }

    HMODULE ModuleHandle = ::GetModuleHandleW(L"user32.dll");
    if (!ModuleHandle)
    {
        return FALSE;
    }

    typedef BOOL(WINAPI* ProcType)(HWND, BOOL);

    ProcType ProcAddress = reinterpret_cast<ProcType>(
        ::GetProcAddress(ModuleHandle, "EnableChildWindowDpiMessage"));
    if (!ProcAddress)
    {
        return FALSE;
    }

    return ProcAddress(WindowHandle, TRUE);
}

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResult Mile::GetDpiForMonitor(
    _In_ HMONITOR hMonitor,
    _In_ MONITOR_DPI_TYPE dpiType,
    _Out_ UINT* dpiX,
    _Out_ UINT* dpiY)
{
    Mile::HResult hr = S_OK;

    HMODULE ModuleHandle = ::LoadLibraryExW(
        L"SHCore.dll",
        nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (ModuleHandle)
    {
        using ProcType = decltype(::GetDpiForMonitor)*;

        ProcType ProcAddress = reinterpret_cast<ProcType>(
            ::GetProcAddress(ModuleHandle, "GetDpiForMonitor"));
        if (ProcAddress)
        {
            hr = ProcAddress(hMonitor, dpiType, dpiX, dpiY);
        }
        else
        {
            hr = Mile::HResultFromLastError(FALSE);
        }

        ::FreeLibrary(ModuleHandle);
    }
    else
    {
        hr = Mile::HResultFromLastError(FALSE);
    }

    return hr;
}

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResultFromLastError Mile::CreateSessionToken(
    _In_ DWORD SessionId,
    _Out_ PHANDLE TokenHandle)
{
    return ::WTSQueryUserToken(SessionId, TokenHandle);
}

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResultFromLastError Mile::CreateSystemToken(
    _In_ DWORD DesiredAccess,
    _Out_ PHANDLE TokenHandle)
{
    // If the specified process is the System Idle Process (0x00000000), the
    // function fails and the last error code is ERROR_INVALID_PARAMETER.
    // So this is why 0 is the default value of dwLsassPID and dwWinLogonPID.

    // For fix the issue that @_kod0k and @DennyAmaro mentioned in
    // https://forums.mydigitallife.net/threads/59268/page-28#post-1672011 and
    // https://forums.mydigitallife.net/threads/59268/page-28#post-1674985.
    // Mile::CreateSystemToken will try to open the access token from lsass.exe
    // for maximum privileges in the access token, and try to open the access
    // token from winlogon.exe of current active session as fallback.

    // If no source process of SYSTEM access token can be found, the error code
    // will be HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER).

    DWORD dwLsassPID = 0;
    DWORD dwWinLogonPID = 0;
    PWTS_PROCESS_INFOW pProcesses = nullptr;
    DWORD dwProcessCount = 0;
    DWORD dwSessionID = Mile::GetActiveSessionID();

    if (::WTSEnumerateProcessesW(
        WTS_CURRENT_SERVER_HANDLE,
        0,
        1,
        &pProcesses,
        &dwProcessCount))
    {
        for (DWORD i = 0; i < dwProcessCount; ++i)
        {
            PWTS_PROCESS_INFOW pProcess = &pProcesses[i];

            if ((!pProcess->pProcessName) ||
                (!pProcess->pUserSid) ||
                (!::IsWellKnownSid(
                    pProcess->pUserSid,
                    WELL_KNOWN_SID_TYPE::WinLocalSystemSid)))
            {
                continue;
            }

            if ((0 == dwLsassPID) &&
                (0 == pProcess->SessionId) &&
                (0 == ::_wcsicmp(L"lsass.exe", pProcess->pProcessName)))
            {
                dwLsassPID = pProcess->ProcessId;
                continue;
            }

            if ((0 == dwWinLogonPID) &&
                (dwSessionID == pProcess->SessionId) &&
                (0 == ::_wcsicmp(L"winlogon.exe", pProcess->pProcessName)))
            {
                dwWinLogonPID = pProcess->ProcessId;
                continue;
            }
        }

        ::WTSFreeMemory(pProcesses);
    }

    BOOL Result = FALSE;
    HANDLE SystemProcessHandle = nullptr;

    SystemProcessHandle = ::OpenProcess(
        PROCESS_QUERY_INFORMATION,
        FALSE,
        dwLsassPID);
    if (!SystemProcessHandle)
    {
        SystemProcessHandle = ::OpenProcess(
            PROCESS_QUERY_INFORMATION,
            FALSE,
            dwWinLogonPID);
    }

    if (SystemProcessHandle)
    {
        HANDLE SystemTokenHandle = nullptr;
        if (::OpenProcessToken(
            SystemProcessHandle,
            TOKEN_DUPLICATE,
            &SystemTokenHandle))
        {
            Result = ::DuplicateTokenEx(
                SystemTokenHandle,
                DesiredAccess,
                nullptr,
                SecurityIdentification,
                TokenPrimary,
                TokenHandle);

            ::CloseHandle(SystemTokenHandle);
        }

        ::CloseHandle(SystemProcessHandle);
    }

    return Result;
}

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

DWORD Mile::GetActiveSessionID()
{
    DWORD Count = 0;
    PWTS_SESSION_INFOW pSessionInfo = nullptr;
    if (::WTSEnumerateSessionsW(
        WTS_CURRENT_SERVER_HANDLE,
        0,
        1,
        &pSessionInfo,
        &Count))
    {
        for (DWORD i = 0; i < Count; ++i)
        {
            if (pSessionInfo[i].State == WTS_CONNECTSTATE_CLASS::WTSActive)
            {
                return pSessionInfo[i].SessionId;
            }
        }

        ::WTSFreeMemory(pSessionInfo);
    }

    return static_cast<DWORD>(-1);
}

#endif

Mile::HResultFromLastError Mile::SetTokenMandatoryLabel(
    _In_ HANDLE TokenHandle,
    _In_ DWORD MandatoryLabelRid)
{
    BOOL Result = FALSE;

    SID_IDENTIFIER_AUTHORITY SIA = SECURITY_MANDATORY_LABEL_AUTHORITY;

    TOKEN_MANDATORY_LABEL TML;

    if (::AllocateAndInitializeSid(
        &SIA, 1, MandatoryLabelRid, 0, 0, 0, 0, 0, 0, 0, &TML.Label.Sid))
    {
        TML.Label.Attributes = SE_GROUP_INTEGRITY;

        Result = ::SetTokenInformation(
            TokenHandle, TokenIntegrityLevel, &TML, sizeof(TML));

        ::FreeSid(TML.Label.Sid);
    }

    return Result;
}

Mile::HResultFromLastError Mile::GetTokenInformationWithMemory(
    _In_ HANDLE TokenHandle,
    _In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
    _Out_ PVOID* OutputInformation)
{
    if (!OutputInformation)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    *OutputInformation = nullptr;

    BOOL Result = FALSE;

    DWORD Length = 0;
    ::GetTokenInformation(
        TokenHandle,
        TokenInformationClass,
        nullptr,
        0,
        &Length);
    if (ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
    {
        *OutputInformation = ::MileAllocateMemory(Length);
        if (*OutputInformation)
        {
            Result = ::GetTokenInformation(
                TokenHandle,
                TokenInformationClass,
                *OutputInformation,
                Length,
                &Length);
            if (!Result)
            {
                ::MileFreeMemory(*OutputInformation);
                *OutputInformation = nullptr;
            }
        }
        else
        {
            ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        }
    }

    return Result;
}

Mile::HResult Mile::CreateLUAToken(
    _In_ HANDLE ExistingTokenHandle,
    _Out_ PHANDLE TokenHandle)
{
    Mile::HResult hr = E_INVALIDARG;

    PTOKEN_USER pTokenUser = nullptr;
    TOKEN_OWNER Owner = { 0 };
    PTOKEN_DEFAULT_DACL pTokenDacl = nullptr;
    DWORD Length = 0;
    PACL NewDefaultDacl = nullptr;
    TOKEN_DEFAULT_DACL NewTokenDacl = { 0 };
    PACCESS_ALLOWED_ACE pTempAce = nullptr;
    BOOL EnableTokenVirtualization = TRUE;

    do
    {
        if (!TokenHandle)
        {
            break;
        }

        hr = Mile::HResultFromLastError(::CreateRestrictedToken(
            ExistingTokenHandle,
            LUA_TOKEN,
            0,
            nullptr,
            0,
            nullptr,
            0,
            nullptr,
            TokenHandle));
        if (hr != S_OK)
        {
            break;
        }

        hr = Mile::SetTokenMandatoryLabel(
            *TokenHandle, SECURITY_MANDATORY_MEDIUM_RID);
        if (hr != S_OK)
        {
            break;
        }

        hr = Mile::GetTokenInformationWithMemory(
            *TokenHandle,
            TokenUser,
            reinterpret_cast<PVOID*>(&pTokenUser));
        if (hr != S_OK)
        {
            break;
        }

        Owner.Owner = pTokenUser->User.Sid;
        hr = Mile::HResultFromLastError(::SetTokenInformation(
            *TokenHandle, TokenOwner, &Owner, sizeof(TOKEN_OWNER)));
        if (hr != S_OK)
        {
            break;
        }

        hr = Mile::GetTokenInformationWithMemory(
            *TokenHandle,
            TokenDefaultDacl,
            reinterpret_cast<PVOID*>(&pTokenDacl));
        if (hr != S_OK)
        {
            break;
        }

        Length = pTokenDacl->DefaultDacl->AclSize;
        Length += ::GetLengthSid(pTokenUser->User.Sid);
        Length += sizeof(ACCESS_ALLOWED_ACE);

        NewDefaultDacl = reinterpret_cast<PACL>(
            ::MileAllocateMemory(Length));
        if (NewDefaultDacl)
        {
            hr = Mile::HResult::FromWin32(ERROR_NOT_ENOUGH_MEMORY);
            break;
        }
        NewTokenDacl.DefaultDacl = NewDefaultDacl;

        hr = Mile::HResultFromLastError(::InitializeAcl(
            NewTokenDacl.DefaultDacl,
            Length,
            pTokenDacl->DefaultDacl->AclRevision));
        if (hr != S_OK)
        {
            break;
        }

        hr = Mile::HResultFromLastError(::AddAccessAllowedAce(
            NewTokenDacl.DefaultDacl,
            pTokenDacl->DefaultDacl->AclRevision,
            GENERIC_ALL,
            pTokenUser->User.Sid));
        if (hr != S_OK)
        {
            break;
        }

        for (ULONG i = 0;
            ::GetAce(
                pTokenDacl->DefaultDacl,
                i,
                reinterpret_cast<PVOID*>(&pTempAce));
            ++i)
        {
            if (::IsWellKnownSid(
                &pTempAce->SidStart,
                WELL_KNOWN_SID_TYPE::WinBuiltinAdministratorsSid))
                continue;

            ::AddAce(
                NewTokenDacl.DefaultDacl,
                pTokenDacl->DefaultDacl->AclRevision,
                0,
                pTempAce,
                pTempAce->Header.AceSize);
        }

        Length += sizeof(TOKEN_DEFAULT_DACL);
        hr = Mile::HResultFromLastError(::SetTokenInformation(
            *TokenHandle, TokenDefaultDacl, &NewTokenDacl, Length));
        if (hr != S_OK)
        {
            break;
        }

        hr = Mile::HResultFromLastError(::SetTokenInformation(
            *TokenHandle,
            TokenVirtualizationEnabled,
            &EnableTokenVirtualization,
            sizeof(BOOL)));
        if (hr != S_OK)
        {
            break;
        }

    } while (false);

    if (NewDefaultDacl)
    {
        ::MileFreeMemory(NewDefaultDacl);
    }

    if (pTokenDacl)
    {
        ::MileFreeMemory(pTokenDacl);
    }

    if (pTokenUser)
    {
        ::MileFreeMemory(pTokenUser);
    }

    if (hr != S_OK)
    {
        ::CloseHandle(TokenHandle);
        *TokenHandle = INVALID_HANDLE_VALUE;
    }

    return hr;
}

Mile::HResult Mile::CoCreateInstanceByString(
    _In_ LPCWSTR lpszCLSID,
    _In_opt_ LPUNKNOWN pUnkOuter,
    _In_ DWORD dwClsContext,
    _In_ LPCWSTR lpszIID,
    _Out_ LPVOID* ppv)
{
    Mile::HResult hr = S_OK;

    do
    {
        CLSID clsid;
        IID iid;

        hr = ::CLSIDFromString(lpszCLSID, &clsid);
        if (hr != S_OK)
        {
            break;
        }

        hr = ::IIDFromString(lpszIID, &iid);
        if (hr != S_OK)
        {
            break;
        }

        hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, iid, ppv);

    } while (false);

    return hr;
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResult Mile::RegQueryStringValue(
    _In_ HKEY hKey,
    _In_opt_ LPCWSTR lpValueName,
    _Out_ LPWSTR* lpData)
{
    *lpData = nullptr;

    DWORD cbData = 0;
    Mile::HResult hr = Mile::HResult::FromWin32(::RegQueryValueExW(
        hKey,
        lpValueName,
        nullptr,
        nullptr,
        nullptr,
        &cbData));
    if (SUCCEEDED(hr))
    {
        *lpData = reinterpret_cast<LPWSTR>(::MileAllocateMemory(
            cbData * sizeof(wchar_t)));
        if (*lpData)
        {
            DWORD Type = 0;
            hr = Mile::HResult::FromWin32(::RegQueryValueExW(
                hKey,
                lpValueName,
                nullptr,
                &Type,
                reinterpret_cast<LPBYTE>(*lpData),
                &cbData));
            if (SUCCEEDED(hr) && REG_SZ != Type && REG_EXPAND_SZ != Type)
                hr = __HRESULT_FROM_WIN32(ERROR_ILLEGAL_ELEMENT_ADDRESS);

            if (FAILED(hr))
            {
                ::MileFreeMemory(*lpData);
            }
        }
        else
        {
            hr = Mile::HResult::FromWin32(ERROR_NOT_ENOUGH_MEMORY);
        }
    }

    return hr;
}

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResult Mile::CoCheckInterfaceName(
    _In_ LPCWSTR InterfaceID,
    _In_ LPCWSTR InterfaceName)
{
    wchar_t RegistryKeyPath[64];
    if (0 != ::wcscpy_s(RegistryKeyPath, L"Interface\\"))
        return E_INVALIDARG;
    if (0 != ::wcscat_s(RegistryKeyPath, InterfaceID))
        return E_INVALIDARG;

    HKEY hKey = nullptr;
    Mile::HResult hr = Mile::HResult::FromWin32(::RegCreateKeyExW(
        HKEY_CLASSES_ROOT,
        RegistryKeyPath,
        0,
        nullptr,
        0,
        KEY_READ,
        nullptr,
        &hKey,
        nullptr));
    if (SUCCEEDED(hr))
    {
        wchar_t* InterfaceTypeName = nullptr;
        hr = Mile::RegQueryStringValue(hKey, nullptr, &InterfaceTypeName);
        if (SUCCEEDED(hr))
        {
            if (0 != ::_wcsicmp(InterfaceTypeName, InterfaceName))
            {
                hr = E_NOINTERFACE;
            }

            ::MileFreeMemory(InterfaceTypeName);
        }

        ::RegCloseKey(hKey);
    }

    return hr;
}

#endif

Mile::HResultFromLastError Mile::OpenProcessTokenByProcessId(
    _In_ DWORD ProcessId,
    _In_ DWORD DesiredAccess,
    _Out_ PHANDLE TokenHandle)
{
    BOOL Result = FALSE;

    HANDLE ProcessHandle = ::OpenProcess(
        PROCESS_QUERY_INFORMATION,
        FALSE,
        ProcessId);
    if (ProcessHandle)
    {
        Result = ::OpenProcessToken(
            ProcessHandle,
            DesiredAccess,
            TokenHandle);

        ::CloseHandle(ProcessHandle);
    }

    return Result;
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResult Mile::OpenServiceProcessToken(
    _In_ LPCWSTR ServiceName,
    _In_ DWORD DesiredAccess,
    _Out_ PHANDLE TokenHandle)
{
    SERVICE_STATUS_PROCESS ServiceStatus;

    Mile::HResult hr = Mile::HResultFromLastError(
        ::MileStartService(ServiceName, &ServiceStatus));
    if (hr == S_OK)
    {
        hr = Mile::OpenProcessTokenByProcessId(
            ServiceStatus.dwProcessId, DesiredAccess, TokenHandle);
    }

    return hr;
}

#endif

Mile::HResult Mile::AdjustTokenPrivilegesSimple(
    _In_ HANDLE TokenHandle,
    _In_ PLUID_AND_ATTRIBUTES Privileges,
    _In_ DWORD PrivilegeCount)
{
    Mile::HResult hr = E_INVALIDARG;

    if (Privileges && PrivilegeCount)
    {
        DWORD PSize = sizeof(LUID_AND_ATTRIBUTES) * PrivilegeCount;
        DWORD TPSize = PSize + sizeof(DWORD);

        PTOKEN_PRIVILEGES pTP = reinterpret_cast<PTOKEN_PRIVILEGES>(
            ::MileAllocateMemory(TPSize));
        if (pTP)
        {
            pTP->PrivilegeCount = PrivilegeCount;
            ::memcpy(pTP->Privileges, Privileges, PSize);

            ::AdjustTokenPrivileges(
                TokenHandle, FALSE, pTP, TPSize, nullptr, nullptr);
            hr = Mile::HResultFromLastError();

            ::MileFreeMemory(pTP);
        }
        else
        {
            hr = Mile::HResult::FromWin32(ERROR_NOT_ENOUGH_MEMORY);
        }
    }

    return hr;
}

Mile::HResult Mile::AdjustTokenAllPrivileges(
    _In_ HANDLE TokenHandle,
    _In_ DWORD Attributes)
{
    PTOKEN_PRIVILEGES pTokenPrivileges = nullptr;

    Mile::HResult hr = Mile::GetTokenInformationWithMemory(
        TokenHandle,
        TokenPrivileges,
        reinterpret_cast<PVOID*>(&pTokenPrivileges));
    if (hr == S_OK)
    {
        for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i)
        {
            pTokenPrivileges->Privileges[i].Attributes = Attributes;
        }

        hr = Mile::AdjustTokenPrivilegesSimple(
            TokenHandle,
            pTokenPrivileges->Privileges,
            pTokenPrivileges->PrivilegeCount);

        ::MileFreeMemory(pTokenPrivileges);
    }

    return hr;
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

Mile::HResultFromLastError Mile::LoadResource(
    _Out_ Mile::PRESOURCE_INFO ResourceInfo,
    _In_opt_ HMODULE ModuleHandle,
    _In_ LPCWSTR Type,
    _In_ LPCWSTR Name)
{
    if (!ResourceInfo)
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    std::memset(
        ResourceInfo,
        0,
        sizeof(Mile::RESOURCE_INFO));

    HRSRC ResourceFind = ::FindResourceExW(
        ModuleHandle,
        Type,
        Name,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
    if (!ResourceFind)
    {
        return FALSE;
    }

    ResourceInfo->Size = ::SizeofResource(
        ModuleHandle,
        ResourceFind);
    if (ResourceInfo->Size == 0)
    {
        return FALSE;
    }

    HGLOBAL ResourceLoad = ::LoadResource(
        ModuleHandle,
        ResourceFind);
    if (!ResourceLoad)
    {
        return FALSE;
    }

    ResourceInfo->Pointer = ::LockResource(
        ResourceLoad);

    return TRUE;
}

#endif

#pragma endregion

#pragma region Implementations for Windows (C++ Style)

std::wstring Mile::GetHResultMessage(
    Mile::HResult const& Value)
{
    std::wstring Message{ L"Failed to get formatted message." };

    LPWSTR RawMessage = nullptr;
    DWORD RawMessageSize = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK,
        nullptr,
        Value,
        0,
        reinterpret_cast<LPTSTR>(&RawMessage),
        0,
        nullptr);
    if (RawMessageSize)
    {
        Message = std::wstring(RawMessage, RawMessageSize);
        if (Value.IsFailed())
        {
            Message += Mile::FormatWideString(L" (0x%08lX)", Value);
        }

        ::LocalFree(RawMessage);
    }

    return Message;
}

std::wstring Mile::GetSystemDirectoryW()
{
    std::wstring Path;

    UINT Length = ::GetSystemDirectoryW(nullptr, 0);
    if (Length)
    {
        Path.resize(Length);
        Length = ::GetSystemDirectoryW(&Path[0], Length);
        Path.resize(Length);
    }

    return Path;
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

std::wstring Mile::GetWindowsDirectoryW()
{
    std::wstring Path;

    UINT Length = ::GetSystemWindowsDirectoryW(nullptr, 0);
    if (Length)
    {
        Path.resize(Length);
        Length = ::GetSystemWindowsDirectoryW(&Path[0], Length);
        Path.resize(Length);
    }

    return Path;
}

#endif

std::wstring Mile::ExpandEnvironmentStringsW(
    std::wstring const& SourceString)
{
    std::wstring DestinationString;

    UINT Length = ::ExpandEnvironmentStringsW(
        SourceString.c_str(),
        nullptr,
        0);
    if (Length)
    {
        DestinationString.resize(Length);
        Length = ::ExpandEnvironmentStringsW(
            SourceString.c_str(),
            &DestinationString[0],
            Length);
        DestinationString.resize(Length - 1);
    }

    return DestinationString;
}

std::wstring Mile::GetCurrentProcessModulePath()
{
    // 32767 is the maximum path length without the terminating null character.
    std::wstring Path(32767, L'\0');
    Path.resize(::GetModuleFileNameW(
        nullptr, &Path[0], static_cast<DWORD>(Path.size())));
    return Path;
}

std::wstring Mile::ConvertByteSizeToUtf16String(
    std::uint64_t ByteSize)
{
    const wchar_t* Systems[] =
    {
        L"Byte",
        L"Bytes",
        L"KiB",
        L"MiB",
        L"GiB",
        L"TiB",
        L"PiB",
        L"EiB"
    };

    size_t nSystem = 0;
    double result = static_cast<double>(ByteSize);

    if (ByteSize > 1)
    {
        for (
            nSystem = 1;
            nSystem < sizeof(Systems) / sizeof(*Systems);
            ++nSystem)
        {
            if (1024.0 > result)
                break;

            result /= 1024.0;
        }

        result = static_cast<uint64_t>(result * 100) / 100.0;
    }

    return Mile::FormatWideString(L"%.1lf %s", result, Systems[nSystem]);
}

std::string Mile::ConvertByteSizeToUtf8String(
    std::uint64_t ByteSize)
{
    const char* Systems[] =
    {
        "Byte",
        "Bytes",
        "KiB",
        "MiB",
        "GiB",
        "TiB",
        "PiB",
        "EiB"
    };

    size_t nSystem = 0;
    double result = static_cast<double>(ByteSize);

    if (ByteSize > 1)
    {
        for (
            nSystem = 1;
            nSystem < sizeof(Systems) / sizeof(*Systems);
            ++nSystem)
        {
            if (1024.0 > result)
                break;

            result /= 1024.0;
        }

        result = static_cast<uint64_t>(result * 100) / 100.0;
    }

    return Mile::FormatString("%.1lf %s", result, Systems[nSystem]);
}

#pragma endregion
