#include <ptoria/networkevent.h>
#include <hooking/hookmanager.h>
#include <ptoria/scriptservice.h>

void NetworkEvent::InvokeCmdHook(NetworkEvent* _this, NetMessage* message, void* sender)
{
    std::printf("NetworkEvent::InvokeCmd called with message %p and sender %p\n", message, sender);

    if (ScriptService::exec != nullptr)
    {
        DynValue* callback = (DynValue*)ScriptService::exec->Registry()->Get((UnityObject*)UnityString::New("InvokeServerCallback"));
        if (callback != nullptr && callback->Type() == DynValue::DataType::Function)
        {
            std::printf("Invoking Lua callback for InvokeCmd...\n");
            callback->AsFunction()->Call({(UnityObject*)message});
        }
        else
        {
            std::printf("No valid Lua callback found for InvokeCmd.\n");
        }
    }

    return HookManager::Call(InvokeCmdHook, _this, message, sender);
}


void NetworkEvent::InstallHooks()
{
    HookManager::Install(Unity::GetMethod<"InvokeCmd">(StaticClass<NetworkEvent>())->Cast<void, NetworkEvent*, NetMessage*, void*>(), InvokeCmdHook);
}