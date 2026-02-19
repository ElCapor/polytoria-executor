#ifndef DRAWINGLIB_H
#define DRAWINGLIB_H

#include <moonsharp/cinterface.h>
#include <vector>
#include <mutex>
#include <string>

// Drawing command types
enum class DrawCommandType {
    Line,
    Rect,
    FilledRect,
    Circle,
    FilledCircle,
    Text,
};

// A single draw command queued from Lua, rendered on the DX11 thread
struct DrawCommand {
    DrawCommandType type;
    float x1, y1, x2, y2;      // positions / width+height
    float r, g, b, a;           // color (0-255)
    float thickness;
    float radius;
    int segments;
    std::string text;
};

namespace DrawingLib {
    // Thread-safe command buffer
    extern std::mutex commandMutex;
    extern std::vector<DrawCommand> commands;

    // Called from the DX11 render thread (HookPresent) to draw queued commands
    void RenderDrawCommands();
}

// Lua callbacks
DynValue* draw_line(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* draw_rect(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* draw_circle(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* draw_text_func(void*, ScriptExecutionContext*, CallbackArguments*);
DynValue* draw_clear(void*, ScriptExecutionContext*, CallbackArguments*);

#endif /* DRAWINGLIB_H */
