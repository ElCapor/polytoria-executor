#ifndef ICON_MANAGER_H
#define ICON_MANAGER_H

// Icon Manager for Dear ImGui
// Based on: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
// 
// This provides utilities for loading and rendering PNG icons from embedded data.
// The PNG data should be in RGBA format (4 bytes per pixel).
//
// Usage:
//   1. Generate embedded_icons.h using png_to_header.py
//   2. Call IconManager::Initialize() after ImGui and DX11 are ready
//   3. Use IconManager::DrawIcon() to render icons in your UI
//   4. Call IconManager::Shutdown() before destroying the device

#include <imgui.h>
#include <string>
#include <unordered_map>

// Forward declarations for DirectX 11
struct ID3D11Device;
struct ID3D11ShaderResourceView;

class IconManager
{
public:
    // Initialize the icon manager - call after DX11 device is created
    // This creates textures for all embedded icons
    static void Initialize(ID3D11Device* device);
    
    // Shutdown and release all icon textures
    static void Shutdown();
    
    // Check if initialized
    static bool IsInitialized() { return s_initialized; }
    
    // Draw an icon by name
    // Returns true if icon was found and drawn
    static bool DrawIcon(const char* name, ImVec2 size = ImVec2(0, 0));
    
    // Draw an icon by name with custom UV coordinates (for atlases)
    static bool DrawIcon(const char* name, ImVec2 size, const ImVec2& uv0, const ImVec2& uv1);
    
    // Get icon dimensions
    static bool GetIconSize(const char* name, int* outWidth, int* outHeight);
    
    // Get the texture ID for an icon (for advanced use)
    static ImTextureID GetIconTextureID(const char* name);
    
    // Get the shader resource view for an icon (DX11 specific)
    static ID3D11ShaderResourceView* GetIconSRV(const char* name);
    
    // Create a texture from raw RGBA data
    // Returns the SRV, caller is responsible for releasing
    static ID3D11ShaderResourceView* CreateTextureFromRGBA(ID3D11Device* device, const unsigned char* data, int width, int height);
    
    // Load all icons from EmbeddedIcons namespace (requires embedded_icons.h to be included)
    static void LoadEmbeddedIcons(ID3D11Device* device);

private:
    struct IconTexture
    {
        ID3D11ShaderResourceView* srv;
        int width;
        int height;
    };
    
    static bool s_initialized;
    static ID3D11Device* s_device;
    static std::unordered_map<std::string, IconTexture> s_icons;
    
    // Create texture for a single icon
    static bool CreateIconTexture(const char* name, const unsigned char* data, int width, int height);
};

#endif // ICON_MANAGER_H
