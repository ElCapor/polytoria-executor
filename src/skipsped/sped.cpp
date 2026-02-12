#include <skipsped/sped.hpp>
#include <Windows.h>
#include <iostream>
#include <skipsped/sped.hpp>
#include <dxhook/dx11hook.hpp>
#include <HookManager.hpp>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#define WINDOWS_MODE 1
#include <unity/u.hpp>
#include <chrono>
#include <polytoria/polytoria.hpp>
#include <ShlObj_core.h>
#include <nfd.h>
#include <skipsped/net_tracer.hpp>
#include <polytoria/networking/mirror_hook.hpp>

using namespace sped;

void Sped::Init()
{
    LoadConsole();
    LoadUnityAPI();
    LoadImGui();

    polytoria::ScriptService::InstallHooks();
    mirror::InstallHooks();

    polytoria::Game *game = polytoria::Game::GetInstance();
    if (game)
    {
        std::cout << "Successfully accessed Polytoria Game instance" << std::endl;
        std::cout << "Game Name: " << game->GetName()->ToString() << std::endl;
        for (auto child : game->GetChildren()->ToVector())
        {
            std::cout << "Child Name: " << child->GetName()->ToString() << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to access Polytoria Game instance" << std::endl;
    }
}

bool Sped::Render(IDXGISwapChain *swap, UINT swapInterval, UINT flags)
{

    if (state == SpedState::X)
    {
        return false;
    }
    else if (state == SpedState::Ready)
    {
        ImGui::Begin("SkipSped @ElCapor", nullptr, ImGuiWindowFlags_MenuBar);
        ImGui::Text("#1 Open Source Polytoria Mod Menu");
        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Explorer"))
            {
                RenderExplorerTab();

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Executor"))
            {
                RenderExecutorTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Save Instance"))
            {
                RenderSaveInstanceTab();

                ImGui::EndTabItem();
            }

            // Closable Decompiler Tabs
            for (auto it = mDecompileTabs.begin(); it != mDecompileTabs.end();)
            {
                bool isOpen = true;
                if (ImGui::BeginTabItem(("Decompile: " + it->instance->GetName()->ToString()).c_str(), &isOpen))
                {
                    RenderDecompileTab(&(*it));
                    ImGui::EndTabItem();
                }

                if (!isOpen)
                    it = mDecompileTabs.erase(it);
                else
                    ++it;
            }

            if (ImGui::BeginTabItem("Net Message Logger"))
            {
                RenderNetMessageLoggerTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Settings"))
            {
                ImGui::Text("Settings go here");

                ImGui::Separator();
                ImGui::Text("Made by ElCapor");
                ImGui::Text("GitHub: github.com/ElCapor");
                ImGui::Text("Discord: dispatchcallback");
                ImGui::Separator();

                if (ImGui::Button("Close Console"))
                {
                    FreeConsole();
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    return true;
}

void Sped::RenderDecompileTab(ScriptDecompileTab *tab)
{
    if (!tab->isEditorReady)
    {
        std::string source = polytoria::ScriptService::DecompileScript(tab->instance);
        tab->editor.SetText(source);
        tab->editor.SetLanguageDefinition(TextEditor::LanguageDefinitionId::Lua);
        tab->isEditorReady = true;
    }
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Script..."))
            {
                auto filters = std::vector<FileSelectFilters>{
                    {"Lua Scripts", "lua"},
                    {"Text Files", "txt"},
                };
                auto result = OpenDialog(filters);
                if (result)
                {
                    std::string scriptContent = ReadFileAsString(*result).value_or("");
                    tab->editor.SetText(scriptContent);
                }
            }
            if (ImGui::MenuItem("Save Script..."))
            {
                auto filters = std::vector<FileSelectFilters>{
                    {"Lua Scripts", "lua"},
                    {"Text Files", "txt"},
                };
                auto result = SaveDialog(filters);
                if (result)
                {
                    std::string scriptContent = tab->editor.GetText();
                    WriteStringToFile(*result, scriptContent);
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (tab->instance)
    {
        ImGui::Text(("Script Name: " + tab->instance->GetName()->ToString()).c_str());
        ImGui::Text(("Running: " + std::string(tab->instance->IsRunning() ? "Yes" : "No")).c_str());
        ImGui::Text("Requested Run: %s", tab->instance->GetRequestedRun() ? "Yes" : "No");
    }
    ImGui::Text("[WARNING] Can't save script to game yet idk why it doesnt work");
    if (ImGui::Button("Save Changes To Game"))
    {
        if (tab->instance)
        {
            if (!tab->isEditorReady)
            {
                std::cout << "Editor not ready, cannot save changes" << std::endl;
                return;
            }
            if (tab->instance->IsRunning())
            {
                tab->instance->SetRunning(false);
            }

            std::string updatedSource = tab->editor.GetText();
            tab->instance->SetSource(US::New(updatedSource));
            //polytoria::ScriptService::RunScript(tab->instance);

        }
    }
    ImGui::Separator();
    tab->editor.Render(("Decompiled Script: " + tab->instance->GetName()->ToString()).c_str());
}

void Sped::OpenNewScriptDecompileTab(polytoria::BaseScript *instance) { mDecompileTabs.push_back({instance}); }

void Sped::RenderExecutorTab()
{
    if (!isEditorSetup)
    {
        mScriptEditor = new TextEditor();
        mScriptEditor->SetLanguageDefinition(TextEditor::LanguageDefinitionId::Lua);
        isEditorSetup = true;
    }
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Script..."))
            {
                auto filters = std::vector<FileSelectFilters>{
                    {"Lua Scripts", "lua"},
                    {"Text Files", "txt"},
                };
                auto result = OpenDialog(filters);
                if (result)
                {
                    std::string scriptContent = ReadFileAsString(*result).value_or("");
                    mScriptEditor->SetText(scriptContent);
                }
            }
            if (ImGui::MenuItem("Save Script..."))
            {
                auto filters = std::vector<FileSelectFilters>{
                    {"Lua Scripts", "lua"},
                    {"Text Files", "txt"},
                };
                auto result = SaveDialog(filters);
                if (result)
                {
                    std::string scriptContent = mScriptEditor->GetText();
                    WriteStringToFile(*result, scriptContent);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Run"))
        {
            if (ImGui::MenuItem("Execute"))
            {
                std::string script = mScriptEditor->GetText();
                if (!script.empty())
                {
                    polytoria::ScriptService::RunScript(script);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    mScriptEditor->Render("Lua Script Executor");
}

std::optional<std::string> Sped::SaveDialog(std::vector<FileSelectFilters> filters)
{
    std::string savePath;
    NFD_Init();

    std::vector<nfdu8filteritem_t> nfdFilters;
    for (const auto &filter : filters)
    {
        nfdFilters.push_back({filter.description.c_str(), filter.extensions.c_str()});
    }

    nfdu8char_t *outPath;
    nfdsavedialogu8args_t args = {0};
    args.filterList = nfdFilters.data();
    args.filterCount = static_cast<nfdfiltersize_t>(nfdFilters.size());
    nfdresult_t result = NFD_SaveDialogU8_With(&outPath, &args);

    if (result == NFD_OKAY)
    {
        savePath = outPath;
        NFD_FreePathU8(outPath);
        NFD_Quit();
        return savePath;
    }
    else if (result == NFD_CANCEL)
    {
        NFD_Quit();
        return std::nullopt;
    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
        NFD_Quit();
        return std::nullopt;
    }
}

std::optional<std::string> Sped::OpenDialog(std::vector<FileSelectFilters> filters)
{
    std::string openPath;
    NFD_Init();

    std::vector<nfdu8filteritem_t> nfdFilters;
    for (const auto &filter : filters)
    {
        nfdFilters.push_back({filter.description.c_str(), filter.extensions.c_str()});
    }

    nfdu8char_t *outPath;
    nfdopendialogu8args_t args = {0};
    args.filterList = nfdFilters.data();
    args.filterCount = static_cast<nfdfiltersize_t>(nfdFilters.size());
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);

    if (result == NFD_OKAY)
    {
        openPath = outPath;
        NFD_FreePathU8(outPath);
        NFD_Quit();
        return openPath;
    }
    else if (result == NFD_CANCEL)
    {
        NFD_Quit();
        return std::nullopt;
    }
    else
    {
        printf("Error: %s\n", NFD_GetError());
        NFD_Quit();
        return std::nullopt;
    }
}

std::optional<std::string> Sped::ReadFileAsString(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Sped::WriteStringToFile(const std::string &filePath, const std::string &content)
{
    std::ofstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filePath << std::endl;
        return;
    }

    file << content;
    if (file.fail())
    {
        std::cerr << "Failed to write to file: " << filePath << std::endl;
    }
}

void Sped::RenderSaveInstanceTab()
{
    ImGui::Text("Save Instance");
    // if (ImGui::Button("Save to File"))
    // {
    //     std::cout << "[INFO] Attempting to save instance..." << std::endl;
    //     char desktopPath[MAX_PATH];
    //     if (SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath) == S_OK)
    //     {
    //         std::cout << "[INFO] Desktop path: " << desktopPath << std::endl;
    //     }
    //     else
    //     {
    //         std::cerr << "[ERROR] Failed to get desktop path!" << std::endl;
    //     }

    //     auto game_name = polytoria::Game::GetGameName();
    //     if (game_name)
    //     {
    //         std::cout << "[INFO] Game name: " << game_name->ToString() << std::endl;
    //     }
    //     else
    //     {
    //         std::cerr << "[ERROR] Failed to get game name!" << std::endl;
    //     }

    //     auto game_id = polytoria::Game::GetGameID();
    //     if (game_id)
    //     {
    //         std::cout << "[INFO] Game ID: " << game_id << std::endl;
    //     }
    //     else
    //     {
    //         std::cerr << "[ERROR] Failed to get game ID!" << std::endl;
    //         std::cerr << "[ERROR] Are you in solo mode ???" << std::endl;
    //         return;
    //     }

    //     std::string path = std::string(desktopPath) + "\\" + game_name->ToString() + ".poly";
    //     std::cout << "[INFO] Save path: " << path << std::endl;
    //     polytoria::GameIO::SaveInstance(path.c_str());
    // }

    if (ImGui::Button("Save Instance"))
    {
        std::vector<FileSelectFilters> filters = {
            {"Polytoria Instance Files", "poly"},
        };
        auto savePathOpt = SaveDialog(filters);
        if (savePathOpt)
        {
            std::string savePath = *savePathOpt;
            std::cout << "[INFO] Saving instance to: " << savePath << std::endl;
            polytoria::GameIO::SaveInstance(savePath.c_str());
        }
        else
        {
            std::cout << "[INFO] Save cancelled by user." << std::endl;
        }
    }
}

/**
 * @brief Render the instance tree recursively
 *
 * @param instance
 * @param sped
 */
void RenderInstanceTree(polytoria::Instance *instance, Sped &sped)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (sped.selectedInstance == instance)
        flags |= ImGuiTreeNodeFlags_Selected;

    auto children = instance->GetChildren();
    bool hasChildren = children && children->max_length > 0;

    if (!hasChildren)
        flags |= ImGuiTreeNodeFlags_Leaf;

    bool opened = ImGui::TreeNodeEx(instance, flags, "%s", instance->GetName()->ToString().c_str());

    if (ImGui::IsItemClicked())
    {
        sped.selectedInstance = instance;
    }

    if (opened)
    {
        if (hasChildren)
        {
            for (auto child : children->ToVector())
            {
                RenderInstanceTree(child, sped);
            }
        }
        ImGui::TreePop();
    }
}

bool Sped::IsTabAlreadyOpen(polytoria::BaseScript *instance)
{
    for (const auto &tab : mDecompileTabs)
    {
        if (tab.instance == instance)
            return true;
    }
    return false;
}

void Sped::RenderExplorerTab()
{
    polytoria::Game *game = polytoria::Game::GetInstance();
    if (game)
    {
        float availableHeight = ImGui::GetContentRegionAvail().y;

        // Top part: Tree Explorer
        ImGui::BeginChild("TreeRegion", ImVec2(0, availableHeight * 0.6f), true);
        RenderInstanceTree(game, *this);
        ImGui::EndChild();

        ImGui::Separator();

        // Bottom part: Properties Explorer
        ImGui::BeginChild("PropertiesRegion", ImVec2(0, 0), true);
        if (selectedInstance)
        {
            ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "Properties: %s", selectedInstance->GetName()->ToString().c_str());
            ImGui::Separator();

            if (ImGui::BeginTable("PropertiesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                // Name
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Name");
                ImGui::TableSetColumnIndex(1);
                std::string nameStr = selectedInstance->GetName()->ToString();
                if (ImGui::Selectable(nameStr.c_str()))
                    ImGui::SetClipboardText(nameStr.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", nameStr.c_str());

                // Type
                auto type = selectedInstance->GetType();
                if (type)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Class");
                    ImGui::TableSetColumnIndex(1);
                    std::string classStr = type->GetFullNameOrDefault()->ToString();
                    if (ImGui::Selectable(classStr.c_str()))
                        ImGui::SetClipboardText(classStr.c_str());
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Click to copy: %s", classStr.c_str());

                    if (type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.LocalScript" || type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.Script" || type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.ModuleScript" || type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.ScriptInstance")
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Script Source");
                        ImGui::TableSetColumnIndex(1);
                        if (ImGui::Button("View Source"))
                        {
                            if (!IsTabAlreadyOpen(reinterpret_cast<polytoria::BaseScript *>(selectedInstance)))
                            {
                                polytoria::BaseScript *scriptInstance = reinterpret_cast<polytoria::BaseScript *>(selectedInstance);
                                OpenNewScriptDecompileTab(scriptInstance);
                            }
                        }
                    }
                }

                // Memory Address
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Address");
                ImGui::TableSetColumnIndex(1);
                char addrBuf[32];
                snprintf(addrBuf, sizeof(addrBuf), "0x%p", selectedInstance);
                if (ImGui::Selectable(addrBuf))
                    ImGui::SetClipboardText(addrBuf);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", addrBuf);

                // Full Name
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Full Name");
                ImGui::TableSetColumnIndex(1);
                std::string fullNameStr = selectedInstance->GetFullName()->ToString();
                if (ImGui::Selectable(fullNameStr.c_str()))
                    ImGui::SetClipboardText(fullNameStr.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", fullNameStr.c_str());

                ImGui::EndTable();
            }
        }
        else
        {
            ImGui::TextDisabled("Select an instance to see its properties");
        }
        ImGui::EndChild();
    }
    else
    {
        ImGui::Text("Failed to access Polytoria Game instance");
    }
}

void Sped::LoadUnityAPI()
{
    // Code to load Unity API here
    HMODULE gameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (gameAssembly == nullptr)
    {
        std::cout << "Failed to load GameAssembly.dll" << std::endl;
        return;
    }
    std::cout << "Successfully loaded GameAssembly.dll" << std::endl;

    UnityResolve::Init(gameAssembly, UnityResolve::Mode::Il2Cpp);
    UnityResolve::ThreadAttach();

    for (UF* field: polytoria::Script::GetClass()->fields)
    {
        std::cout << "Field: " << field->name << " | Type: " << field->type->name << std::endl; 
    }
    state = SpedState::Ready;
}

void Sped::LoadImGui()
{
    IDXGISwapChain *dummySwapChain = dx11::CreateSwapChain();
    dx11::HookSwapChain(dummySwapChain, HookedPresent);
}

void Sped::LoadConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
}

HRESULT Sped::HookedPresent(IDXGISwapChain *swap, UINT swapInterval, UINT flags)
{
    static bool init = false;
    if (!init)
    {
        dx11::GetSwapChain(swap);
        ID3D11Device *dev = dx11::GetDevice();
        ID3D11DeviceContext *ctx = dx11::GetContext();
        DXGI_SWAP_CHAIN_DESC sd;
        swap->GetDesc(&sd);
        HWND wnd = sd.OutputWindow;
        dx11::GetRenderTarget();

        UnityResolve::ThreadAttach();

        dx11::oWndProc = (WNDPROC)SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR)dx11::WndProc);

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGuiStyle &style = ImGui::GetStyle();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        style.FrameRounding = 3.0f;
        style.GrabRounding = 3.0f;

        io.Fonts->AddFontDefault();

        ImGui_ImplWin32_Init(wnd);
        ImGui_ImplDX11_Init(dev, ctx);
        init = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetInstance().Render(swap, swapInterval, flags))
    {
        //
    }

    ImGui::Render();
    ID3D11RenderTargetView *target = dx11::GetRenderTarget();
    dx11::GetContext()->OMSetRenderTargets(1, &target, 0);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return HookManager::Scall(HookedPresent, swap, swapInterval, flags);
}


bool NetTracer::isReady = false;
polytoria::Script* NetTracer::exec;
polytoria::DynValue* NetTracer::callback;

void Sped::RenderNetMessageLoggerTab()
{
    ImGui::Text("Net Message Logger");
    ImGui::Text("[WIP]");

    if (!NetTracer::isReady)
    {
        NetTracer::InstallHooks();
        NetTracer::isReady = true;
    }

    ImGui::Text("Callback : %p", NetTracer::callback);
    ImGui::Text("Exec %p", NetTracer::exec);

    if (ImGui::Button("Try callback"))
    {
        NetTracer::callback->GetFunction()->Call({US::New("Hi")});
    }

    // if (ImGui::Button("wowie"))
    // {
    //     UK* obj = U::Get("mscorlib.dll")->Get("Object", "System");
    //     auto arr = U::UnityType::Array<UO*>::New(obj, 1);
    //     std::cout << arr->GetType()->GetFullNameOrDefault()->ToString() << std::endl;

    //     // returns System.Object[]
    // }

}