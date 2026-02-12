#include <polytoria/networking/mirror_hook.hpp>
#include <polytoria/networking/network_stream.hpp>
#include <polytoria/networking/deserializer_core.hpp>
#include <unity/u.hpp>

using namespace mirror;

struct ArraySegment {
    UArray<uint8_t>* array;
    int offset;
    int count;
};

typedef void (*Send_t)(UO* self, ArraySegment* segment, int channelId);

void dtSend(UO* self, ArraySegment* segment, int channelId)
{
    std::printf("Sent packet on channel %d\n", channelId);

    NetworkStream stream = NetworkStream((const char*)segment->array->GetData() + segment->offset, (const char*)segment->array->GetData() + segment->offset + segment->count);
    
    stream.PrintPayload();

    try
    {
        mirror::DeserializePacket(stream);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error while deserializing packet: " << e.what() << '\n';
    }
    
    

    HookManager::Call(dtSend, self, segment, channelId);
}

void mirror::InstallHooks()
{
    // we hook Mirror::LocalConnectionToServer::Send for outgoing packets

    static auto method = U::Get("Mirror.dll")->Get("LocalConnectionToServer", "Mirror")->Get<UM>("Send");
    
    if(!method) {
        std::printf("Failed to find method to hook\n");
        return;
    }

    std::printf("Hooking Mirror::LocalConnectionToServer::Send at address %p\n", method->function);
    HookManager::Install((Send_t)method->function, dtSend);
}