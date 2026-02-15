#include <mirror/localconnectiontoserver.h>
#include <hooking/hookmanager.h>
#include <network/stream.h>
#include <network/deserializer.h>
#include <ui/packetlog.h>
#include <network/arraysegment.h>

void LocalConnectionToServer::SendHook(LocalConnectionToServer *self, ArraySegment *segment, int channelId)
{
    std::printf("Sent packet on channel %d\n", channelId);

    // Get raw packet data
    const uint8_t* data = (uint8_t*)segment->array->GetData() + segment->offset;
    size_t size = segment->count;
    
    // Create network stream for parsing
    NetworkStream stream = NetworkStream((const char*)data, (const char*)data + size);
    
    // Parse packet info
    std::string parsedInfo;
    try
    {
        // Try to deserialize and get info
        stream.PrintPayload();
        
        // Reset stream position for deserialization
        stream.SetReadPosition(0);
        
        // Try to parse the packet
        try
        {
            mirror::DeserializePacket(stream);
            parsedInfo = "Parsed successfully";
        }
        catch(const std::exception& e)
        {
            parsedInfo = std::string("Parse error: ") + e.what();
        }
    }
    catch(const std::exception& e)
    {
        parsedInfo = std::string("Error: ") + e.what();
    }
    
    // Log packet to UI
    PacketLogUI::LogPacket(PacketDirection::Outgoing, channelId, data, size, parsedInfo);
    
    // Check if interception is enabled
    if (PacketLogUI::IsInterceptionEnabled())
    {
        // Check if we should block all packets
        if (PacketLogUI::GetSettings().blockAllPackets)
        {
            // Don't send the packet
            return;
        }
        
        // Could add packet modification logic here
        // For now, just allow the packet through
    }

    HookManager::Call(SendHook, self, segment, channelId);
}