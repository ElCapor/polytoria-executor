#ifndef DX11HOOK_H
#define DX11HOOK_H

/**
 * @file dx11hook.hpp
 * @author ElCapor (infosec@yu-tech.cloud)
 * @brief Direct X 11 Hooking Library
 * @version 0.1
 * @date 2026-02-12
 * 
 * CC-BY-SA 4.0 Copyright (c) 2026 @ElCapor
 * 
 */
#include <d3d11.h>
#include <iostream>
#include <vector>

namespace dx11 {
    using Present_t = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
    using ImGuiDraw = void(*)();

    IDXGISwapChain *CreateSwapChain();
    void HookSwapChain(IDXGISwapChain *dummySwapChain, Present_t hookedFunction);
    IDXGISwapChain * GetSwapChain(IDXGISwapChain * swp);
    ID3D11Device * GetDevice();
    ID3D11DeviceContext * GetContext();
    ID3D11RenderTargetView * GetRenderTarget();
}


#endif /* DX11HOOK_H */
