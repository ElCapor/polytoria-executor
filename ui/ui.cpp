#include <ui/ui.h>
#include <hooking/dx11hook.h>
#include <hooking/hookmanager.h>
#include <unity/unity.h>


// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
UI::UiState UI::state = UI::NotReady;

int UI::keybind = VK_DELETE;

// +--------------------------------------------------------+
// |                       Functions                        |
// +--------------------------------------------------------+

// Custom present, as the one in dx11hook , is not permissible enough
HRESULT HookPresent(IDXGISwapChain* swap, UINT swapInterval, UINT flags);


// +--------------------------------------------------------+
// |                     User Interface                     |
// +--------------------------------------------------------+

void UI::Setup()
{
    // hook dx11 swap chain
    IDXGISwapChain *dummySwapChain = dx11::CreateSwapChain();
    dx11::HookSwapChain(dummySwapChain, HookPresent);
}

void UI::Draw()
{
    switch (state)
    {
    case NotReady:
        DrawWaitingScreen();
        break;
    case Ready:
        DrawMainUI();
        break;
    case Closed:
        DrawClosedHint();
        break;
    }
}

// +--------------------------------------------------------+
// |                        WndProc                         |
// +--------------------------------------------------------+
WNDPROC oWndProc = nullptr;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProcHook(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && wParam == UI::keybind) {
		if (UI::state == UI::Ready) {
            UI::state = UI::Closed;
        } else if (UI::state == UI::Closed) 
        {
            UI::state = UI::Ready;
        }
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	if (UI::state == UI::Ready) {

		// Block mouse input
		if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
			return true;

        // Need to fix this, it doesn't seem to work well, have to look into unity to block really everything
		if (uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEHWHEEL)
			return true;

		// Block keyboard input
		if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
			return true;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
// +--------------------------------------------------------+
// |                      Direct X 11                       |
// +--------------------------------------------------------+
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <ui/premiumstyle.h>

HRESULT HookPresent(IDXGISwapChain* swap, UINT swapInterval, UINT flags)
{
    static bool init = false;
    if (!init)
    {
        // Attach to unity
        // so that the ui can make unity calls
        Unity::ThreadAttach();

        dx11::GetSwapChain(swap);
        ID3D11Device *dev = dx11::GetDevice();
        ID3D11DeviceContext *ctx = dx11::GetContext();
        DXGI_SWAP_CHAIN_DESC sd;
        swap->GetDesc(&sd);
        HWND wnd = sd.OutputWindow;
        dx11::GetRenderTarget();

        oWndProc = (WNDPROC)SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        // Initialize and apply premium style
        PremiumStyle::Initialize();
        PremiumStyle::LoadFonts();
        PremiumStyle::ApplyStyle();

        ImGui_ImplWin32_Init(wnd);
        ImGui_ImplDX11_Init(dev, ctx);
        init = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI::Draw();
    
    ImGui::Render();
    ID3D11RenderTargetView *target = dx11::GetRenderTarget();
    dx11::GetContext()->OMSetRenderTargets(1, &target, 0);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return HookManager::Scall(HookPresent, swap, swapInterval, flags);
}


// +--------------------------------------------------------+
// |                User Interface (Private)                |
// +--------------------------------------------------------+
#include <ui/explorer.h>
#include <ui/saveinstance.h>
#include <ui/executor.h>
#include <ui/scriptsource.h>
#include <ui/settingsui.h>
#include <ui/packetlog.h>

void UI::DrawWaitingScreen()
{
    const ImVec2 windowSize = ImVec2(320, 120);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) / 2, (ImGui::GetIO().DisplaySize.y - windowSize.y) / 2), ImGuiCond_Always);
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
    }
    
    ImGui::Begin("##waiting", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    
    // Center the text
    const char* title = "PolyHack";
    const char* subtitle = "Waiting for Unity...";
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PushFont(PremiumStyle::FontBold);
    
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    ImGui::SetCursorPosX((windowSize.x - titleSize.x) / 2);
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::TextColored(ImVec4(0.00f, 0.75f, 0.85f, 1.00f), "%s", title);
    else
        ImGui::Text("%s", title);
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PopFont();
    
    ImGui::Spacing();
    
    ImVec2 subtitleSize = ImGui::CalcTextSize(subtitle);
    ImGui::SetCursorPosX((windowSize.x - subtitleSize.x) / 2);
    ImGui::TextDisabled("%s", subtitle);
    
    ImGui::End();
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PopStyleVar(2);
}

void UI::DrawMainUI()
{
    // Set initial window size and position
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    
    // Premium window styling
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    }
    
    ImGui::Begin("PolyHack");
    
    // Header section with branding
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
    
    // Title with accent color
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PushFont(PremiumStyle::FontBold);
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::TextColored(ImVec4(0.00f, 0.75f, 0.85f, 1.00f), "PolyHack");
    else
        ImGui::Text("PolyHack");
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PopFont();
    
    ImGui::SameLine();
    ImGui::TextDisabled("|");
    ImGui::SameLine();
    ImGui::TextDisabled("@ElCapor");
    
    ImGui::PopStyleVar();
    
    // Subtitle
    ImGui::TextDisabled("Open Source Cheat for Polytoria - V3 Rewrite");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Tab bar with premium styling
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    
    if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton))
    {
        if (ImGui::BeginTabItem("Explorer"))
        {
            ExplorerUI::DrawTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Save Instance"))
        {
            SaveInstanceUI::DrawTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Executor"))
        {
            ExecutorUI::DrawTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Packet Log"))
        {
            PacketLogUI::DrawTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Settings"))
        {
            SettingsUI::DrawTab();
            ImGui::EndTabItem();
        }
        for (auto it = ScriptSourceUI::openTabs.begin(); it != ScriptSourceUI::openTabs.end();)
        {
            bool isOpen = true;
            if (ImGui::BeginTabItem(("Decompile: " + it->instance->Name()->ToString()).c_str(), &isOpen))
            {
                ScriptSourceUI::DrawTab(&(*it));
                ImGui::EndTabItem();
            }

            if (!isOpen)
                it = ScriptSourceUI::openTabs.erase(it);
            else
                ++it;
        }
        ImGui::EndTabBar();
    }
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PopStyleVar();
    
    ImGui::End();
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PopStyleVar(2);
}

void UI::DrawClosedHint()
{
    const ImVec2 windowSize = ImVec2(340, 100);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) / 2, (ImGui::GetIO().DisplaySize.y - windowSize.y) / 2), ImGuiCond_Always);
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 16));
    }
    
    ImGui::Begin("##closed", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    
    const char* title = "UI Hidden";
    const char* hint = "Press DELETE to toggle the UI";
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PushFont(PremiumStyle::FontBold);
    
    ImVec2 titleSize = ImGui::CalcTextSize(title);
    ImGui::SetCursorPosX((windowSize.x - titleSize.x) / 2);
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.20f, 1.00f), "%s", title);
    else
        ImGui::Text("%s", title);
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PopFont();
    
    ImGui::Spacing();
    
    ImVec2 hintSize = ImGui::CalcTextSize(hint);
    ImGui::SetCursorPosX((windowSize.x - hintSize.x) / 2);
    ImGui::TextDisabled("%s", hint);
    
    ImGui::End();
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PopStyleVar(2);
}