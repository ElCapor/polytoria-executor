#ifndef LOCALCONNECTIONTOSERVER_H
#define LOCALCONNECTIONTOSERVER_H
#include <unity/unity.h>
#include <unity/object.h>
#include <network/arraysegment.h>

struct LocalConnectionToServer : public Object<LocalConnectionToServer, "LocalConnectionToServer", Unity::Mirror, "Mirror">
{
    static void SendHook(LocalConnectionToServer *self, ArraySegment *segment, int channelId);
};

#endif /* LOCALCONNECTIONTOSERVER_H */
