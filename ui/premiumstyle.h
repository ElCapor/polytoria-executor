#ifndef PREMIUMSTYLE_H
#define PREMIUMSTYLE_H

#include <imgui.h>

namespace PremiumStyle
{
    // Global toggle for premium style
    extern bool IsPremiumEnabled;
    
    // Font pointers for different uses
    extern ImFont* FontRegular;      // SNPro-Regular - body text
    extern ImFont* FontBold;         // SNPro-Bold - titles, headers
    extern ImFont* FontLight;        // SNPro-Light - subtle text
    extern ImFont* FontSemiBold;     // SNPro-SemiBold - subheaders
    extern ImFont* FontMedium;       // SNPro-Medium - emphasis
    extern ImFont* FontExtraBold;    // SNPro-ExtraBold - strong emphasis
    
    // Monospace font for code editors - CRITICAL: Code editors need monospace fonts!
    // SNPro is proportional (variable-width), which breaks text editor character spacing
    extern ImFont* FontMonospace;    // Monospace font for code editors
    
    // Color palette - accessible for custom widgets
    namespace Colors
    {
        // Background colors - sophisticated dark theme with subtle blue undertones
        constexpr ImVec4 WindowBg(0.08f, 0.08f, 0.10f, 1.00f);
        constexpr ImVec4 ChildBg(0.06f, 0.06f, 0.08f, 1.00f);
        constexpr ImVec4 PopupBg(0.12f, 0.12f, 0.16f, 1.00f);
        constexpr ImVec4 MenuBarBg(0.10f, 0.10f, 0.12f, 1.00f);
        
        // Text colors - high contrast
        constexpr ImVec4 Text(0.92f, 0.93f, 0.94f, 1.00f);
        constexpr ImVec4 TextDisabled(0.44f, 0.46f, 0.48f, 1.00f);
        
        // Accent colors - refined cyan-blue
        constexpr ImVec4 Accent(0.00f, 0.75f, 0.85f, 1.00f);         // Primary accent - vibrant cyan
        constexpr ImVec4 AccentHovered(0.15f, 0.85f, 0.95f, 1.00f);  // Lighter on hover
        constexpr ImVec4 AccentActive(0.00f, 0.60f, 0.70f, 1.00f);   // Darker when active
        
        // Frame colors
        constexpr ImVec4 FrameBg(0.14f, 0.14f, 0.18f, 1.00f);
        constexpr ImVec4 FrameBgHovered(0.18f, 0.18f, 0.24f, 1.00f);
        constexpr ImVec4 FrameBgActive(0.22f, 0.22f, 0.30f, 1.00f);
        
        // Border
        constexpr ImVec4 Border(0.20f, 0.20f, 0.26f, 0.50f);
        constexpr ImVec4 BorderShadow(0.00f, 0.00f, 0.00f, 0.00f);
        
        // Title bar
        constexpr ImVec4 TitleBg(0.06f, 0.06f, 0.08f, 1.00f);
        constexpr ImVec4 TitleBgActive(0.00f, 0.75f, 0.85f, 1.00f);
        constexpr ImVec4 TitleBgCollapsed(0.06f, 0.06f, 0.08f, 0.75f);
        
        // Scrollbar
        constexpr ImVec4 ScrollbarGrab(0.30f, 0.30f, 0.36f, 1.00f);
        
        // Separator
        constexpr ImVec4 Separator(0.20f, 0.20f, 0.26f, 0.50f);
        
        // Gold accent for special highlights
        constexpr ImVec4 Gold(0.95f, 0.78f, 0.20f, 1.00f);
        constexpr ImVec4 GoldHovered(1.00f, 0.85f, 0.35f, 1.00f);
    }
    
    // Initialize the premium style - call after ImGui::CreateContext()
    void Initialize();
    
    // Load fonts - call after ImGui IO is available
    void LoadFonts();
    
    // Apply the premium style to ImGui
    void ApplyStyle();
    
    // Apply default ImGui style
    void ApplyDefaultStyle();
    
    // Toggle between premium and default style
    void ToggleStyle();
    
    // Set style based on IsPremiumEnabled
    void UpdateStyle();
    
    // Helper functions for custom styling (only apply when premium is enabled)
    void PushAccentStyle();
    void PopAccentStyle();
    
    void PushWarningStyle();
    void PopWarningStyle();
    
    void PushSuccessStyle();
    void PopSuccessStyle();
    
    // Draw a styled section header
    void DrawSectionHeader(const char* text);
    
    // Draw a styled button with custom colors
    bool StyledButton(const char* label, bool useAccent = true, ImVec2 size = ImVec2(0, 0));
    
    // Helper to check if premium styling should be applied
    inline bool ShouldApplyPremium() { return IsPremiumEnabled; }
}

#endif /* PREMIUMSTYLE_H */
