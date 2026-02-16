#include <network/stream.h>

NetworkStream::NetworkStream(const char* start, const char* end, bool copyData)
    : readPosition(0)
{
    if (copyData)
    {
        data.assign(start, end);
    }
    else
    {
        data = std::vector<uint8_t>(start, end);
    }
}

NetworkStream::NetworkStream(const std::vector<uint8_t>& data)
    : data(data), readPosition(0)
{
}

uint8_t NetworkStream::ReadByte()
{
    if(readPosition >= data.size())
        throw std::runtime_error("Not enough data");

    return data[readPosition++];
}

void NetworkStream::ReadBytes(uint8_t* buffer, size_t size)
{
    if(readPosition + size > data.size())
        throw std::runtime_error("Not enough data");

    std::memcpy(buffer, data.data() + readPosition, size);
    readPosition += size;
}


void NetworkStream::PrintPayload()
{
    for (size_t i = 0; i < GetSize(); i++)
    {
        std::printf("%02x ", data[i]);
    }
    std::printf("\n");
}


// stripped from mirror source
uint64_t NetworkStream::ReadVarUint()
{
    uint8_t a0 = ReadByte();
    if (a0 < 241)
    {
        return a0;
    }

    uint8_t a1 = ReadByte();
    if (a0 <= 248)
    {
        return 240 + ((a0 - (uint64_t)241) << 8) + a1;
    }

    uint8_t a2 = ReadByte();
    if (a0 == 249)
    {
        return 2288 + ((uint64_t)a1 << 8) + a2;
    }

    uint8_t a3 = ReadByte();
    if (a0 == 250)
    {
        return a1 + (((uint64_t)a2) << 8) + (((uint64_t)a3) << 16);
    }

    uint8_t a4 = ReadByte();
    if (a0 == 251)
    {
        return a1 + (((uint64_t)a2) << 8) + (((uint64_t)a3) << 16) + (((uint64_t)a4) << 24);
    }

    uint8_t a5 = ReadByte();
    if (a0 == 252)
    {
        return a1 + (((uint64_t)a2) << 8) + (((uint64_t)a3) << 16) + (((uint64_t)a4) << 24) + (((uint64_t)a5) << 32);
    }

    uint8_t a6 = ReadByte();
    if (a0 == 253)
    {
        return a1 + (((uint64_t)a2) << 8) + (((uint64_t)a3) << 16) + (((uint64_t)a4) << 24) + (((uint64_t)a5) << 32) + (((uint64_t)a6) << 40);
    }

    uint8_t a7 = ReadByte();
    if (a0 == 254)
    {
        return a1 + (((uint64_t)a2) << 8) + (((uint64_t)a3) << 16) + (((uint64_t)a4) << 24) + (((uint64_t)a5) << 32) + (((uint64_t)a6) << 40) + (((uint64_t)a7) << 48);
    }

    uint8_t a8 = ReadByte();
    if (a0 == 255)
    {
        return a1 + (((uint64_t)a2) << 8) + (((uint64_t)a3) << 16) + (((uint64_t)a4) << 24) + (((uint64_t)a5) << 32) + (((uint64_t)a6) << 40) + (((uint64_t)a7) << 48)  + (((uint64_t)a8) << 56);
    }


    throw std::runtime_error("Invalid VarUint encoding");
}