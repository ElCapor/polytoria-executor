# Polytoria Executor Documentation

Welcome to the official documentation for **Polytoria Executor** (Polyhack v3). This documentation covers the Lua scripting environment, custom functions, and the internal architecture of the executor.

## What is Polytoria Executor?

Polytoria Executor is a C++23 DLL that injects into the Polytoria game process (a Unity IL2CPP game). It provides:

- **Custom Lua Environment** - Extended Lua scripting with game-specific functions
- **ImGui UI Overlay** - In-game menu accessed via `DELETE` key
- **Network Interception** - Hooks into Mirror Networking for packet capture/modification
- **Named Pipe Server** - External tools can send Lua scripts via `\\.\pipe\wowiezz_pipe`

## Quick Navigation

=== "Lua Environment"
    - [Lua Functions Reference](lua/functions.md) - Complete list of custom functions
    - [Beginner Guide](lua/beginner.md) - Getting started with scripting
    - [Advanced Guide](lua/advanced.md) - Advanced scripting techniques

=== "Internals"
    - [Architecture Overview](internals/architecture.md) - System design
    - [Hooking System](internals/hooking.md) - How function hooking works
    - [Unity Integration](internals/unity.md) - IL2CPP reflection API

## Getting Started

### Installation

1. Build the project: `xmake config -m release && xmake build`
2. Copy `.download/` contents to game directory
3. Run `injector.exe` or use `version.dll` proxying
4. Press `DELETE` to toggle the UI overlay

### Running Scripts

=== "In-Game Executor"
Use the Executor tab in the ImGui overlay to run Lua scripts directly.

=== "Named Pipe"
```bash
echo "print('Hello from pipe!')" > \\.\pipe\wowiezz_pipe
```

## Key Features

| Feature | Description |
|---------|-------------|
| **Tool Management** | `equiptool`, `unequiptool`, `activatetool` |
| **Mouse Control** | Full mouse simulation and cursor control |
| **Drawing** | 2D overlay rendering (lines, rectangles, circles, text) |
| **HTTP Requests** | `httpget` for external API calls |
| **Chat** | `sendchat` for in-game messaging |
| **Dynamic Code** | `loadstring` for runtime code execution |

## Architecture Highlights

- **Thread-Safe Design** - Drawing commands use mutex-protected buffers
- **Whitelist Security** - Only whitelisted scripts get custom functions
- **IL2CPP Reflection** - Full Unity runtime inspection via UnityResolve
- **Detours Hooking** - Microsoft Detours for function interception
