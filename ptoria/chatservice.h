#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <ptoria/instance.h>

#include <ptoria/singleton.h>

struct ChatService : public InstanceBase,
    public Object<ChatService, "ChatService", Unity::AssemblyCSharp>, 
    public Singleton<ChatService> 
    {
        void SendChat(UnityString* message);
};

#endif