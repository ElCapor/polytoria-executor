/**
 * @file entry.cpp
 * @author ElCapor
 * @brief Entry point for the DLL, handles the DLL main function and initialization.
 * @version 0.1
 * @date 2026-02-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <Windows.h>
#include <thread>
#include <skipsped/sped.hpp>

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        std::thread(sped::main_thread).detach();
        break;
    }
    return TRUE;
}