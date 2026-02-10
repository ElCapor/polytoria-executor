#include <spoofer.hpp>
#include <cstdint>
#include <HookManager.hpp>
#include <iostream>
#include <unity/u.hpp>
#include <random>
#include <algorithm>
#include <simple_filesystem/mod.hpp>


std::string scramble(const std::string& input) {
    std::string scrambled = input;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(scrambled.begin(), scrambled.end(), g);
    return scrambled;
}

US* GetDeviceUniqueIdentifer()
{
    US* deviceId = HookManager::Call(GetDeviceUniqueIdentifer);
    std::string sDeviceId = deviceId->ToString();

    mod::ToriaFile config;
    config.load("hwid.toria");

    if (auto savedHwid = config.get("hwid")) {
        std::cout << "[GetDeviceUniqueIdentifer] Returning saved HWID: " << *savedHwid << std::endl;
        return US::New(*savedHwid);
    }

    std::cout << "[GetDeviceUniqueIdentifer] No saved HWID found. Original: " << sDeviceId << std::endl;
    std::string scrambledDeviceId = scramble(sDeviceId);
    
    config.set("hwid", scrambledDeviceId);
    config.save("hwid.toria");

    std::cout << "[GetDeviceUniqueIdentifer] Scrambled and saved new HWID: " << scrambledDeviceId << std::endl;
    return US::New(scrambledDeviceId);
}

void GameGameHook(void* __this)
{
    std::cout << "[Game.Game] Game.Game method called" << std::endl;
    HookManager::Call(GameGameHook, __this);
    LoadLibraryA("skipsped.dll");
}

void SpooferHooks() {
    HMODULE gameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!gameAssembly) {
        std::cout << "[ERROR] Failed to get GameAssembly.dll handle" << std::endl;
        return;
    }

    U::Init(gameAssembly, U::Mode::Il2Cpp);
    U::ThreadAttach();

    UK* SystemInfo = U::Get("UnityEngine.CoreModule.dll")->Get("SystemInfo");
    if (!SystemInfo) {
        std::cout << "[ERROR] Failed to get SystemInfo class" << std::endl;
        return;
    }

    const auto GetDeviceUniqueIdentifierMethod = SystemInfo->Get<UM>("GetDeviceUniqueIdentifier");
    if (!GetDeviceUniqueIdentifierMethod) {
        std::cout << "[ERROR] Failed to get GetDeviceUniqueIdentifier method" << std::endl;
        return;
    }

    HookManager::Install(GetDeviceUniqueIdentifierMethod->Cast<US*>(), GetDeviceUniqueIdentifer);


    UK* Game = U::Get(ASSEMBLY_CSHARP)->Get("Game");
    if (!Game) {
        std::cout << "[ERROR] Failed to get Game class" << std::endl;
        return;
    }

    UM* GameGame = Game->Get<UM>("Start");
    if (!GameGame) {
        std::cout << "[ERROR] Failed to get Game.Start method" << std::endl;
        return;
    }



    HookManager::Install(GameGame->Cast<void, void*>(), GameGameHook);
    Sleep(100);
    std::cout << "[SpooferHooks] Hooks installed successfully" << std::endl;
}

// UnityPlayer.dll + 0x804710
constexpr std::uintptr_t UNITY_PLR_INITIALIZE_GAMEASSEMBLY = 0x804710;
using UnityPlayer_InitializeGameAssembly_t = bool(*)(void*, void*);

bool InitializeGameAssembly(void* _1, void* _2)
{
    auto ret = HookManager::Call(InitializeGameAssembly, _1, _2);
    SpooferHooks();
    Sleep(100);
    return ret;
}

void OpenConsole()
{
    AllocConsole();
    FILE* fp;
    SetConsoleTitleA("SkipSped Console");
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    std::cout << "[SkipSpoofer] Started" << std::endl;
}

void spoofer::main_thread()
{
    OpenConsole();

    HMODULE unityPlayer = GetModuleHandleA("UnityPlayer.dll");
    if (!unityPlayer)
    {
        std::cout << "[ERROR] Failed to get UnityPlayer.dll handle" << std::endl;
        return;
    }

    auto initializeGameAssemblyAddr = reinterpret_cast<UnityPlayer_InitializeGameAssembly_t>(reinterpret_cast<std::uintptr_t>(unityPlayer) + UNITY_PLR_INITIALIZE_GAMEASSEMBLY);
    if (!initializeGameAssemblyAddr)    {
        std::cout << "[ERROR] Failed to calculate InitializeGameAssembly address" << std::endl;
        return;
    }

    HookManager::Install(initializeGameAssemblyAddr, InitializeGameAssembly);
   
    std::cout << "[SkipSpoofer] Hooks installed, Game will now start..." << std::endl;

}