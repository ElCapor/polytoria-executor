#include <ui/saveinstance.h>
#include <ui/premiumstyle.h>

// +--------------------------------------------------------+
// |                     User Interface                     |
// +--------------------------------------------------------+
#include <imgui.h>
#include <core/filesys.h>
#include <spdlog/spdlog.h>
#include <ptoria/game.h>

void SaveInstanceUI::Init()
{
    
}

void SaveInstanceUI::DrawTab()
{
    // Section header with premium styling
    PremiumStyle::DrawSectionHeader("Save Instance");
    ImGui::Spacing();
    
    // Description text
    ImGui::TextDisabled("Export the current game instance to a file for later analysis.");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Premium styled content area
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.07f, 1.00f));
    }
    
    ImGui::BeginChild("SaveContent", ImVec2(0, 0), true);
    
    // Info box
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 10));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.08f, 0.08f, 0.12f, 1.00f));
    }
    
    ImGui::BeginChild("InfoBox", ImVec2(0, 80), true);
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::TextColored(ImVec4(0.00f, 0.75f, 0.85f, 1.00f), "Export Options");
    else
        ImGui::Text("Export Options");
    ImGui::Spacing();
    ImGui::TextDisabled("Save the entire game instance hierarchy to a .poly file.");
    ImGui::TextDisabled("This includes all instances, properties, and scripts.");
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    
    ImGui::Spacing();
    
    // Save button with accent styling
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 12));
    if (PremiumStyle::StyledButton("Save To File", true, ImVec2(200, 0)))
    {
        std::vector<filesys::FileSelectFilters> filters = {
            {"Polytoria Instance Files", "poly"},
        };
        auto savePathOpt = filesys::SaveDialog(filters);
        if (savePathOpt)
        {
            std::string savePath = *savePathOpt;
            spdlog::info("Saving instance to {}", savePath);
            GameIO::GetSingleton()->SaveToFile(savePath.c_str());
        }
        else
        {
            spdlog::info("Save cancelled or failed");
        }
    }
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PopStyleVar();
    
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}