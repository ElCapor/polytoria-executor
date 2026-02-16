#include <ui/explorer.h>
#include <ui/premiumstyle.h>
#include <ui/iconmanager.h>
#include <ptoria/game.h>
#include <imgui.h>
#include <ui/scriptsource.h>
#include <unity/unity.h>
#include <unordered_map>
#include <mutex>

// +--------------------------------------------------------+
// |                       Variables                        |
// +--------------------------------------------------------+
Instance *ExplorerUI::selectedInstance = nullptr;
static float explorerSplitRatio = 0.55f; // Default ratio for tree/properties split

// +--------------------------------------------------------+
// |                   Performance Cache                    |
// +--------------------------------------------------------+
namespace ExplorerCache
{
    // Cache for instance metadata to avoid repeated Unity API calls
    struct InstanceCache
    {
        std::string name;
        std::string className;
        std::string fullName;
        bool isPopulated = false;
    };
    
    static std::unordered_map<Instance*, InstanceCache> s_cache;
    static std::mutex s_cacheMutex;
    
    // Get or create cache entry for an instance
    InstanceCache& GetCache(Instance* instance)
    {
        std::lock_guard<std::mutex> lock(s_cacheMutex);
        auto it = s_cache.find(instance);
        if (it != s_cache.end())
        {
            return it->second;
        }
        
        // Create new entry
        auto& cache = s_cache[instance];
        cache.isPopulated = false;
        return cache;
    }
    
    // Populate cache entry (called once per instance)
    void PopulateCache(Instance* instance, InstanceCache& cache)
    {
        if (cache.isPopulated)
            return;
            
        // Get name
        auto nameStr = instance->Name();
        if (nameStr)
            cache.name = nameStr->ToString();
        else
            cache.name = "Unknown";
        
        // Get type info
        auto instanceObject = Unity::CastToUnityObject(instance);
        if (instanceObject)
        {
            auto type = instanceObject->GetType();
            if (type)
            {
                std::string fullName = type->GetFullNameOrDefault()->ToString();
                cache.fullName = fullName;
                
                // Extract class name from full name
                size_t lastDot = fullName.find_last_of('.');
                if (lastDot != std::string::npos && lastDot + 1 < fullName.length())
                    cache.className = fullName.substr(lastDot + 1);
                else
                    cache.className = fullName;
            }
        }
        
        cache.isPopulated = true;
    }
    
    // Clear cache (call when hierarchy might have changed)
    void ClearCache()
    {
        std::lock_guard<std::mutex> lock(s_cacheMutex);
        s_cache.clear();
    }
    
    // Remove specific instance from cache
    void RemoveFromCache(Instance* instance)
    {
        std::lock_guard<std::mutex> lock(s_cacheMutex);
        s_cache.erase(instance);
    }
    
    // Get cached name (populates if needed)
    const std::string& GetName(Instance* instance)
    {
        auto& cache = GetCache(instance);
        PopulateCache(instance, cache);
        return cache.name;
    }
    
    // Get cached class name (populates if needed)
    const std::string& GetClassName(Instance* instance)
    {
        auto& cache = GetCache(instance);
        PopulateCache(instance, cache);
        return cache.className;
    }
    
    // Get cached full name (populates if needed)
    const std::string& GetFullName(Instance* instance)
    {
        auto& cache = GetCache(instance);
        PopulateCache(instance, cache);
        return cache.fullName;
    }
}

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
    // Clear cache on init
    ExplorerCache::ClearCache();
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
    
    // Refresh button to manually clear cache
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80);
    if (ImGui::SmallButton("Refresh"))
    {
        ExplorerCache::ClearCache();
    }
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
        // Use cached name
        const std::string& nameStr = ExplorerCache::GetName(selectedInstance);
        
        // Properties header with gold accent
        if (PremiumStyle::IsPremiumEnabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.78f, 0.20f, 1.00f));
            if (PremiumStyle::FontBold)
                ImGui::PushFont(PremiumStyle::FontBold);
        }
        ImGui::Text("Properties: %s", nameStr.c_str());
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

            // Name - use cached value
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Name");
            else
                ImGui::Text("Name");
            ImGui::TableSetColumnIndex(1);
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

            // Class - use cached value
            const std::string& classStr = ExplorerCache::GetFullName(selectedInstance);
            const std::string& className = ExplorerCache::GetClassName(selectedInstance);
            
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Class");
            else
                ImGui::Text("Class");
            ImGui::TableSetColumnIndex(1);
            if (PremiumStyle::IsPremiumEnabled)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.75f, 0.85f, 1.00f));
                if (ImGui::Selectable(std::format("{}##class", className).c_str()))
                    ImGui::SetClipboardText(classStr.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", classStr.c_str());
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Selectable(std::format("{}##class", className).c_str()))
                    ImGui::SetClipboardText(classStr.c_str());
            }

            // Script Source button for script types
            if (className == "LocalScript" || className == "Script" || 
                className == "ModuleScript" || className == "ScriptInstance")
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

            // Full Name - use cached value
            const std::string& fullNameStr = ExplorerCache::GetFullName(selectedInstance);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.75f, 1.00f), "Full Name");
            else
                ImGui::Text("Full Name");
            ImGui::TableSetColumnIndex(1);
            if (PremiumStyle::IsPremiumEnabled)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.94f, 0.95f, 0.96f, 1.00f));
                if (ImGui::Selectable(std::format("{}##fullname", fullNameStr).c_str()))
                    ImGui::SetClipboardText(fullNameStr.c_str());
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Click to copy: %s", fullNameStr.c_str());
                ImGui::PopStyleColor();
            }
            else
            {
                if (ImGui::Selectable(std::format("{}##fullname", fullNameStr).c_str()))
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

    // Get children - use direct array access instead of ToVector()
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

    // Use cached class name for icon lookup
    const std::string& className = ExplorerCache::GetClassName(instance);

    // Draw icon before tree node if available
    float iconSize = 24.0f;
    bool hasIcon = false;
    if (IconManager::IsInitialized() && !className.empty())
    {
        ImVec2 size(iconSize, iconSize);
        if (IconManager::DrawIcon(className.c_str(), size))
        {
            hasIcon = true;
            ImGui::SameLine(0.0f, 6.0f);
        }
    }

    // Use cached name
    const std::string& nameStr = ExplorerCache::GetName(instance);
    bool opened = ImGui::TreeNodeEx(instance, flags, "%s", nameStr.c_str());

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
            
            // OPTIMIZATION: Iterate directly over array instead of ToVector()
            for (int i = 0; i < children->max_length; i++)
            {
                Instance* child = (Instance*)children->At(i);
                if (child)
                {
                    RenderInstanceTree(child);
                }
            }
            
            if (PremiumStyle::IsPremiumEnabled)
                ImGui::PopStyleVar();
        }
        ImGui::TreePop();
    }
}
