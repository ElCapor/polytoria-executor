#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#include "MinHook.h"
#include "gui/gui.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace rbx {
    class c_renderer {
    public:
        using execute_cb_t = c_gui::execute_cb_t;
        using present_fn = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
        using resize_buffers_fn = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

        static void render();
        static void set_execute_callback(execute_cb_t cb);

    private:
        static present_fn original_present;
        static resize_buffers_fn original_resize_buffers;
        static WNDPROC original_wnd_proc;

        static HWND window;
        static ID3D11Device* device;
        static ID3D11DeviceContext* device_context;
        static IDXGISwapChain* swap_chain;
        static ID3D11RenderTargetView* render_target_view;
        static bool is_init;
        static bool show;

        static bool init_hooks();
        static bool init_imgui(IDXGISwapChain* swapchain);
        static void create_render_target();
        static void cleanup_render_target();

        static HRESULT WINAPI present_h(IDXGISwapChain* swapchain, UINT sync_intervals, UINT flags);
        static HRESULT WINAPI resize_buffers_h(IDXGISwapChain* swapchain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags);
        static LRESULT CALLBACK wnd_proc_h(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
    };
}
