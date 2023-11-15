/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Windows.h
 * PURPOSE:   Definition for Windows
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: MouriNaruto (KurikoMouri@outlook.jp)
 */

#ifndef MILE_WINDOWS
#define MILE_WINDOWS

#include "Mile.Portable.h"

#include <Mile.Helpers.h>
#include <Mile.Helpers.CppBase.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)
#include <ShellScalingApi.h>
#endif

namespace Mile
{
#pragma region Definitions and Implementations for Windows

    /**
     * @brief A type representing an HRESULT error code.
    */
    class HResult
    {
    public:

        /**
         * @brief The HRESULT error code represented by the HResult object.
        */
        HRESULT Value{ S_OK };

    public:

        /**
         * @brief Initializes a new instance of the HResult object.
         * @return A new instance of the HResult object.
        */
        constexpr HResult() noexcept = default;

        /**
         * @brief Initializes a new instance of the HResult object by an
         *        HRESULT code.
         * @param Value An HRESULT code that initializes the HResult object.
         * @return A new instance of the HResult object.
        */
        constexpr HResult(
            _In_ HRESULT const Value) noexcept : Value(Value)
        {
        }

        /**
         * @brief Initializes a new instance of the HResult object by a system
         *        error code.
         * @param Code The system error code.
         * @return A new instance of the HResult object.
        */
        static HResult FromWin32(
            _In_ DWORD Code) noexcept
        {
            return HResult(::HRESULT_FROM_WIN32(Code));
        }

    public:

        /**
         * @brief Retrieves the HRESULT error code for the error represented by
         *        the HResult object.
         * @return An HRESULT error code.
        */
        constexpr operator HRESULT() const noexcept
        {
            return this->Value;
        }

        /**
         * @brief Test for success on HRESULT error code represented by the
         *        HResult object.
         * @return The test result.
        */
        bool IsSucceeded() const noexcept
        {
            return SUCCEEDED(this->Value);
        }

        /**
         * @brief Test for failed on HRESULT error code represented by the
         *        HResult object.
         * @return The test result.
        */
        bool IsFailed() const noexcept
        {
            return FAILED(this->Value);
        }

        /**
         * @brief Test for errors on HRESULT error code represented by the
         *        HResult object.
         * @return The test result.
        */
        bool IsError() const noexcept
        {
            return IS_ERROR(this->Value);
        }

        /**
         * @brief Extracts the facility portion of HRESULT error code
         *        represented by the HResult object.
         * @return The facility portion value of HRESULT error code represented
         *         by the HResult object.
        */
        DWORD GetFacility() const noexcept
        {
            return HRESULT_FACILITY(this->Value);
        }

        /**
         * @brief Extracts the code portion of HRESULT error code represented
         *        by the HResult object.
         * @return The code portion value of HRESULT error code represented by
         *         the HResult object.
        */
        DWORD GetCode() const noexcept
        {
            return HRESULT_CODE(this->Value);
        }
    };

    /**
     * @brief A type representing a converter which converts the calling
     *        thread's last-error code to the HResult object.
    */
    class HResultFromLastError
    {
    private:

        /**
         * @brief Indicates needed the evaluation of the Win32 BOOL value.
        */
        bool m_EvaluateWithWin32Bool;

        /**
         * @brief The Win32 BOOL value.
        */
        BOOL m_Value;

    public:

        /**
         * @brief Initializes a new instance of the HResultFromLastError object
         *        by the calling thread's last-error code.
        */
        HResultFromLastError() :
            m_EvaluateWithWin32Bool(false),
            m_Value(FALSE)
        {
        }

        /**
         * @brief Initializes a new instance of the HResultFromLastError object
         *        by the calling thread's last-error code with the evaluation
         *        of the Win32 BOOL value.
         * @param Result The Win32 BOOL value.
        */
        HResultFromLastError(
            _In_ BOOL Result) :
            m_EvaluateWithWin32Bool(true),
            m_Value(Result)
        {
        }

        /**
         * @brief Converts the calling thread's last-error code to the HResult
         *        object.
        */
        operator HResult()
        {
            // Return if Win32 BOOL value is TRUE.
            // By design, If the this->m_Value is euqal to true,
            // the this->m_EvaluateWithWin32Bool is also euqal to true.
            if (this->m_Value)
            {
                return S_OK;
            }

            HResult hr = HResult::FromWin32(::GetLastError());

            // Set hr failed when hr succeed if it needs the evaluation of the
            // Win32 BOOL value and the Win32 BOOL value is FALSE.
            if (this->m_EvaluateWithWin32Bool && hr == S_OK)
            {
                hr = HResult::FromWin32(ERROR_FUNCTION_FAILED);
            }

            return hr;
        }

        /**
         * @brief Converts the calling thread's last-error code to the HRESULT
         *        value.
        */
        operator HRESULT()
        {
            return this->operator Mile::HResult();
        }
    };

    /**
     * @brief Wraps a critical section object.
    */
    class CriticalSection : DisableCopyConstruction, DisableMoveConstruction
    {
    public:

        /**
         * @brief Initializes a critical section object.
         * @param lpCriticalSection A pointer to the critical section object.
         * @remark For more information, see InitializeCriticalSection.
         */
        static void Initialize(
            _Out_ LPCRITICAL_SECTION lpCriticalSection) noexcept
        {
            ::InitializeCriticalSection(lpCriticalSection);
        }

        /**
         * @brief Releases all resources used by an unowned critical section
         *        object.
         * @param lpCriticalSection A pointer to the critical section object.
         * @remark For more information, see DeleteCriticalSection.
         */
        static void Delete(
            _Inout_ LPCRITICAL_SECTION lpCriticalSection) noexcept
        {
            ::DeleteCriticalSection(lpCriticalSection);
        }

        /**
         * @brief Waits for ownership of the specified critical section object.
         *        The function returns when the calling thread is granted
         *        ownership.
         * @param lpCriticalSection A pointer to the critical section object.
         * @remark For more information, see EnterCriticalSection.
         */
        static void Enter(
            _Inout_ LPCRITICAL_SECTION lpCriticalSection) noexcept
        {
            ::EnterCriticalSection(lpCriticalSection);
        }

        /**
         * @brief Attempts to enter a critical section without blocking. If the
         *        call is successful, the calling thread takes ownership of the
         *        critical section.
         * @param lpCriticalSection A pointer to the critical section object.
         * @return If the critical section is successfully entered or the
         *         current thread already owns the critical section, the return
         *         value is true. If another thread already owns the critical
         *         section, the return value is false.
         * @remark For more information, see TryEnterCriticalSection.
         */
        static bool TryEnter(
            _Inout_ LPCRITICAL_SECTION lpCriticalSection) noexcept
        {
            return FALSE != ::TryEnterCriticalSection(lpCriticalSection);
        }

        /**
         * @brief Releases ownership of the specified critical section object.
         * @param lpCriticalSection A pointer to the critical section object.
         * @remark For more information, see LeaveCriticalSection.
         */
        static void Leave(
            _Inout_ LPCRITICAL_SECTION lpCriticalSection) noexcept
        {
            ::LeaveCriticalSection(lpCriticalSection);
        }

    private:

        /**
         * @brief The raw critical section object.
        */
        CRITICAL_SECTION m_RawObject;

    public:

        /**
         * @brief Initializes the critical section object.
        */
        CriticalSection() noexcept
        {
            Initialize(&this->m_RawObject);
        }

        /**
         * @brief Releases all resources used by the critical section object.
        */
        ~CriticalSection() noexcept
        {
            Delete(&this->m_RawObject);
        }

        /**
         * @brief Waits for ownership of the critical section object. The
         *        function returns when the calling thread is granted ownership.
        */
        void Lock() noexcept
        {
            Enter(&this->m_RawObject);
        }

        /**
         * @brief Attempts to enter the critical section without blocking. If
         *        the call is successful, the calling thread takes ownership of
         *        the critical section.
         * @return If the critical section is successfully entered or the
         *         current thread already owns the critical section, the return
         *         value is true. If another thread already owns the critical
         *         section, the return value is false.
        */
        bool TryLock() noexcept
        {
            return TryEnter(&this->m_RawObject);
        }

        /**
         * @brief Releases ownership of the critical section object.
        */
        void Unlock() noexcept
        {
            Leave(&this->m_RawObject);
        }
    };

    /**
     * @brief Provides automatic locking and unlocking of a critical section.
    */
    class AutoCriticalSectionLock
    {
    private:

        /**
         * @brief The critical section object.
        */
        CriticalSection& m_Object;

    public:

        /**
         * @brief Lock the critical section object.
         * @param Object The critical section object.
        */
        explicit AutoCriticalSectionLock(
            CriticalSection& Object) noexcept :
            m_Object(Object)
        {
            this->m_Object.Lock();
        }

        /**
         * @brief Unlock the critical section object.
        */
        ~AutoCriticalSectionLock() noexcept
        {
            this->m_Object.Unlock();
        }
    };

    /**
     * @brief Provides automatic trying to locking and unlocking of a critical
     *        section.
    */
    class AutoCriticalSectionTryLock
    {
    private:

        /**
         * @brief The critical section object.
        */
        CriticalSection& m_Object;

        /**
         * @brief The lock status.
        */
        bool m_IsLocked;

    public:

        /**
         * @brief Try to lock the critical section object.
         * @param Object The critical section object.
        */
        explicit AutoCriticalSectionTryLock(
            CriticalSection& Object) noexcept :
            m_Object(Object)
        {
            this->m_IsLocked = this->m_Object.TryLock();
        }

        /**
         * @brief Try to unlock the critical section object.
        */
        ~AutoCriticalSectionTryLock() noexcept
        {
            if (this->m_IsLocked)
            {
                this->m_Object.Unlock();
            }
        }

        /**
         * @brief Check the lock status.
         * @return The lock status.
        */
        bool IsLocked() const
        {
            return this->m_IsLocked;
        }
    };

    /**
     * @brief Provides automatic locking and unlocking of a raw critical section.
    */
    class AutoRawCriticalSectionLock
    {
    private:

        /**
         * @brief The raw critical section object.
        */
        CRITICAL_SECTION& m_Object;

    public:

        /**
         * @brief Lock the raw critical section object.
         * @param Object The raw critical section object.
        */
        explicit AutoRawCriticalSectionLock(
            CRITICAL_SECTION& Object) noexcept :
            m_Object(Object)
        {
            CriticalSection::Enter(&this->m_Object);
        }

        /**
         * @brief Unlock the raw critical section object.
        */
        ~AutoRawCriticalSectionLock() noexcept
        {
            CriticalSection::Leave(&this->m_Object);
        }
    };

    /**
     * @brief Provides automatic trying to locking and unlocking of a raw
     *        critical section.
    */
    class AutoRawCriticalSectionTryLock
    {
    private:

        /**
         * @brief The raw critical section object.
        */
        CRITICAL_SECTION& m_Object;

        /**
         * @brief The lock status.
        */
        bool m_IsLocked;

    public:

        /**
         * @brief Try to lock the raw critical section object.
         * @param Object The raw critical section object.
        */
        explicit AutoRawCriticalSectionTryLock(
            CRITICAL_SECTION& Object) noexcept :
            m_Object(Object)
        {
            this->m_IsLocked = CriticalSection::TryEnter(&this->m_Object);
        }

        /**
         * @brief Try to unlock the raw critical section object.
        */
        ~AutoRawCriticalSectionTryLock() noexcept
        {
            if (this->m_IsLocked)
            {
                CriticalSection::Leave(&this->m_Object);
            }
        }

        /**
         * @brief Check the lock status.
         * @return The lock status.
        */
        bool IsLocked() const
        {
            return this->m_IsLocked;
        }
    };

    /**
     * @brief Wraps a slim reader/writer (SRW) lock.
    */
    class SRWLock : DisableCopyConstruction, DisableMoveConstruction
    {
    public:

        /**
         * @brief Initialize a slim reader/writer (SRW) lock.
         * @param SRWLock A pointer to the SRW lock.
         * @remark For more information, see InitializeSRWLock.
         */
        static void Initialize(
            _Out_ PSRWLOCK SRWLock) noexcept
        {
            ::InitializeSRWLock(SRWLock);
        }

        /**
         * @brief Acquires a slim reader/writer (SRW) lock in exclusive mode.
         * @param SRWLock A pointer to the SRW lock.
         * @remark For more information, see AcquireSRWLockExclusive.
         */
        static void AcquireExclusive(
            _Inout_ PSRWLOCK SRWLock) noexcept
        {
            ::AcquireSRWLockExclusive(SRWLock);
        }

        /**
         * @brief Attempts to acquire a slim reader/writer (SRW) lock in
         *        exclusive mode. If the call is successful, the calling thread
         *        takes ownership of the lock.
         * @param SRWLock A pointer to the SRW lock.
         * @return If the lock is successfully acquired, the return value is
         *         true. If the current thread could not acquire the lock, the
         *         return value is false.
         * @remark For more information, see TryAcquireSRWLockExclusive.
         */
        static bool TryAcquireExclusive(
            _Inout_ PSRWLOCK SRWLock) noexcept
        {
            return FALSE != ::TryAcquireSRWLockExclusive(SRWLock);
        }

        /**
         * @brief Releases a slim reader/writer (SRW) lock that was acquired in
         *        exclusive mode.
         *
         * @param SRWLock A pointer to the SRW lock.
         * @remark For more information, see ReleaseSRWLockExclusive.
         */
        static void ReleaseExclusive(
            _Inout_ PSRWLOCK SRWLock) noexcept
        {
            ::ReleaseSRWLockExclusive(SRWLock);
        }

        /**
         * @brief Acquires a slim reader/writer (SRW) lock in shared mode.
         * @param SRWLock A pointer to the SRW lock.
         * @remark For more information, see AcquireSRWLockShared.
         */
        static void AcquireShared(
            _Inout_ PSRWLOCK SRWLock) noexcept
        {
            ::AcquireSRWLockShared(SRWLock);
        }

        /**
         * @brief Attempts to acquire a slim reader/writer (SRW) lock in shared
         *        mode. If the call is successful, the calling thread takes
         *        ownership of the lock.
         * @param SRWLock A pointer to the SRW lock.
         * @return If the lock is successfully acquired, the return value is
         *         true. If the current thread could not acquire the lock, the
         *         return value is false.
         * @remark For more information, see TryAcquireSRWLockShared.
         */
        static bool TryAcquireShared(
            _Inout_ PSRWLOCK SRWLock) noexcept
        {
            return FALSE != ::TryAcquireSRWLockShared(SRWLock);
        }

        /**
         * @brief Releases a slim reader/writer (SRW) lock that was acquired in
         *        shared mode.
         * @param SRWLock A pointer to the SRW lock.
         * @remark For more information, see ReleaseSRWLockShared.
         */
        static void ReleaseShared(
            _Inout_ PSRWLOCK SRWLock) noexcept
        {
            ::ReleaseSRWLockShared(SRWLock);
        }

    private:

        /**
         * @brief The raw slim reader/writer (SRW) lock object.
        */
        SRWLOCK m_RawObject;

    public:

        /**
         * @brief Initialize the slim reader/writer (SRW) lock.
        */
        SRWLock() noexcept
        {
            Initialize(&this->m_RawObject);
        }

        /**
         * @brief Acquires the slim reader/writer (SRW) lock in exclusive mode.
        */
        void LockExclusive() noexcept
        {
            AcquireExclusive(&this->m_RawObject);
        }

        /**
         * @brief Attempts to acquire the slim reader/writer (SRW) lock in
         *        exclusive mode. If the call is successful, the calling thread
         *        takes ownership of the lock.
         * @return If the lock is successfully acquired, the return value is
         *         true. If the current thread could not acquire the lock, the
         *         return value is false.
        */
        bool TryLockExclusive() noexcept
        {
            return TryAcquireExclusive(&this->m_RawObject);
        }

        /**
         * @brief Releases the slim reader/writer (SRW) lock that was acquired
         *        in exclusive mode.
        */
        void UnlockExclusive() noexcept
        {
            ReleaseExclusive(&this->m_RawObject);
        }

        /**
         * @brief Acquires the slim reader/writer (SRW) lock in shared mode.
        */
        void LockShared() noexcept
        {
            AcquireShared(&this->m_RawObject);
        }

        /**
         * @brief Attempts to acquire the slim reader/writer (SRW) lock in
         *        shared mode. If the call is successful, the calling thread
         *        takes ownership of the lock.
         * @return If the lock is successfully acquired, the return value is
         *         true. If the current thread could not acquire the lock, the
         *         return value is false.
        */
        bool TryLockShared() noexcept
        {
            return TryAcquireShared(&this->m_RawObject);
        }

        /**
         * @brief Releases the slim reader/writer (SRW) lock that was acquired
         *        in shared mode.
        */
        void UnlockShared() noexcept
        {
            ReleaseShared(&this->m_RawObject);
        }
    };

    /**
     * @brief Provides automatic exclusive locking and unlocking of a slim
     *        reader/writer (SRW) lock.
    */
    class AutoSRWExclusiveLock
    {
    private:

        /**
         * @brief The slim reader/writer (SRW) lock object.
        */
        SRWLock& m_Object;

    public:

        /**
         * @brief Exclusive lock the slim reader/writer (SRW) lock object.
         * @param Object The slim reader/writer (SRW) lock object.
        */
        explicit AutoSRWExclusiveLock(
            SRWLock& Object) noexcept :
            m_Object(Object)
        {
            this->m_Object.LockExclusive();
        }

        /**
         * @brief Exclusive unlock the slim reader/writer (SRW) lock object.
        */
        ~AutoSRWExclusiveLock() noexcept
        {
            this->m_Object.UnlockExclusive();
        }
    };

    /**
     * @brief Provides automatic trying to exclusive locking and unlocking of a
     *        slim reader/writer (SRW) lock.
    */
    class AutoSRWExclusiveTryLock
    {
    private:

        /**
         * @brief The slim reader/writer (SRW) lock object.
        */
        SRWLock& m_Object;

        /**
         * @brief The lock status.
        */
        bool m_IsLocked;

    public:

        /**
         * @brief Try to exclusive lock the slim reader/writer (SRW) lock
         *        object.
         * @param Object The slim reader/writer (SRW) lock object.
        */
        explicit AutoSRWExclusiveTryLock(
            SRWLock& Object) noexcept :
            m_Object(Object)
        {
            this->m_IsLocked = this->m_Object.TryLockExclusive();
        }

        /**
         * @brief Try to exclusive unlock the slim reader/writer (SRW) lock
         *        object.
        */
        ~AutoSRWExclusiveTryLock() noexcept
        {
            if (this->m_IsLocked)
            {
                this->m_Object.UnlockExclusive();
            }
        }

        /**
         * @brief Check the lock status.
         * @return The lock status.
        */
        bool IsLocked() const
        {
            return this->m_IsLocked;
        }
    };

    /**
     * @brief Provides automatic shared locking and unlocking of a slim
     *        reader/writer (SRW) lock.
    */
    class AutoSRWSharedLock
    {
    private:

        /**
         * @brief The slim reader/writer (SRW) lock object.
        */
        SRWLock& m_Object;

    public:

        /**
         * @brief Shared lock the slim reader/writer (SRW) lock object.
         * @param Object The slim reader/writer (SRW) lock object.
        */
        explicit AutoSRWSharedLock(
            SRWLock& Object) noexcept :
            m_Object(Object)
        {
            this->m_Object.LockShared();
        }

        /**
         * @brief Shared unlock the slim reader/writer (SRW) lock object.
        */
        ~AutoSRWSharedLock() noexcept
        {
            this->m_Object.UnlockShared();
        }
    };

    /**
     * @brief Provides automatic trying to shared locking and unlocking of a
     *        slim reader/writer (SRW) lock.
    */
    class AutoSRWSharedTryLock
    {
    private:

        /**
         * @brief The slim reader/writer (SRW) lock object.
        */
        SRWLock& m_Object;

        /**
         * @brief The lock status.
        */
        bool m_IsLocked;

    public:

        /**
         * @brief Try to shared lock the slim reader/writer (SRW) lock object.
         * @param Object The slim reader/writer (SRW) lock object.
        */
        explicit AutoSRWSharedTryLock(
            SRWLock& Object) noexcept :
            m_Object(Object)
        {
            this->m_IsLocked = this->m_Object.TryLockShared();
        }

        /**
         * @brief Try to shared unlock the slim reader/writer (SRW) lock
         *        object.
        */
        ~AutoSRWSharedTryLock() noexcept
        {
            if (this->m_IsLocked)
            {
                this->m_Object.UnlockShared();
            }
        }

        /**
         * @brief Check the lock status.
         * @return The lock status.
        */
        bool IsLocked() const
        {
            return this->m_IsLocked;
        }
    };

    /**
     * @brief Provides automatic exclusive locking and unlocking of a raw slim
     *        reader/writer (SRW) lock.
    */
    class AutoRawSRWExclusiveLock
    {
    private:

        /**
         * @brief The slim reader/writer (SRW) lock object.
        */
        SRWLOCK& m_Object;

    public:

        /**
         * @brief Exclusive lock the raw slim reader/writer (SRW) lock object.
         * @param Object The raw slim reader/writer (SRW) lock object.
        */
        explicit AutoRawSRWExclusiveLock(
            SRWLOCK& Object) noexcept :
            m_Object(Object)
        {
            SRWLock::AcquireExclusive(&this->m_Object);
        }

        /**
         * @brief Exclusive unlock the raw slim reader/writer (SRW) lock
         *        object.
        */
        ~AutoRawSRWExclusiveLock() noexcept
        {
            SRWLock::ReleaseExclusive(&this->m_Object);
        }
    };

    /**
     * @brief Provides automatic trying to exclusive locking and unlocking of a
     *        raw slim reader/writer (SRW) lock.
    */
    class AutoRawSRWExclusiveTryLock
    {
    private:

        /**
         * @brief The raw slim reader/writer (SRW) lock object.
        */
        SRWLOCK& m_Object;

        /**
         * @brief The lock status.
        */
        bool m_IsLocked;

    public:

        /**
         * @brief Try to exclusive lock the raw slim reader/writer (SRW) lock
         *        object.
         * @param Object The slim reader/writer (SRW) lock object.
        */
        explicit AutoRawSRWExclusiveTryLock(
            SRWLOCK& Object) noexcept :
            m_Object(Object)
        {
            this->m_IsLocked = SRWLock::TryAcquireExclusive(&this->m_Object);
        }

        /**
         * @brief Try to exclusive unlock the raw slim reader/writer (SRW) lock
         *        object.
        */
        ~AutoRawSRWExclusiveTryLock() noexcept
        {
            if (this->m_IsLocked)
            {
                SRWLock::ReleaseExclusive(&this->m_Object);
            }
        }

        /**
         * @brief Check the lock status.
         * @return The lock status.
        */
        bool IsLocked() const
        {
            return this->m_IsLocked;
        }
    };

    /**
     * @brief Provides automatic shared locking and unlocking of a raw slim
     *        reader/writer (SRW) lock.
    */
    class AutoRawSRWSharedLock
    {
    private:

        /**
         * @brief The raw slim reader/writer (SRW) lock object.
        */
        SRWLOCK& m_Object;

    public:

        /**
         * @brief Shared lock the raw slim reader/writer (SRW) lock object.
         * @param Object The raw slim reader/writer (SRW) lock object.
        */
        explicit AutoRawSRWSharedLock(
            SRWLOCK& Object) noexcept :
            m_Object(Object)
        {
            SRWLock::AcquireShared(&this->m_Object);
        }

        /**
         * @brief Shared unlock the raw slim reader/writer (SRW) lock object.
        */
        ~AutoRawSRWSharedLock() noexcept
        {
            SRWLock::ReleaseShared(&this->m_Object);
        }
    };

    /**
     * @brief Provides automatic trying to shared locking and unlocking of a
     *        raw slim reader/writer (SRW) lock.
    */
    class AutoRawSRWSharedTryLock
    {
    private:

        /**
         * @brief The raw slim reader/writer (SRW) lock object.
        */
        SRWLOCK& m_Object;

        /**
         * @brief The lock status.
        */
        bool m_IsLocked;

    public:

        /**
         * @brief Try to shared lock the raw slim reader/writer (SRW) lock object.
         * @param Object The raw slim reader/writer (SRW) lock object.
        */
        explicit AutoRawSRWSharedTryLock(
            SRWLOCK& Object) noexcept :
            m_Object(Object)
        {
            this->m_IsLocked = SRWLock::TryAcquireShared(&this->m_Object);
        }

        /**
         * @brief Try to shared unlock the raw slim reader/writer (SRW) lock
         *        object.
        */
        ~AutoRawSRWSharedTryLock() noexcept
        {
            if (this->m_IsLocked)
            {
                SRWLock::ReleaseShared(&this->m_Object);
            }
        }

        /**
         * @brief Check the lock status.
         * @return The lock status.
        */
        bool IsLocked() const
        {
            return this->m_IsLocked;
        }
    };

#pragma endregion

#pragma region Definitions for Windows (Win32 Style)

    /**
     * @brief Sets the system's compression (Compact OS) state.
     * @param DeploymentState The system's compression (Compact OS) state. If
     *                        this value is TRUE, the system state means
     *                        Compact. If it is FALSE, the system state means
     *                        non Compact.
     * @return An HResult object containing the error code.
    */
    HResult GetCompactOsDeploymentState(
        _Out_ PDWORD DeploymentState);

    /**
     * @brief Gets the system's compression (Compact OS) state.
     * @param DeploymentState The system's compression (Compact OS) state. If
     *                        this value is TRUE, the function sets the system
     *                        state to Compact. If it is FALSE, the function
     *                        sets the system state to non Compact.
     * @return An HResult object containing the error code.
    */
    HResult SetCompactOsDeploymentState(
        _In_ DWORD DeploymentState);

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Enables WM_DPICHANGED message for child window for the associated
     *        window.
     * @param WindowHandle The window you want to enable WM_DPICHANGED message
     *                     for child window.
     * @return If the function succeeds, the return value is non-zero. If the
     *         function fails, the return value is zero.
     * @remarks You need to use this function in Windows 10 Threshold 1 or
     *          Windows 10 Threshold 2.
    */
    BOOL EnableChildWindowDpiMessage(
        _In_ HWND WindowHandle);

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Queries the dots per inch (dpi) of a display.
     * @param hMonitor Handle of the monitor being queried.
     * @param dpiType The type of DPI being queried. Possible values are from
     *                the MONITOR_DPI_TYPE enumeration.
     * @param dpiX The value of the DPI along the X axis. This value always
     *             refers to the horizontal edge, even when the screen is
     *             rotated.
     * @param dpiY The value of the DPI along the Y axis. This value always
     *             refers to the vertical edge, even when the screen is
     *             rotated.
     * @return An HResult object containing the error code.
     * @remark For more information, see GetDpiForMonitor.
    */
    HResult GetDpiForMonitor(
        _In_ HMONITOR hMonitor,
        _In_ MONITOR_DPI_TYPE dpiType,
        _Out_ UINT* dpiX,
        _Out_ UINT* dpiY);

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Obtains the primary access token of the logged-on user specified
     *        by the session ID. To call this function successfully, the
     *        calling application must be running within the context of the
     *        LocalSystem account and have the SE_TCB_NAME privilege.
     * @param SessionId A Remote Desktop Services session identifier.
     * @param TokenHandle If the function succeeds, receives a pointer to the
     *                    token handle for the logged-on user. Note that you
     *                    must call the CloseHandle function to close this
     *                    handle.
     * @return An HResultFromLastError object An containing the HResult object
     *         containing the error code.
     * @remark For more information, see WTSQueryUserToken.
    */
    HResultFromLastError CreateSessionToken(
        _In_ DWORD SessionId,
        _Out_ PHANDLE TokenHandle);

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Obtains the primary access token of the SYSTEM user. To call this
     *        function successfully, the calling application must be running
     *        within the context of the Administrator account and have the
     *        SE_DEBUG_NAME privilege enabled.
     * @param DesiredAccess The access to the process object. This access right
     *                      is checked against the security descriptor for the
     *                      process. This parameter can be one or more of the
     *                      process access rights.
     * @param TokenHandle If the function succeeds, receives a pointer to the
     *                    token handle for the SYSTEM user. Note that you
     *                    must call the CloseHandle function to close this
     *                    handle.
     * @return An HResultFromLastError object An containing the HResult object
     *         containing the error code.
    */
    HResultFromLastError CreateSystemToken(
        _In_ DWORD DesiredAccess,
        _Out_ PHANDLE TokenHandle);

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Retrieves the session identifier of the active session.
     * @return The session identifier of the active session. If there is no
     *         active session attached, this function returns 0xFFFFFFFF.
    */
    DWORD GetActiveSessionID();

#endif

    /**
     * @brief Sets mandatory label for a specified access token. The
     *        information that this function sets replaces existing
     *        information. The calling process must have appropriate access
     *        rights to set the information.
     * @param TokenHandle A handle to the access token for which information is
     *                    to be set.
     * @param MandatoryLabelRid The value of the mandatory label for the
     *                          process. This parameter can be one of the
     *                          following values.
     *                          SECURITY_MANDATORY_UNTRUSTED_RID
     *                          SECURITY_MANDATORY_LOW_RID
     *                          SECURITY_MANDATORY_MEDIUM_RID
     *                          SECURITY_MANDATORY_MEDIUM_PLUS_RID
     *                          SECURITY_MANDATORY_HIGH_RID
     *                          SECURITY_MANDATORY_SYSTEM_RID
     *                          SECURITY_MANDATORY_PROTECTED_PROCESS_RID
     * @return An HResultFromLastError object An containing the HResult object
     *         containing the error code.
    */
    HResultFromLastError SetTokenMandatoryLabel(
        _In_ HANDLE TokenHandle,
        _In_ DWORD MandatoryLabelRid);

    /**
     * @brief Retrieves a specified type of information about an access token.
     *        The calling process must have appropriate access rights to obtain
     *        the information.
     * @param TokenHandle A handle to an access token from which information is
     *                    retrieved.
     * @param TokenInformationClass Specifies a value from the
     *                              TOKEN_INFORMATION_CLASS enumerated type to
     *                              identify the type of information the
     *                              function retrieves.
     * @param OutputInformation A pointer to a buffer the function fills with
     *                          the requested information. When you have
     *                          finished using the information, free it by
     *                          calling the MileFreeMemory method. You
     *                          should also set the pointer to nullptr.
     * @return An HResultFromLastError object An containing the HResult object
     *         containing the error code.
     * @remark For more information, see GetTokenInformation.
    */
    HResultFromLastError GetTokenInformationWithMemory(
        _In_ HANDLE TokenHandle,
        _In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
        _Out_ PVOID* OutputInformation);

    /**
     * @brief Creates a new access token that is a LUA version of an existing
     *        access token.
     * @param ExistingTokenHandle A handle to a primary or impersonation token.
     *                            The token can also be a restricted token. The
     *                            handle must have TOKEN_DUPLICATE access to
     *                            the token.
     * @param TokenHandle A pointer to a variable that receives a handle to the
     *                    new restricted token.
     * @return An HResult object containing the error code.
    */
    HResult CreateLUAToken(
        _In_ HANDLE ExistingTokenHandle,
        _Out_ PHANDLE TokenHandle);

    /**
     * @brief Creates a single uninitialized object of the class associated
     *        with a specified CLSID.
     * @param lpszCLSID The string representation of the CLSID.
     * @param pUnkOuter If nullptr, indicates that the object is not being
     *                  created as part of an aggregate. If non-nullptr,
     *                  pointer to the aggregate object's IUnknown interface
     *                  (the controlling IUnknown).
     * @param dwClsContext Context in which the code that manages the newly
     *                     created object will run. The values are taken from
     *                     the enumeration CLSCTX.
     * @param lpszIID The string representation of the IID.
     * @param ppv Address of pointer variable that receives the interface
     *            pointer requested in riid. Upon successful return, *ppv
     *            contains the requested interface pointer. Upon failure, *ppv
     *            contains nullptr.
     * @return An HResult object containing the error code.
    */
    HResult CoCreateInstanceByString(
        _In_ LPCWSTR lpszCLSID,
        _In_opt_ LPUNKNOWN pUnkOuter,
        _In_ DWORD dwClsContext,
        _In_ LPCWSTR lpszIID,
        _Out_ LPVOID* ppv);

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Retrieves the string type data for the specified value name
     *        associated with an open registry key.
     * @param hKey A handle to an open registry key.
     * @param lpValueName The name of the registry value.
     * @param lpData A pointer to a buffer that receives the value's data. When
     *               you have finished using the information, free it by
     *               calling the MileFreeMemory method. You should also
     *               set the pointer to nullptr.
     * @return An HResult object containing the error code.
    */
    HResult RegQueryStringValue(
        _In_ HKEY hKey,
        _In_opt_ LPCWSTR lpValueName,
        _Out_ LPWSTR* lpData);

#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Determines whether the interface id have the correct interface
     *        name.
     * @param InterfaceID A pointer to the string representation of the IID.
     * @param InterfaceName A pointer to the interface name string.
     * @return An HResult object containing the error code.
    */
    HResult CoCheckInterfaceName(
        _In_ LPCWSTR InterfaceID,
        _In_ LPCWSTR InterfaceName);

#endif

    /**
     * @brief Opens the access token associated with a process.
     * @param ProcessId The identifier of the local process to be opened.
     * @param DesiredAccess The access to the process object. This access right
     *                      is checked against the security descriptor for the
     *                      process. This parameter can be one or more of the
     *                      process access rights.
     * @param TokenHandle A pointer to a handle that identifies the newly
     *                    opened access token when the function returns.
     * @return An HResultFromLastError object An containing the HResult object
     *         containing the error code.
    */
    HResultFromLastError OpenProcessTokenByProcessId(
        _In_ DWORD ProcessId,
        _In_ DWORD DesiredAccess,
        _Out_ PHANDLE TokenHandle);

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Opens the access token associated with a service process, the
     *        calling application must be running within the context of the
     *        Administrator account and have the SE_DEBUG_NAME privilege
     *        enabled.
     * @param ServiceName The name of the service to be started. This is the
     *                    name specified by the ServiceName parameter of the
     *                    CreateService function when the service object was
     *                    created, not the service display name that is shown
     *                    by user interface applications to identify the
     *                    service. The maximum string length is 256 characters.
     *                    The service control manager database preserves the
     *                    case of the characters, but service name comparisons
     *                    are always case insensitive. Forward-slash (/) and
     *                    backslash (\) are invalid service name characters.
     * @param DesiredAccess The access to the process object. This access right
     *                      is checked against the security descriptor for the
     *                      process. This parameter can be one or more of the
     *                      process access rights.
     * @param TokenHandle A pointer to a handle that identifies the newly
     *                    opened access token when the function returns.
     * @return An HResult object containing the error code.
    */
    HResult OpenServiceProcessToken(
        _In_ LPCWSTR ServiceName,
        _In_ DWORD DesiredAccess,
        _Out_ PHANDLE TokenHandle);

#endif

    /**
     * @brief Enables or disables privileges in the specified access token.
     * @param TokenHandle A handle to the access token that contains the
     *                    privileges to be modified. The handle must have
     *                    TOKEN_ADJUST_PRIVILEGES access to the token.
     * @param Privileges A pointer to an array of LUID_AND_ATTRIBUTES
     *                   structures that specifies an array of privileges and
     *                   their attributes. Each structure contains the LUID and
     *                   attributes of a privilege. To get the name of the
     *                   privilege associated with a LUID, call the
     *                   LookupPrivilegeValue function, passing the address of
     *                   the LUID as the value of the lpLuid parameter. The
     *                   attributes of a privilege can be a combination of the
     *                   following values.
     *                   SE_PRIVILEGE_ENABLED
     *                       The function enables the privilege.
     *                   SE_PRIVILEGE_REMOVED
     *                       The privilege is removed from the list of
     *                       privileges in the token.
     *                   None
     *                       The function disables the privilege.
     * @param PrivilegeCount The number of entries in the Privileges array.
     * @return An HResult object containing the error code.
    */
    HResult AdjustTokenPrivilegesSimple(
        _In_ HANDLE TokenHandle,
        _In_ PLUID_AND_ATTRIBUTES Privileges,
        _In_ DWORD PrivilegeCount);

    /**
     * @brief Enables or disables all privileges in the specified access token.
     * @param TokenHandle A handle to the access token that contains the
     *                    privileges to be modified. The handle must have
     *                    TOKEN_ADJUST_PRIVILEGES access to the token.
     * @param Attributes The attributes of all privileges can be a combination
     *                   of the following values.
     *                   SE_PRIVILEGE_ENABLED
     *                       The function enables the privilege.
     *                   SE_PRIVILEGE_REMOVED
     *                       The privilege is removed from the list of
     *                       privileges in the token.
     *                   None
     *                       The function disables the privilege.
     * @return An HResult object containing the error code.
    */
    HResult AdjustTokenAllPrivileges(
        _In_ HANDLE TokenHandle,
        _In_ DWORD Attributes);

#pragma endregion

#pragma region Definitions for Windows (C++ Style)

    /**
     * @brief Retrieves the message for the error represented by the HResult object.
     * @param Value The HResult object which need to retrieve the message.
     * @return A std::wstring containing the error messsage.
    */
    std::wstring GetHResultMessage(
        HResult const& Value);

    /**
     * @brief Retrieves the path of the system directory. The system directory
     *        contains system files such as dynamic-link libraries and drivers.
     * @return The path of the system directory if successful, an empty string
     *         otherwise.
    */
    std::wstring GetSystemDirectoryW();

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)

    /**
     * @brief Retrieves the path of the shared Windows directory on a
     *        multi-user system.
     * @return The path of the shared Windows directory on a multi-user system
     *         if successful, an empty string otherwise.
    */
    std::wstring GetWindowsDirectoryW();

#endif

    /**
     * @brief Expands environment variable strings and replaces them with the
     *        values defined for the current user.
     * @param SourceString The string that contains one or more environment
                           variable strings (in the %variableName% form) you
                           need to expand.
     * @return The result string of expanding the environment variable strings
     *         if successful, an empty string otherwise.
    */
    std::wstring ExpandEnvironmentStringsW(
        std::wstring const& SourceString);

    /**
     * @brief Retrieves the path of the executable file of the current process.
     * @return The path of the executable file of the current process if
     *         successful, an empty string otherwise.
    */
    std::wstring GetCurrentProcessModulePath();

    /**
     * @brief Converts a numeric value into a UTF-16 string that represents
     *        the number expressed as a size value in byte, bytes, kibibytes,
     *        mebibytes, gibibytes, tebibytes, pebibytes or exbibytes,
     *        depending on the size.
     * @param ByteSize The numeric byte size value to be converted.
     * @return A formatted string if successful, an empty string otherwise.
    */
    std::wstring ConvertByteSizeToUtf16String(
        std::uint64_t ByteSize);

    /**
     * @brief Converts a numeric value into a UTF-8 string that represents
     *        the number expressed as a size value in byte, bytes, kibibytes,
     *        mebibytes, gibibytes, tebibytes, pebibytes or exbibytes,
     *        depending on the size.
     * @param ByteSize The numeric byte size value to be converted.
     * @return A formatted string if successful, an empty string otherwise.
    */
    std::string ConvertByteSizeToUtf8String(
        std::uint64_t ByteSize);

#pragma endregion
}

#endif // !MILE_WINDOWS
