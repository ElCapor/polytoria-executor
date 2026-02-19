#include <ptoria/drawinglib.h>
#include <unity/unity.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

// Thread-safe command buffer
std::mutex DrawingLib::commandMutex;
std::vector<DrawCommand> DrawingLib::commands;

// Helper: extract number from DynValue arg
static double GetNum(CallbackArguments* args, int idx) {
    DynValue* val = args->RawGet(idx, false);
    if (val == nullptr || val->Type() != DynValue::DataType::Number) return 0.0;
    return val->AsNumber();
}

// Helper: build ImU32 color from RGBA (0-255)
static ImU32 MakeColor(float r, float g, float b, float a) {
    return IM_COL32(
        static_cast<int>(r) & 0xFF,
        static_cast<int>(g) & 0xFF,
        static_cast<int>(b) & 0xFF,
        static_cast<int>(a) & 0xFF
    );
}

// ============================================================
// Render (called from DX11 render thread in HookPresent)
// ============================================================
void DrawingLib::RenderDrawCommands() {
    std::lock_guard<std::mutex> lock(commandMutex);
    if (commands.empty()) return;

    ImDrawList* dl = ImGui::GetForegroundDrawList();
    if (!dl) return;

    for (const auto& cmd : commands) {
        ImU32 color = MakeColor(cmd.r, cmd.g, cmd.b, cmd.a);

        switch (cmd.type) {
            case DrawCommandType::Line:
                dl->AddLine(ImVec2(cmd.x1, cmd.y1), ImVec2(cmd.x2, cmd.y2), color, cmd.thickness);
                break;

            case DrawCommandType::Rect:
                dl->AddRect(ImVec2(cmd.x1, cmd.y1),
                    ImVec2(cmd.x1 + cmd.x2, cmd.y1 + cmd.y2),
                    color, 0.0f, 0, cmd.thickness);
                break;

            case DrawCommandType::FilledRect:
                dl->AddRectFilled(ImVec2(cmd.x1, cmd.y1),
                    ImVec2(cmd.x1 + cmd.x2, cmd.y1 + cmd.y2),
                    color);
                break;

            case DrawCommandType::Circle:
                dl->AddCircle(ImVec2(cmd.x1, cmd.y1), cmd.radius, color, cmd.segments, cmd.thickness);
                break;

            case DrawCommandType::FilledCircle:
                dl->AddCircleFilled(ImVec2(cmd.x1, cmd.y1), cmd.radius, color, cmd.segments);
                break;

            case DrawCommandType::Text:
                dl->AddText(ImVec2(cmd.x1, cmd.y1), color, cmd.text.c_str());
                break;
        }
    }
    DrawingLib::commands.clear();
}

// ============================================================
// Lua Callbacks (called from game thread)
// ============================================================

// draw_line(x1, y1, x2, y2, r, g, b, a, thickness)
DynValue* draw_line(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 9)
        return DynValue::FromString("draw_line requires 9 args: x1, y1, x2, y2, r, g, b, a, thickness");

    DrawCommand cmd{};
    cmd.type = DrawCommandType::Line;
    cmd.x1 = static_cast<float>(GetNum(args, 0));
    cmd.y1 = static_cast<float>(GetNum(args, 1));
    cmd.x2 = static_cast<float>(GetNum(args, 2));
    cmd.y2 = static_cast<float>(GetNum(args, 3));
    cmd.r  = static_cast<float>(GetNum(args, 4));
    cmd.g  = static_cast<float>(GetNum(args, 5));
    cmd.b  = static_cast<float>(GetNum(args, 6));
    cmd.a  = static_cast<float>(GetNum(args, 7));
    cmd.thickness = static_cast<float>(GetNum(args, 8));
    if (cmd.thickness <= 0) cmd.thickness = 1.0f;

    {
        std::lock_guard<std::mutex> lock(DrawingLib::commandMutex);
        DrawingLib::commands.push_back(cmd);
    }
    return DynValue::FromNil();
}

// draw_rect(x, y, w, h, r, g, b, a, thickness, filled)
DynValue* draw_rect(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 10)
        return DynValue::FromString("draw_rect requires 10 args: x, y, w, h, r, g, b, a, thickness, filled");

    DrawCommand cmd{};
    float filled = static_cast<float>(GetNum(args, 9));
    cmd.type = (filled > 0) ? DrawCommandType::FilledRect : DrawCommandType::Rect;
    cmd.x1 = static_cast<float>(GetNum(args, 0));
    cmd.y1 = static_cast<float>(GetNum(args, 1));
    cmd.x2 = static_cast<float>(GetNum(args, 2)); // width
    cmd.y2 = static_cast<float>(GetNum(args, 3)); // height
    cmd.r  = static_cast<float>(GetNum(args, 4));
    cmd.g  = static_cast<float>(GetNum(args, 5));
    cmd.b  = static_cast<float>(GetNum(args, 6));
    cmd.a  = static_cast<float>(GetNum(args, 7));
    cmd.thickness = static_cast<float>(GetNum(args, 8));
    if (cmd.thickness <= 0) cmd.thickness = 1.0f;

    {
        std::lock_guard<std::mutex> lock(DrawingLib::commandMutex);
        DrawingLib::commands.push_back(cmd);
    }
    return DynValue::FromNil();
}

// draw_circle(x, y, radius, r, g, b, a, thickness, segments, filled)
DynValue* draw_circle(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 10)
        return DynValue::FromString("draw_circle requires 10 args: x, y, radius, r, g, b, a, thickness, segments, filled");

    DrawCommand cmd{};
    float filled = static_cast<float>(GetNum(args, 9));
    cmd.type = (filled > 0) ? DrawCommandType::FilledCircle : DrawCommandType::Circle;
    cmd.x1 = static_cast<float>(GetNum(args, 0));
    cmd.y1 = static_cast<float>(GetNum(args, 1));
    cmd.radius = static_cast<float>(GetNum(args, 2));
    cmd.r  = static_cast<float>(GetNum(args, 3));
    cmd.g  = static_cast<float>(GetNum(args, 4));
    cmd.b  = static_cast<float>(GetNum(args, 5));
    cmd.a  = static_cast<float>(GetNum(args, 6));
    cmd.thickness = static_cast<float>(GetNum(args, 7));
    cmd.segments = static_cast<int>(GetNum(args, 8));
    if (cmd.thickness <= 0) cmd.thickness = 1.0f;
    if (cmd.segments <= 0) cmd.segments = 32;

    {
        std::lock_guard<std::mutex> lock(DrawingLib::commandMutex);
        DrawingLib::commands.push_back(cmd);
    }
    return DynValue::FromNil();
}

// draw_text(x, y, r, g, b, a, text)
DynValue* draw_text_func(void*, ScriptExecutionContext*, CallbackArguments* args) {
    if (args->Count() < 7)
        return DynValue::FromString("draw_text requires 7 args: x, y, r, g, b, a, text");

    // Extract the text string argument
    DynValue* textArg = args->RawGet(6, false);
    if (textArg == nullptr || textArg->Type() != DynValue::DataType::String)
        return DynValue::FromString("draw_text: argument 7 must be a string");

    UnityClass* stringClass = Unity::GetClass<"String", "mscorlib.dll", "System">();
    UnityString* str = (UnityString*)textArg->Cast(stringClass->GetType());
    if (str == nullptr)
        return DynValue::FromString("draw_text: failed to cast text string");

    DrawCommand cmd{};
    cmd.type = DrawCommandType::Text;
    cmd.x1 = static_cast<float>(GetNum(args, 0));
    cmd.y1 = static_cast<float>(GetNum(args, 1));
    cmd.r  = static_cast<float>(GetNum(args, 2));
    cmd.g  = static_cast<float>(GetNum(args, 3));
    cmd.b  = static_cast<float>(GetNum(args, 4));
    cmd.a  = static_cast<float>(GetNum(args, 5));
    cmd.text = str->ToString();

    {
        std::lock_guard<std::mutex> lock(DrawingLib::commandMutex);
        DrawingLib::commands.push_back(cmd);
    }
    return DynValue::FromNil();
}

// draw_clear() — Clears all draw commands
DynValue* draw_clear(void*, ScriptExecutionContext*, CallbackArguments*) {
    std::lock_guard<std::mutex> lock(DrawingLib::commandMutex);
    DrawingLib::commands.clear();
    return DynValue::FromNil();
}
