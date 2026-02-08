#include "renderer.h"

HWND rbx::c_renderer::window = nullptr;
ID3D11Device* rbx::c_renderer::device = nullptr;
ID3D11DeviceContext* rbx::c_renderer::device_context = nullptr;
IDXGISwapChain* rbx::c_renderer::swap_chain = nullptr;
ID3D11RenderTargetView* rbx::c_renderer::render_target_view = nullptr;

rbx::c_renderer::present_fn rbx::c_renderer::original_present = nullptr;
rbx::c_renderer::resize_buffers_fn rbx::c_renderer::original_resize_buffers = nullptr;
WNDPROC rbx::c_renderer::original_wnd_proc = nullptr;

bool rbx::c_renderer::is_init = false;
bool rbx::c_renderer::show = true;

static bool get_swapchain_vtable(rbx::c_renderer::present_fn& out_present, rbx::c_renderer::resize_buffers_fn& out_resize) {
    const wchar_t* klass = L"MiisploitDummyWindow";
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, DefWindowProcW, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, klass, NULL };
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowW(klass, L"", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        UnregisterClassW(klass, wc.hInstance);
        return false;
    }

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11Device* tmp_device = nullptr;
    ID3D11DeviceContext* tmp_context = nullptr;
    IDXGISwapChain* tmp_swapchain = nullptr;
    D3D_FEATURE_LEVEL feature_level;
    const D3D_FEATURE_LEVEL levels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0,
        levels, 2, D3D11_SDK_VERSION, &sd, &tmp_swapchain, &tmp_device, &feature_level, &tmp_context
    );

    if (FAILED(hr) || !tmp_swapchain) {
        if (tmp_swapchain) tmp_swapchain->Release();
        if (tmp_context) tmp_context->Release();
        if (tmp_device) tmp_device->Release();
        DestroyWindow(hwnd);
        UnregisterClassW(klass, wc.hInstance);
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(tmp_swapchain);
    out_present = reinterpret_cast<rbx::c_renderer::present_fn>(vtable[8]);
    out_resize = reinterpret_cast<rbx::c_renderer::resize_buffers_fn>(vtable[13]);

    tmp_swapchain->Release();
    tmp_context->Release();
    tmp_device->Release();
    DestroyWindow(hwnd);
    UnregisterClassW(klass, wc.hInstance);
    return true;
}

bool rbx::c_renderer::init_hooks() {
    present_fn target_present = nullptr;
    resize_buffers_fn target_resize = nullptr;
    if (!get_swapchain_vtable(target_present, target_resize))
        return false;

    auto mh_init = MH_Initialize();
    if (mh_init != MH_OK && mh_init != MH_ERROR_ALREADY_INITIALIZED)
        return false;

    if (MH_CreateHook(reinterpret_cast<LPVOID>(target_present), &present_h, reinterpret_cast<LPVOID*>(&original_present)) != MH_OK)
        return false;

    if (MH_CreateHook(reinterpret_cast<LPVOID>(target_resize), &resize_buffers_h, reinterpret_cast<LPVOID*>(&original_resize_buffers)) != MH_OK)
        return false;

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
        return false;

    return true;
}

bool rbx::c_renderer::init_imgui(IDXGISwapChain* swapchain) {
    if (!swapchain) return false;
    swap_chain = swapchain;
    swap_chain->AddRef();

    if (FAILED(swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device))))
        return false;

    device->GetImmediateContext(&device_context);

    DXGI_SWAP_CHAIN_DESC desc = {};
    if (FAILED(swapchain->GetDesc(&desc)))
        return false;

    window = desc.OutputWindow;
    if (!window)
        return false;

    create_render_target();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, device_context);

    c_gui::editor.SetLanguageDefinition(c_gui::editor.Lua());
    c_gui::editor.SetText("-- Miisploit");

    original_wnd_proc = (WNDPROC)SetWindowLongPtrW(window, GWLP_WNDPROC, (LONG_PTR)wnd_proc_h);
    return true;
}

void rbx::c_renderer::create_render_target() {
    if (!swap_chain || !device)
        return;

    ID3D11Texture2D* back_buffer = nullptr;
    if (SUCCEEDED(swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)))) {
        device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
        back_buffer->Release();
    }
}

void rbx::c_renderer::cleanup_render_target() {
    if (render_target_view) {
        render_target_view->Release();
        render_target_view = nullptr;
    }
}

HRESULT WINAPI rbx::c_renderer::present_h(IDXGISwapChain* swapchain, UINT sync_intervals, UINT flags) {
    if (!is_init) {
        if (init_imgui(swapchain))
            is_init = true;
    }

    if (is_init && show) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        c_gui::draw();

        ImGui::Render();
        device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return original_present(swapchain, sync_intervals, flags);
}

HRESULT WINAPI rbx::c_renderer::resize_buffers_h(IDXGISwapChain* swapchain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags) {
    cleanup_render_target();
    const HRESULT hr = original_resize_buffers(swapchain, buffer_count, width, height, new_format, flags);
    create_render_target();
    return hr;
}

LRESULT CALLBACK rbx::c_renderer::wnd_proc_h(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (msg == WM_KEYDOWN && wparam == VK_INSERT)
        show = !show;

    if (show && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wparam, lparam))
        return true;

    return CallWindowProc(original_wnd_proc, hWnd, msg, wparam, lparam);
}

void rbx::c_renderer::set_execute_callback(execute_cb_t cb) {
    c_gui::set_execute_callback(cb);
}

void rbx::c_renderer::render() {
    init_hooks();
}
