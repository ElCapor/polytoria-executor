# Hooking System

The Polytoria Executor uses Microsoft Detours for function hooking. This document explains how the hooking system works and how to add new hooks.

## Overview

The hooking system is implemented in [`hooking/hookmanager.h`](hooking/hookmanager.h) and wraps Microsoft Detours to provide a safe and easy-to-use interface for intercepting function calls.

## HookManager API

### Installing a Hook

```cpp
// hooking/hookmanager.h
template<typename Fn>
static auto Install(Fn func, Fn handler) -> bool
```

**Parameters:**
- `func` - Original function pointer
- `handler` - Your hook function

**Returns:** `true` if hook installed successfully

### Calling the Original

```cpp
// Call original with default calling convention
template<typename RType, typename... Params>
static auto Call(RType(*handler)(Params...), Params... params) -> RType

// __cdecl calling convention
template<typename RType, typename... Params>
static auto Ccall(RType(__cdecl* handler)(Params...), Params... params) -> RType

// __stdcall calling convention
template<typename RType, typename... Params>
static auto Scall(RType(__stdcall* handler)(Params...), Params... params) -> RType

// __fastcall calling convention
template<typename RType, typename... Params>
static auto Fcall(RType(__fastcall* handler)(Params...), Params... params) -> RType

// __vectorcall calling convention
template<typename RType, typename... Params>
static auto Vcall(RType(__vectorcall* handler)(Params...), Params... params) -> RType
```

### Detaching a Hook

```cpp
// Detach specific hook
template<typename Fn>
static auto Detach(Fn handler) -> void

// Detach all hooks
static auto DetachAll() -> void
```

## Usage Examples

### Basic Hook

```cpp
// 1. Define your hook function (must match original signature)
void MyHook(SomeClass* self, int param1, UnityString* param2) {
    spdlog::info("Hook called with param: {}", param2->ToString());
    
    // Call the original function
    HookManager::Call(MyHook, self, param1, param2);
}

// 2. Install the hook
auto method = Unity::GetMethod<"OriginalMethodName">(StaticClass<SomeClass>());
nasec::Assert(method != nullptr, "Failed to get method for hooking");
HookManager::Install(method->Cast<void, SomeClass*, int, UnityString*>(), MyHook);
```

### Hooking with Return Value

```cpp
// Hook with return value
int MyHook(SomeClass* self, int value) {
    spdlog::info("Original value: {}", value);
    
    // Modify the value before passing to original
    int modified = value * 2;
    
    // Call original and get result
    int result = HookManager::Call(MyHook, self, modified);
    
    // Modify result before returning
    return result + 1;
}
```

### Blocking Function Calls

```cpp
// Hook that blocks the original function
bool MyBlockerHook(SomeClass* self) {
    spdlog::info("Blocking original call!");
    
    // Don't call the original - block it
    // Just return a custom value
    return true;
}

// Install with intent to block
HookManager::Install(originalFunc, MyBlockerHook);
```

### Using Different Calling Conventions

```cpp
// DX11 functions typically use __stdcall
void DX11Hook(IDXGISwapChain* chain) {
    // Custom logic
    HookManager::Scall(DX11Hook, chain);
}
HookManager::Install(originalDX11, DX11Hook);

// Most game functions use default (__cdecl on x64)
void GameHook(int param) {
    HookManager::Call(GameHook, param);
}
```

## Common Hooks in the Executor

### Script Execution Hook

```cpp
// ptoria/scriptservice.cpp
DynValue* ScriptService::ExecuteScriptInstanceHook(ScriptService* self, Script* script, BaseScript* instance) {
    // Check whitelist
    if (std::find(whitelisted.begin(), whitelisted.end(), instance) == whitelisted.end()) {
        return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
    }
    
    // Inject custom environment
    InstallEnvironnement(script);
    
    return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
}

// Install in ScriptService::InstallHooks()
void ScriptService::InstallHooks() {
    HookManager::Install(
        Unity::GetMethod<"ExecuteScriptInstance">(StaticClass<ScriptService>())
            ->Cast<DynValue*, ScriptService*, Script*, BaseScript*>(),
        ExecuteScriptInstanceHook
    );
}
```

### Network Hook (Mirror)

```cpp
// mirror/localconnectiontoserver.cpp
void LocalConnectionToServer::SendHook(NetworkMessage* msg) {
    spdlog::info("Intercepted network message");
    
    // Optionally modify msg here
    
    // Forward to original
    HookManager::Call(SendHook, msg);
}
```

### DX11 Hook

```cpp
// hooking/dx11hook.cpp
// DX11 uses __stdcall (Scall)
HRESULT __stdcall HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    // First entry - initialize
    static bool initialized = false;
    if (!initialized) {
        Unity::ThreadAttach();
        dx11::Init(pSwapChain);
        initialized = true;
    }
    
    // Render ImGui
    ui::HookPresent();
    
    // Call original
    return HookManager::Scall(HookPresent, pSwapChain, SyncInterval, Flags);
}
```

## Important Rules

### DO

- ✅ Always call the original function (unless blocking intentionally)
- ✅ Match the calling convention exactly
- ✅ Use `nasec::Assert` to verify method pointers
- ✅ Call `HookManager::DetachAll()` on unload

### DON'T

- ❌ Install hooks in `DllMain` (deadlock risk)
- ❌ Hold locks while calling originals (deadlock risk)
- ❌ Install hooks on unverified methods
- ❌ Forget to call the original

## Troubleshooting

### "Hook not working"

- Verify method pointer is correct with `nasec::Assert`
- Check calling convention matches
- Ensure hook signature matches original exactly

### "Game crashes on startup"

- Hooks installed too early (before `Unity::Init()`)
- Method pointer is wrong
- Calling convention mismatch

### "Deadlock"

- Holding lock while calling original function
- Installing hooks inside DllMain

## Adding New Hooks

See [agents.md: How to Hook a New Game Method](../agents.md#7-how-to-hook-a-new-game-method) for detailed instructions.
