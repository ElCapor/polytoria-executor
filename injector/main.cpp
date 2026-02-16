#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <filesystem>
#include <thread>
#include <chrono>

DWORD GetProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    pid = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

DWORD WaitForProcess(const std::wstring& processName) {
    std::wcout << L"Waiting for process: " << processName << L"..." << std::endl;
    while (true) {
        DWORD pid = GetProcessIdByName(processName);
        if (pid != 0) return pid;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool InjectDLL(DWORD pid, const std::string& dllPath) {
    if (!std::filesystem::exists(dllPath)) {
        std::cerr << "DLL not found: " << dllPath << std::endl;
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!process) {
        std::cerr << "Failed to open process (Are you Admin?): " << GetLastError() << std::endl;
        return false;
    }

    void* allocatedMemory = VirtualAllocEx(process, nullptr, dllPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocatedMemory) {
        std::cerr << "Failed to allocate memory: " << GetLastError() << std::endl;
        CloseHandle(process);
        return false;
    }

    if (!WriteProcessMemory(process, allocatedMemory, dllPath.c_str(), dllPath.length() + 1, nullptr)) {
        std::cerr << "Failed to write memory: " << GetLastError() << std::endl;
        VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    HANDLE thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), allocatedMemory, 0, nullptr);
    if (!thread) {
        std::cerr << "Failed to create remote thread: " << GetLastError() << std::endl;
        VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    WaitForSingleObject(thread, INFINITE);
    
    VirtualFreeEx(process, allocatedMemory, 0, MEM_RELEASE);
    CloseHandle(thread);
    CloseHandle(process);
    return true;
}

int main(int argc, char* argv[]) {
    std::wstring targetProcess = L"Polytoria Client.exe";
    std::string dllPath = "wowiezz.dll";

    if (argc >= 2) {
        std::string processNameStr = argv[1];
        targetProcess = std::wstring(processNameStr.begin(), processNameStr.end());
    }
    
    if (argc >= 3) {
        dllPath = argv[2];
    } else {
        // If no DLL path provided, look for it in common places or just use current dir
        if (!std::filesystem::exists(dllPath)) {
            // Check debug folder
            if (std::filesystem::exists("build/windows/x64/debug/wowiezz.dll")) {
                dllPath = "build/windows/x64/debug/wowiezz.dll";
            }
        }
    }

    dllPath = std::filesystem::absolute(dllPath).string();

    DWORD pid = GetProcessIdByName(targetProcess);
    if (pid == 0) {
        pid = WaitForProcess(targetProcess);
    }

    std::wcout << L"Found process " << targetProcess << L" with PID: " << pid << std::endl;
    std::cout << "Will inject DLL in 6 seconds: " << dllPath << std::endl;
    
    Sleep(6000);
    if (InjectDLL(pid, dllPath)) {
        std::cout << "Successfully injected!" << std::endl;
    } else {
        std::cerr << "Injection failed." << std::endl;
        return 1;
    }

    return 0;
}
