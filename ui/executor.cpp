#include <ui/executor.h>
#include <ui/premiumstyle.h>
#include <core/filesys.h>
#include <ptoria/scriptservice.h>
#include <ptoria/scriptinstance.h>
#include <imgui.h>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
TextEditor* ExecutorUI::scriptEditor;


// +--------------------------------------------------------+
// |                     User Interface                     |
// +--------------------------------------------------------+

void ExecutorUI::Init()
{
    static bool initialized = false;
    if (!initialized)
    {
        scriptEditor = new TextEditor();
        scriptEditor->SetLanguageDefinition(TextEditor::LanguageDefinitionId::Lua);
        scriptEditor->SetPalette(TextEditor::PaletteId::Dark);
        initialized = true;
    }
}

void ExecutorUI::DrawTab()
{
    Init();
    
    // Section header with premium styling
    PremiumStyle::DrawSectionHeader("Script Executor");
    ImGui::Spacing();
    
    // Toolbar with styled buttons
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));
    }
    
    // Run button with accent styling
    if (PremiumStyle::StyledButton("Run Script", true, ImVec2(120, 0)))
    {
        std::string script = scriptEditor->GetText();
        if (!script.empty())
        {
            ScriptService::RunScript<ScriptInstance>(script);
        }
    }
    
    ImGui::SameLine();
    
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
            scriptEditor->SetText(scriptContent);
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
            std::string scriptContent = scriptEditor->GetText();
            filesys::WriteStringToFile(*result, scriptContent);
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Clear"))
    {
        scriptEditor->SetText("");
    }
    
    if (PremiumStyle::IsPremiumEnabled)
        ImGui::PopStyleVar(2);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Editor with child window styling
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.04f, 0.04f, 0.06f, 1.00f));
    }
    
    scriptEditor->Render("##LuaScriptEditor", false, ImVec2(0, 0), true);
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}