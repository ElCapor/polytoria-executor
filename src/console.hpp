#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <functional>

namespace Console {
    
    // Callback to run script
    std::function<void(const std::string&)> RunScriptCallback;
    
    // Globals
    bool g_NetLogEnabled = false;
    bool g_RConsoleReady = false;
    
    void SetRunScriptCallback(std::function<void(const std::string&)> callback) {
        RunScriptCallback = callback;
    }

    // Set Windows Clipboard
    void SetClipboard(const std::string& text) {
        if (!OpenClipboard(NULL)) return;
        EmptyClipboard();
        HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (!hGlob) {
            CloseClipboard();
            return;
        }
        memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
        GlobalUnlock(hGlob);
        SetClipboardData(CF_TEXT, hGlob);
        CloseClipboard();
    }

    // Clear Console
    void Clear() {
        system("cls");
    }

    // Set Console Title
    void SetTitle(const std::string& title) {
        SetConsoleTitleA(title.c_str());
    }

    void EnsureConsole() {
        if (g_RConsoleReady) return;
        AllocConsole();
        FILE* fpOut = nullptr;
        FILE* fpIn = nullptr;
        FILE* fpErr = nullptr;
        freopen_s(&fpOut, "CONOUT$", "w", stdout);
        freopen_s(&fpErr, "CONOUT$", "w", stderr);
        freopen_s(&fpIn, "CONIN$", "r", stdin);
        SetConsoleTitleA("Miisploit RConsole");
        g_RConsoleReady = true;
    }

    void DestroyConsole() {
        if (!g_RConsoleReady) return;
        FreeConsole();
        g_RConsoleReady = false;
    }

    void Initialize() {
        // Lazy-create console on first rconsole call.
    }
    
    // Log message from Bridge/Game
    void Log(const std::string& msg) {
        if (!g_RConsoleReady) return;
        std::cout << "[Output] " << msg << std::endl;
    }
}

#endif // CONSOLE_HPP
