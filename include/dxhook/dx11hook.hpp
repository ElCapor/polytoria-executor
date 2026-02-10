//
// @project     sdk-explorer
//
// @author      ElCapor
//
// @license     CC BY-NC-SA 4.0
//

#pragma once

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
    HRESULT HookedPresent(IDXGISwapChain *swap, UINT swapInterval, UINT flags);


    LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    extern WNDPROC oWndProc;
    extern std::vector<ImGuiDraw> m_drawList;

}
