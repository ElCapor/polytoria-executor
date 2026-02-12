#include <polytoria/networking/network_stream.hpp>

#ifndef NETWORK_MESSAGE_HPP
#define NETWORK_MESSAGE_HPP

namespace mirror::messages
{
    /*
        Id=56979 = Polytoria.Networking.PTAuthenticator+AuthRequestMessage
        Id=43708 = Mirror.ReadyMessage
        Id=39124 = Mirror.CommandMessage
        Id=17487 = Mirror.NetworkPingMessage
        Id=27095 = Mirror.NetworkPongMessage
        Id=12339 = Mirror.EntityStateMessage
        Id=57097 = Mirror.TimeSnapshotMessage
        Id=49414 = Mirror.AddPlayerMessage
        Id=44385 = Mirror.ObjectDestroyMessage
        Id=32753 = Mirror.ObjectHideMessage
        Id=34916 = Mirror.SpawnMessage
        Id=12504 = Mirror.ObjectSpawnStartedMessage
        Id=43444 = Mirror.ObjectSpawnFinishedMessage
        Id=4267 = Mirror.ChangeOwnerMessage
        Id=40238 = Mirror.RpcMessage
        Id=15807 = Polytoria.Networking.PTAuthenticator+AuthResponseMessage
        Id=43378 = Mirror.NotReadyMessage
        Id=3552 = Mirror.SceneMessage
    */
    enum MsgId : uint16_t
    {
        AuthRequestMessageId = 56979,
        AuthResponseMessageId = 15807,
        ReadyMessageId = 43708,
        CommandMessageId = 39124,
        NetworkPingMessageId = 17487,
        NetworkPongMessageId = 27095,
        EntityStateMessageId = 12339,
        TimeSnapshotMessageId = 57097,
        AddPlayerMessageId = 49414,
        ObjectDestroyMessageId = 44385,
        ObjectHideMessageId = 32753,
        SpawnMessageId = 34916,
        ObjectSpawnStartedMessageId = 12504,
        ObjectSpawnFinishedMessageId = 43444,
        ChangeOwnerMessageId = 4267,
        RpcMessageId = 40238,
        NotReadyMessageId = 43378 ,
        SceneMessageId = 3552,
    };

    class NetworkMessage
    {
    public:
        MsgId msgId;

        virtual ~NetworkMessage() = default;
        virtual void Deserialize(NetworkStream& stream) = 0;
        virtual void Print() = 0;
    };
}

#endif /* NETWORK_MESSAGE_HPP */