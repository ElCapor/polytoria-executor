#include <ui/iconmanager.h>

// Include embedded icons if available
// Uncomment this line after generating embedded_icons.h with png_to_header.py
#include <ui/embedded_icons.h>

#include <d3d11.h>
#include <cstdio>

// Static member definitions
bool IconManager::s_initialized = false;
ID3D11Device* IconManager::s_device = nullptr;
std::unordered_map<std::string, IconManager::IconTexture> IconManager::s_icons;

void IconManager::Initialize(ID3D11Device* device)
{
    if (s_initialized)
    {
        return;
    }
    
    s_device = device;
    s_initialized = true;
    
    // Load embedded icons if the header is included
    // Uncomment after generating embedded_icons.h
    LoadEmbeddedIcons(device);
    
    printf("[IconManager] Initialized\n");
}

void IconManager::Shutdown()
{
    if (!s_initialized)
    {
        return;
    }
    
    // Release all shader resource views
    for (auto& pair : s_icons)
    {
        if (pair.second.srv)
        {
            pair.second.srv->Release();
        }
    }
    s_icons.clear();
    
    s_device = nullptr;
    s_initialized = false;
    
    printf("[IconManager] Shutdown complete\n");
}

bool IconManager::DrawIcon(const char* name, ImVec2 size)
{
    auto it = s_icons.find(name);
    if (it == s_icons.end())
    {
        return false;
    }
    
    // Default to icon's native size if not specified
    if (size.x <= 0) size.x = (float)it->second.width;
    if (size.y <= 0) size.y = (float)it->second.height;
    
    // ImGui::Image signature for newer ImGui versions: (ImTextureRef tex_ref, const ImVec2& image_size, const ImVec2& uv0, const ImVec2& uv1)
    ImGui::Image((ImTextureRef)(void*)it->second.srv, size, ImVec2(0, 0), ImVec2(1, 1));
    return true;
}

bool IconManager::DrawIcon(const char* name, ImVec2 size, const ImVec2& uv0, const ImVec2& uv1)
{
    auto it = s_icons.find(name);
    if (it == s_icons.end())
    {
        return false;
    }
    
    // Default to icon's native size if not specified
    if (size.x <= 0) size.x = (float)it->second.width;
    if (size.y <= 0) size.y = (float)it->second.height;
    
    ImGui::Image((ImTextureRef)(void*)it->second.srv, size, uv0, uv1);
    return true;
}

bool IconManager::GetIconSize(const char* name, int* outWidth, int* outHeight)
{
    auto it = s_icons.find(name);
    if (it == s_icons.end())
    {
        return false;
    }
    
    if (outWidth) *outWidth = it->second.width;
    if (outHeight) *outHeight = it->second.height;
    return true;
}

ImTextureID IconManager::GetIconTextureID(const char* name)
{
    auto it = s_icons.find(name);
    if (it == s_icons.end())
    {
        return (ImTextureID)nullptr;
    }
    return (ImTextureID)it->second.srv;
}

ID3D11ShaderResourceView* IconManager::GetIconSRV(const char* name)
{
    auto it = s_icons.find(name);
    if (it == s_icons.end())
    {
        return nullptr;
    }
    return it->second.srv;
}

ID3D11ShaderResourceView* IconManager::CreateTextureFromRGBA(ID3D11Device* device, const unsigned char* data, int width, int height)
{
    if (!device || !data || width <= 0 || height <= 0)
    {
        return nullptr;
    }
    
    // Create the texture
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    
    D3D11_SUBRESOURCE_DATA subResource = {};
    subResource.pSysMem = data;
    subResource.SysMemPitch = width * 4;  // 4 bytes per pixel (RGBA)
    subResource.SysMemSlicePitch = 0;
    
    ID3D11Texture2D* texture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &texture);
    if (FAILED(hr) || !texture)
    {
        printf("[IconManager] Failed to create texture: 0x%08X\n", hr);
        return nullptr;
    }
    
    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    
    ID3D11ShaderResourceView* srv = nullptr;
    hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);
    
    // We can release the texture after creating the SRV
    // The SRV holds a reference to the texture
    texture->Release();
    
    if (FAILED(hr) || !srv)
    {
        printf("[IconManager] Failed to create SRV: 0x%08X\n", hr);
        return nullptr;
    }
    
    return srv;
}

bool IconManager::CreateIconTexture(const char* name, const unsigned char* data, int width, int height)
{
    if (!s_device)
    {
        printf("[IconManager] No device set, call Initialize() first\n");
        return false;
    }
    
    ID3D11ShaderResourceView* srv = CreateTextureFromRGBA(s_device, data, width, height);
    if (!srv)
    {
        return false;
    }
    
    IconTexture icon;
    icon.srv = srv;
    icon.width = width;
    icon.height = height;
    
    s_icons[name] = icon;
    return true;
}

void IconManager::LoadEmbeddedIcons(ID3D11Device* device)
{
    // This function requires embedded_icons.h to be included
    // Uncomment the implementation below after generating the header
    
    
    printf("[IconManager] Loading %zu embedded icons...\n", EmbeddedIcons::IconCount);
    
    for (size_t i = 0; i < EmbeddedIcons::IconCount; ++i)
    {
        const auto& icon = EmbeddedIcons::AllIcons[i];
        if (CreateIconTexture(icon.name, icon.data, icon.width, icon.height))
        {
            printf("[IconManager] Loaded icon: %s (%dx%d)\n", icon.name, icon.width, icon.height);
        }
        else
        {
            printf("[IconManager] Failed to load icon: %s\n", icon.name);
        }
    }
    
    
    printf("[IconManager] LoadEmbeddedIcons() called - uncomment implementation after generating embedded_icons.h\n");
}
