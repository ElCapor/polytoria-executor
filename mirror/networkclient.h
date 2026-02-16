#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <unity/unity.h>
#include <unity/object.h>
#include <cstdint>

struct NetworkClient : public Object<NetworkClient, "NetworkClient", Unity::Mirror, "Mirror">
{
    static UnityDictionary<std::uint32_t, UnityComponent*>* Spawned();
};

#endif /* NETWORKCLIENT_H */
