#include <ui/settingsui.h>
#include <ui/premiumstyle.h>
#include <imgui.h>

// +--------------------------------------------------------+
// |                     User Interface                     |
// +--------------------------------------------------------+

void SettingsUI::Init()
{
    // Nothing to initialize
}

void SettingsUI::DrawTab()
{
    // Section header
    PremiumStyle::DrawSectionHeader("Appearance Settings");
    
    ImGui::Spacing();
    
    // Premium Style Toggle
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 8));
    
    bool premiumEnabled = PremiumStyle::IsPremiumEnabled;
    if (ImGui::Checkbox("Enable Premium Style", &premiumEnabled))
    {
        PremiumStyle::IsPremiumEnabled = premiumEnabled;
        PremiumStyle::UpdateStyle();
    }
    
    ImGui::PopStyleVar();
    
    // Description
    ImGui::TextDisabled("Toggle between premium dark theme and default ImGui appearance.");
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Current style info
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
        ImGui::Text("Current Style: Premium Dark");
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        
        // Font info
        ImGui::Text("Font: SNPro Regular");
        ImGui::Text("Accent Color: Cyan (#00BFD5)");
        
        ImGui::Spacing();
        
        // Style details
        ImGui::BeginChild("StyleDetails", ImVec2(0, 0), true);
        
        ImGui::Text("Style Features:");
        ImGui::BulletText("Rounded corners (6-8px)");
        ImGui::BulletText("Generous padding for better clickability");
        ImGui::BulletText("High contrast text on dark backgrounds");
        ImGui::BulletText("Cyan accent color for interactive elements");
        ImGui::BulletText("Subtle borders and shadows");
        ImGui::BulletText("Professional typography with SNPro font");
        
        ImGui::EndChild();
    }
    else
    {
        ImGui::Text("Current Style: Default ImGui");
        
        ImGui::Spacing();
        
        ImGui::Text("Font: Default");
        ImGui::Text("Accent Color: Blue (#4296F9)");
    }
    
    ImGui::Spacing();
    
    // Quick actions
    ImGui::Separator();
    ImGui::Spacing();
    
    PremiumStyle::DrawSectionHeader("Quick Actions");
    
    ImGui::Spacing();
    
    if (PremiumStyle::StyledButton("Reset to Premium", true, ImVec2(150, 0)))
    {
        PremiumStyle::IsPremiumEnabled = true;
        PremiumStyle::UpdateStyle();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Reset to Default", ImVec2(150, 0)))
    {
        PremiumStyle::IsPremiumEnabled = false;
        PremiumStyle::UpdateStyle();
    }
}
