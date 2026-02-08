#include <Windows.h>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "unity.hpp"
#include "environment.hpp"
#include "bridge.hpp"
#include "renderer/renderer.h"

#define PIPE_NAME "\\\\.\\pipe\\PolytoriaPipe"

std::atomic<bool> g_Running = true;
HANDLE g_ShutdownEvent = NULL;

void FileLog(const std::string& msg) {
    std::ofstream fs("miisploit_log.txt", std::ios::app);
    if (fs.is_open()) {
        fs << "[Log] " << msg << std::endl;
        fs.close();
    }
}

using U = UnityResolve;
using UT = U::UnityType;
using UC = U::UnityType::Component;

void RunLuaScript(const std::string& script);

static void ExecuteFromOverlay(const char* script) {
    if (!script) return;
    RunLuaScript(script);
}


void RunLuaScript(const std::string& script)
{
    // Wrap user script with environment setup
    const std::string fullScript = Environment::PrepareScript(script);
    
    const auto pAssembly = U::Get("Assembly-CSharp.dll");
    if (!pAssembly) { return; }

    const auto pScriptService = pAssembly->Get("ScriptService");
    const auto pGame = pAssembly->Get("Game");
    const auto pScriptInstance = pAssembly->Get("ScriptInstance");
    const auto pBaseScript = pAssembly->Get("BaseScript");

    if (pScriptService && pGame && pScriptInstance && pBaseScript) {
        const auto pGetScriptServiceInstanceField = pScriptService->Get<U::Field>("<Instance>k__BackingField");
        const auto pGameInstanceField = pGame->Get<U::Field>("singleton");

        UC* gameInstance = nullptr;
        if (pGameInstanceField) {
            pGameInstanceField->GetStaticValue<UC*>(&gameInstance);
        } else {
            return;
        }

        UC* scriptServiceInstance = nullptr;
        if (pGetScriptServiceInstanceField) {
            pGetScriptServiceInstanceField->GetStaticValue<UC*>(&scriptServiceInstance);
        } else {
            return;
        }

        if (scriptServiceInstance && gameInstance) {
            const auto pGameObject = gameInstance->GetGameObject();
            if (pGameObject) {
                UC* scriptInstance = pGameObject->AddComponent<UC*>(pScriptInstance);
                if (scriptInstance) {
                    const auto Script = UT::String::New(fullScript);
                    if (Script) {
                        // Recursive lambda to set field value in script instance or its base script
                        // This is necessary because of wierd il2cpp inheritance
                        auto setFieldRecursive = [&](const std::string& name, auto value) -> bool {
                            if (pScriptInstance->Get<U::Field>(name)) {
                                pScriptInstance->SetValue(scriptInstance, name, value);
                                return true;
                            }
                            if (pBaseScript && pBaseScript->Get<U::Field>(name)) {
                                pBaseScript->SetValue(scriptInstance, name, value);
                                return true;
                            }
                            return false;
                        };

                        // idk why Source with cap is not inherited ig , but empirical data show source works too so...
                        setFieldRecursive("source", Script);
                        setFieldRecursive("running", false);

                        const auto pRunScriptMethod = pScriptService->Get<U::Method>("RunScript");
                        if (pRunScriptMethod) {
                            const auto pRunScript = pRunScriptMethod->Cast<void, UC*, UC*>();
                            if (pRunScript) pRunScript(scriptServiceInstance, scriptInstance);
                        }
                    }
                }
            }

        }
    }
}


// Simple JSON Serializer for Explorer
// Safe Wrapper for getting GameObject from Component
static void* SafeGetGameObjectInternal(UC* component) {
    __try {
        if (component) return component->GetGameObject();
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return nullptr;
}

// Safe Wrapper for getting Name String Pointer
static void* SafeGetNamePtrInternal(void* gameObject) {
    __try {
        if (gameObject) return ((U::UnityType::GameObject*)gameObject)->GetName();
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return nullptr;
}

// Safe Wrapper for getting FullName String Pointer
static void* SafeGetClassNamePtrInternal(void* gameObject) {
    __try {
        if (gameObject) {
            auto type = ((U::UnityType::GameObject*)gameObject)->GetType();
            if (type) return type->GetFullName();
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return nullptr;
}

// Safe Wrapper for getting Transform
static void* SafeGetTransformInternal(void* gameObject) {
    __try {
        if (gameObject) return ((U::UnityType::GameObject*)gameObject)->GetTransform();
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return nullptr;
}

// Safe Wrapper for getting Child
static void* SafeGetChildInternal(void* transform, int index) {
    __try {
        if (transform) return ((U::UnityType::Transform*)transform)->GetChild(index);
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return nullptr;
}

// Safe Wrapper for getting Child Count
static int SafeGetChildCountInternal(void* transform) {
    __try {
        if (transform) return ((U::UnityType::Transform*)transform)->GetChildCount();
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    return 0;
}

std::string SerializeObject(UC* instance, int depth = 0) {
    if (!instance || depth > 5) return "null";

    void* pGameObject = SafeGetGameObjectInternal(instance);
    
    std::string name = "Unknown";
    U::UnityType::String* nameStr = (U::UnityType::String*)SafeGetNamePtrInternal(pGameObject);
    if (nameStr) name = nameStr->ToString();

    std::string className = "Unknown";
    U::UnityType::String* classStr = (U::UnityType::String*)SafeGetClassNamePtrInternal(pGameObject);
    if (classStr) className = classStr->ToString();
    
    std::stringstream ss;
    ss << "{\"Name\":\"" << name << "\",\"ClassName\":\"" << className << "\",\"Address\":\"0x" << std::hex << (uintptr_t)instance << "\",\"Children\":[";

    void* transform = SafeGetTransformInternal(pGameObject);
    if (transform) {
        int childCount = SafeGetChildCountInternal(transform);
        for (int i = 0; i < childCount; ++i) {
            void* child = SafeGetChildInternal(transform, i);
            if (child) {
                if (i > 0) ss << ",";
                ss << SerializeObject((UC*)child, depth + 1);
            }
        }
    }

    ss << "]}";
    return ss.str();
}

DWORD WINAPI MainThread(LPVOID param)
{
    // Full working executor

    HANDLE g = GetModuleHandleA("GameAssembly.dll");
    U::Init(g, U::Mode::Il2Cpp);
    U::ThreadAttach();
    
    // Initialize Bridge
    Bridge::Initialize();
    
    // Initialize Console (UnityExplorer-Lite)
    Console::SetRunScriptCallback(RunLuaScript);
    Console::Initialize();

    // Hook ImGui overlay and wire execute button
    rbx::c_renderer::set_execute_callback(ExecuteFromOverlay);
    // Start ImGui renderer window
    rbx::c_renderer::render();

    char* buffer = (char*)malloc(9999999);
    if (!buffer) return 1;

    while (g_Running)
    {
        HANDLE hPipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            9999999,
            9999999,
            0,
            NULL
        );

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            if (!g_Running) break;
            Sleep(100);
            continue;
        }

        OVERLAPPED overlapped = { 0 };
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!overlapped.hEvent)
        {
            CloseHandle(hPipe);
            break;
        }

        BOOL connected = ConnectNamedPipe(hPipe, &overlapped);
        if (!connected)
        {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING)
            {
                HANDLE waitHandles[2] = { overlapped.hEvent, g_ShutdownEvent };
                DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
                if (waitResult == WAIT_OBJECT_0)
                {
                    connected = TRUE;
                }
                else
                {
                    CancelIo(hPipe);
                    connected = FALSE;
                }
            }
            else if (err == ERROR_PIPE_CONNECTED)
            {
                connected = TRUE;
            }
        }

        if (connected && g_Running)
        {
            DWORD bytesRead;
            ResetEvent(overlapped.hEvent);
            if (ReadFile(hPipe, buffer, 9999998, &bytesRead, &overlapped) || GetLastError() == ERROR_IO_PENDING)
            {
                HANDLE waitHandles[2] = { overlapped.hEvent, g_ShutdownEvent };
                DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
                if (waitResult == WAIT_OBJECT_0)
                {
                    if (GetOverlappedResult(hPipe, &overlapped, &bytesRead, FALSE))
                    {
                        buffer[bytesRead] = '\0';
                        std::string input(buffer);

                        // Check for REQUEST
                        if (input.rfind("__REQ:", 0) == 0) {
                            std::string cmd = input.substr(6);
                            if (cmd.find("FETCH_EXPLORER") != std::string::npos) {
                                FileLog("Received FETCH_EXPLORER request");
                                std::string response = "{\"Name\":\"No Game Instance\",\"ClassName\":\"Error\",\"Address\":\"0x0\",\"Children\":[]}";

                                auto pAssembly = U::Get("Assembly-CSharp.dll");
                                if (pAssembly) {
                                    auto pGame = pAssembly->Get("Game");
                                    if (pGame) {
                                        auto pGameInstanceField = pGame->Get<U::Field>("singleton");
                                        if (!pGameInstanceField) pGameInstanceField = pGame->Get<U::Field>("<Instance>k__BackingField");
                                        if (!pGameInstanceField) pGameInstanceField = pGame->Get<U::Field>("instance");
                                        if (!pGameInstanceField) pGameInstanceField = pGame->Get<U::Field>("Instance");

                                        if (pGameInstanceField) {
                                            UC* gameInstance = nullptr;
                                            pGameInstanceField->GetStaticValue<UC*>(&gameInstance);

                                            if (gameInstance) {
                                                FileLog("Found Game instance, serializing...");
                                                response = SerializeObject(gameInstance);
                                                FileLog("Serialized Size: " + std::to_string(response.length()));
                                            } else {
                                                FileLog("Game singleton is NULL");
                                                response = "{\"Name\":\"Error: Game Instance NULL\",\"ClassName\":\"Error\",\"Address\":\"0x0\",\"Children\":[]}";
                                            }
                                        } else {
                                            FileLog("Singleton field not found");
                                            response = "{\"Name\":\"Error: Singleton Field Missing\",\"ClassName\":\"Error\",\"Address\":\"0x0\",\"Children\":[]}";
                                        }
                                    } else {
                                        FileLog("Game class not found");
                                        response = "{\"Name\":\"Error: Game Class Not Found\",\"ClassName\":\"Error\",\"Address\":\"0x0\",\"Children\":[]}";
                                    }
                                } else {
                                    FileLog("Assembly-CSharp not found");
                                    response = "{\"Name\":\"Error: Assembly Not Found\",\"ClassName\":\"Error\",\"Address\":\"0x0\",\"Children\":[]}";
                                }

                                DWORD bytesWritten;
                                WriteFile(hPipe, response.c_str(), (DWORD)response.length(), &bytesWritten, &overlapped);
                                GetOverlappedResult(hPipe, &overlapped, &bytesWritten, TRUE);
                                FileLog("Sent response to UI");
                            }
                        } else {
                            RunLuaScript(input);
                        }
                    }
                }
                else
                {
                    CancelIo(hPipe);
                }
            }
        }

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
        CloseHandle(overlapped.hEvent);
    }

    free(buffer);
    U::ThreadDetach();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            g_ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
            break;
        
        case DLL_PROCESS_DETACH:
            g_Running = false;
            if (g_ShutdownEvent) {
                SetEvent(g_ShutdownEvent);
            }
            // Give the thread a moment to exit (optional but good practice)
            Sleep(100); 
            if (g_ShutdownEvent) CloseHandle(g_ShutdownEvent);
            break;
    }
    return TRUE;
}
