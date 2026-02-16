# Polyhack v3

<div align="center">
    <img src="polyhack.png">
    

![Polyhack Banner](https://img.shields.io/badge/Polyhack-v3.0-blue?style=for-the-badge)

**A Modern Script Executor for Polytoria**

[Features](#features) • [Architecture](#architecture) • [Source Tree](#source-tree) • [Building](#building) • [Why v3](#why-v3)

</div>

---

## What is Polyhack?

Polyhack is a sophisticated script executor designed for [Polytoria](https://polytoria.com), a user-generated content game platform similar to Roblox. Built as a DLL injection-based tool, Polyhack enables users to execute custom Lua scripts within the game environment, providing powerful capabilities for testing, debugging, and extending gameplay functionality.

The project leverages the MoonSharp Lua interpreter to execute scripts within Polytoria's scripting environment, with comprehensive bindings to the game's internal API through Unity engine reflection.

---

## Features

### 🚀 Script Execution
- **Custom Lua Environment** - Extended Lua environment with custom functions for game manipulation
- **loadstring()** - Dynamic code execution from strings
- **Named Pipe Server** - Execute scripts from external tools via Windows named pipes
- **Whitelist System** - Scripts are properly whitelisted to bypass security checks

### 🎮 Game Interaction
- **Tool Manipulation** - `activatetool()`, `equiptool()`, `unequiptool()`, `serverequiptool()`
- **Chat Integration** - `sendchat()` function for programmatic chat messages
- **Click Detection** - `fireclickdetector()` for remote interaction
- **Executor Identification** - `identifyexecutor()` "elcapor" by default

### 🔍 Instance Explorer
- **Real-time Hierarchy** - Browse the game's instance tree in real-time
- **Property Inspector** - View and modify properties on any game instance
- **Performance Optimized** - Intelligent caching system to minimize Unity API calls
- **Icon Support** - Visual icons for different instance types

### 📦 Packet Logger
- **Network Monitoring** - Capture and inspect network packets
- **Message Analysis** - View incoming and outgoing game messages
- **Debugging Support** - Essential for understanding game protocols

### 🎨 Modern UI
- **ImGui-Based Interface** - Clean, professional overlay rendered via DirectX 11 hooks
- **Toggleable Overlay** - Press DELETE key to show/hide the UI
- **Cursor Management** - Automatic cursor state handling when UI is active
- **Custom Fonts** - SNPro font family for polished typography
- **Syntax Highlighting** - Full-featured text editor with Lua syntax highlighting

### 🔧 Developer Tools
- **Memory Editor** - Inspect and edit game memory
- **Save Instance** - Export game instances to files
- **Script Source Viewer** - View source code of game scripts

---

## Architecture

Polyhack v3 follows a modular architecture designed for maintainability and extensibility:

```
┌─────────────────────────────────────────────────────────────┐
│                      Polyhack v3                            │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐  │
│  │   DLL       │  │   Cheat     │  │        UI           │  │
│  │   Entry     │──│   Core      │──│   (ImGui/DX11)      │  │
│  └─────────────┘  └─────────────┘  └─────────────────────┘  │
│         │                │                    │              │
│         ▼                ▼                    ▼              │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              Hooking Layer (Detours)                 │    │
│  │  • DirectX 11 Present Hook                          │    │
│  │  • ScriptService Hooks                              │    │
│  │  • Network Hooks                                    │    │
│  └─────────────────────────────────────────────────────┘    │
│                          │                                   │
│                          ▼                                   │
│  ┌─────────────────────────────────────────────────────┐    │
│  │           Polytoria Bindings (ptoria/)              │    │
│  │  • Instance, Game, Player, Tool                     │    │
│  │  • ScriptService, NetworkEvent                      │    │
│  └─────────────────────────────────────────────────────┘    │
│                          │                                   │
│                          ▼                                   │
│  ┌─────────────────────────────────────────────────────┐    │
│  │           Unity Engine Interface (unity/)           │    │
│  │  • Type System • Reflection • Memory Access         │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

---

## Source Tree

```
polytoria-executor/
├── xmake.lua              # Build configuration (xmake build system)
├── .editorconfig          # Editor configuration
├── .gitignore             # Git ignore rules
│
├── dll/                   # DLL Entry Point
│   └── entry.cpp          # DllMain - injection entry point
│
├── cheat/                 # Core Cheat Logic
│   ├── cheat.cpp          # Main thread initialization
│   ├── cheat.h            # Main function declarations
│   ├── pipe.cpp           # Named pipe server for external scripts
│   ├── pipe.h             # Pipe server interface
│   └── ingameui.h         # In-game Lua UI definition
│
├── core/                  # Core Utilities
│   ├── core.cpp           # Console initialization, logging setup
│   └── core.h             # Core function declarations
│
├── hooking/               # Hooking Infrastructure
│   ├── dx11hook.cpp       # DirectX 11 swap chain hooking
│   ├── dx11hook.h         # DX11 hook interface
│   └── hookmanager.h      # Detours-based hook management system
│
├── ptoria/                # Polytoria Game Bindings
│   ├── instance.cpp/h     # Base Instance class binding
│   ├── game.cpp/h         # Game singleton access
│   ├── player.cpp/h       # Player class binding
│   ├── tool.cpp/h         # Tool class binding
│   ├── scriptservice.cpp/h # Script execution hooks
│   ├── scriptinstance.h   # Script instance type
│   ├── basescript.cpp/h   # Base script class
│   ├── networkevent.cpp/h # Network event handling
│   ├── netmessage.cpp/h   # Network message types
│   ├── chatservice.h      # Chat service binding
│   └── singleton.h        # Singleton pattern utilities
│
├── ui/                    # User Interface (ImGui)
│   ├── ui.cpp/h           # Main UI setup and rendering
│   ├── executor.cpp/h     # Script executor tab
│   ├── explorer.cpp/h     # Instance explorer tab
│   ├── packetlog.cpp/h    # Packet logger tab
│   ├── about.cpp/h        # About dialog
│   ├── settingsui.cpp/h   # Settings interface
│   ├── saveinstance.cpp/h # Instance saving functionality
│   ├── scriptsource.cpp/h # Script source viewer
│   ├── texeditor.cpp/h    # Text editor with syntax highlighting
│   ├── memoryeditor.h     # Memory editor widget
│   ├── iconmanager.cpp/h  # Icon loading and management
│   ├── premiumstyle.cpp/h # Custom ImGui styling
│   ├── languagesdefiniton.cpp # Syntax highlighting definitions
│   └── embedded_icons.h   # Embedded icon data
│
├── mirror/                # Network Mirroring/Hooks
│   ├── hooks.cpp/h        # General hooks
│   ├── localconnectiontoserver.cpp/h # Server connection hooks
│   └── networkclient.cpp/h # Network client hooks
│
├── moonsharp/             # MoonSharp Lua Interpreter
│   └── ...                # Lua interpreter integration
│
├── unity/                 # Unity Engine Interface
│   └── ...                # Unity type system, reflection
│
├── nasec/                 # Security/Assertion Utilities
│   └── ...                # Assertions and safety checks
│
├── network/               # Network Utilities
│   └── ...                # Network helper functions
│
├── fonts/                 # Font Assets
│   └── SNPro-*.ttf        # SNPro font family (various weights)
│
├── icons/                 # SVG Icons
│   └── *.svg              # Instance type icons (vector)
│
├── icons_png/             # PNG Icons
│   └── *.png              # Instance type icons (raster)
│
├── scripts/               # Utility Scripts
│   └── ...                # Helper scripts
│
├── injector/              # DLL Injector
│   └── main.cpp           # Standalone injector executable
│
├── WpfUI/                 # WPF UI Components
│   └── ...                # Windows Presentation Foundation UI
│
└── .github/               # GitHub Configuration
    └── ...                # Workflows, templates
```

---

## Building

### Prerequisites

- **Windows 10/11** (x64)
- **Visual Studio 2022** with C++23 support
- **xmake** build system
- **Git** for cloning dependencies

### Build Steps

```bash
# Clone the repository
git clone https://github.com/ElCapor/polytoria-executor.git
cd polytoria-executor

# Configure and build
xmake config -m release
xmake build

# Output DLL will be in build/ directory
```

### Dependencies (automatically managed by xmake)

- **Microsoft Detours** - Function hooking library
- **ImGui** - Immediate mode GUI
- **spdlog** - Fast C++ logging library
- **Boost** - C++ utilities (regex support)
- **nativefiledialog-extended** - Native file dialogs
- **DirectX 11** - Graphics API

---

## Why v3 is the Best

Polyhack v3 represents a complete rewrite from the ground up, incorporating lessons learned from previous versions and leveraging modern C++ practices.

### 1. Modern C++23

```cpp
// Previous versions used older C++ standards
// v3 leverages C++23 features for cleaner, safer code

// Ranges and views for cleaner iteration
for (const auto key : holderMap | std::views::keys) 
    Disable(key);

// Template metaprogramming for type-safe Unity bindings
template<typename T>
static void RunScript(std::string source) {
    BaseScript *script = BaseScript::New<T>(source);
    whitelisted.push_back(script);
    RunScript(script);
}
```

### 2. Modular Architecture

Previous versions were monolithic, making maintenance difficult. v3 separates concerns:

| Module | Responsibility |
|--------|---------------|
| `ptoria/` | Game-specific bindings |
| `unity/` | Engine abstraction layer |
| `hooking/` | Hook infrastructure |
| `ui/` | User interface |
| `mirror/` | Network interception |

This separation allows:
- Easier debugging
- Independent testing of components
- Cleaner code organization
- Better collaboration

### 3. Performance Optimizations

**Explorer Caching System:**
```cpp
namespace ExplorerCache {
    struct InstanceCache {
        std::string name;
        std::string className;
        std::string fullName;
        bool isPopulated = false;
    };
    
    static std::unordered_map<Instance*, InstanceCache> s_cache;
    static std::mutex s_cacheMutex;
}
```

The v3 explorer caches instance metadata to avoid repeated Unity API calls, significantly improving UI responsiveness.

### 4. Robust Hook Management

The new [`HookManager`](hooking/hookmanager.h) class provides:

- **Type-safe hook installation** - Template-based API
- **Automatic cleanup** - `DetachAll()` for clean unloading
- **Thread safety** - Mutex-protected hook registry
- **Multiple calling conventions** - Support for `__cdecl`, `__stdcall`, `__fastcall`, `__vectorcall`

```cpp
// Clean hook installation
HookManager::Install(originalFunction, hookFunction);

// Call original safely
auto result = HookManager::Call(hookFunction, args...);

// Clean detachment
HookManager::Detach(hookFunction);
```

### 5. Named Pipe Script Execution

v3 introduces a named pipe server for external script execution:

```cpp
// External tools can send scripts to execute
HANDLE hPipe = CreateFileA(
    "\\\\.\\pipe\\polyhack_pipe",
    GENERIC_WRITE, 0, nullptr,
    OPEN_EXISTING, 0, nullptr
);
WriteFile(hPipe, scriptCode, length, &bytesWritten, nullptr);
```

This enables:
- Integration with external editors
- Automated testing workflows
- Remote script execution

### 6. Professional UI

- **Custom ImGui styling** with premium appearance
- **Syntax-highlighted code editor** with Lua support
- **Memory editor** for advanced debugging
- **Responsive layout** with proper cursor management

### 7. Comprehensive Game Bindings

v3 includes complete bindings for Polytoria's type system:

- `Instance` - Base game object
- `Game` - Game singleton
- `Player` - Player entities
- `Tool` - Equippable tools
- `ScriptService` - Script execution
- `NetworkEvent` - Network communication
- `ChatService` - Chat functionality

### 8. Safety Features

```cpp
// Assertions with descriptive messages
nasec::Assert(gameInstance != nullptr, "Failed to get Game singleton instance");

// Proper DLL unloading
case DLL_PROCESS_DETACH:
    StopPipeServer();
    IconManager::Shutdown();
    break;
```

### 9. Extensible Custom Lua Environment

v3 provides a rich set of custom functions:

| Function | Description |
|----------|-------------|
| `loadstring(source)` | Compile and execute Lua code |
| `activatetool(tool)` | Activate a tool |
| `equiptool(tool)` | Equip a tool locally |
| `unequiptool(tool)` | Unequip a tool |
| `serverequiptool(player, tool)` | Server-side tool equipping |
| `sendchat(message)` | Send chat message |
| `fireclickdetector(instance)` | Fire click detector |
| `identifyexecutor()` | Returns "elcapor" |

### 10. Clean Build System

Using xmake instead of Visual Studio projects:

```lua
target("wowiezz")
    set_kind("shared")
    set_languages("c++23")
    add_packages("spdlog", "microsoft-detours", "imgui", "boost")
    add_links("user32", "dbghelp", "d3d11")
```

Benefits:
- Cross-platform potential
- Reproducible builds
- Automatic dependency management
- Simpler configuration

---

## Usage

1. **Inject the DLL** into the Polytoria game process using the injector or any DLL injector
2. **Press DELETE** to toggle the UI overlay
3. **Use the Executor tab** to write and execute Lua scripts
4. **Use the Explorer tab** to browse and inspect game instances
5. **Use the Packet Log tab** to monitor network traffic

### Example Scripts

```lua
-- Print all game children
for _, child in ipairs(game:GetChildren()) do
    print(child.Name)
end

-- Send a chat message
sendchat("Hello from Polyhack!")

-- Execute dynamic code
local func = loadstring("return 2 + 2")
print(func()) -- Output: 4
```

---

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting PRs.

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

---

## License

CC-BY-SA 4.0 Copyright (c) 2026 @ElCapor

---

## Disclaimer

This tool is intended for educational purposes and legitimate game testing only. Use responsibly and in accordance with Polytoria's Terms of Service. The developers are not responsible for any consequences resulting from misuse of this software.

---

<div align="center">

**Made with ❤️ by the Polyhack Team**

</div>
