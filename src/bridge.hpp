#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <Windows.h>
#include <string>
#include <iostream>
#include <functional>
#include <regex>
#include <thread>
#include <unordered_map>
#include <sstream>
#include <cstring>

#include "MinHook.h"
#include "unity.hpp"
#include "environment.hpp"
#include "console.hpp"

namespace Bridge {

    // IL2CPP Method Pointer Types
    typedef void (UNITY_CALLING_CONVENTION* DebugLogObj_t)(UnityResolve::UnityType::Object*);
    typedef void (UNITY_CALLING_CONVENTION* DebugLogStr_t)(UnityResolve::UnityType::String*);
    DebugLogObj_t OriginalDebugLogObj = nullptr;
    DebugLogStr_t OriginalDebugLogStr = nullptr;

    // NetworkEvent Hook
    typedef void (UNITY_CALLING_CONVENTION *FireServer_t)(void* instance, void* args);
    FireServer_t OriginalFireServer = nullptr;

    void UNITY_CALLING_CONVENTION HookedFireServer(void* instance, void* args) {
        if (Console::g_NetLogEnabled) {
            Console::Log("[Network] FireServer called! (Instance: " + std::to_string((uintptr_t)instance) + ")");
            // Argument inspection would require resolving System.Array or Il2CppArray structure
        }
        if (OriginalFireServer) OriginalFireServer(instance, args);
    }
    
    using CmdHandler = std::function<void(const std::string&)>;
    static std::unordered_map<std::string, CmdHandler> g_CmdHandlers;

    static std::string WideToUtf8(const wchar_t* data, int len) {
        if (!data || len <= 0) return "";
        int size = WideCharToMultiByte(CP_UTF8, 0, data, len, nullptr, 0, nullptr, nullptr);
        if (size <= 0) return "";
        std::string out(size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, data, len, out.data(), size, nullptr, nullptr);
        return out;
    }

    static bool TryReadIl2CppString(UnityResolve::UnityType::String* str, std::string& out) {
        if (!str) return false;
        int len = str->m_stringLength;
        if (len < 0 || len > (1 << 20)) return false;
        out = WideToUtf8(reinterpret_cast<const wchar_t*>(str->m_firstChar), len);
        return true;
    }

    static bool IsSystemString(UnityResolve::UnityType::Object* obj) {
        if (!obj) return false;
        void* klass = UnityResolve::Invoke<void*>("il2cpp_object_get_class", obj);
        if (!klass) return false;
        const char* name = UnityResolve::Invoke<const char*>("il2cpp_class_get_name", klass);
        const char* namesp = UnityResolve::Invoke<const char*>("il2cpp_class_get_namespace", klass);
        return name && namesp && std::strcmp(name, "String") == 0 && std::strcmp(namesp, "System") == 0;
    }

    static std::string ReadString(UnityResolve::UnityType::String* str) {
        std::string out;
        if (TryReadIl2CppString(str, out)) return out;
        return str ? str->ToString() : "";
    }

    static std::string ToSafeString(UnityResolve::UnityType::Object* obj) {
        if (!obj) return "";
        if (IsSystemString(obj)) {
            return ReadString(reinterpret_cast<UnityResolve::UnityType::String*>(obj));
        }
        auto s = obj->ToString();
        return ReadString(s);
    }

    static void EnsureCmdHandlers() {
        static bool inited = false;
        if (inited) return;
        inited = true;

        g_CmdHandlers["CLIPBOARD"] = [](const std::string& payload) {
            Console::SetClipboard(payload);
        };

        g_CmdHandlers["RCON_CREATE"] = [](const std::string&) {
            Console::EnsureConsole();
        };

        g_CmdHandlers["RCON_DESTROY"] = [](const std::string&) {
            Console::DestroyConsole();
        };

        g_CmdHandlers["RCON_CLEAR"] = [](const std::string&) {
            if (!Console::g_RConsoleReady) return;
            Console::Clear();
        };

        g_CmdHandlers["RCON_TITLE"] = [](const std::string& payload) {
            if (!Console::g_RConsoleReady) return;
            Console::SetTitle(payload);
        };

        g_CmdHandlers["RCON_LOG"] = [](const std::string& payload) {
            if (!Console::g_RConsoleReady) return;
            if (payload.empty()) return;
            size_t space = payload.find(' ');
            if (space != std::string::npos) {
                std::string type = payload.substr(0, space);
                std::string text = payload.substr(space + 1);
                Console::Log("[" + type + "] " + text);
            } else {
                Console::Log(payload);
            }
        };
    }

    static bool HandleInternalCmd(const std::string& msg) {
        size_t pos = msg.find("__MII_CMD:");
        if (pos == std::string::npos) return false;

        EnsureCmdHandlers();

        std::string cmdStr = msg.substr(pos + 10);
        size_t start = cmdStr.find_first_not_of(" \t");
        if (start == std::string::npos) return true;
        cmdStr = cmdStr.substr(start);

        size_t space = cmdStr.find(' ');
        std::string cmd = (space == std::string::npos) ? cmdStr : cmdStr.substr(0, space);
        std::string payload = (space == std::string::npos) ? "" : cmdStr.substr(space + 1);

        auto it = g_CmdHandlers.find(cmd);
        if (it != g_CmdHandlers.end()) {
            it->second(payload);
        }
        return true;
    }

    // Hooked Debug.Log(object)
    void UNITY_CALLING_CONVENTION HookedDebugLogObj(UnityResolve::UnityType::Object* message) {
        if (message) {
            std::string msg = ToSafeString(message);
            
            if (HandleInternalCmd(msg)) return; // Suppress from normal log

            Console::Log(msg);
        }
        if (OriginalDebugLogObj) OriginalDebugLogObj(message);
    }

    // Hooked Debug.Log(string)
    void UNITY_CALLING_CONVENTION HookedDebugLogStr(UnityResolve::UnityType::String* message) {
        if (message) {
            std::string msg = ReadString(message);

            if (HandleInternalCmd(msg)) return;

            Console::Log(msg);
        }
        if (OriginalDebugLogStr) OriginalDebugLogStr(message);
    }
    
    // Hooked Debug.Log (With Context)
    typedef void (UNITY_CALLING_CONVENTION* DebugLogObjCtx_t)(UnityResolve::UnityType::Object*, void*);
    typedef void (UNITY_CALLING_CONVENTION* DebugLogStrCtx_t)(UnityResolve::UnityType::String*, void*);
    DebugLogObjCtx_t OriginalDebugLogObjCtx = nullptr;
    DebugLogStrCtx_t OriginalDebugLogStrCtx = nullptr;
    
    void UNITY_CALLING_CONVENTION HookedDebugLogObjCtx(UnityResolve::UnityType::Object* message, void* context) {
        if (message) {
            std::string msg = ToSafeString(message);
            if (HandleInternalCmd(msg)) return;
            Console::Log(msg);
        }
        if (OriginalDebugLogObjCtx) OriginalDebugLogObjCtx(message, context);
    }

    void UNITY_CALLING_CONVENTION HookedDebugLogStrCtx(UnityResolve::UnityType::String* message, void* context) {
        if (message) {
            std::string msg = ReadString(message);
            if (HandleInternalCmd(msg)) return;
            Console::Log(msg);
        }
        if (OriginalDebugLogStrCtx) OriginalDebugLogStrCtx(message, context);
    }

    // Logger helper
    void Log(const std::string& msg) {
        Console::Log(msg);
    }

    void Initialize() {
        Log("Initializing Bridge...");
        
        if (MH_Initialize() != MH_OK) {
            Log("MinHook Init Failed");
            return;
        }

        // Resolve UnityEngine.Debug
        auto pAssembly = UnityResolve::Get("UnityEngine.CoreModule.dll");
        if (!pAssembly) pAssembly = UnityResolve::Get("UnityEngine.dll");
        
        if (!pAssembly) { Log("Assembly not found!"); return; }

        const auto pDebug = pAssembly->Get("Debug", "UnityEngine");
        if (!pDebug) { Log("Debug class not found!"); return; }

        // Hook Debug.Log(object)
        const auto pLogObj = pDebug->Get<UnityResolve::Method>("Log", { "System.Object" });
        if (pLogObj) {
            Log("Hooking Debug.Log(object)");
            MH_CreateHook((void*)pLogObj->function, &HookedDebugLogObj, reinterpret_cast<LPVOID*>(&OriginalDebugLogObj));
        }

        // Hook Debug.Log(string)
        const auto pLogStr = pDebug->Get<UnityResolve::Method>("Log", { "System.String" });
        if (pLogStr && (!pLogObj || pLogStr->function != pLogObj->function)) {
             Log("Hooking Debug.Log(string)");
             MH_CreateHook((void*)pLogStr->function, &HookedDebugLogStr, reinterpret_cast<LPVOID*>(&OriginalDebugLogStr));
        }
        
        // Hook Debug.Log(object, Object)
        const auto pLogObjCtx = pDebug->Get<UnityResolve::Method>("Log", { "System.Object", "UnityEngine.Object" });
        if (pLogObjCtx) {
            Log("Hooking Debug.Log(object, context)");
            MH_CreateHook((void*)pLogObjCtx->function, &HookedDebugLogObjCtx, reinterpret_cast<LPVOID*>(&OriginalDebugLogObjCtx));
        }

        // Hook Debug.Log(string, Object)
        const auto pLogStrCtx = pDebug->Get<UnityResolve::Method>("Log", { "System.String", "UnityEngine.Object" });
        if (pLogStrCtx && (!pLogObjCtx || pLogStrCtx->function != pLogObjCtx->function)) {
            Log("Hooking Debug.Log(string, context)");
            MH_CreateHook((void*)pLogStrCtx->function, &HookedDebugLogStrCtx, reinterpret_cast<LPVOID*>(&OriginalDebugLogStrCtx));
        }

        // Hook NetworkEvent.FireServer
        const auto pAssemblyCS = UnityResolve::Get("Assembly-CSharp.dll");
        if (pAssemblyCS) {
            // Try with no namespace first, or check Polytoria namespace
            auto pNetworkEvent = pAssemblyCS->Get("NetworkEvent");
            if (!pNetworkEvent) pNetworkEvent = pAssemblyCS->Get("NetworkEvent", "Polytoria.Datamodel");
            
            if (pNetworkEvent) {
                const auto pFireServer = pNetworkEvent->Get<UnityResolve::Method>("FireServer");
                if (pFireServer) {
                    Console::Log("Found NetworkEvent.FireServer at: " + std::to_string((uintptr_t)pFireServer->function));
                    MH_CreateHook((void*)pFireServer->function, &HookedFireServer, reinterpret_cast<LPVOID*>(&OriginalFireServer));
                } else {
                    Console::Log("NetworkEvent.FireServer method not found!");
                }
            } else {
                Console::Log("NetworkEvent class not found in Assembly-CSharp!");
            }
        }

        // Enable Hook
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            Log("EnableHook Failed!");
            return;
        }
        
        Log("Bridge Initialized Successfully!");
    }
    
    void Shutdown() {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
    }
}

#endif // BRIDGE_HPP
