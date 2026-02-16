#include <ptoria/networkevent.h>
#include <hooking/hookmanager.h>
#include <ptoria/scriptservice.h>

void NetworkEvent::InvokeCmdHook(NetworkEvent* _this, NetMessage* message, void* sender)
{
    std::printf("NetworkEvent::InvokeCmd called with message %p and sender %p\n", message, sender);


    return HookManager::Call(InvokeCmdHook, _this, message, sender);
}


void NetworkEvent::InstallHooks()
{
    HookManager::Install(Unity::GetMethod<"InvokeCmd">(StaticClass<NetworkEvent>())->Cast<void, NetworkEvent*, NetMessage*, void*>(), InvokeCmdHook);
}