#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <network/stream.h>

namespace mirror
{
    void DeserializePacket(NetworkStream& stream);
}

#endif /* DESERIALIZER_H */
