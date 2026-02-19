# Lua Environment

The Polytoria Executor provides an extended Lua environment based on MoonSharp, integrated into the game's script runtime. This section covers how the Lua environment works, what custom functions are available, and how to use them effectively.

## Overview

The executor injects custom Lua functions into the game's script environment when scripts are executed through the `ScriptService`. This allows for powerful scripting capabilities that interact directly with the game engine.

## How It Works

1. **Script Execution Hook**: The executor hooks `ScriptService.ExecuteScriptInstance` to intercept script execution
2. **Whitelist System**: Only scripts added to `ScriptService::whitelisted` receive the custom environment
3. **Function Injection**: Custom functions are registered via `InstallEnvironnement()` using MoonSharp callbacks

## Environment Components

The custom Lua environment consists of several function categories:

### Core Functions
- [`loadstring`](functions.md#loadstring) - Execute dynamic Lua code
- [`identifyexecutor`](functions.md#identifyexecutor) - Identify the executor

### Tool Functions
- [`equiptool`](functions.md#equiptool) - Equip a tool to local player
- [`unequiptool`](functions.md#unequiptool) - Unequip a tool
- [`activatetool`](functions.md#activatetool) - Activate a tool
- [`serverequiptool`](functions.md#serverequiptool) - Equip tool to any player

### Chat Functions
- [`sendchat`](functions.md#sendchat) - Send chat messages

### Mouse Functions
- [`mouse_move`](functions.md#mouse_move) - Move cursor
- [`mouse_get_position`](functions.md#mouse_get_position) - Get cursor position
- [`mouse_click`](functions.md#mouse_click) - Simulate click
- [`mouse_down`](functions.md#mouse_down) - Mouse button press
- [`mouse_up`](functions.md#mouse_up) - Mouse button release
- [`mouse_scroll`](functions.md#mouse_scroll) - Scroll wheel
- [`mouse_lock`](functions.md#mouse_lock) - Lock/unlock cursor
- [`mouse_visible`](functions.md#mouse_visible) - Show/hide cursor

### Drawing Functions
- [`draw_line`](functions.md#draw_line) - Draw lines
- [`draw_rect`](functions.md#draw_rect) - Draw rectangles
- [`draw_circle`](functions.md#draw_circle) - Draw circles
- [`draw_text`](functions.md#draw_text) - Draw text
- [`draw_clear`](functions.md#draw_clear) - Clear drawings

### Network Functions
- [`fireclickdetector`](functions.md#fireclickdetector) - Trigger click events

### HTTP Functions
- [`httpget`](functions.md#httpget) - Make HTTP GET requests

## MoonSharp Integration

The executor uses [MoonSharp](http://www.moonsharp.org/) as its Lua interpreter, which provides:

- C#/.NET integration
- Coroutine support
- Dynamic code execution
- User data type support for game objects

## Related Documentation

- [Functions Reference](functions.md) - Complete function documentation
- [Beginner Guide](beginner.md) - Getting started with scripting
- [Advanced Guide](advanced.md) - Advanced techniques
