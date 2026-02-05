#include <Windows.h>
#include <atomic>
#include "unity.hpp"

std::atomic<bool> g_Running = true;
HANDLE g_ShutdownEvent = NULL;

using U = UnityResolve;
using UT = U::UnityType;
using UC = U::UnityType::Component;


void RunLuaScript(const std::string& script)
{
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
                    const auto Script = UT::String::New(script);
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


DWORD WINAPI MainThread(LPVOID param)
{
    // Full working executor
    
    HANDLE g = GetModuleHandleA("GameAssembly.dll");
    U::Init(g, U::Mode::Il2Cpp);
    U::ThreadAttach();

    const auto script = R"(
        while true do
            print("Hello from Lua!")
            wait(1000)
        end
    )";

    RunLuaScript(script);

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
            break;
        
        case DLL_PROCESS_DETACH:
            g_Running = false;
            if (g_ShutdownEvent) {
                SetEvent(g_ShutdownEvent);
            }
    }
    return TRUE;
}