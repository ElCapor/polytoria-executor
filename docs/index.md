# Polytoria Executor Documentation :material-shield-check:

Welcome to the official documentation for **Polytoria Executor** (Polyhack v3). 
This documentation covers the Lua scripting environment, custom functions, and the internal architecture of the executor.

## :material-star: What is Polytoria Executor?

Polytoria Executor is a high-performance C++23 DLL designed for injection into Polytoria game processes. It bridges the gap between low-level Unity IL2CPP and the Lua scripting environment.

!!! quote "Core Philosophy"
    Reliability, Security, and Ease of Use.

---

<div class="grid cards" markdown>

-   :material-language-lua: __Custom Lua Environment__

    ---

    Extended Lua scripting with game-specific functions and a robust whitelist system.

    [:octicons-arrow-right-24: Lua Reference](lua/functions.md)

-   :material-monitor-screenshot: __ImGui UI Overlay__

    ---

    Professional in-game menu accessed via `DELETE` key for script management and options.

    [:octicons-arrow-right-24: Getting Started](lua/beginner.md)

-   :material-lan-connect: __Network Interception__

    ---

    Deep hooks into Mirror Networking for advanced packet capture and modification.

    [:octicons-arrow-right-24: Networking](lua/functions/network.md)

-   :material-pipe: __Named Pipe Server__

    ---

    Seamless integration with external tools via standard named pipe communication.

    [:octicons-arrow-right-24: External Tools](internals/architecture.md#pipe-server-thread)

</div>

---

## :material-lightning-bolt: Quick Setup

### Installation

!!! info "Step 1: Build"
    Build the project using `xmake`:
    `xmake config -m release && xmake build`

!!! info "Step 2: Install"
    Copy everything from the `.download/` directory into your game root.

!!! info "Step 3: Inject"
    Run `injector.exe` or use `version.dll` proxying for automatic loading.

!!! info "Step 4: Control"
    Press :octicons-key-24: **DELETE** to toggle the UI overlay in-game.

---

## :material-table: Key Features at a Glance

| Category | Features |
| :--- | :--- |
| **Tool Management** | `equiptool`, `unequiptool`, `activatetool` |
| **Input Simulation** | Full `mouse_*` control and coordinate tracking |
| **Visuals** | 2D Drawing API (Lines, Rects, Circles, Text) |
| **Integration** | `httpget` requests and unified `sendchat` |
| **Performance** | Multi-threaded design with thread-safe buffers |

---

## :material-hammer-wrench: Architecture Highlights

- **Thread-Safe Design** - Mutex-protected drawing buffers and safe thread handovers.
- **Whitelist Security** - Granular control over which scripts can access custom APIs.
- **IL2CPP Reflection** - Native Unity runtime inspection via UnityResolve.
- **Detours Hooking** - Industrial-grade function interception using Microsoft Detours.

