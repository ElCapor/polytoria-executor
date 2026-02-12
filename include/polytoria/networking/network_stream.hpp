// Little network stream class to do handle proper deserializing of Packets
#ifndef NETWORK_STREAM_HPP
#define NETWORK_STREAM_HPP

#include <vector>
#include <iostream>


class NetworkStream{
private:
    std::vector<uint8_t> data;
    size_t readPosition;
public:
    NetworkStream(const char* start, const char* end, bool copyData = false);
    NetworkStream(const std::vector<uint8_t>& data);

    template<typename T>
    T read(){
        T result;
        if(readPosition + sizeof(T) > data.size())
            throw std::runtime_error("Not enough data");

        std::memcpy(&result, data.data() + readPosition, sizeof(T));
        readPosition += sizeof(T);
        return result;
    }

    template<typename T>
    void read(T& val)
    {
        if(readPosition + sizeof(T) > data.size())
            throw std::runtime_error("Not enough data");

        val = *(T*)(data.data() + readPosition);
        readPosition += sizeof(T);
    }

    uint8_t ReadByte();
    void ReadBytes(uint8_t* buffer, size_t size);

    [[nodiscard]] size_t GetSize() const { return data.size(); }

    [[nodiscard]] const char* GetData() { return (const char*)data.data(); };

    [[nodiscard]] const size_t& GetReadPosition() const { return readPosition; }
    void SetReadPosition(size_t position) { readPosition = position; }

    uint64_t ReadVarUint();

    void PrintPayload();
};

#endif /* NETWORK_STREAM_HPP */