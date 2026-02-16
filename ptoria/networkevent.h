#ifndef NETWORKEVENT_H
#define NETWORKEVENT_H

#include <ptoria/instance.h>
#include <ptoria/netmessage.h>

struct NetworkEvent : public InstanceBase,
                      public Object<NetworkEvent, "NetworkEvent", Unity::AssemblyCSharp> {
    static void InvokeCmdHook(NetworkEvent* _this, NetMessage* message, void* sender);
                        
    static void InstallHooks();
};

#endif /* NETWORKEVENT_H */
