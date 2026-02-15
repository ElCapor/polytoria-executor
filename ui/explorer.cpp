#include <ui/explorer.h>
#include <ui/premiumstyle.h>
#include <ptoria/game.h>
#include <imgui.h>
#include <ui/scriptsource.h>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
Instance *ExplorerUI::selectedInstance = nullptr;
static float explorerSplitRatio = 0.55f; // Default ratio for tree/properties split

// +--------------------------------------------------------+
// |                       Functions                        |
// +--------------------------------------------------------+
void RenderInstanceTree(Instance *instance);

// Helper function to draw a vertical splitter
static bool VerticalSplitter(float* ratio, float minRatio = 0.2f, float maxRatio = 0.8f)
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    ImGui::Button("##Splitter", ImVec2(-1, 6.0f));
    ImGui::PopStyleColor(3);
    
    if (ImGui::IsItemActive())
    {
        float mouseY = ImGui::GetMousePos().y;
        float windowTop = ImGui::GetItemRectMin().y - (*ratio) * ImGui::GetWindowHeight();
        float windowBottom = windowTop + ImGui::GetWindowHeight();
        
        float newRatio = (mouseY - windowTop) / (windowBottom - windowTop);
        // Manual clamp since ImClamp may not be available
        if (newRatio < minRatio) newRatio = minRatio;
        if (newRatio > maxRatio) newRatio = maxRatio;
        
        if (newRatio != *ratio)
        {
            *ratio = newRatio;
            return true;
        }
    }
    
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }
    
    return false;
}

// +--------------------------------------------------------+
// |                   Explorer Interface                   |
// +--------------------------------------------------------+

void ExplorerUI::Init()
{
}

void ExplorerUI::DrawTab()
{
    static Game *gameInstance = Game::GetSingleton();
    if (!gameInstance)
        return;

    float availableHeight = ImGui::GetContentRegionAvail().y;
    float splitterHeight = 6.0f;
    float spacingHeight = 8.0f;
    float treeHeight = (availableHeight - splitterHeight - spacingHeight) * explorerSplitRatio;
    float propertiesHeight = (availableHeight - splitterHeight - spacingHeight) * (1.0f - explorerSplitRatio);

    // Tree Explorer section with premium styling
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.07f, 1.00f));
    }
    
    ImGui::BeginChild("TreeRegion", ImVec2(0, treeHeight), true);
    
    // Section header
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
        if (PremiumStyle::FontBold)
            ImGui::PushFont(PremiumStyle::FontBold);
    }
    ImGui::Text("Instance Explorer");
    if (PremiumStyle::IsPremiumEnabled)
    {
        if (PremiumStyle::FontBold)
            ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    RenderInstanceTree((Instance *)gameInstance);
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    
    // Resizable splitter
    VerticalSplitter(&explorerSplitRatio);

    // Properties section with premium styling
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.07f, 1.00f));
    }
    
    ImGui::BeginChild("PropertiesRegion", ImVec2(0, propertiesHeight), true);
    
    if (selectedInstance)
    {
        // Properties header with gold accent
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.78f, 0.20f, 1.00f));
            if (PremiumStyle::FontBold)
                ImGui::PushFont(PremiumStyle::FontBold);
        }
        ImGui::Text("Properties: %s", selectedInstance->Name()->ToString().c_str());
        if (PremiumStyle::IsPremiumEnabled)
        {
            if (PremiumStyle::FontBold)
                ImGui::PopFont();
            ImGui::PopStyleColor();
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Premium table styling
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 6));
        if (ImGui::BeginTable("PropertiesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp))
        {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // Name
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Name");
            else
                ImGui::Text("Name");
            ImGui::TableSetColumnIndex(1);
            std::string nameStr = selectedInstance->Name()->ToString();
            if (PremiumStyle::IsPremiumEnabled)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.95f, 0.96f, 1.00f));
                if (ImGui::Selectable(nameStr.c_str()))
                    ImGui::SetClipboardText(nameStr.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", nameStr.c_str());
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Selectable(nameStr.c_str()))
                    ImGui::SetClipboardText(nameStr.c_str());
            }

            // Type
            auto selectedInstanceObject = Unity::CastToUnityObject(selectedInstance);
            auto type = selectedInstanceObject->GetType();
            if (type)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (PremiumStyle::IsPremiumEnabled)
                    ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Class");
                else
                    ImGui::Text("Class");
                ImGui::TableSetColumnIndex(1);
                std::string classStr = type->GetFullNameOrDefault()->ToString();
                if (PremiumStyle::IsPremiumEnabled)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
                    if (ImGui::Selectable(classStr.c_str()))
                        ImGui::SetClipboardText(classStr.c_str());
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Click to copy: %s", classStr.c_str());
                    ImGui::PopStyleColor();
                }
                else
                {
                    if (ImGui::Selectable(classStr.c_str()))
                        ImGui::SetClipboardText(classStr.c_str());
                }

                // Script Source button for script types
                if (type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.LocalScript" || 
                    type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.Script" || 
                    type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.ModuleScript" || 
                    type->GetFullNameOrDefault()->ToString() == "Polytoria.Datamodel.ScriptInstance")
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (PremiumStyle::IsPremiumEnabled)
                        ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Script Source");
                    else
                        ImGui::Text("Script Source");
                    ImGui::TableSetColumnIndex(1);
                    if (PremiumStyle::StyledButton("View Source", true, ImVec2(100, 0)))
                    {
                        if (!ScriptSourceUI::IsTabAlreadyOpen(Unity::Cast<BaseScript>(selectedInstance)))
                        {
                            BaseScript *scriptInstance = Unity::Cast<BaseScript>(selectedInstance);
                            ScriptSourceUI::OpenNewScriptDecompileTab(scriptInstance);
                        }
                    }
                }
            }

            // Memory Address
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Address");
            else
                ImGui::Text("Address");
            ImGui::TableSetColumnIndex(1);
            char addrBuf[32];
            snprintf(addrBuf, sizeof(addrBuf), "0x%p", selectedInstance);
            if (PremiumStyle::IsPremiumEnabled)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.60f, 0.80f, 0.60f, 1.00f));
                if (ImGui::Selectable(addrBuf))
                    ImGui::SetClipboardText(addrBuf);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", addrBuf);
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Selectable(addrBuf))
                    ImGui::SetClipboardText(addrBuf);
            }

            // Full Name
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Full Name");
            else
                ImGui::Text("Full Name");
            ImGui::TableSetColumnIndex(1);
            std::string fullNameStr = selectedInstance->FullName()->ToString();
            if (PremiumStyle::IsPremiumEnabled)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.95f, 0.96f, 1.00f));
                if (ImGui::Selectable(fullNameStr.c_str()))
                    ImGui::SetClipboardText(fullNameStr.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", fullNameStr.c_str());
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Selectable(fullNameStr.c_str()))
                    ImGui::SetClipboardText(fullNameStr.c_str());
            }

            ImGui::EndTable();
        }
        if (PremiumStyle::IsPremiumEnabled)
            ImGui::PopStyleVar();
    }
    else
    {
        // Empty state with centered text
        ImVec2 textSize = ImGui::CalcTextSize("Select an instance to see its properties");
        ImVec2 regionSize = ImGui::GetContentRegionAvail();
        ImGui::SetCursorPosX((regionSize.x - textSize.x) / 2);
        ImGui::SetCursorPosY((regionSize.y - textSize.y) / 2);
        ImGui::TextDisabled("Select an instance to see its properties");
    }
    ImGui::EndChild();
    
    if (PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
}

// +--------------------------------------------------------+
// |                        Helpers                         |
// +--------------------------------------------------------+
void RenderInstanceTree(Instance *instance)
{
    if (!instance)
        return;

    // Premium tree node styling
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    if (PremiumStyle::IsPremiumEnabled)
        flags |= ImGuiTreeNodeFlags_FramePadding;

    auto children = instance->Children();
    bool hasChildren = children && children->max_length > 0;

    if (!hasChildren)
        flags |= ImGuiTreeNodeFlags_Leaf;

    // Highlight selected node
    bool isSelected = (ExplorerUI::selectedInstance == instance);
    if (isSelected && PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.00f, 0.75f, 0.85f, 0.40f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.00f, 0.75f, 0.85f, 0.50f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.00f, 0.75f, 0.85f, 0.60f));
    }

    bool opened = ImGui::TreeNodeEx(instance, flags, "%s", instance->Name()->ToString().c_str());

    if (isSelected && PremiumStyle::IsPremiumEnabled)
    {
        ImGui::PopStyleColor(3);
    }

    if (ImGui::IsItemClicked())
    {
        ExplorerUI::selectedInstance = instance;
    }

    if (opened)
    {
        if (hasChildren)
        {
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 20.0f);
            for (auto child : children->ToVector())
            {
                RenderInstanceTree((Instance *)child);
            }
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::PopStyleVar();
        }
        ImGui::TreePop();
    }
}