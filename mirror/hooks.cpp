#include <mirror/hooks.h>
#include <hooking/hookmanager.h>
#include <mirror/localconnectiontoserver.h>
#include <mirror/networkclient.h>

void mirror::InstallHooks()
{
    HookManager::Install(Unity::GetMethod<"Send">(StaticClass<LocalConnectionToServer>())->Cast<void, LocalConnectionToServer*, ArraySegment*, int>(), LocalConnectionToServer::SendHook);
}