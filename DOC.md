# Polyhack v3 — Complete Technical Documentation

> **Polyhack** is a DLL injection-based script executor for [Polytoria](https://polytoria.com), built in **C++23**. It injects into the game process, hooks into Unity's IL2CPP runtime and DirectX 11 rendering pipeline, and exposes a custom Lua scripting environment (MoonSharp) with game-manipulation functions, an ImGui overlay UI, a packet logger, and an HWID spoofer for multi-instance support.

---

## Table of Contents

- [High-Level Architecture](#high-level-architecture)
- [Build System](#build-system)
- [Build Targets](#build-targets)
- [Repository Structure](#repository-structure)
- [Initialization Flow](#initialization-flow)
- [Module Reference](#module-reference)
  - [dll/ — DLL Entry Point](#dll--dll-entry-point)
  - [core/ — Core Utilities](#core--core-utilities)
  - [cheat/ — Main Thread & Pipe Server](#cheat--main-thread--pipe-server)
  - [hooking/ — Hook Infrastructure](#hooking--hook-infrastructure)
  - [unity/ — Unity IL2CPP Interface](#unity--unity-il2cpp-interface)
  - [nasec/ — Assertions & Compile-Time Utilities](#nasec--assertions--compile-time-utilities)
  - [ptoria/ — Polytoria Game Bindings](#ptoria--polytoria-game-bindings)
  - [moonsharp/ — MoonSharp Lua Bindings](#moonsharp--moonsharp-lua-bindings)
  - [mirror/ — Network Interception (Mirror Networking)](#mirror--network-interception-mirror-networking)
  - [network/ — Network Protocol Utilities](#network--network-protocol-utilities)
  - [ui/ — ImGui User Interface](#ui--imgui-user-interface)
  - [hwidspoofer/ — HWID Spoofer & Multi-Client](#hwidspoofer--hwid-spoofer--multi-client)
  - [injector/ — Standalone DLL Injector](#injector--standalone-dll-injector)
  - [scripts/ — Build-Time Python Utilities](#scripts--build-time-python-utilities)
  - [WpfUI/ — External WPF UI](#wpfui--external-wpf-ui)
- [Custom Lua Environment](#custom-lua-environment)
- [Key Design Patterns](#key-design-patterns)
- [Data Flow Diagrams](#data-flow-diagrams)
- [Dependencies](#dependencies)

---

## High-Level Architecture

```
┌──────────────────────────────────────────────────────────────────────┐
│                          Polyhack v3 DLL                            │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  DllMain (dll/entry.cpp)                                             │
│    ├─► main_thread (cheat/cheat.cpp)                                 │
│    │     ├─► OpenConsole()            ← core/core.cpp                │
│    │     ├─► Wait for GameAssembly.dll                                │
│    │     ├─► Unity::Init()            ← unity/unity.cpp              │
│    │     ├─► Wait for Game singleton                                 │
│    │     ├─► ScriptService::InstallHooks() ← ptoria/scriptservice    │
│    │     ├─► mirror::InstallHooks()   ← mirror/hooks.cpp             │
│    │     ├─► StartPipeServer()        ← cheat/pipe.cpp               │
│    │     └─► UI::Setup()              ← ui/ui.cpp                    │
│    │           └─► DX11 SwapChain Hook → ImGui overlay               │
│    │                                                                 │
│    └─► DLL_PROCESS_DETACH                                            │
│          ├─► StopPipeServer()                                        │
│          └─► IconManager::Shutdown()                                 │
│                                                                      │
│  ┌───────────────┐ ┌───────────────┐ ┌───────────────────────────┐   │
│  │  Unity Layer  │ │  Hooking      │ │  UI Layer (ImGui/DX11)    │   │
│  │  (il2cpp,     │ │  (Detours,    │ │  Explorer, Executor,      │   │
│  │  reflection)  │ │  HookManager) │ │  PacketLog, Settings,     │   │
│  └───────────────┘ └───────────────┘ │  About, SaveInstance,     │   │
│                                       │  ScriptSource, Memory)    │   │
│  ┌───────────────┐ ┌───────────────┐ └───────────────────────────┘   │
│  │  Ptoria       │ │  MoonSharp    │                                 │
│  │  (Game, Inst, │ │  (Script,     │ ┌───────────────────────────┐   │
│  │  Player, Tool │ │  DynValue,    │ │  Mirror / Network         │   │
│  │  ScriptSvc)   │ │  CInterface)  │ │  (Packet capture/block)   │   │
│  └───────────────┘ └───────────────┘ └───────────────────────────┘   │
└──────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────┐
│  version.dll (HWID Spoofer)                                          │
│    ├─► Hook LoadLibraryW → detect GameAssembly.dll loading           │
│    ├─► Hook GetDeviceUniqueID → return scrambled HWID                │
│    ├─► Hook CreateMutexA → bypass single-instance lock               │
│    └─► Auto-load wowiezz.dll when GameAssembly is ready              │
└──────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────┐
│  injector.exe — Standalone CreateRemoteThread DLL injector           │
│    ├─► Finds "Polytoria Client.exe" by process name                  │
│    ├─► Waits 6 seconds, then injects wowiezz.dll                     │
│    └─► Supports custom process name and DLL path via CLI args        │
└──────────────────────────────────────────────────────────────────────┘
```

---

## Build System

**Build tool:** [xmake](https://xmake.io) — `xmake.lua` at the repo root.

| Setting | Value |
|---------|-------|
| Architecture | x64 only |
| Language | C++23 |
| Modes | `debug`, `release` |

### Build Commands

```bash
# Configure
xmake config -m release

# Build all targets
xmake build

# Build specific target
xmake build wowiezz
xmake build injector
xmake build version
```

### Optional Pre-Build Step — Font/Icon Embedding

```bash
# Convert TTF fonts to embedded C++ arrays
python scripts/ttf_to_header.py fonts/ ui/embeddedfonts.h ui/embeddedfonts.cpp

# Convert SVG icons to PNG, then to embedded C++ arrays
python scripts/svg_to_png.py icons/ icons_png/
python scripts/png_to_header.py icons_png/ ui/embedded_icons.h
```

---

## Build Targets

| Target | Kind | Output | Description |
|--------|------|--------|-------------|
| `wowiezz` | `shared` (DLL) | `.download/wowiezz.dll` | Main executor DLL — the core of Polyhack |
| `injector` | `binary` (EXE) | `.download/injector.exe` | Standalone DLL injector |
| `version` | `shared` (DLL) | `.download/version.dll` | HWID Spoofer + Multi-client DLL proxy |

### Target Compilation Scope

- **wowiezz**: Compiles `**.cpp` **excluding** `injector/*.cpp`, `hwidspoofer/proxy.cpp`, `hwidspoofer/hwidspoofer.cpp`, `hwidspoofer/multiclient.cpp`
- **injector**: Compiles only `injector/main.cpp`
- **version**: Compiles `hwidspoofer/proxy.cpp`, `hwidspoofer/hwidspoofer.cpp`, `hwidspoofer/multiclient.cpp`, `unity/unity.cpp`, `core/core.cpp`

---

## Repository Structure

```
polytoria-executor/
├── xmake.lua                  # Build configuration
├── .editorconfig              # Editor formatting rules
├── .github/workflows/build.yml # CI/CD pipeline
│
├── dll/                       # DLL Entry Point
│   └── entry.cpp              # DllMain → spawns main_thread
│
├── core/                      # Core Utilities
│   ├── core.cpp/h             # OpenConsole(), spdlog init
│   ├── filesys.cpp/h          # File dialogs (NFD), read/write, open URL
│
├── cheat/                     # Main Cheat Logic
│   ├── cheat.cpp/h            # main_thread() — full init sequence
│   ├── pipe.cpp/h             # Named pipe server (\\.\pipe\wowiezz_pipe)
│   └── ingameui.h             # In-game Lua UI (raw Lua string)
│
├── hooking/                   # Hooking Infrastructure
│   ├── hookmanager.h          # Template-based Detours wrapper (Install/Call/Detach)
│   ├── dx11hook.cpp/h         # DX11 SwapChain Present + ResizeBuffers hooks
│
├── unity/                     # Unity Engine IL2CPP Interface
│   ├── unity.cpp/h            # Unity::Init(), GetClass<>, GetMethod<>, field access
│   ├── unity_internal.h       # UnityResolve library (3rd-party, 100k+ lines)
│   ├── il2cpp.h               # IL2CPP internal types, MethodInfo creation, delegate binding
│   └── object.h               # CRTP Object<> base, StaticClass<T>(), ObjectParams
│
├── nasec/                     # Security / Metaprogramming
│   ├── assert.h               # nasec::Assert() with stack traces (via dbghelp)
│   └── meta.h                 # Compile-time String<N> for template NTTPs
│
├── ptoria/                    # Polytoria Game-Specific Bindings
│   ├── instance.cpp/h         # InstanceBase: Name(), Children(), FullName(), CmdClicked()
│   ├── game.cpp/h             # Game singleton: GameName(), GameID()
│   ├── player.cpp/h           # Player: EquipTool()
│   ├── tool.cpp/h             # Tool: CmdActivate(), CmdEquip(), CmdUnequip()
│   ├── basescript.cpp/h       # BaseScript: New<T>(), Source(), Running(), SetSource()
│   ├── scriptinstance.h       # ScriptInstance type (derives BaseScript)
│   ├── scriptservice.cpp/h    # ScriptService: hooks, whitelist, custom Lua env
│   ├── networkevent.cpp/h     # NetworkEvent: InvokeServer()
│   ├── netmessage.cpp/h       # NetworkMessage types
│   ├── chatservice.h          # ChatService: SendChat()
│   └── singleton.h            # Singleton<T> via static field "singleton"
│
├── moonsharp/                 # MoonSharp Lua Interpreter Bindings
│   ├── cinterface.cpp/h       # RegisterCallback() — bridges C++ → MoonSharp Lua
│   ├── script.cpp/h           # Script: Globals(), Registry(), loadstring()
│   ├── dynvalue.cpp/h         # DynValue: FromString(), FromCallback(), FromNil(), Type()
│   ├── closure.cpp/h          # Closure function wrapper
│   ├── table.cpp/h            # Table: Set(), RawGet()
│   ├── callbackarguments.cpp/h # CallbackArguments: Count(), RawGet()
│   └── scriptexecutioncontext.cpp/h # ScriptExecutionContext: OwnerScript()
│
├── mirror/                    # Mirror Networking Hooks
│   ├── hooks.cpp/h            # mirror::InstallHooks() — hooks Send()
│   ├── localconnectiontoserver.cpp/h # SendHook — packet capture & blocking
│   └── networkclient.cpp/h    # NetworkClient bindings
│
├── network/                   # Network Protocol Parsing
│   ├── arraysegment.h         # ArraySegment wrapper (il2cpp managed array)
│   ├── commandmessage.cpp/h   # CommandMessage deserialization
│   ├── deserializer.cpp/h     # Packet deserialization logic
│   ├── networkmessage.h       # NetworkMessage type definitions
│   ├── networkstream.cpp      # NetworkStream: read primitives, print payload
│   └── stream.h               # Stream interface with read position tracking
│
├── ui/                        # ImGui User Interface
│   ├── ui.cpp/h               # UI::Setup(), Draw(), WndProc hook, state machine
│   ├── executor.cpp/h         # Script executor tab (text editor + execute)
│   ├── explorer.cpp/h         # Instance tree explorer (cached, icon support)
│   ├── packetlog.cpp/h        # Packet logger tab (capture/filter/intercept)
│   ├── settingsui.cpp/h       # Settings tab (keybind, input blocking)
│   ├── about.cpp/h            # About dialog with credits
│   ├── saveinstance.cpp/h     # Save game instance to file
│   ├── scriptsource.cpp/h     # Decompile and view script source
│   ├── texeditor.cpp/h        # Full text editor with syntax highlighting
│   ├── texteditor.h           # TextEditor class (ImGuiColorTextEdit)
│   ├── memoryeditor.h         # Memory editor widget (hex editor)
│   ├── premiumstyle.cpp/h     # Custom dark theme + font loading
│   ├── iconmanager.cpp/h      # Icon texture loading + management
│   ├── languagesdefiniton.cpp # Lua syntax highlighting definitions
│   ├── embedded_icons.h       # Embedded PNG icon data (binary arrays)
│   └── embeddedfonts.cpp/h    # Embedded SNPro font data (binary arrays)
│
├── hwidspoofer/               # HWID Spoofer & Multi-Client
│   ├── hwidspoofer.cpp/h      # Main spoofer thread: hooks LoadLibraryW, GetDeviceUniqueID
│   ├── multiclient.cpp/h      # CloseAllMutexesInCurrentProcess() via NtQuerySystemInformation
│   └── proxy.cpp              # version.dll DLL proxy (forwards real version.dll exports)
│
├── injector/                  # Standalone DLL Injector
│   └── main.cpp               # CreateRemoteThread injection into Polytoria Client.exe
│
├── scripts/                   # Build-Time Python Utilities
│   ├── ttf_to_header.py       # TTF → C++ embedded arrays
│   ├── svg_to_png.py          # SVG → PNG conversion (with rasterization)
│   ├── png_to_header.py       # PNG → C++ embedded arrays
│   └── requirements.txt       # Python deps (Pillow, cairosvg)
│
├── fonts/                     # SNPro Font Family (16 TTF files)
├── icons/                     # SVG Instance Type Icons (112 files)
├── icons_png/                 # Rasterized PNG Icons
├── WpfUI/                     # External WPF UI (C# .NET 8, optional)
├── .download/                 # Pre-built binaries / build output staging
└── screenshots/               # Demo screenshots/GIFs
```

---

## Initialization Flow

The complete startup sequence when `wowiezz.dll` is injected:

### 1. DLL Entry (`dll/entry.cpp`)

```
DllMain(DLL_PROCESS_ATTACH)
  └─► std::thread(main_thread).detach()
```

`DllMain` spawns `main_thread()` on a detached thread to avoid blocking the loader lock.

### 2. Main Thread (`cheat/cheat.cpp`)

```
main_thread():
  1. OpenConsole()                        — Allocate debug console, configure spdlog
  2. Wait for GameAssembly.dll            — Spin until IL2CPP runtime is loaded
  3. Unity::Init()                        — Initialize UnityResolve with IL2CPP mode
  4. Unity::ThreadAttach()                — Attach current thread to IL2CPP GC domain
  5. Get Assembly-CSharp.dll              — Load game assembly for reflection
  6. Wait for Game singleton              — Poll Game::singleton static field
  7. Set UI::state = Ready                — Signal UI can start rendering
  8. Resolve Instance, Game classes       — Cache UnityClass pointers
  9. Get Game singleton instance          — Verify and log game metadata
  10. ScriptService::InstallHooks()       — Hook ExecuteScriptInstance via Detours
  11. mirror::InstallHooks()              — Hook LocalConnectionToServer.Send
  12. StartPipeServer()                   — Start named pipe for external scripts
  13. UI::Setup()                         — Hook DX11 SwapChain, install WndProc hook
```

### 3. UI Thread (runs on DX11 render thread)

```
HookPresent() [first call]:
  1. Unity::ThreadAttach()                — This thread also needs IL2CPP access
  2. Cache swap chain, device, context
  3. Get window handle from swap chain desc
  4. Hook WndProc (SetWindowLongPtr)
  5. Initialize ImGui context
  6. PremiumStyle::Initialize()           — Load fonts, apply dark theme
  7. IconManager::LoadEmbeddedIcons()     — Load all embedded PNG icon textures
  8. ImGui_ImplWin32_Init + ImGui_ImplDX11_Init

HookPresent() [every frame]:
  1. ImGui NewFrame
  2. UI::Draw()  →  dispatches to DrawWaitingScreen / DrawMainUI / DrawClosedHint
  3. ImGui Render
  4. Set render target, call ImGui_ImplDX11_RenderDrawData
  5. Call original Present
```

### 4. DLL Unload (`dll/entry.cpp`)

```
DllMain(DLL_PROCESS_DETACH)
  ├─► StopPipeServer()           — Signal stop, join pipe thread, cleanup handles
  └─► IconManager::Shutdown()    — Release all DX11 icon textures
```

---

## Module Reference

### dll/ — DLL Entry Point

| File | Purpose |
|------|---------|
| `entry.cpp` | `DllMain` — only responsibility is spawning `main_thread` and cleanup on detach |

**Key rule:** `DllMain` must never do heavy work (loader lock). All init is deferred to `main_thread()`.

---

### core/ — Core Utilities

| File | Key Functions |
|------|--------------|
| `core.cpp` | `OpenConsole()` — allocates a Windows console, redirects stdout/stderr/stdin, sets spdlog pattern |
| `filesys.cpp` | `SaveDialog()`, `OpenDialog()` — native file dialogs via NFD library |
| | `ReadFileAsString()`, `WriteStringToFile()` — simple file I/O |
| | `OpenUrlInBrowser()` — opens URL via `system("start ...")` |

---

### cheat/ — Main Thread & Pipe Server

#### `cheat.cpp` — `main_thread()`

The central bootstrap function. See [Initialization Flow](#initialization-flow) above.

#### `pipe.cpp` — Named Pipe Server

**Pipe name:** `\\.\pipe\wowiezz_pipe`  
**Buffer size:** 65536 bytes  
**Mode:** `PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE`, duplex, overlapped I/O

| Function | Description |
|----------|-------------|
| `StartPipeServer()` | Creates stop event, spawns `PipeServerThreadFunc` on background thread |
| `StopPipeServer()` | Signals stop event, joins thread, cleans up handles |
| `IsPipeServerRunning()` | Atomic bool check |

**Server loop:**
1. Create named pipe instance
2. `ConnectNamedPipe()` with overlapped I/O
3. `WaitForMultipleObjects()` for client connect OR stop event
4. Read message data into buffer (loop with `PeekNamedPipe`)
5. Execute via `ScriptService::RunScript<ScriptInstance>(script)`
6. Flush, disconnect, close pipe, loop

**Thread safety:** Uses `std::atomic<bool>` for `g_PipeServerRunning` and `g_PipeServerShouldStop`. Stop event (`g_hStopEvent`) used for clean async cancellation.

#### `ingameui.h` — In-Game Lua UI

Contains a raw Lua string (`const std::string ingameui`) that creates a draggable in-game executor UI using Polytoria's GUI API (`Instance.New("GUI")`, `UIView`, `UITextInput`, `UIButton`). Features:
- Execute button (calls `loadstring` on text input)
- Clear button
- Minimize/expand toggle
- Dragging via `MouseDown`/`MouseUp`/`Rendered` events

---

### hooking/ — Hook Infrastructure

#### `hookmanager.h` — `HookManager` Class

A **static, template-based wrapper** around Microsoft Detours for type-safe function hooking.

| Method | Signature | Description |
|--------|-----------|-------------|
| `Install<Fn>` | `(Fn func, Fn handler) → bool` | Attach hook, store original → handler mapping |
| `GetOrigin<Fn>` | `(Fn handler) → Fn` | Retrieve original function pointer for a hook |
| `Detach<Fn>` | `(Fn handler) → void` | Remove single hook |
| `DetachAll` | `() → void` | Remove all hooks (uses `std::views::keys`) |
| `Call<RType, Params...>` | `(handler, params...) → RType` | Call original (default calling convention) |
| `Ccall` | Same | `__cdecl` variant |
| `Scall` | Same | `__stdcall` variant |
| `Fcall` | Same | `__fastcall` variant |
| `Vcall` | Same | `__vectorcall` variant |

**Internal storage:** `std::unordered_map<void*, void*>` mapping handler → original, protected by `std::mutex`.

#### `dx11hook.cpp/h` — DirectX 11 Hooking

| Function | Description |
|----------|-------------|
| `CreateSwapChain()` | Creates dummy D3D11 device + swap chain to get vtable |
| `HookSwapChain()` | Hooks `Present` (vtable[8]) and `ResizeBuffers` (vtable[13]) via HookManager |
| `GetSwapChain/GetDevice/GetContext/GetRenderTarget` | Lazy-init accessors (static local pattern) |
| `hkResizeBuffers()` | Handles window resize: invalidate ImGui, release RTV, call original, recreate RTV + viewport |

---

### unity/ — Unity IL2CPP Interface

#### `unity.cpp/h` — Main Unity API

| Function | Description |
|----------|-------------|
| `Unity::Init()` | Gets `GameAssembly.dll` module handle, calls `UnityResolve::Init` in IL2CPP mode |
| `Unity::ThreadAttach()` | Attaches current thread to IL2CPP domain (GC registration) |
| `Unity::ThreadDetach()` | Detaches thread from IL2CPP domain |

**Template API** (all use compile-time `nasec::meta::String` for NTTP):

| Template | Description |
|----------|-------------|
| `GetAssembly<Name>()` | Get assembly by name (cached with static local) |
| `GetClass<Name, Assembly, Namespace, Parent>()` | Get class by name (cached) |
| `GetMethod<Name, Types...>(klass)` | Get method by name + optional param types (cached) |
| `GetField<Name>(klass)` | Get field by name (cached) |
| `GetFieldValue<T, Name>(klass, obj)` | Read field at offset from object |
| `SetFieldValue<T, Name>(klass, obj, value)` | Write field at offset on object |
| `GetStaticFieldValue<T, Name>(klass)` | Read static field via IL2CPP API |
| `SetStaticFieldValue<T, Name>(klass, value)` | Write static field via IL2CPP API |

**Assembly constants:**
- `AssemblyCSharp` = `"Assembly-CSharp.dll"`
- `AssemblyFirstPass` = `"Assembly-CSharp-firstpass.dll"`
- `AssemblyUnityEngine` = `"UnityEngine.CoreModule.dll"`
- `Mirror` = `"Mirror.dll"`

#### `object.h` — CRTP Type Registration System

```cpp
// Deriving from Object<T, "ClassName", "Assembly.dll"> registers a type
struct Game : public InstanceBase,
              public Object<Game, "Game", Unity::AssemblyCSharp>,
              public Singleton<Game> { ... };

// StaticClass<T>() resolves the UnityClass* via compile-time params
UnityClass* klass = StaticClass<Game>();
```

**`StaticClass<T>()`** extracts `ClassName`, `Assembly`, `Namespace`, `Parent` from the `Object<>` base via `ObjectParamsT<T>` and calls `Unity::GetClass<>()`.

**`nasec::meta::is_object<T>`** — SFINAE trait to check if a type derives from `Object`.

#### `il2cpp.h` — IL2CPP Internal Structures

Low-level IL2CPP type definitions used for:
- **`MethodInfo` construction** — `CreateCustomMethodInfo()` allocates and fills a `MethodInfo` struct for C++ functions to be callable from IL2CPP
- **`Il2CppDelegate` binding** — `BindDelegate()` wires a delegate's method pointer and invoker
- **Multicast delegate invocation** — `multicast_invoke_impl_2_params_()` iterates delegate invocation list
- Array access macros: `il2cpp_array_get`, `il2cpp_array_addr`, etc.

#### `unity_internal.h` — UnityResolve Library

Third-party header-only library (~100k lines) providing:
- IL2CPP API wrapper (assembly enumeration, class/method/field resolution)
- Type system (`UnityType::String`, `Array<T>`, `List<T>`, `Dictionary<K,V>`, etc.)
- `Cursor` control (`ShowAndUnlock`, `set_visible`, `set_lockState`)
- `GameObject` operations (`Create`, `AddComponent`)
- Thread attach/detach

---

### nasec/ — Assertions & Compile-Time Utilities

#### `assert.h` — `nasec::Assert()`

Rich assertion with:
- Expression string + custom message
- Source file, line, function name
- **Full stack trace** via `dbghelp.dll` (`StackWalk64`, `SymFromAddr`, `SymGetLineFromAddr64`)
- Outputs to `spdlog::error` and `std::cerr`
- Terminates process on failure

#### `meta.h` — Compile-Time String

```cpp
template<std::size_t N>
struct String {
    char data[N];
    constexpr String(const char (&str)[N]);
};
```

Enables strings as non-type template parameters (NTTPs) in C++20/23:
```cpp
Unity::GetMethod<"get_Name">(klass);
```

---

### ptoria/ — Polytoria Game Bindings

Type definitions for Polytoria's game object model, all using the `Object<>` CRTP pattern.

#### Inheritance Hierarchy

```
InstanceBase (raw methods: Name, Children, FullName, CmdClicked)
├── Instance : InstanceBase, Object<Instance, "Instance">
├── BaseScriptBase : InstanceBase (SetRunning, Source, SetSource, ...)
│   └── BaseScript : BaseScriptBase, Object<BaseScript, "BaseScript">
│       └── ScriptInstance : Object<ScriptInstance, "ScriptInstance">
├── Game : InstanceBase, Object<Game, "Game">, Singleton<Game>
├── Player : InstanceBase, Object<Player, "Player">
├── Tool : InstanceBase, Object<Tool, "Tool">
├── ScriptService : InstanceBase, Object<ScriptService, "ScriptService">
├── NetworkEvent : Object<NetworkEvent, "NetworkEvent">
├── ChatService : Object<ChatService, "ChatService">
└── GameIO : InstanceBase, Object<GameIO, "GameIO">, Singleton<GameIO>
```

#### Key Functions

| Class | Method | Description |
|-------|--------|-------------|
| `InstanceBase` | `Name()` | Invoke `get_Name` → `UnityString*` |
| `InstanceBase` | `Children()` | Invoke `GetChildren` → `UnityArray<InstanceBase*>*` |
| `InstanceBase` | `FullName()` | Invoke `get_FullName` → `UnityString*` |
| `InstanceBase` | `CmdClicked()` | Invoke `CmdClicked` (click detector) |
| `Game` | `GetSingleton()` | Via `Singleton<Game>` — reads `Game.singleton` static field |
| `Game` | `GameName()` | Static field `GameName` |
| `Game` | `GameID()` | Static field `gameID` |
| `Player` | `EquipTool(Tool*)` | Invoke `EquipTool` |
| `Tool` | `CmdActivate()` | Invoke `CmdActivate` |
| `Tool` | `CmdEquip()` | Invoke `CmdEquip` |
| `Tool` | `CmdUnequip()` | Invoke `CmdUnequip` |
| `BaseScript` | `New<T>(source)` | Create GameObject, AddComponent, SetSource |
| `ScriptService` | `GetInstance()` | Invoke `get_Instance` |
| `ScriptService` | `RunScript<T>(source)` | Create BaseScript, whitelist, invoke RunScript |
| `ScriptService` | `InstallHooks()` | Hook `ExecuteScriptInstance` via Detours |

#### Singleton Pattern

```cpp
template <typename T>
struct Singleton {
    static T* GetSingleton() {
        // Reads the "singleton" static field from the class
        auto uf = StaticClass<T>()->Get<UnityField>("singleton");
        uf->GetStaticValue(&instance);
        return instance;
    }
};
```

#### Script Execution Hook

`ScriptService::ExecuteScriptInstanceHook` intercepts all script executions:

1. Check if the `BaseScript*` is in the `whitelisted` vector
2. **If NOT whitelisted:** call original (game scripts run normally)
3. **If whitelisted:** call `InstallEnvironnement(script)` to inject custom Lua globals, then call original

This is the core mechanism that gives executor scripts access to custom functions while leaving game scripts untouched.

---

### moonsharp/ — MoonSharp Lua Bindings

C++ bindings to interact with MoonSharp (the Lua interpreter used by Polytoria, compiled as IL2CPP).

#### Key Types

| Type | Assembly | Namespace | Purpose |
|------|----------|-----------|---------|
| `Script` | `Assembly-CSharp-firstpass.dll` | `MoonSharp.Interpreter` | Lua script context, has `Globals()` table and `loadstring()` |
| `DynValue` | Same | Same | Dynamic Lua value (Nil, String, Number, UserData, Function, etc.) |
| `Table` | Same | Same | Lua table, `Set(key, value)` |
| `Closure` | Same | Same | Lua function wrapper |
| `CallbackArguments` | Same | Same | Argument list for C++ callbacks: `Count()`, `RawGet(index)` |
| `ScriptExecutionContext` | Same | Same | Execution context: `OwnerScript()` |

#### `RegisterCallback()` — The C++ → Lua Bridge

The most critical function in the moonsharp layer:

```cpp
void RegisterCallback(Table* globals, const std::string& name, CallbackFunction func) {
    // 1. Create a custom IL2CPP MethodInfo for our C++ function
    auto mi = il2cpp::CreateCustomMethodInfo(name, params, retType, funcPtr);

    // 2. Allocate a Func<object, ScriptExecutionContext, CallbackArguments, DynValue> delegate
    auto delegate = il2cpp_object_new(Func_3_class);

    // 3. Bind our MethodInfo to the delegate
    il2cpp::BindDelegate(delegate, mi);

    // 4. Wrap in DynValue::FromCallback and set on globals table
    globals->Set(name, DynValue::FromCallback(delegate));
}
```

This bridges C++ functions into the MoonSharp Lua environment by:
1. Creating a fake IL2CPP `MethodInfo` pointing to our native function
2. Constructing a managed delegate that wraps it
3. Converting to a MoonSharp `DynValue` callback
4. Registering on the script's globals table

---

### mirror/ — Network Interception (Mirror Networking)

Hooks the [Mirror Networking](https://mirror-networking.com/) library used by Polytoria for multiplayer.

| File | Purpose |
|------|---------|
| `hooks.cpp` | `mirror::InstallHooks()` — hooks `LocalConnectionToServer.Send()` |
| `localconnectiontoserver.cpp` | `SendHook()` — intercepts outgoing packets |
| `networkclient.cpp/h` | `NetworkClient` type bindings |

**SendHook flow:**
1. Check if packet logging is enabled in UI
2. Extract raw packet data from `ArraySegment`
3. Create `NetworkStream` for parsing
4. Attempt deserialization via `mirror::DeserializePacket()`
5. Log packet to `PacketLogUI`
6. If interception enabled + block mode → drop packet (return without calling original)
7. Otherwise, call original `Send()`

---

### network/ — Network Protocol Utilities

| File | Purpose |
|------|---------|
| `arraysegment.h` | Wrapper for IL2CPP `ArraySegment<byte>` (array pointer, offset, count) |
| `stream.h` | `NetworkStream` — sequential byte reader with position tracking |
| `networkstream.cpp` | `PrintPayload()`, read primitives (uint16, uint32, etc.) |
| `networkmessage.h` | `NetworkMessageType` enum and header structure |
| `commandmessage.cpp/h` | `CommandMessage` parsing |
| `deserializer.cpp/h` | `mirror::DeserializePacket()` — routes by message type |

---

### ui/ — ImGui User Interface

The UI runs on the DX11 render thread via the `Present` hook.

#### State Machine

```cpp
enum UiState { NotReady, Ready, Closed };
```

- **NotReady:** Shows fading "Waiting for Unity..." overlay
- **Ready:** Shows full tabbed UI window (toggle via DELETE key)
- **Closed:** Shows fading "UI Hidden" hint, then nothing

#### WndProc Hook

The `WndProcHook` handles:
1. **DELETE key** → toggle UI state (saves/restores Unity cursor state)
2. **ImGui input forwarding** → `ImGui_ImplWin32_WndProcHandler`
3. **Input blocking** → when UI is open, blocks mouse, keyboard, WM_INPUT, scroll wheel to prevent game interaction

#### UI Tabs

| Tab | File | Description |
|-----|------|-------------|
| **Explorer** | `explorer.cpp/h` | Instance tree browser with caching, icons, property inspector |
| **Save Instance** | `saveinstance.cpp/h` | Export game instances via `GameIO::SaveToFile()` |
| **Executor** | `executor.cpp/h` | Lua text editor (syntax highlighting) + Execute/Clear/Open/Save buttons |
| **Packet Log** | `packetlog.cpp/h` | Capture, display, filter, intercept network packets |
| **Settings** | `settingsui.cpp/h` | Configure keybind, input blocking toggle |
| **About** | `about.cpp/h` | Credits, version info, links |
| **Decompile: [name]** | `scriptsource.cpp/h` | Dynamic tabs showing decompiled script source |

#### Explorer Caching System

The explorer uses an `ExplorerCache` with an `unordered_map<Instance*, InstanceCache>` to avoid repeated Unity API calls:

```cpp
struct InstanceCache {
    std::string name;
    std::string className;
    std::string fullName;
    bool isPopulated = false;
};
```

Cache is mutex-protected and populated lazily on first access.

#### Styling

`PremiumStyle` provides:
- Dark color scheme with cyan accent (`#00BFD8`)
- Custom fonts (SNPro family: Regular, Bold, SemiBold, Light, Medium) from embedded binary data
- Rounded corners, padding, spacing
- `IsPremiumEnabled` flag to toggle between premium and default ImGui style

---

### hwidspoofer/ — HWID Spoofer & Multi-Client

Built as `version.dll` (DLL proxy) — loaded automatically by Windows before the game starts.

#### `proxy.cpp`

Forwards all real `version.dll` exports to the original system DLL, allowing this DLL to masquerade as `version.dll` in the game directory.

#### `hwidspoofer.cpp`

**Init flow:**
1. Hook `LoadLibraryW` (kernel32) → detect when `GameAssembly.dll` is loaded
2. Hook `CreateMutexA` (kernel32) → intercept single-instance mutex creation
3. When `GameAssembly.dll` loads:
   - Hook `GetDeviceUniqueID` at hardcoded offset (`0x1A56880`)
   - Initialize Unity, load `wowiezz.dll` if present

**GetDeviceUniqueID hook:**
- Gets original HWID string
- `scramble()` it (random shuffle of characters)
- Return the scrambled HWID to the game

**CreateMutexA hook:**
- If mutex name contains `"SingleInstanceMutex"` → return fake handle `0xDEADBEEF` (bypasses single-instance lock)
- All other mutexes → call original

#### `multiclient.cpp`

Uses undocumented NT APIs for aggressive mutex cleanup:
1. `NtQueryObject` → determine "Mutant" (mutex) type index dynamically
2. `NtQuerySystemInformation(SystemExtendedHandleInformation)` → enumerate all handles system-wide
3. Filter for current process + Mutant type
4. `DuplicateHandle(DUPLICATE_CLOSE_SOURCE)` to close each mutex

---

### injector/ — Standalone DLL Injector

Classic `CreateRemoteThread` + `LoadLibraryA` DLL injection.

| Function | Description |
|----------|-------------|
| `GetProcessIdByName()` | Enumerate processes via `CreateToolhelp32Snapshot` |
| `WaitForProcess()` | Spin until target process appears |
| `InjectDLL()` | `VirtualAllocEx` → `WriteProcessMemory` → `CreateRemoteThread` |

**Usage:** `injector.exe [process_name] [dll_path]`  
**Defaults:** `"Polytoria Client.exe"`, `"wowiezz.dll"` (searches build output dirs)  
**Delay:** 6 seconds after finding process before injection

---

### scripts/ — Build-Time Python Utilities

| Script | Purpose |
|--------|---------|
| `ttf_to_header.py` | Convert TTF font files to C++ header/source with embedded byte arrays |
| `svg_to_png.py` | Rasterize SVG icons to PNG using CairoSVG |
| `png_to_header.py` | Convert PNG files to C++ header with embedded byte arrays |

All generators produce hex-encoded `unsigned char[]` arrays with accompanying size constants.

---

### WpfUI/ — External WPF UI

A .NET 8 WPF application that provides an out-of-process script executor UI. Communicates with the injected DLL via named pipes. Built separately with `dotnet build` and copied to `.download/` during the xmake `after_build` step.

---

## Custom Lua Environment

When a script is **whitelisted** (created via executor or pipe), `InstallEnvironnement()` registers these globals:

| Lua Function | C++ Handler | Signature | Description |
|-------------|-------------|-----------|-------------|
| `loadstring(source)` | `loadstring()` | `string → function` | Compile Lua code string into callable function |
| `activatetool(tool)` | `activatetool()` | `Tool → nil` | Activate a tool (CmdActivate) |
| `equiptool(tool)` | `equip()` | `Tool → nil` | Equip a tool locally (CmdEquip) |
| `unequiptool(tool)` | `unequip()` | `Tool → nil` | Unequip a tool (CmdUnequip) |
| `serverequiptool(player, tool)` | `serverequiptool()` | `Player, Tool → nil` | Server-side equip tool (EquipTool) |
| `sendchat(message)` | `sendchat()` | `string → nil` | Send chat message via ChatService |
| `fireclickdetector(instance)` | `fireclickdetector()` | `Instance → nil` | Fire click detector (CmdClicked) |
| `identifyexecutor()` | `identifyexecutor()` | `→ string` | Returns `"elcapor"` |
| `poop()` | `poop()` | `→ string` | Debug test, returns `"Hello from C++!"` |

**Argument validation pattern:** Every callback function follows the same pattern:
1. Check argument count
2. Get `DynValue*` from `args->RawGet(index, false)`
3. Validate type (e.g., `DynValue::DataType::UserData` for game objects, `String` for strings)
4. Cast to concrete type via `DynValue::Cast(StaticClass<T>()->GetType())`
5. Null-check the cast result
6. Perform the action
7. Return `DynValue::FromNil()` or `DynValue::FromString()`

---

## Key Design Patterns

### 1. CRTP Object Registration

All game types derive from `Object<Derived, "ClassName", "Assembly">`, enabling:
- `StaticClass<T>()` → automatic class resolution via compile-time string params
- `nasec::meta::is_object<T>` → SFINAE type trait
- Type-safe singleton access via `Singleton<T>`

### 2. Static Local Caching

Unity reflection results (assemblies, classes, methods, fields) are cached in function-scoped `static` locals:
```cpp
template <nasec::meta::String Name>
UnityAssembly* GetAssembly() {
    static UnityAssembly* assembly = nullptr;
    if (!assembly) assembly = UnityResolve::Get(Name.data);
    return assembly;
}
```

This ensures each lookup happens exactly once, with zero runtime cost after first call.

### 3. Compile-Time String NTTPs

`nasec::meta::String<N>` allows string literals as template parameters:
```cpp
Unity::GetMethod<"get_Name">(klass);
Unity::GetClass<"Game", "Assembly-CSharp.dll">();
```

### 4. Whitelist-Based Script Injection

The executor doesn't modify game scripts. Instead:
1. Scripts created by the executor are added to `ScriptService::whitelisted`
2. The `ExecuteScriptInstance` hook checks membership
3. Only whitelisted scripts receive the custom Lua environment
4. Game scripts run completely unmodified

### 5. Thread Architecture

| Thread | Purpose |
|--------|---------|
| Game main thread | Normal game execution |
| `main_thread` | Init + bootstrap (detached) |
| DX11 render thread | ImGui rendering (via Present hook callback) |
| Pipe server thread | Named pipe listener (joinable, clean shutdown) |

---

## Data Flow Diagrams

### Script Execution

```
User types Lua code in Executor tab
       │
       ▼
ExecutorUI::DrawTab()
       │
       ▼
ScriptService::RunScript<ScriptInstance>(source)
       │
       ├─► BaseScript::New<ScriptInstance>(source)  ← creates GameObject + component
       ├─► whitelisted.push_back(script)
       └─► ScriptService::RunScript(script)         ← invokes game's RunScript
              │
              ▼
       ExecuteScriptInstanceHook (Detours hook)
              │
              ├─ Is script in whitelisted?
              │     YES → InstallEnvironnement(script.Globals)  ← inject custom functions
              │     NO  → pass through unmodified
              │
              └─► Call original ExecuteScriptInstance
                     │
                     ▼
              MoonSharp interprets Lua code
```

### External Pipe Execution

```
External tool (WpfUI / any client)
       │
       ▼
WriteFile(\\.\pipe\wowiezz_pipe, scriptCode)
       │
       ▼
PipeServerThreadFunc()
       │
       ├─► ReadFile() → accumulate in buffer
       └─► ScriptService::RunScript<ScriptInstance>(script)
              │
              ▼
       [Same flow as above]
```

### Packet Interception

```
Game sends network message
       │
       ▼
LocalConnectionToServer::Send(segment, channelId)
       │
       ▼  (Detours hook)
SendHook()
       │
       ├─ Is PacketLogUI enabled?
       │     YES → Parse packet, log to UI
       │           ├─ Is interception enabled + block mode?
       │           │     YES → DROP packet (return)
       │           │     NO  → fall through
       │     NO  → fall through
       │
       └─► Call original Send() → packet goes to server
```

---

## Dependencies

| Package | Purpose | Managed By |
|---------|---------|-----------|
| `microsoft-detours` | Function hooking (DetourAttach/Detach) | xmake |
| `imgui` | Immediate-mode GUI (DX11 + Win32 backends) | xmake |
| `spdlog` | Logging (header-only mode) | xmake |
| `boost` | Regex support for text processing | xmake |
| `nativefiledialog-extended` | Native OS file open/save dialogs | xmake |
| DirectX 11 SDK | Rendering (d3d11.lib, system) | System/xmake |
| `user32.lib` | Window management | System |
| `dbghelp.lib` | Stack traces in assertions | System |
| UnityResolve | IL2CPP reflection library | Bundled (`unity_internal.h`) |

---

*CC-BY-SA 4.0 Copyright (c) 2026 @ElCapor*
