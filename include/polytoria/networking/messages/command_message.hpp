#include <polytoria/networking/messages/network_message.hpp>

#include <unity/u.hpp>

namespace mirror::messages
{
    class CommandMessage : public NetworkMessage
    {
    public:
        CommandMessage() {
            msgId = MsgId::CommandMessageId;
        };

        uint32_t netId;
        uint8_t componentIndex;
        uint16_t functionHash;
        std::vector<uint8_t> payload;

        void Deserialize(NetworkStream& stream) override
        {
            netId = stream.ReadVarUint();
            componentIndex = stream.ReadByte();
            functionHash = stream.read<uint16_t>();

            int payloadSize = stream.ReadVarUint() - 1;
            payload.resize(payloadSize);
            stream.ReadBytes(payload.data(), payloadSize);
        }

        UC* GetTargetIdentity()
        {
            UDictionary<uint32_t, UC*>* spawnedDict = nullptr;
            U::Get(ASSEMBLY_MIRROR)->Get("NetworkClient")->Get<UF>("spawned")->GetStaticValue(&spawnedDict);

            return spawnedDict->GetValueByKey(netId);
        }

        UG* GetTarget()
        {
            return GetTargetIdentity()->GetGameObject();
        }

        void Print() override
        {
            std::printf("CommandMessage: netId=%d(%s), componentIndex=%d, functionHash=%02x\n", netId, GetTarget()->GetName()->ToString().c_str(), componentIndex, functionHash);
        }
    };
}

