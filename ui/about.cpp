#include <ui/about.h>
#include <string>
#include <vector>
#include <chrono>
#include <imgui.h>
#include <core/filesys.h>
#include <ui/premiumstyle.h>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
struct Contributor
{
    std::string name;
    std::string link;
    std::string what;
    ImVec4 color;  // Unique accent color for each contributor
};

// Contributors with unique accent colors
static std::vector<Contributor> contributors = {
    // in order of contribution
    {"0x108", "https://github.com/NtReadVirtualMemory", "Fantastic friend + Helped rewriting parts of the v1/v2", ImVec4(0.4f, 0.6f, 1.0f, 1.0f)},   // Blue
    {"Nasec", "https://github.com/NASCeria", "Smart boi => Custom Functions + Networking", ImVec4(0.6f, 1.0f, 0.4f, 1.0f)},                    // Green
    {"Eli", "https://github.com/geringverdien", "Custom functions", ImVec4(1.0f, 0.6f, 0.8f, 1.0f)},                                          // Pink
    {"Ficello", "https://github.com/1337Skid", "Early findings and first scripts", ImVec4(1.0f, 0.8f, 0.3f, 1.0f)}                          // Gold
};

// +--------------------------------------------------------+
// |                       Functions                        |
// +--------------------------------------------------------+
void AboutUI::Init()
{
}

// Helper to draw a contributor card
static void DrawContributorCard(const Contributor& contributor, int index, float width)
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Card background with contributor's accent color as subtle border
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.10f, 0.10f, 0.14f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(contributor.color.x, contributor.color.y, contributor.color.z, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
    
    std::string childId = "contributor_" + std::to_string(index);
    ImGui::BeginChild(childId.c_str(), ImVec2(width, 85.0f), true, ImGuiWindowFlags_NoScrollbar);
    
    // Contributor name with their accent color
    ImGui::PushStyleColor(ImGuiCol_Text, contributor.color);
    if (PremiumStyle::FontSemiBold)
        ImGui::PushFont(PremiumStyle::FontSemiBold);
    ImGui::Text("%s", contributor.name.c_str());
    if (PremiumStyle::FontSemiBold)
        ImGui::PopFont();
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // "View Profile" button styled with their color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(contributor.color.x * 0.3f, contributor.color.y * 0.3f, contributor.color.z * 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(contributor.color.x * 0.5f, contributor.color.y * 0.5f, contributor.color.z * 0.5f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(contributor.color.x * 0.7f, contributor.color.y * 0.7f, contributor.color.z * 0.7f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, contributor.color);
    
    std::string btnLabel = "View Profile##" + std::to_string(index);
    if (ImGui::SmallButton(btnLabel.c_str()))
    {
        filesys::OpenUrlInBrowser(contributor.link);
    }
    
    ImGui::PopStyleColor(4);
    
    // Contribution description
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
    ImGui::TextWrapped("%s", contributor.what.c_str());
    ImGui::PopStyleColor();
    
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void AboutUI::DrawTab()
{
    ImGuiStyle& style = ImGui::GetStyle();
    float windowWidth = ImGui::GetContentRegionAvail().x;
    
    // ============== HEADER SECTION ==============
    // Title with gradient-like effect using accent color
    ImGui::PushStyleColor(ImGuiCol_Text, PremiumStyle::Colors::Accent);
    if (PremiumStyle::FontExtraBold)
        ImGui::PushFont(PremiumStyle::FontExtraBold);
    
    ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("PolyHack").x) * 0.5f);
    ImGui::Text("PolyHack");
    
    if (PremiumStyle::FontExtraBold)
        ImGui::PopFont();
    ImGui::PopStyleColor();
    
    // Subtitle
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.65f, 1.0f));
    if (PremiumStyle::FontLight)
        ImGui::PushFont(PremiumStyle::FontLight);
    
    std::string subtitle = "Polytoria Mod Menu";
    ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(subtitle.c_str()).x) * 0.5f);
    ImGui::Text("%s", subtitle.c_str());
    
    if (PremiumStyle::FontLight)
        ImGui::PopFont();
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Version badge
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.75f, 0.85f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.75f, 0.85f, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.75f, 0.85f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_Text, PremiumStyle::Colors::Accent);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    
    std::string versionText = " v3.0 ";
    ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(versionText.c_str()).x - style.FramePadding.x * 2) * 0.5f);
    ImGui::SmallButton(versionText.c_str());
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // ============== PHILOSOPHY SECTION ==============
    PremiumStyle::DrawSectionHeader("Our Philosophy");
    
    ImGui::Spacing();
    
    // Philosophy box with special styling
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.08f, 0.10f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.75f, 0.85f, 0.3f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 12));
    
    ImGui::BeginChild("philosophy_box", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4 + 24), true);
    
    // Main philosophy text
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.9f, 1.0f));
    ImGui::TextWrapped("PolyHack is an open-source learning tool designed for educational purposes. ");
    ImGui::TextWrapped("Our mission is to provide a transparent, accessible codebase that helps developers ");
    ImGui::TextWrapped("understand game internals, reverse engineering, and software development concepts.");
    ImGui::PopStyleColor();
    
    ImGui::EndChild();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    
    ImGui::Spacing();
    
    // Open source call-to-action
    ImGui::PushStyleColor(ImGuiCol_Text, PremiumStyle::Colors::Gold);
    if (PremiumStyle::FontMedium)
        ImGui::PushFont(PremiumStyle::FontMedium);
    
    ImGui::TextWrapped("We welcome contributions of any nature!");
    
    if (PremiumStyle::FontMedium)
        ImGui::PopFont();
    ImGui::PopStyleColor();
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
    ImGui::TextWrapped("Whether it's code, documentation, bug reports, or feature suggestions - ");
    ImGui::TextWrapped("every contribution helps make PolyHack better for everyone.");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // ============== LINKS SECTION ==============
    PremiumStyle::DrawSectionHeader("Quick Links");
    
    ImGui::Spacing();
    
    // Centered link buttons
    float buttonWidth = 140.0f;
    float totalButtonsWidth = buttonWidth * 3 + style.ItemSpacing.x * 2;
    ImGui::SetCursorPosX((windowWidth - totalButtonsWidth) * 0.5f);
    
    // GitHub button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.45f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    
    if (ImGui::Button("GitHub Repository", ImVec2(buttonWidth, 0)))
    {
        filesys::OpenUrlInBrowser("https://github.com/ElCapor/PolytoriaExecutor");
    }
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    
    // Discord button with purple accent
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.3f, 0.55f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.4f, 0.65f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.55f, 0.5f, 0.75f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.7f, 1.0f, 1.0f));
    
    if (ImGui::Button("Discord Server", ImVec2(buttonWidth, 0)))
    {
        filesys::OpenUrlInBrowser("https://discord.gg/NrXZmyPRvh");
    }
    ImGui::PopStyleColor(4);
    
    ImGui::SameLine();
    
    // YouTube button with red accent
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.7f, 0.7f, 1.0f));
    
    if (ImGui::Button("YouTube", ImVec2(buttonWidth, 0)))
    {
        filesys::OpenUrlInBrowser("https://www.youtube.com/@SkipSped");
    }
    ImGui::PopStyleColor(4);
    
    ImGui::PopStyleVar();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // ============== CONTRIBUTORS SECTION ==============
    PremiumStyle::DrawSectionHeader("Contributors");
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.65f, 1.0f));
    ImGui::TextWrapped("Special thanks to everyone who has contributed to PolyHack:");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Draw contributor cards in a 2-column grid layout
    // Calculate card width to fit 2 columns with equal spacing
    float cardWidth = (windowWidth - style.ItemSpacing.x) * 0.5f;
    
    // Row 1: Contributors 0 and 1
    DrawContributorCard(contributors[0], 0, cardWidth);
    ImGui::SameLine();
    DrawContributorCard(contributors[1], 1, cardWidth);
    
    ImGui::Spacing();
    
    // Row 2: Contributors 2 and 3
    DrawContributorCard(contributors[2], 2, cardWidth);
    ImGui::SameLine();
    DrawContributorCard(contributors[3], 3, cardWidth);
    
    ImGui::Spacing();
    
    // "Become a contributor" call-to-action
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.75f, 0.85f, 0.15f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.75f, 0.85f, 0.25f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.75f, 0.85f, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_Text, PremiumStyle::Colors::Accent);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    
    ImGui::SetCursorPosX((windowWidth - 200.0f) * 0.5f);
    if (ImGui::Button("Become a Contributor!", ImVec2(200.0f, 0)))
    {
        filesys::OpenUrlInBrowser("https://github.com/ElCapor/polytoria-executor
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // ============== DISCLAIMER SECTION ==============
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
    if (PremiumStyle::FontLight)
        ImGui::PushFont(PremiumStyle::FontLight);
    
    ImGui::TextWrapped("Disclaimer: PolyHack is intended for educational and testing purposes only. ");
    ImGui::TextWrapped("Use responsibly and respect the game's terms of service.");
    
    if (PremiumStyle::FontLight)
        ImGui::PopFont();
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    
    // Author credit
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.45f, 1.0f));
    std::string authorText = "Created with love by ElCapor";
    ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(authorText.c_str()).x) * 0.5f);
    ImGui::Text("%s", authorText.c_str());
    ImGui::PopStyleColor();
}
