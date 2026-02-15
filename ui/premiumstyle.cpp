#include <ui/premiumstyle.h>
#include <imgui.h>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
namespace PremiumStyle
{
    bool IsPremiumEnabled = true;
    ImFont* FontRegular = nullptr;
    ImFont* FontBold = nullptr;
    ImFont* FontLight = nullptr;
    ImFont* FontSemiBold = nullptr;
    ImFont* FontMedium = nullptr;
    ImFont* FontExtraBold = nullptr;
}

// +--------------------------------------------------------+
// |                   Style Functions                      |
// +--------------------------------------------------------+
void PremiumStyle::Initialize()
{
    // Initial setup - fonts are loaded separately
}

void PremiumStyle::LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    
    // Configure font loading
    ImFontConfig config;
    config.OversampleH = 2;
    config.OversampleV = 1;
    config.PixelSnapH = true;
    
    // Load SNPro font family
    FontRegular = io.Fonts->AddFontFromFileTTF("fonts/SNPro-Regular.ttf", 16.0f, &config);
    FontBold = io.Fonts->AddFontFromFileTTF("fonts/SNPro-Bold.ttf", 16.0f, &config);
    FontLight = io.Fonts->AddFontFromFileTTF("fonts/SNPro-Light.ttf", 16.0f, &config);
    FontSemiBold = io.Fonts->AddFontFromFileTTF("fonts/SNPro-SemiBold.ttf", 16.0f, &config);
    FontMedium = io.Fonts->AddFontFromFileTTF("fonts/SNPro-Medium.ttf", 16.0f, &config);
    FontExtraBold = io.Fonts->AddFontFromFileTTF("fonts/SNPro-ExtraBold.ttf", 16.0f, &config);
    
    // If font loading failed, fall back to default
    if (!FontRegular)
    {
        FontRegular = io.FontDefault;
    }
    if (!FontBold)
    {
        FontBold = FontRegular;
    }
    if (!FontLight)
    {
        FontLight = FontRegular;
    }
    if (!FontSemiBold)
    {
        FontSemiBold = FontBold;
    }
    if (!FontMedium)
    {
        FontMedium = FontRegular;
    }
    if (!FontExtraBold)
    {
        FontExtraBold = FontBold;
    }
}

void PremiumStyle::ApplyStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Set style shaping - modern, polished appearance
    style.FrameRounding = 6.0f;
    style.WindowRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ChildRounding = 6.0f;
    
    // Generous padding for easy clicking
    style.FramePadding = ImVec2(10.0f, 6.0f);
    style.ItemSpacing = ImVec2(10.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.WindowPadding = ImVec2(12.0f, 10.0f);
    
    // Borders
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    
    // Minimum sizes
    style.WindowMinSize = ImVec2(200, 100);
    style.GrabMinSize = 12.0f;
    
    // Anti-aliasing
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    
    // --- Premium Dark Color Scheme ---
    // Backgrounds: Sophisticated, slightly blue-grey dark
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

    // Text: High contrast, crisp
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.94f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.46f, 0.48f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.75f, 0.85f, 0.35f);

    // Borders: Very subtle, just to define edges if needed
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.26f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Input fields, buttons base): Slightly distinct from background
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);

    // Accent: A refined, vibrant cyan
    ImVec4 accent_color = ImVec4(0.00f, 0.75f, 0.85f, 1.00f);
    ImVec4 accent_hovered = ImVec4(0.15f, 0.85f, 0.95f, 1.00f);
    ImVec4 accent_active = ImVec4(0.00f, 0.60f, 0.70f, 1.00f);

    colors[ImGuiCol_TitleBgActive] = accent_color;
    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.06f, 0.08f, 0.75f);

    colors[ImGuiCol_Button] = colors[ImGuiCol_FrameBg];
    colors[ImGuiCol_ButtonHovered] = accent_hovered;
    colors[ImGuiCol_ButtonActive] = accent_active;

    colors[ImGuiCol_Header] = ImVec4(accent_color.x, accent_color.y, accent_color.z, 0.50f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(accent_hovered.x, accent_hovered.y, accent_hovered.z, 0.70f);
    colors[ImGuiCol_HeaderActive] = accent_active;

    colors[ImGuiCol_SliderGrab] = accent_color;
    colors[ImGuiCol_SliderGrabActive] = accent_hovered;
    colors[ImGuiCol_CheckMark] = accent_color;

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.04f, 0.04f, 0.06f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.36f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = accent_hovered;
    colors[ImGuiCol_ScrollbarGrabActive] = accent_active;

    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = accent_hovered;
    colors[ImGuiCol_SeparatorActive] = accent_active;

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.36f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = accent_hovered;
    colors[ImGuiCol_ResizeGripActive] = accent_active;

    colors[ImGuiCol_PlotLines] = accent_color;
    colors[ImGuiCol_PlotLinesHovered] = accent_hovered;
    colors[ImGuiCol_PlotHistogram] = accent_color;
    colors[ImGuiCol_PlotHistogramHovered] = accent_hovered;
    
    colors[ImGuiCol_DragDropTarget] = ImVec4(accent_color.x, accent_color.y, accent_color.z, 0.90f);
    colors[ImGuiCol_NavHighlight] = accent_color;
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
    
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.10f, 0.14f, 1.00f);
    colors[ImGuiCol_TabHovered] = accent_color;
    colors[ImGuiCol_TabActive] = ImVec4(accent_color.x, accent_color.y, accent_color.z, 0.80f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);
    
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.10f, 0.10f, 0.14f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.20f, 0.26f, 0.60f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.16f, 0.16f, 0.22f, 0.40f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.06f, 0.06f, 0.10f, 0.50f);
}

void PremiumStyle::ApplyDefaultStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Reset to ImGui default style
    style.FrameRounding = 3.0f;
    style.WindowRounding = 0.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.PopupRounding = 0.0f;
    style.ChildRounding = 0.0f;
    
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    
    style.WindowMinSize = ImVec2(32, 32);
    style.GrabMinSize = 10.0f;
    
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    
    // Reset colors to default ImGui dark style
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.23f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void PremiumStyle::ToggleStyle()
{
    IsPremiumEnabled = !IsPremiumEnabled;
    UpdateStyle();
}

void PremiumStyle::UpdateStyle()
{
    if (IsPremiumEnabled)
    {
        ApplyStyle();
    }
    else
    {
        ApplyDefaultStyle();
    }
}

// +--------------------------------------------------------+
// |                   Helper Functions                     |
// +--------------------------------------------------------+
void PremiumStyle::PushAccentStyle()
{
    if (!IsPremiumEnabled) return;
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.85f, 0.95f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.00f, 0.60f, 0.70f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
}

void PremiumStyle::PopAccentStyle()
{
    if (!IsPremiumEnabled) return;
    
    ImGui::PopStyleColor(4);
}

void PremiumStyle::PushWarningStyle()
{
    if (!IsPremiumEnabled) return;
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.85f, 0.40f, 0.20f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.95f, 0.50f, 0.30f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.30f, 0.10f, 1.00f));
}

void PremiumStyle::PopWarningStyle()
{
    if (!IsPremiumEnabled) return;
    
    ImGui::PopStyleColor(3);
}

void PremiumStyle::PushSuccessStyle()
{
    if (!IsPremiumEnabled) return;
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.70f, 0.35f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.80f, 0.45f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.60f, 0.25f, 1.00f));
}

void PremiumStyle::PopSuccessStyle()
{
    if (!IsPremiumEnabled) return;
    
    ImGui::PopStyleColor(3);
}

void PremiumStyle::DrawSectionHeader(const char* text)
{
    if (IsPremiumEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
        if (FontBold)
            ImGui::PushFont(FontBold);
    }
    
    ImGui::Text("%s", text);
    
    if (IsPremiumEnabled)
    {
        if (FontBold)
            ImGui::PopFont();
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }
}

bool PremiumStyle::StyledButton(const char* label, bool useAccent, ImVec2 size)
{
    bool result = false;
    
    if (IsPremiumEnabled && useAccent)
    {
        PushAccentStyle();
        result = ImGui::Button(label, size);
        PopAccentStyle();
    }
    else
    {
        result = ImGui::Button(label, size);
    }
    
    return result;
}
