# Architecture Overview

This document provides a detailed overview of the Polytoria Executor's architecture, including the initialization sequence, thread model, and key components.

## System Overview

The executor is a C++23 DLL that injects into Polytoria (a Unity IL2CPP game). It operates by hooking into various game systems to provide enhanced scripting capabilities and a custom UI overlay.

```
┌─────────────────────────────────────────────────────────────┐
│                    Polytoria Game Process                    │
│  ┌─────────────┐  ┌─────────────┐  ┌──────────────────┐   │
│  │ GameAssembly│  │  Mirror.dll │  │  Unity Engine    │   │
│  │    .dll     │  │  (Network)  │  │  (DX11 Render)   │   │
│  └──────┬──────┘  └──────┬──────┘  └────────┬─────────┘   │
│         │                │                   │              │
│         ▼                ▼                   ▼              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │                  Polyhack v3 (wowiezz.dll)            │  │
│  │  ┌──────────┐  ┌──────────┐  ┌────────────────────┐  │  │
│  │  │ IL2CPP   │  │  Hook    │  │    ImGui UI        │  │  │
│  │  │ Resolve  │  │ Manager  │  │    Overlay         │  │  │
│  │  └────┬─────┘  └────┬─────┘  └─────────┬──────────┘  │  │
│  │       │             │                   │              │  │
│  │       ▼             │                   │              │  │
│  │  ┌────────────┐     │                   │              │  │
│  │  │   MoonSharp│◄────┴───────────────────┘              │  │
│  │  │   Bridge   │                                        │  │
│  │  └─────┬──────┘                                        │  │
│  │        │                                               │  │
│  │        ▼                                               │  │
│  │  ┌──────────────────────────────────────────┐         │  │
│  │  │     Custom Lua Functions                  │         │  │
│  │  │  • equiptool  • sendchat  • draw_*      │         │  │
│  │  │  • mouse_*    • httpget   • loadstring  │         │  │
│  │  └──────────────────────────────────────────┘         │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Initialization Sequence

The initialization follows a strict order that must not be changed:

```cpp
// cheat/cheat.cpp - main_thread()

1. OpenConsole()           // Open debug console
2. Wait for GameAssembly.dll   // IL2CPP must be loaded first
3. Unity::Init()          // Initialize UnityResolve
4. Unity::ThreadAttach()  // Attach to Unity's garbage collector
5. Get Assembly-CSharp    // Get game assembly reference
6. Wait for Game singleton    // Wait for game to be ready
7. UI::state = Ready     // Mark UI as ready
8. ScriptService::InstallHooks()  // Hook script execution
9. mirror::InstallHooks() // Hook network functions
10. StartPipeServer()    // Start named pipe server
11. UI::Setup()          // Hook DX11, start render loop
```

### Why Order Matters

1. **GameAssembly.dll must load first** - IL2CPP runtime doesn't exist until this DLL is loaded
2. **Unity::Init() before any Unity API** - UnityResolve needs to initialize before reflection
3. **ThreadAttach before IL2CPP calls** - Every thread must attach to Unity's GC before making calls
4. **Game singleton must exist** - Many functions depend on the game being initialized
5. **UI::Setup() last** - DX11 hooks must be installed after everything else is ready

## Thread Model

### Main Thread (Bootstrap)

The main thread handles initialization and exits after setup:

```cpp
// cheat/cheat.cpp
int main_thread() {
    // All initialization happens here
    // Thread exits after UI::Setup() completes
}
```

### DX11 Render Thread

The DirectX 11 render thread runs the ImGui overlay:

- **Entry Point**: First call to [`HookPresent()`](hooking/dx11hook.cpp)
- **IL2CPP Access**: Calls [`Unity::ThreadAttach()`](unity/unity.h) on first entry
- **Responsibilities**: 
  - Render ImGui UI
  - Execute [`DrawingLib::RenderDrawCommands()`](ptoria/drawinglib.cpp)

### Pipe Server Thread

Handles external script execution via named pipe:

```cpp
// cheat/pipe.cpp
// Pipe: \\.\pipe\wowiezz_pipe
// Note: NO direct IL2CPP access - delegates to game thread
```

### Game Threads

Original game threads that have various hooks installed:
- `ScriptService.ExecuteScriptInstance` - Script execution
- `LocalConnectionToServer.Send` - Network packets

## Whitelist System

The whitelist is the core security mechanism:

```cpp
// ptoria/scriptservice.cpp
std::vector<BaseScript*> ScriptService::whitelisted;
```

**How it works:**

1. Scripts are added to `whitelisted` before execution
2. When `ExecuteScriptInstance` is called, the hook checks if the script is whitelisted
3. Only whitelisted scripts get the custom Lua environment
4. Game scripts run unmodified

```cpp
DynValue* ScriptService::ExecuteScriptInstanceHook(ScriptService* self, Script* script, BaseScript* instance) {
    if (std::find(whitelisted.begin(), whitelisted.end(), instance) == whitelisted.end()) {
        // Not whitelisted - run original without modifications
        return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
    }
    
    // Whitelisted - inject custom environment
    InstallEnvironnement(script);
    
    return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
}
```

## Component Interactions

### Script Execution Flow

```
┌──────────────┐      ┌───────────────────────┐      ┌─────────────────┐
│ External     │      │  ScriptService        │      │  MoonSharp      │
│ Tool/Pipe    │─────►│  RunScript<T>()       │─────►│  Interpreter    │
└──────────────┘      └───────────────────────┘      └────────┬────────┘
                                                               │
                              ┌───────────────────────────────┘
                              ▼
                    ┌───────────────────────┐
                    │ InstallEnvironnement  │
                    │ • Register callbacks  │
                    │ • Add custom globals  │
                    └───────────────────────┘
```

### Drawing Flow

```
┌──────────────┐      ┌───────────────────────┐      ┌─────────────────┐
│ Lua Script  │      │  Draw Commands        │      │  DX11 Render   │
│ draw_*()     │─────►│  queued to buffer     │─────►│  Thread         │
└──────────────┘      └───────────────────────┘      └─────────────────┘
                              │                        │
                              ▼                        ▼
                    ┌─────────────────┐      ┌─────────────────────┐
                    │ mutex-protected │      │  ImGui::GetFore-   │
                    │ command queue   │      │  groundDrawList()  │
                    └─────────────────┘      └─────────────────────┘
```

### Mouse Control Flow

```
┌──────────────┐      ┌───────────────────────┐      ┌─────────────────┐
│ Lua Script  │      │  Mouse Functions      │      │  Win32 API     │
│ mouse_move  │─────►│  (ptoria/mouse*)       │─────►│  SetCursorPos   │
│ mouse_click │      │  + Unity Cursor API    │      │  mouse_event    │
└──────────────┘      └───────────────────────┘      └─────────────────┘
```

## Memory Layout

### Key Global Objects

| Object | Type | Purpose |
|--------|------|---------|
| `ScriptService::whitelisted` | `std::vector<BaseScript*>` | Whitelisted scripts |
| `DrawingLib::commands` | `std::vector<DrawCommand>` | Pending draw commands |
| `DrawingLib::commandMutex` | `std::mutex` | Thread synchronization |

### IL2CPP Objects

Game objects are accessed via IL2CPP reflection:
- All objects are garbage collected
- References can become invalid between frames
- Must call `Unity::ThreadAttach()` on new threads

## Build System

The project uses [xmake](https://xmake.io/) as its build system:

```bash
# Debug build
xmake config -m debug
xmake build

# Release build  
xmake config -m release
xmake build
```

### Build Targets

| Target | Output | Purpose |
|--------|--------|---------|
| `wowiezz.dll` | Main DLL | Core executor |
| `injector.exe` | Executable | DLL injector |
| `version.dll` | DLL | HWID spoofer |

## Error Handling

The executor uses [`nasec::Assert()`](nasec/assert.h) for critical error handling:

```cpp
// Example: Assert with stack trace
nasec::Assert(method != nullptr, "Failed to get method for hooking");
```

Features:
- Stack traces via `StackWalk64`
- File and line information
- Halts execution on failure
