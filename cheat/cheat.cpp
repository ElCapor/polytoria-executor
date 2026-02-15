#include <ptoria/instance.h>
#include <nasec/assert.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <core/core.h>
#include <ptoria/game.h>
#include <ptoria/scriptinstance.h>
#include <ptoria/scriptservice.h>
#include <mirror/hooks.h>
#include <ptoria/networkevent.h>
#include <ui/ui.h>

int main_thread()
{
    OpenConsole();

    //NetworkEvent::InstallHooks();
    Unity::Init();
    Unity::ThreadAttach();
    UI::state = UI::UiState::Ready;

    UnityAssembly* assembly = Unity::GetAssembly<Unity::AssemblyCSharp>();

    
    spdlog::info("Assembly Name: {}", assembly->name);
    UnityClass* instanceClass = StaticClass<Instance>();
    nasec::Assert(instanceClass != nullptr, "Failed to get Instance class");
    

    UnityClass* gameClass = StaticClass<Game>();
    nasec::Assert(gameClass != nullptr, "Failed to get Game class");

    spdlog::info("Instance Class Name: {}", instanceClass->name);
    spdlog::info("Game Class Name: {}", gameClass->name);

    Game* gameInstance = Game::GetSingleton();
    nasec::Assert(gameInstance != nullptr, "Failed to get Game singleton instance");

    spdlog::info("Game Instance Address: 0x{:016X}", reinterpret_cast<uintptr_t>(gameInstance));

    spdlog::info("Game Instance Name: {}", gameInstance->Name()->ToString());
    spdlog::info("Game Instance Full Name: {}", gameInstance->FullName()->ToString());
    for (auto& child: gameInstance->Children()->ToVector()) {
        spdlog::info("Child Instance Name: {}", child->Name()->ToString());
    }

    spdlog::info("ScriptInstance Class Name: {}", StaticClass<ScriptInstance>()->name);
    ScriptService::InstallHooks();
    ScriptService* scriptService = ScriptService::GetInstance();
    nasec::Assert(scriptService != nullptr, "Failed to get ScriptService instance");
    ScriptService::RunScript<ScriptInstance>(R"(InvokeServerHook(game["Hidden"]["DraggerPlace"], function(msg) print(msg) end))");
    ScriptService::RunScript<ScriptInstance>(
        R"delim(
            ---@diagnostic disable: assign-type-mismatch
            -- Instance2Lua (github.com/geringverdien/Polytoria-Instance2Lua) --
            local GUI = Instance.New("GUI");
            GUI.Name = "Executor";
            GUI.Visible = true;
            GUI.Parent = game["PlayerGUI"];
            local Holder = Instance.New("UIView");
            Holder.Name = "Holder";
            Holder.PivotPoint = Vector2.New(0.5, 0.5);
            Holder.PositionOffset = Vector2.New(0, 0);
            Holder.PositionRelative = Vector2.New(0.5, 0.5);
            Holder.Rotation = 0;
            Holder.SizeOffset = Vector2.New(560, 323);
            Holder.SizeRelative = Vector2.New(0, 0);
            Holder.Visible = true;
            Holder.ClipDescendants = false;
            Holder.Color = Color.New(0.000, 0.000, 0.000, 0.000);
            Holder.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Holder.BorderWidth = 0;
            Holder.CornerRadius = 0;
            Holder.Parent = GUI;
            local Body = Instance.New("UIView");
            Body.Name = "Body";
            Body.PivotPoint = Vector2.New(0.5, 0.5);
            Body.PositionOffset = Vector2.New(0, 0);
            Body.PositionRelative = Vector2.New(0.5, 0.5);
            Body.Rotation = 0;
            Body.SizeOffset = Vector2.New(0, 0);
            Body.SizeRelative = Vector2.New(1, 1);
            Body.Visible = true;
            Body.ClipDescendants = false;
            Body.Color = Color.New(0.247, 0.247, 0.247, 1.000);
            Body.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Body.BorderWidth = 0;
            Body.CornerRadius = 0;
            Body.Parent = Holder;
            local Code = Instance.New("UITextInput");
            Code.Name = "Code";
            Code.PivotPoint = Vector2.New(0.5, 0.5);
            Code.PositionOffset = Vector2.New(0, 0);
            Code.PositionRelative = Vector2.New(0.5, 0.5647);
            Code.Rotation = 0;
            Code.SizeOffset = Vector2.New(0, 0);
            Code.SizeRelative = Vector2.New(1, 0.7379);
            Code.Visible = true;
            Code.ClipDescendants = false;
            Code.Color = Color.New(0.118, 0.118, 0.118, 1.000);
            Code.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Code.BorderWidth = 0;
            Code.CornerRadius = 0;
            Code.Text = "";
            Code.Placeholder = "code here";
            Code.PlaceholderColor = Color.New(0.500, 0.500, 0.500, 1.000);
            Code.TextColor = Color.New(0.949, 0.949, 0.949, 1.000);
            Code.FontSize = 1;
            Code.Font = TextFontPreset.Montserrat;
            Code.AutoSize = true;
            Code.MaxFontSize = 8;
            Code.JustifyText = TextJustify.Left;
            Code.VerticalAlign = TextVerticalAlign.Top;
            Code.IsReadOnly = false;
            Code.IsMultiline = true;
            Code.Parent = Body;
            local Execute = Instance.New("UIButton");
            Execute.Name = "Execute";
            Execute.PivotPoint = Vector2.New(0.5, 0.5);
            Execute.PositionOffset = Vector2.New(0, 0);
            Execute.PositionRelative = Vector2.New(0.2, 0.0971);
            Execute.Rotation = 0;
            Execute.SizeOffset = Vector2.New(0, 0);
            Execute.SizeRelative = Vector2.New(0.3368, 0.1489);
            Execute.Visible = true;
            Execute.ClipDescendants = false;
            Execute.Color = Color.New(0.118, 0.118, 0.118, 1.000);
            Execute.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Execute.BorderWidth = 0;
            Execute.CornerRadius = 0;
            Execute.Text = "Execute";
            Execute.TextColor = Color.New(0.973, 0.973, 0.973, 1.000);
            Execute.FontSize = 16;
            Execute.Font = TextFontPreset.Montserrat;
            Execute.AutoSize = false;
            Execute.MaxFontSize = 16;
            Execute.JustifyText = TextJustify.Center;
            Execute.VerticalAlign = TextVerticalAlign.Middle;
            Execute.Interactable = true;
            Execute.Parent = Body;
            local Clear = Instance.New("UIButton");
            Clear.Name = "Clear";
            Clear.PivotPoint = Vector2.New(0.5, 0.5);
            Clear.PositionOffset = Vector2.New(0, 0);
            Clear.PositionRelative = Vector2.New(0.7842, 0.0971);
            Clear.Rotation = 0;
            Clear.SizeOffset = Vector2.New(0, 0);
            Clear.SizeRelative = Vector2.New(0.3368, 0.1489);
            Clear.Visible = true;
            Clear.ClipDescendants = false;
            Clear.Color = Color.New(0.118, 0.118, 0.118, 1.000);
            Clear.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Clear.BorderWidth = 0;
            Clear.CornerRadius = 0;
            Clear.Text = "Clear";
            Clear.TextColor = Color.New(0.788, 0.788, 0.788, 1.000);
            Clear.FontSize = 16;
            Clear.Font = TextFontPreset.Montserrat;
            Clear.AutoSize = false;
            Clear.MaxFontSize = 16;
            Clear.JustifyText = TextJustify.Center;
            Clear.VerticalAlign = TextVerticalAlign.Middle;
            Clear.Interactable = true;
            Clear.Parent = Body;
            local Minimize = Instance.New("UIButton");
            Minimize.Name = "Minimize";
            Minimize.PivotPoint = Vector2.New(0.5, 0.5);
            Minimize.PositionOffset = Vector2.New(0, 0);
            Minimize.PositionRelative = Vector2.New(0.908, 0.9721);
            Minimize.Rotation = 0;
            Minimize.SizeOffset = Vector2.New(0, 0);
            Minimize.SizeRelative = Vector2.New(0.1054, 0.065);
            Minimize.Visible = true;
            Minimize.ClipDescendants = false;
            Minimize.Color = Color.New(1.000, 1.000, 1.000, 0.000);
            Minimize.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Minimize.BorderWidth = 0;
            Minimize.CornerRadius = 0;
            Minimize.Text = "-";
            Minimize.TextColor = Color.New(0.000, 0.000, 0.000, 1.000);
            Minimize.FontSize = 32;
            Minimize.Font = TextFontPreset.Montserrat;
            Minimize.AutoSize = false;
            Minimize.MaxFontSize = 32;
            Minimize.JustifyText = TextJustify.Center;
            Minimize.VerticalAlign = TextVerticalAlign.Middle;
            Minimize.Interactable = true;
            Minimize.Parent = Body;
            local Open = Instance.New("UIButton");
            Open.Name = "Open";
            Open.PivotPoint = Vector2.New(0.5, 0.5);
            Open.PositionOffset = Vector2.New(0, 0);
            Open.PositionRelative = Vector2.New(0.7842, 0.9675);
            Open.Rotation = 0;
            Open.SizeOffset = Vector2.New(0, 0);
            Open.SizeRelative = Vector2.New(0.1339, 0.065);
            Open.Visible = true;
            Open.ClipDescendants = false;
            Open.Color = Color.New(0.212, 0.212, 0.212, 0.530);
            Open.BorderColor = Color.New(0.000, 0.000, 0.000, 0.000);
            Open.BorderWidth = 0;
            Open.CornerRadius = 0;
            Open.Text = "+";
            Open.TextColor = Color.New(0.000, 0.000, 0.000, 1.000);
            Open.FontSize = 32;
            Open.Font = TextFontPreset.Montserrat;
            Open.AutoSize = false;
            Open.MaxFontSize = 32;
            Open.JustifyText = TextJustify.Center;
            Open.VerticalAlign = TextVerticalAlign.Middle;
            Open.Interactable = true;
            Open.Parent = Holder;

            local function onExecute()
                local source = Code.Text
                local res = loadstring(source)
                print(res)
                res()
            end

            Execute.Clicked:Connect(onExecute)
            Clear.Clicked:Connect(function()
                Code.Text = ""
            end)
            Minimize.Clicked:Connect(function()
                Body.Visible = false
            end)
            Open.Clicked:Connect(function()
                Body.Visible = true
            end)

            local isDragging = false 
            local mousepos = Input.MousePosition
            Body.MouseDown:Connect(function()
                isDragging = true
                mousepos = Input.MousePosition
            end)

            Body.MouseUp:Connect(function()
                isDragging = false
            end)

            game.Rendered:Connect(function()
                if isDragging then
                    local delta = Input.MousePosition - mousepos
                    Holder.PositionOffset = Holder.PositionOffset + delta
                    mousepos = Input.MousePosition
                end
            end)
        )delim");
    
    mirror::InstallHooks();

    UI::Setup();
    return 0;
}