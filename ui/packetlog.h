#ifndef PACKETLOG_H
#define PACKETLOG_H

#include <imgui.h>
#include <vector>
#include <string>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <memory>

// Forward declarations
namespace messages {
    struct NetworkMessage;
}

// Packet direction
enum class PacketDirection
{
    Outgoing,   // Client -> Server
    Incoming    // Server -> Client
};

// Captured packet data
struct CapturedPacket
{
    uint64_t id;                        // Sequential packet ID
    PacketDirection direction;          // Direction of packet
    int channelId;                      // Network channel
    std::vector<uint8_t> rawData;       // Raw packet bytes
    std::string parsedInfo;             // Parsed packet info
    std::chrono::system_clock::time_point timestamp;
    bool isModified;                    // Whether packet was modified
    bool isBlocked;                     // Whether packet was blocked
    
    // Get formatted timestamp
    std::string GetFormattedTime() const;
    
    // Get direction string
    std::string GetDirectionString() const;
    
    // Get hex string of raw data
    std::string GetHexDump(size_t maxBytes = 256) const;
};

// Packet logger settings
struct PacketLogSettings
{
    bool logOutgoing = true;            // Log outgoing packets
    bool logIncoming = true;            // Log incoming packets
    bool autoScroll = true;             // Auto-scroll to new packets
    bool showHexView = true;            // Show hex view in details
    int maxPackets = 1000;              // Maximum packets to keep
    bool interceptEnabled = false;      // Enable packet interception
    bool blockAllPackets = false;       // Block all packets (for testing)
};

class PacketLogUI
{
public:
    static void Init();
    static void DrawTab();
    
    // Called from hooks to log packets
    static void LogPacket(PacketDirection direction, int channelId, 
                          const uint8_t* data, size_t size, 
                          const std::string& parsedInfo = "");
    
    // Get settings (for hooks to check)
    static PacketLogSettings& GetSettings() { return settings; }
    
    // Get mutex for thread-safe access
    static std::mutex& GetMutex() { return packetMutex; }
    
    // Check if interception is enabled
    static bool IsInterceptionEnabled() { return settings.interceptEnabled; }
    
    // Get packet to modify (if interception is active)
    static CapturedPacket* GetPendingPacket() { return pendingPacket.get(); }
    
    // Set pending packet for modification
    static void SetPendingPacket(std::unique_ptr<CapturedPacket> packet) { pendingPacket = std::move(packet); }

private:
    static void DrawPacketList();
    static void DrawPacketDetails();
    static void DrawHexEditor();
    static void DrawInterceptionPanel();
    static void DrawFilterBar();
    
    static std::vector<CapturedPacket> packets;
    static std::mutex packetMutex;
    static PacketLogSettings settings;
    static int selectedPacketIndex;
    static uint64_t nextPacketId;
    static std::unique_ptr<CapturedPacket> pendingPacket;
    static std::string searchFilter;
    static int hexViewOffset;
};

#endif /* PACKETLOG_H */
