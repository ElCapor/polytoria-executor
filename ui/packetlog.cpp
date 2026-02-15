#include <ui/packetlog.h>
#include <ui/premiumstyle.h>
#include <imgui.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
std::vector<CapturedPacket> PacketLogUI::packets;
std::mutex PacketLogUI::packetMutex;
PacketLogSettings PacketLogUI::settings;
int PacketLogUI::selectedPacketIndex = -1;
uint64_t PacketLogUI::nextPacketId = 1;
std::unique_ptr<CapturedPacket> PacketLogUI::pendingPacket = nullptr;
std::string PacketLogUI::searchFilter;
int PacketLogUI::hexViewOffset = 0;

// +--------------------------------------------------------+
// |                   CapturedPacket Methods               |
// +--------------------------------------------------------+
std::string CapturedPacket::GetFormattedTime() const
{
    auto timeT = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeT), "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string CapturedPacket::GetDirectionString() const
{
    return direction == PacketDirection::Outgoing ? "OUT" : "IN";
}

std::string CapturedPacket::GetHexDump(size_t maxBytes) const
{
    std::stringstream ss;
    size_t bytesToShow = std::min(maxBytes, rawData.size());
    
    for (size_t i = 0; i < bytesToShow; ++i)
    {
        ss << std::hex << std::setfill('0') << std::setw(2) 
           << static_cast<int>(rawData[i]) << " ";
        
        if ((i + 1) % 16 == 0)
            ss << "\n";
    }
    
    if (bytesToShow < rawData.size())
        ss << "\n... (" << (rawData.size() - bytesToShow) << " more bytes)";
    
    return ss.str();
}

// +--------------------------------------------------------+
// |                   PacketLogUI Methods                  |
// +--------------------------------------------------------+
void PacketLogUI::Init()
{
    packets.clear();
    selectedPacketIndex = -1;
    nextPacketId = 1;
}

void PacketLogUI::LogPacket(PacketDirection direction, int channelId, 
                            const uint8_t* data, size_t size, 
                            const std::string& parsedInfo)
{
    std::lock_guard<std::mutex> lock(packetMutex);
    
    // Check if we should log this direction
    if (direction == PacketDirection::Outgoing && !settings.logOutgoing)
        return;
    if (direction == PacketDirection::Incoming && !settings.logIncoming)
        return;
    
    // Create new packet
    CapturedPacket packet;
    packet.id = nextPacketId++;
    packet.direction = direction;
    packet.channelId = channelId;
    packet.rawData.assign(data, data + size);
    packet.parsedInfo = parsedInfo;
    packet.timestamp = std::chrono::system_clock::now();
    packet.isModified = false;
    packet.isBlocked = false;
    
    // Add to list
    packets.push_back(std::move(packet));
    
    // Trim old packets if needed
    while (packets.size() > static_cast<size_t>(settings.maxPackets))
    {
        packets.erase(packets.begin());
        if (selectedPacketIndex > 0)
            selectedPacketIndex--;
    }
}

void PacketLogUI::DrawTab()
{
    // Section header
    PremiumStyle::DrawSectionHeader("Packet Logger");
    ImGui::Spacing();
    
    // Draw filter bar
    DrawFilterBar();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Main content area - split into list and details
    float availableHeight = ImGui::GetContentRegionAvail().y;
    float listHeight = availableHeight * 0.5f;
    float detailsHeight = availableHeight * 0.5f - 30.0f;
    
    // Packet list
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.07f, 1.00f));
    }
    
    ImGui::BeginChild("PacketList", ImVec2(0, listHeight), true);
    DrawPacketList();
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    
    ImGui::Spacing();
    
    // Packet details
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.07f, 1.00f));
    }
    
    ImGui::BeginChild("PacketDetails", ImVec2(0, detailsHeight), true);
    DrawPacketDetails();
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    
    // Interception panel
    if (settings.interceptEnabled)
    {
        ImGui::Spacing();
        DrawInterceptionPanel();
    }
}

void PacketLogUI::DrawFilterBar()
{
    // Controls row
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
    
    // Logging toggles
    ImGui::Checkbox("Log Outgoing", &settings.logOutgoing);
    ImGui::SameLine();
    ImGui::Checkbox("Log Incoming", &settings.logIncoming);
    ImGui::SameLine();
    ImGui::Checkbox("Auto Scroll", &settings.autoScroll);
    ImGui::SameLine();
    ImGui::Checkbox("Show Hex", &settings.showHexView);
    
    // Interception toggle
    ImGui::SameLine();
    if (settings.interceptEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.78f, 0.20f, 1.00f));
        ImGui::Checkbox("Intercept Mode", &settings.interceptEnabled);
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::Checkbox("Intercept Mode", &settings.interceptEnabled);
    }
    
    ImGui::PopStyleVar();
    
    // Second row
    ImGui::Spacing();
    
    // Search filter
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::PushItemWidth(200);
    static char searchBuffer[256] = "";
    if (ImGui::InputText("##SearchFilter", searchBuffer, sizeof(searchBuffer)))
    {
        searchFilter = searchBuffer;
    }
    ImGui::PopItemWidth();
    
    ImGui::SameLine();
    
    // Clear button
    if (ImGui::Button("Clear Log"))
    {
        std::lock_guard<std::mutex> lock(packetMutex);
        packets.clear();
        selectedPacketIndex = -1;
    }
    
    ImGui::SameLine();
    
    // Packet count
    {
        std::lock_guard<std::mutex> lock(packetMutex);
        ImGui::TextDisabled("(%zu packets)", packets.size());
    }
}

void PacketLogUI::DrawPacketList()
{
    std::lock_guard<std::mutex> lock(packetMutex);
    
    // Table header
    if (ImGui::BeginTable("PacketsTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Dir", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Ch", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        // Filter packets
        std::vector<int> filteredIndices;
        for (int i = 0; i < static_cast<int>(packets.size()); ++i)
        {
            const auto& packet = packets[i];
            
            // Apply search filter
            if (!searchFilter.empty())
            {
                std::string info = packet.parsedInfo;
                std::string searchLower = searchFilter;
                std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
                std::transform(info.begin(), info.end(), info.begin(), ::tolower);
                
                if (info.find(searchLower) == std::string::npos)
                    continue;
            }
            
            filteredIndices.push_back(i);
        }
        
        // Draw packets
        for (int i : filteredIndices)
        {
            const auto& packet = packets[i];
            
            ImGui::TableNextRow();
            
            // ID
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%llu", packet.id);
            
            // Time
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", packet.GetFormattedTime().c_str());
            
            // Direction
            ImGui::TableSetColumnIndex(2);
            if (PremiumStyle::IsPremiumEnabled)
            {
                if (packet.direction == PacketDirection::Outgoing)
                    ImGui::TextColored(ImVec4(0.20f, 0.85f, 0.40f, 1.00f), "OUT");
                else
                    ImGui::TextColored(ImVec4(0.85f, 0.40f, 0.40f, 1.00f), "IN");
            }
            else
            {
                ImGui::Text("%s", packet.GetDirectionString().c_str());
            }
            
            // Channel
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%d", packet.channelId);
            
            // Info
            ImGui::TableSetColumnIndex(4);
            std::string info = packet.parsedInfo.empty() ? 
                ("Raw packet (" + std::to_string(packet.rawData.size()) + " bytes)") : 
                packet.parsedInfo;
            
            // Modified/blocked indicators
            if (packet.isModified)
            {
                ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.20f, 1.00f), "[MOD] ");
                ImGui::SameLine(0, 0);
            }
            if (packet.isBlocked)
            {
                ImGui::TextColored(ImVec4(0.85f, 0.40f, 0.40f, 1.00f), "[BLK] ");
                ImGui::SameLine(0, 0);
            }
            
            ImGui::Text("%s", info.c_str());
            
            // Selectable row
            if (ImGui::IsItemClicked())
            {
                selectedPacketIndex = i;
            }
            
            // Highlight selected
            if (i == selectedPacketIndex)
            {
                ImU32 rowColor = ImGui::GetColorU32(ImVec4(0.00f, 0.75f, 0.85f, 0.30f));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, rowColor);
            }
        }
        
        ImGui::EndTable();
    }
}

void PacketLogUI::DrawPacketDetails()
{
    if (selectedPacketIndex < 0 || selectedPacketIndex >= static_cast<int>(packets.size()))
    {
        ImGui::TextDisabled("Select a packet to view details");
        return;
    }
    
    std::lock_guard<std::mutex> lock(packetMutex);
    const auto& packet = packets[selectedPacketIndex];
    
    // Header
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PushFont(PremiumStyle::FontBold);
    
    ImGui::Text("Packet #%llu", packet.id);
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PopFont();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Details table
    if (ImGui::BeginTable("DetailsTable", 2, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        
        // Direction
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Direction");
        ImGui::TableSetColumnIndex(1);
        if (PremiumStyle::IsPremiumEnabled)
        {
            if (packet.direction == PacketDirection::Outgoing)
                ImGui::TextColored(ImVec4(0.20f, 0.85f, 0.40f, 1.00f), "Outgoing (Client -> Server)");
            else
                ImGui::TextColored(ImVec4(0.85f, 0.40f, 0.40f, 1.00f), "Incoming (Server -> Client)");
        }
        else
        {
            ImGui::Text("%s", packet.direction == PacketDirection::Outgoing ? "Outgoing" : "Incoming");
        }
        
        // Channel
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Channel");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%d", packet.channelId);
        
        // Size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu bytes", packet.rawData.size());
        
        // Timestamp
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Timestamp");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", packet.GetFormattedTime().c_str());
        
        // Status
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Status");
        ImGui::TableSetColumnIndex(1);
        if (packet.isBlocked)
            ImGui::TextColored(ImVec4(0.85f, 0.40f, 0.40f, 1.00f), "Blocked");
        else if (packet.isModified)
            ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.20f, 1.00f), "Modified");
        else
            ImGui::Text("Normal");
        
        // Parsed info
        if (!packet.parsedInfo.empty())
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Parsed");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextWrapped("%s", packet.parsedInfo.c_str());
        }
        
        ImGui::EndTable();
    }
    
    // Hex view
    if (settings.showHexView && !packet.rawData.empty())
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
            ImGui::PushFont(PremiumStyle::FontBold);
        ImGui::Text("Hex View");
        if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
            ImGui::PopFont();
        
        ImGui::Spacing();
        
        // Hex dump in a child window
        ImGui::BeginChild("HexView", ImVec2(0, 150), true);
        
        const uint8_t* data = packet.rawData.data();
        size_t size = packet.rawData.size();
        
        // Display hex dump with addresses
        for (size_t i = 0; i < size; i += 16)
        {
            // Address
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%04zX: ", i);
            ImGui::SameLine();
            
            // Hex bytes
            std::string hexLine;
            std::string asciiLine;
            
            for (size_t j = 0; j < 16 && (i + j) < size; ++j)
            {
                uint8_t byte = data[i + j];
                
                // Hex
                char hex[4];
                snprintf(hex, sizeof(hex), "%02X ", byte);
                hexLine += hex;
                
                // ASCII
                asciiLine += (byte >= 32 && byte < 127) ? static_cast<char>(byte) : '.';
            }
            
            // Pad hex line
            while (hexLine.length() < 48)
                hexLine += "   ";
            
            ImGui::Text("%s  %s", hexLine.c_str(), asciiLine.c_str());
        }
        
        ImGui::EndChild();
    }
    
    // Actions
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::Button("Copy Hex"))
    {
        std::string hexDump = packet.GetHexDump(0);
        ImGui::SetClipboardText(hexDump.c_str());
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Copy Raw Data"))
    {
        std::string raw(packet.rawData.begin(), packet.rawData.end());
        ImGui::SetClipboardText(raw.c_str());
    }
}

void PacketLogUI::DrawInterceptionPanel()
{
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.08f, 0.05f, 1.00f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
    }
    
    ImGui::BeginChild("InterceptionPanel", ImVec2(0, 100), true);
    
    // Warning header
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.78f, 0.20f, 1.00f));
        if (PremiumStyle::FontBold)
            ImGui::PushFont(PremiumStyle::FontBold);
    }
    
    ImGui::Text("INTERCEPTION MODE ACTIVE");
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        if (PremiumStyle::FontBold)
            ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    
    ImGui::Spacing();
    
    // Block all toggle
    ImGui::Checkbox("Block All Packets", &settings.blockAllPackets);
    
    // Pending packet info
    if (pendingPacket)
    {
        ImGui::Text("Pending: Packet #%llu (%zu bytes)", 
                    pendingPacket->id, pendingPacket->rawData.size());
        
        ImGui::SameLine();
        
        if (ImGui::Button("Allow"))
        {
            pendingPacket.reset();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Block"))
        {
            if (pendingPacket)
                pendingPacket->isBlocked = true;
            pendingPacket.reset();
        }
    }
    else
    {
        ImGui::TextDisabled("No pending packet");
    }
    
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
}

void PacketLogUI::DrawHexEditor()
{
    // Reserved for future hex editor integration
    // Can integrate with memoryeditor.h if needed
}
