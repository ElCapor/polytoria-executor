/**
 * @file spoofer.cpp
 * @author ElCapor (infosec@yu-tech.cloud)
 * @brief
 * @version 0.1
 * @date 2026-02-11
 *
 * CC-BY-SA 4.0 Copyright (c) 2026 @ElCapor
 *
 */
#include <hwidspoofer/hwidspoofer.h>
#include <spdlog/spdlog.h>
#include <cstdint>
#include <hooking/hookmanager.h>
#include <unity/unity.h>
#include <random>
#include <algorithm>
#include <core/core.h>
#include <filesystem>
#include <hwidspoofer/multiclient.h>

std::string scramble(const std::string &input)
{
    std::string scrambled = input;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(scrambled.begin(), scrambled.end(), g);
    return scrambled;
}

// UnityPlayer.dll + 0x804710
// constexpr std::uintptr_t UNITY_PLR_INITIALIZE_GAMEASSEMBLY = 0x804710;
// using UnityPlayer_InitializeGameAssembly_t = bool(*)(void*, void*);

// +--------------------------------------------------------+
// |                       Functions                        |
// +--------------------------------------------------------+
HMODULE LoadLibraryW_Hook(LPCWSTR lpLibFileName);

void spoofer::main_thread()
{
    OpenConsole();
    
    spdlog::info("[SkipSpoofer] Initializing SkipSpoofer...");

    // I am gonna test a new approach to hook gameassembly...
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (!kernel32)
    {
        spdlog::error("[SkipSpoofer] Failed to get kernel32.dll handle");
        return;
    }

    // Get the address of LoadLibraryW
    auto LoadLibraryW_addr = GetProcAddress(kernel32, "LoadLibraryW");
    if (!LoadLibraryW_addr)
    {
        spdlog::error("[SkipSpoofer] Failed to get LoadLibraryW address");
        return;
    }

    // Hook LoadLibraryW to detect when GameAssembly.dll is loaded
    HookManager::Install(reinterpret_cast<HMODULE(*)(LPCWSTR)>(LoadLibraryW_addr), LoadLibraryW_Hook);
    spdlog::info("[SkipSpoofer] LoadLibraryW hooked successfully");
}

// +--------------------------------------------------------+
// |                       ADDRESSES                        |
// +--------------------------------------------------------+
constexpr std::uintptr_t GetDeviceUniqueID_Addr = 0x1A56880;
// it takes a MethodInfo* ptr, but because of stdcall, it's in a register, so we can ignore it
using GetDeviceUniqueID_t = UnityString*(*)();

// +--------------------------------------------------------+
// |                       HWID Spoof                       |
// +--------------------------------------------------------+
UnityString*GetDeviceUniqueID_Hook()
{
    static bool init = false;
    if (!init)
    {
        spdlog::info("[SkipSpoofer] Device Unique ID hook triggered, initializing Unity and cleanup...");
        Unity::Init();
        Unity::ThreadAttach();
        // good time to load dll if it exists
        if (std::filesystem::exists("wowiezz.dll"))
        {
            spdlog::info("[SkipSpoofer] Found wowiezz.dll, loading...");
            LoadLibraryA("wowiezz.dll");
        }

            
        spdlog::info("[SkipSpoofer] Closing all mutexes for multi-client support...");
        multiclient::CloseAllMutexesInCurrentProcess();
        init = true;
    }
    UnityString*originalID = HookManager::Call(GetDeviceUniqueID_Hook);
    if (!originalID) {
        spdlog::warn("[SkipSpoofer] originalID is null, might be in a VM. Returning scrambled ID.");
        return UnityString::New(scramble("9dafafc41f9790ff32f5e65ef03c503b97d8fdb3"));
    } else if (originalID->ToString() == "")
    {
        spdlog::warn("[SkipSpoofer] originalID is empty string. Returning scrambled ID.");
        return UnityString::New(scramble("9dafafc41f9790ff32f5e65ef03c503b97d8fdb3"));
    }

    std::string originalIDStr = originalID->ToString();
    std::string scrambledID = scramble(originalIDStr);
    UnityString* fakeID = UnityString::New(scrambledID);
    spdlog::info("[SkipSpoofer] Original HWID: {}", originalIDStr);
    spdlog::info("[SkipSpoofer] Scrambled HWID: {}", scrambledID);
    return fakeID;
}

// +--------------------------------------------------------+
// |                         HOOKS                          |
// +--------------------------------------------------------+
HMODULE LoadLibraryW_Hook(LPCWSTR lpLibFileName)
{
    // WE are going to wait for gameassembly to be loaded, then we will hook it
    HMODULE module = HookManager::Call(LoadLibraryW_Hook, lpLibFileName);

    std::wstring libName(lpLibFileName);
    if (libName.find(L"GameAssembly.dll") != std::wstring::npos)
    {
        spdlog::info("[SkipSpoofer] GameAssembly.dll loaded, hooking...");
        spdlog::info("[SkipSpoofer] Base address: 0x{:X}", (uintptr_t)module);

        // Hook GetDeviceUniqueID
        GetDeviceUniqueID_t originalGetDeviceUniqueID = reinterpret_cast<GetDeviceUniqueID_t>((std::uintptr_t)module + GetDeviceUniqueID_Addr);
        HookManager::Install(originalGetDeviceUniqueID, GetDeviceUniqueID_Hook);
        spdlog::info("[SkipSpoofer] GetDeviceUniqueID hook installed");
    }

    return module;
}

//
// Unity forces a single instance through a mutex: \Sessions\1\BaseNamedObjects\t-1-4-155-Polytoria-Client-exe-SingleInstanceMutex-Default
//
void ForceCloseMutex()
{
    const char*mutexName = "t-1-4-155-Polytoria-Client-exe-SingleInstanceMutex-Default";
}