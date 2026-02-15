#include <ui/scriptsource.h>
#include <ui/premiumstyle.h>
#include <imgui.h>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
std::vector<ScriptSourceUI::ScriptDecompileTab> ScriptSourceUI::openTabs;

// +--------------------------------------------------------+
// |                     User Interface                     |
// +--------------------------------------------------------+
#include <ptoria/scriptservice.h>
#include <core/filesys.h>

void ScriptSourceUI::DrawTab(ScriptDecompileTab *tab)
{
    if (!tab->isEditorReady)
    {
        tab->editor = new TextEditor();
        std::string source = ScriptService::ScriptSource(tab->instance);
        tab->editor->SetText(source);
        tab->editor->SetLanguageDefinition(TextEditor::LanguageDefinitionId::Lua);
        tab->editor->SetPalette(TextEditor::PaletteId::Dark);
        tab->isEditorReady = true;
    }
    
    // Script info header with premium styling
    if (tab->instance)
    {
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.06f, 0.06f, 0.08f, 1.00f));
        }
        
        ImGui::BeginChild("ScriptInfo", ImVec2(0, 0), true);
        
        // Script name with accent color
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
            if (PremiumStyle::FontBold)
                ImGui::PushFont(PremiumStyle::FontBold);
        }
        ImGui::Text("%s", tab->instance->Name()->ToString().c_str());
        if (PremiumStyle::IsPremiumEnabled)
        {
            if (PremiumStyle::FontBold)
                ImGui::PopFont();
            ImGui::PopStyleColor();
        }
        
        ImGui::SameLine();
        ImGui::TextDisabled("(Decompiled Script)");
        
        ImGui::Spacing();
        
        // Status indicators
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16, 4));
        
        // Running status
        bool isRunning = tab->instance->Running();
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Running:");
        else
            ImGui::Text("Running:");
        ImGui::SameLine();
        if (isRunning)
        {
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.20f, 0.85f, 0.40f, 1.00f), "Yes");
            else
                ImGui::Text("Yes");
        }
        else
        {
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.85f, 0.40f, 0.40f, 1.00f), "No");
            else
                ImGui::Text("No");
        }
        
        // Requested Run status
        ImGui::SameLine();
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Requested Run:");
        else
            ImGui::Text("Requested Run:");
        ImGui::SameLine();
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::TextColored(tab->instance->RequestedRun() ? ImVec4(0.20f, 0.85f, 0.40f, 1.00f) : ImVec4(0.85f, 0.40f, 0.40f, 1.00f), 
                tab->instance->RequestedRun() ? "Yes" : "No");
        }
        else
        {
            ImGui::Text(tab->instance->RequestedRun() ? "Yes" : "No");
        }
        
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::PopStyleVar();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Toolbar
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
        
        // File operations
        if (ImGui::Button("Open..."))
        {
            auto filters = std::vector<filesys::FileSelectFilters>{
                {"Lua Scripts", "lua"},
                {"Text Files", "txt"},
            };
            auto result = filesys::OpenDialog(filters);
            if (result)
            {
                std::string scriptContent = filesys::ReadFileAsString(*result).value_or("");
                tab->editor->SetText(scriptContent);
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Save..."))
        {
            auto filters = std::vector<filesys::FileSelectFilters>{
                {"Lua Scripts", "lua"},
                {"Text Files", "txt"},
            };
            auto result = filesys::SaveDialog(filters);
            if (result)
            {
                std::string scriptContent = tab->editor->GetText();
                filesys::WriteStringToFile(*result, scriptContent);
            }
        }
        
        ImGui::SameLine();
        
        // Warning indicator
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.78f, 0.20f, 1.00f));
            ImGui::Text("[!] Experimental");
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::Text("[!] Experimental");
        }
        
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Can't save script to game yet - feature in development");
        }
        
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::PopStyleVar();
        
        ImGui::Spacing();
        
        // Save to game button with warning styling
        if (PremiumStyle::StyledButton("Save Changes To Game", false, ImVec2(160, 0)))
        {
            if (tab->instance)
            {
                if (!tab->isEditorReady)
                {
                    std::cout << "Editor not ready, cannot save changes" << std::endl;
                }
                else
                {
                    if (tab->instance->Running())
                    {
                        tab->instance->SetRunning(false);
                    }

                    std::string updatedSource = tab->editor->GetText();
                    tab->instance->SetSource(UnityString::New(updatedSource));
                }
            }
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Editor with premium styling
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.04f, 0.04f, 0.06f, 1.00f));
        }
        
        tab->editor->Render("##ScriptEditor", false, ImVec2(0, 0), true);
        
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
        
        ImGui::EndChild();
        
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
    }
}

bool ScriptSourceUI::IsTabAlreadyOpen(BaseScript *instance)
{
    for (const auto &tab : openTabs)
    {
        if (tab.instance == instance)
            return true;
    }
    return false;
}

void ScriptSourceUI::OpenNewScriptDecompileTab(BaseScript *instance)
{
    openTabs.push_back({instance});
}
