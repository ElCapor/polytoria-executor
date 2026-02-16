#include <network/deserializer.h>
#include <network/commandmessage.h>

using namespace mirror;

std::unique_ptr<messages::NetworkMessage> CreateMessageFromId(messages::MsgId msgId)
{
    switch (msgId)
    {
        case messages::MsgId::CommandMessageId:
            return std::make_unique<messages::CommandMessage>();
        default:
            throw std::runtime_error("Unknown message ID");
    }
}

void mirror::DeserializePacket(NetworkStream& stream)
{
    messages::MsgId msgId = stream.read<messages::MsgId>();
    std::unique_ptr<messages::NetworkMessage> message = CreateMessageFromId(msgId);
    message->Deserialize(stream);
    message->Print();
}