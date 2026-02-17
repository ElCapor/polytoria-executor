#include <hwidspoofer/multiclient.h>
#include <spdlog/spdlog.h>

#include <windows.h>
#include <winternl.h>

// -------------------------------------------------------------------
// Type definitions for internal system structures
// -------------------------------------------------------------------
typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
{
    PVOID Object;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR HandleValue;
    ULONG GrantedAccess;
    USHORT CreatorBackTraceIndex;
    USHORT ObjectTypeIndex;
    ULONG HandleAttributes;
    ULONG Reserved;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];
} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

// Function pointers for NT APIs
typedef NTSTATUS (NTAPI *pNtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef NTSTATUS (NTAPI *pNtQueryObject)(
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength);

typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING TypeName;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccessMask;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    UCHAR TypeIndex; // since WINBLUE
    CHAR ReservedByte;
    ULONG PoolType;
    ULONG DefaultPagedPoolCharge;
    ULONG DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

// -------------------------------------------------------------------
// Dynamically retrieve the object type index for "Mutant" on the
// current Windows version.
// -------------------------------------------------------------------
USHORT GetMutantTypeIndex()
{
    spdlog::debug("[MultiClient] Retrieving Mutant type index...");

    // NtQueryObject with ObjectTypeInformation returns a structure
    // that contains the type name and its index. We create a dummy
    // mutant, query its type, and extract the index.
    HANDLE hDummy = CreateMutexW(NULL, FALSE, L"DummyMutexForTypeIndex");
    if (!hDummy)
    {
        spdlog::error("[MultiClient] Failed to create dummy mutex, error: {}", GetLastError());
        return 0;
    }

    // We need NtQueryObject from ntdll.dll
    pNtQueryObject NtQueryObject = (pNtQueryObject)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "NtQueryObject");
    if (!NtQueryObject)
    {
        spdlog::error("[MultiClient] Failed to resolve NtQueryObject from ntdll.dll");
        CloseHandle(hDummy);
        return 0;
    }

    // First call to get required buffer size
    ULONG size = 0;
    NTSTATUS status = NtQueryObject(hDummy, ObjectTypeInformation, NULL, 0, &size);
    if (status != 0xC0000004 && status != 0) // STATUS_INFO_LENGTH_MISMATCH is expected
    {
        spdlog::error("[MultiClient] NtQueryObject (size query) failed with status: 0x{:08X}", (unsigned int)status);
        CloseHandle(hDummy);
        return 0;
    }

    // Allocate buffer
    PBYTE buffer = (PBYTE)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if (!buffer)
    {
        spdlog::error("[MultiClient] VirtualAlloc failed to allocate {} bytes", size);
        CloseHandle(hDummy);
        return 0;
    }

    status = NtQueryObject(hDummy, ObjectTypeInformation, buffer, size, NULL);
    if (status < 0)
    {
        spdlog::error("[MultiClient] NtQueryObject failed with status: 0x{:08X}", (unsigned int)status);
        VirtualFree(buffer, 0, MEM_RELEASE);
        CloseHandle(hDummy);
        return 0;
    }

    // The structure returned is OBJECT_TYPE_INFORMATION, which contains
    // a UNICODE_STRING for the type name and a ULONG TypeIndex.
    // We just need the TypeIndex.
    POBJECT_TYPE_INFORMATION pTypeInfo = (POBJECT_TYPE_INFORMATION)buffer;
    USHORT typeIndex = (USHORT)pTypeInfo->TypeIndex;   // Note: TypeIndex is ULONG, but fits in USHORT

    spdlog::info("[MultiClient] Detected Mutant type index: {}", typeIndex);

    VirtualFree(buffer, 0, MEM_RELEASE);
    CloseHandle(hDummy);
    return typeIndex;
}

// -------------------------------------------------------------------
// Main function: close all mutexes owned by the current process.
// Must be called early from DllMain (or a dedicated thread) to avoid
// interfering with the application after it has started.
// -------------------------------------------------------------------
void multiclient::CloseAllMutexesInCurrentProcess()
{
    spdlog::info("[MultiClient] Starting mutex cleanup...");

    // 1. Dynamically resolve the Mutant type index
    static USHORT mutantTypeIndex = 0;
    if (mutantTypeIndex == 0)
    {
        mutantTypeIndex = GetMutantTypeIndex();
        if (mutantTypeIndex == 0)
        {
            // Fallback: common values on modern Windows (10/11)
            spdlog::warn("[MultiClient] Could not determine Mutant type index, using fallback (19)");
            mutantTypeIndex = 19;   // Often 19, but not guaranteed
        }
    }

    // 2. Get NtQuerySystemInformation
    pNtQuerySystemInformation NtQuerySystemInformation = (pNtQuerySystemInformation)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation");
    if (!NtQuerySystemInformation)
    {
        spdlog::error("[MultiClient] Failed to resolve NtQuerySystemInformation from ntdll.dll");
        return;
    }

    DWORD currentPid = GetCurrentProcessId();
    spdlog::debug("[MultiClient] Current PID: {}", currentPid);

    // 3. Loop with a reasonable buffer size – grow if needed
    ULONG bufferSize = 0x100000; // Start with 1MB - much more reasonable for modern Windows
    PBYTE buffer = NULL;
    NTSTATUS status;
    int retries = 0;

    spdlog::info("[MultiClient] Querying system handle information...");
    do
    {
        if (buffer)
            VirtualFree(buffer, 0, MEM_RELEASE);

        buffer = (PBYTE)VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
        if (!buffer)
        {
            spdlog::error("[MultiClient] VirtualAlloc failed to allocate {} bytes for handle info", bufferSize);
            return;
        }

        ULONG requiredSize = 0;
        status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)64, // SystemExtendedHandleInformation = 64
                                          buffer, bufferSize, &requiredSize);
        if (status == 0xC0000004) // STATUS_INFO_LENGTH_MISMATCH
        {
            // If the system returns requiredSize, use it plus some padding, otherwise double the buffer
            if (requiredSize > 0)
                bufferSize = requiredSize + 0x10000; // Add 64KB extra safety padding
            else
                bufferSize *= 2;
            
            spdlog::info("[MultiClient] Buffer too small, increasing size to {} bytes and retrying... (attempt {})", bufferSize, retries + 1);
            
            if (++retries > 20) // Safety break
            {
                spdlog::error("[MultiClient] Exceeded maximum retries for handle query");
                VirtualFree(buffer, 0, MEM_RELEASE);
                return;
            }
            continue;
        }
        else if (status < 0)
        {
            spdlog::error("[MultiClient] NtQuerySystemInformation failed with status: 0x{:08X}", (unsigned int)status);
            VirtualFree(buffer, 0, MEM_RELEASE);
            return;
        }
        break;
    } while (TRUE);

    // 4. Parse the handle list
    PSYSTEM_HANDLE_INFORMATION_EX handleInfo = (PSYSTEM_HANDLE_INFORMATION_EX)buffer;
    spdlog::info("[MultiClient] Found {} system-wide handles. Filtering for current process mutants...", handleInfo->NumberOfHandles);

    HANDLE hCurrentProcess = GetCurrentProcess();
    int mutantsClosed = 0;
    for (ULONG_PTR i = 0; i < handleInfo->NumberOfHandles; i++)
    {
        PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX entry = &handleInfo->Handles[i];

        // Filter: only our process and only Mutant objects
        if (entry->UniqueProcessId != currentPid)
            continue;
        if (entry->ObjectTypeIndex != mutantTypeIndex)
            continue;

        // 5. Attempt to close this handle using DUPLICATE_CLOSE_SOURCE
        HANDLE hDup = NULL;
        BOOL ret = DuplicateHandle(
            hCurrentProcess,              // Current process owns the handle
            (HANDLE)entry->HandleValue,   // Handle value to close
            hCurrentProcess,              // Target process (us)
            &hDup,                        // Receive temporary duplicate
            0,                            // Same access
            FALSE,
            DUPLICATE_CLOSE_SOURCE);      // Closes the source handle

        if (ret)
        {
            spdlog::info("[MultiClient] Successfully closed mutant handle: 0x{:X}", entry->HandleValue);
            mutantsClosed++;
            if (hDup)
            {
                // We must close our local copy.
                CloseHandle(hDup);
            }
        }
    }

    spdlog::info("[MultiClient] Mutex cleanup complete. {} mutants closed.", mutantsClosed);

    VirtualFree(buffer, 0, MEM_RELEASE);
}