#include <ui/ui.h>
#include <hooking/dx11hook.h>
#include <hooking/hookmanager.h>
#include <unity/unity.h>


// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
UI::UiState UI::state = UI::NotReady;

int UI::keybind = VK_DELETE;

bool UI::blockGameInput = true; // Default: block game input when UI is open

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

// Track previous cursor state to restore when UI closes
static bool s_WasCursorVisible = true;
static UnityResolve::UnityType::CursorLockMode s_PreviousLockState = UnityResolve::UnityType::CursorLockMode::None;
static bool s_CursorStateSaved = false;

// Save and restore cursor state
static void SaveCursorState()
{
    if (!s_CursorStateSaved)
    {
        s_WasCursorVisible = UnityResolve::UnityType::Cursor::get_visible();
        s_PreviousLockState = UnityResolve::UnityType::Cursor::get_lockState();
        s_CursorStateSaved = true;
    }
}

static void RestoreCursorState()
{
    if (s_CursorStateSaved)
    {
        UnityResolve::UnityType::Cursor::set_visible(s_WasCursorVisible);
        UnityResolve::UnityType::Cursor::set_lockState(s_PreviousLockState);
        s_CursorStateSaved = false;
    }
}

static void ForceShowCursor()
{
    UnityResolve::UnityType::Cursor::ShowAndUnlock();
}

LRESULT __stdcall WndProcHook(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Handle UI toggle keybind
    if (uMsg == WM_KEYDOWN && wParam == UI::keybind) {
		if (UI::state == UI::Ready) {
            // Closing UI - restore cursor state
            RestoreCursorState();
            UI::state = UI::Closed;
        } else if (UI::state == UI::Closed) 
        {
            // Opening UI - save state and force show cursor
            SaveCursorState();
            ForceShowCursor();
            UI::state = UI::Ready;
        }
	}

    // Let ImGui process the message first
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

    // Block all input when UI is open and input blocking is enabled
	if (UI::state == UI::Ready && UI::blockGameInput) {
		// Block standard mouse input (WM_MOUSEMOVE, WM_LBUTTONDOWN, etc.)
		if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
			return true;

        // Block mouse scroll wheel
		if (uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEHWHEEL)
			return true;

        // Block Raw Input (WM_INPUT) - Unity uses this for mouse/keyboard
        // This is the key fix for scroll wheel still reaching the game
		if (uMsg == WM_INPUT)
			return true;

		// Block keyboard input
		if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
			return true;
        
        // Block mouse activate (prevents game from regaining focus)
        if (uMsg == WM_MOUSEACTIVATE)
            return true;
            
        // Block capture changes
        if (uMsg == WM_CAPTURECHANGED)
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
#include <ui/iconmanager.h>
#include <ptoria/drawinglib.h>

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

        // Initialize icon manager and load embedded icons
        IconManager::Initialize(dev);
        IconManager::LoadEmbeddedIcons(dev);

        ImGui_ImplWin32_Init(wnd);
        ImGui_ImplDX11_Init(dev, ctx);
        init = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    UI::Draw();
    
    // Render drawing library commands (thread-safe, drawn on foreground)
    DrawingLib::RenderDrawCommands();
    
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
#include <ui/about.h>
#include <chrono>

// Waiting screen fade variables
static bool waitingScreenShown = false;
static bool waitingScreenHidden = false;
static std::chrono::steady_clock::time_point waitingScreenShowTime;

// Closed hint fade variables
static bool closedHintShown = false;
static bool closedHintHidden = false;
static std::chrono::steady_clock::time_point closedHintShowTime;

void UI::DrawWaitingScreen()
{
    // If already hidden, don't show again
    if (waitingScreenHidden)
        return;
    
    // Initialize show time on first display
    if (!waitingScreenShown)
    {
        waitingScreenShown = true;
        waitingScreenShowTime = std::chrono::steady_clock::now();
    }
    
    // Calculate fade (5 seconds = 5000ms)
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - waitingScreenShowTime).count();
    const float fadeDuration = 5000.0f;
    
    // Hide completely after fade
    if (elapsed >= fadeDuration)
    {
        waitingScreenHidden = true;
        return;
    }
    
    // Calculate alpha (1.0 -> 0.0 over fade duration)
    float alpha = 1.0f - (elapsed / fadeDuration);
    
    const ImVec2 windowSize = ImVec2(280, 80);
    const float padding = 20.0f;
    
    // Position at bottom-left of screen
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(padding, ImGui::GetIO().DisplaySize.y - windowSize.y - padding), ImGuiCond_Always);
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    }
    else
    {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    }
    
    ImGui::Begin("##waiting", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);
    
    const char* title = "PolyHack";
    const char* subtitle = "Waiting for Unity...";
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PushFont(PremiumStyle::FontBold);
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::TextColored(ImVec4(0.00f, 0.75f, 0.85f, 1.00f), "%s", title);
    else
        ImGui::Text("%s", title);
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PopFont();
    
    ImGui::TextDisabled("%s", subtitle);
    
    ImGui::End();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleVar(3);
    }
    else
    {
        ImGui::PopStyleVar();
    }
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
        if (ImGui::BeginTabItem("About"))
        {
            AboutUI::DrawTab();
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
    // If already hidden, don't show again
    if (closedHintHidden)
        return;
    
    // Initialize show time on first display
    if (!closedHintShown)
    {
        closedHintShown = true;
        closedHintShowTime = std::chrono::steady_clock::now();
    }
    
    // Calculate fade (5 seconds = 5000ms)
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - closedHintShowTime).count();
    const float fadeDuration = 5000.0f;
    
    // Hide completely after fade
    if (elapsed >= fadeDuration)
    {
        closedHintHidden = true;
        return;
    }
    
    // Calculate alpha (1.0 -> 0.0 over fade duration)
    float alpha = 1.0f - (elapsed / fadeDuration);
    
    const ImVec2 windowSize = ImVec2(280, 80);
    const float padding = 20.0f;
    
    // Position at bottom-left of screen
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(padding, ImGui::GetIO().DisplaySize.y - windowSize.y - padding), ImGuiCond_Always);
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    }
    else
    {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
    }
    
    ImGui::Begin("##closed", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);
    
    const char* title = "UI Hidden";
    const char* hint = "Press DELETE to toggle";
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PushFont(PremiumStyle::FontBold);
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.20f, 1.00f), "%s", title);
    else
        ImGui::Text("%s", title);
    
    if (PremiumStyle::IsPremiumEnabled && PremiumStyle::FontBold)
        ImGui::PopFont();
    
    ImGui::TextDisabled("%s", hint);
    
    ImGui::End();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleVar(3);
    }
    else
    {
        ImGui::PopStyleVar();
    }
}