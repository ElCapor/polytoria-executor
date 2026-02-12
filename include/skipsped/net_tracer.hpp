#ifndef NET_TRACER
#define NET_TRACER
#include <HookManager.hpp>

#include <polytoria/polytoria.hpp>
#include <tuple>
using namespace polytoria;

struct NetTracer {

    static bool isReady;
    
    static void InvokeServerHook(NetworkEvent* evt,NetMessage* message)
    {
        U::ThreadAttach();
        std::cout << "[INFO] Invoking server hook for message: " << evt->GetName()->ToString() << std::endl;

        // InvokeServerHook(game["Hidden"]["DraggerPlace"], function(msg) print(msg:GetVector3("Position")) end)
        if (NetTracer::callback != nullptr)
        {
            Closure* closure = NetTracer::callback->GetFunction();
            if (closure != nullptr)
            {
                closure->Call({(UO*)message});
            } else {
                std::cout << "Closure is empty wthhh" << std::endl;
            }
        }
        HookManager::Call(InvokeServerHook, evt, message);
    }
    
    static void UserCode_InvokeClientRpc__NetMessage(NetworkEvent* evt, NetMessage* message)
    {
        std::cout << "[INFO] Invoking client RPC hook for message: " << evt->GetName()->ToString() << std::endl;
        HookManager::Call(UserCode_InvokeClientRpc__NetMessage, evt, message);
    }

    static Script* exec;
    static DynValue* callback;

    static void InstallHooks()
    {
        UM* method = NetworkEvent::GetClass()->Get<UM>("InvokeServer");
        if (method) {
            std::cout << "[INFO] Installing hook for NetworkEvent::InvokeServer" << std::endl;
            HookManager::Install(method->Cast<void, NetworkEvent*, NetMessage*>(), InvokeServerHook);
            isReady = true;
        } else {
            std::cout << "[ERROR] Failed to find method NetworkEvent::InvokeServer" << std::endl;
        }

        UM* rpcMethod = NetworkEvent::GetClass()->Get<UM>("UserCode_InvokeClientRpc__NetMessage");
        if (rpcMethod) {
            std::cout << "[INFO] Installing hook for NetworkEvent::UserCode_InvokeClientRpc__NetMessage" << std::endl;
            HookManager::Install(rpcMethod->Cast<void, NetworkEvent*, NetMessage*>(), UserCode_InvokeClientRpc__NetMessage);
        } else {
            std::cout << "[ERROR] Failed to find method NetworkEvent::UserCode_InvokeClientRpc__NetMessage" << std::endl;
        }

        std::cout << "[INFO] NetTracer hooks installed" << std::endl;
    }
};




#endif /* NET_TRACER */
