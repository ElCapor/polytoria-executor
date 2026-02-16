#include <mirror/networkclient.h>

UnityDictionary<std::uint32_t, UnityComponent*>* NetworkClient::Spawned()
{
    return Unity::GetStaticFieldValue<UnityDictionary<std::uint32_t, UnityComponent*>*, "spawned">(StaticClass<NetworkClient>());
}