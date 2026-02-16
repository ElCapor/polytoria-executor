#ifndef COMMANDS_H
#define COMMANDS_H

#include <network/networkmessage.h>
#include <unity/unity.h>

namespace mirror::messages
{
    class CommandMessage : public NetworkMessage
    {
    public:
        uint32_t netId;
        uint8_t componentIndex;
        uint16_t functionHash;
        std::vector<uint8_t> payload;

        void Deserialize(NetworkStream& stream) override;
        UnityComponent* TargetIdentity();
        UnityGameObject* Target();
        void Print() override;

        CommandMessage() {
            msgId = MsgId::CommandMessageId;
        };
    };
}

#endif /* COMMANDS_H */
