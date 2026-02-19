# Lua Environment :material-language-lua:

The **Polytoria Executor** provides an extended Lua environment based on MoonSharp, deeply integrated into the game's script runtime. This section explores the capabilities of the custom Lua layer, its security model, and how it interacts with the Unity engine.

---

## :material-eye: Overview

The executor doesn't just run Lua; it enhances the existing game environment. By hooking into the `ScriptService`, we inject powerful custom functions that are not natively available to game scripts.

### :material-cog-sync: How It Works

1.  **Script Interception**: The executor hooks `ScriptService.ExecuteScriptInstance`.
2.  **Security Check**: The [Whitelist System](../internals/architecture.md#whitelist-system) verifies the script pointer.
3.  **Environment Injection**: Custom functions are registered via `InstallEnvironnement()`.
4.  **Standard Lua Support**: Standard Lua 5.2 / MoonSharp functions are preserved.

---

## :material-view-list: Environment Modules

The custom Lua environment is organized into several key modules. 

!!! tip "Navigating the Modules"
    Click on any module title below to view the detailed function reference for that category.

| Module | Description | Key Functions |
| :--- | :--- | :--- |
| **[:material-cog: Core](functions/core.md)** | Core execution and identification | `loadstring`, `identifyexecutor` |
| **[:material-tools: Tooling](functions/tool.md)** | Game tool interaction | `equiptool`, `activatetool` |
| **[:material-chat: Interaction](functions/chat.md)** | Social and world interaction | `sendchat` |
| **[:material-mouse: Input](functions/mouse.md)** | Direct hardware simulation | `mouse_move`, `mouse_click` |
| **[:material-format-paint: Drawing](functions/drawing.md)** | Real-time overlay graphics | `draw_line`, `draw_text` |
| **[:material-lan: Networking](functions/network.md)** | Game protocol interaction | `fireclickdetector` |
| **[:material-web: HTTP](functions/http.md)** | External web communication | `httpget` |

---

## :material-cogs: MoonSharp Integration

We use [MoonSharp](http://www.moonsharp.org/) as our primary Lua engine. This choice provides several advantages:

-   **Seamless C++ Bridge**: Fast communication between Lua and the executor's C++ core.
-   **Safe Execution**: Sandboxed environment for script execution.
-   **Robust Coroutines**: Support for complex asynchronous scripts without blocking the game.
-   **Dynamic Typing**: Flexible handling of Unity game objects as user data.

---

## :material-book-open-variant: Learning Path

Are you ready to start scripting? Follow our guided path:

-   [:material-baby-face-outline: **Beginner Guide**](beginner.md) - From "Hello World" to your first game script.
-   [:material-vlc: **Advanced Guide**](advanced.md) - Master high-level techniques and performance optimization.
-   [:material-api: **Functions Reference**](functions.md) - Complete alphabetical list of every custom function.

