# Unity Integration

The Polytoria Executor uses UnityResolve for IL2CPP reflection. This document explains how to interact with the Unity engine from C++.

## Overview

UnityResolve is a custom IL2CPP reflection library that provides:
- Assembly loading and inspection
- Class and method resolution
- Field access (static and instance)
- Method invocation

## Unity Namespace

All Unity-related functionality is in the `Unity` namespace:

```cpp
#include <unity/unity.h>
```

### Type Aliases

```cpp
using UnityAssembly = UnityResolve::Assembly;
using UnityClass    = UnityResolve::Class;
using UnityField    = UnityResolve::Field;
using UnityMethod   = UnityResolve::Method;
using UnityString   = UnityType::String;
using UnityObject   = UnityType::Object;
using UnityGameObject = UnityType::GameObject;
using UnityComponent = UnityType::Component;
```

### Assembly Constants

```cpp
namespace Unity {
    static constexpr auto AssemblyCSharp      = "Assembly-CSharp.dll";
    static constexpr auto AssemblyFirstPass   = "Assembly-CSharp-firstpass.dll";
    static constexpr auto AssemblyUnityEngine = "UnityEngine.CoreModule.dll";
    static constexpr auto Mirror              = "Mirror.dll";
}
```

## Basic Operations

### Getting an Assembly

```cpp
// Get Assembly-CSharp
UnityAssembly* assembly = Unity::GetAssembly<Unity::AssemblyCSharp>();

// Get UnityEngine
UnityAssembly* unityEngine = Unity::GetAssembly<Unity::AssemblyUnityEngine>();
```

### Getting a Class

```cpp
// Basic class lookup
UnityClass* instanceClass = Unity::GetClass<"Instance", Unity::AssemblyCSharp>();

// With namespace
UnityClass* playerClass = Unity::GetClass<"Player", Unity::AssemblyCSharp, "Polytoria">();

// With parent class
UnityClass* toolClass = Unity::GetClass<"Tool", Unity::AssemblyCSharp, "*", "Instance">();
```

Template parameters:
- `Name` - Class name
- `Assembly` - Assembly name
- `Namespace` - Namespace (default: `"*"`)
- `Parent` - Parent class (default: `"*"`)

### Getting a Method

```cpp
UnityClass* toolClass = StaticClass<Tool>();

// Get method with no parameters
UnityMethod* activateMethod = Unity::GetMethod<"CmdActivate">(toolClass);

// Get method with parameters (for overloads)
UnityMethod* sendChatMethod = Unity::GetMethod<"SendChat", "System.String">(chatClass);
```

### Invoking Methods

```cpp
// Instance method
toolClass->CmdActivate();  // Calls method on 'this'

// Static method
Unity::GetMethod<"SomeStaticMethod">(klass)->Invoke<void>();

// With parameters
Unity::GetMethod<"SendChat", "System.String">(chatClass)
    ->Invoke<void, void*, UnityString*>(chatInstance, message);
```

## Object System

The executor uses a CRTP (Curiously Recurring Template Pattern) system for type-safe game object access.

### Defining Game Types

```cpp
// ptoria/tool.h
struct Tool : public Object<Tool, "Tool", Unity::AssemblyCSharp> {
    void CmdActivate();
    void CmdUnequip();
    void CmdEquip();
};

// ptoria/tool.cpp
void Tool::CmdActivate() {
    Unity::GetMethod<"CmdActivate">(StaticClass<Tool>())->Invoke<void>(this);
}
```

### Using the Object System

```cpp
// Get static class reference
UnityClass* klass = StaticClass<Tool>();

// Create objects
UnityGameObject* go = UnityGameObject::Create("MyTool");
Tool* tool = go->AddComponent<Tool*>(StaticClass<Tool>());

// Call methods
tool->CmdActivate();
```

### StaticClass Helper

The `StaticClass<T>()` function automatically resolves the class:

```cpp
template<typename T>
auto StaticClass() -> UnityClass* {
    using Params = ObjectParamsT<T>;
    return Unity::GetClass<Params::_class,
                           Params::_assembly,
                           Params::_namespace,
                           Params::_parent>();
}
```

## Field Access

### Instance Fields

```cpp
// Get field value
auto value = Unity::GetFieldValue<int, "someField">(klass, object);

// Set field value
Unity::SetFieldValue<int, "someField">(klass, object, 42);
```

### Static Fields

```cpp
// Get static field
auto instance = Unity::GetStaticFieldValue<UnityObject*, "Instance">(klass);

// Set static field
Unity::SetStaticFieldValue<UnityObject*>(klass, &instance);
```

## Working with Strings

### Creating Strings

```cpp
UnityString* str = UnityString::New("Hello World");
```

### Converting to std::string

```cpp
UnityString* str = /* get from game */;
std::string cppStr = str->ToString();
```

### Using in Method Calls

```cpp
// Create and pass to method
UnityString* msg = UnityString::New("Chat message");
Unity::GetMethod<"SendChat", "System.String">(chatClass)
    ->Invoke<void, void*, UnityString*>(chatInstance, msg);
```

## Thread Safety

### ThreadAttach

Every thread that calls Unity APIs must be attached:

```cpp
// Attach current thread
Unity::ThreadAttach();

// Now you can make Unity API calls

// Detach when done (optional)
Unity::ThreadDetach();
```

When is this needed?
- DX11 render thread (first call in HookPresent)
- Pipe server thread (if making Unity calls)
- Any new threads you create

### Important Notes

- Objects can be garbage collected between frames
- Don't hold references across long periods
- Always null-check returned objects

## Advanced: Creating Objects

### Creating GameObjects

```cpp
UnityGameObject* go = UnityGameObject::Create("GameObjectName");
```

### Adding Components

```cpp
// Add a component to a GameObject
Tool* tool = go->AddComponent<Tool*>(StaticClass<Tool>());
```

### Creating via IL2CPP

```cpp
// Direct IL2CPP object creation
auto obj = UnityResolve::Invoke<UnityObject*>("il2cpp_object_new", classType);
```

## MoonSharp Integration

The Unity integration works with MoonSharp to expose game objects to Lua:

### UserData Registration

Game objects are exposed as MoonSharp UserData:

```cpp
// In DynValue
UnityObject* Cast(void* csType);

// Usage in callbacks
DynValue* tool = args->RawGet(0, false);
Tool* toolObj = (Tool*)tool->Cast(StaticClass<Tool>()->GetType());
```

### Example: Tool Callback

```cpp
DynValue* equiptool(void*, ScriptExecutionContext*, CallbackArguments* args) {
    // Get tool from arguments
    DynValue* toolArg = args->RawGet(0, false);
    
    // Verify type
    if (toolArg->Type() != DynValue::DataType::UserData) {
        return DynValue::FromString("Invalid argument, expected Tool");
    }
    
    // Cast to Tool*
    Tool* tool = (Tool*)toolArg->Cast(StaticClass<Tool>()->GetType());
    
    // Call method
    tool->CmdEquip();
    
    return DynValue::FromNil();
}
```

## Common Patterns

### Waiting for Singleton

```cpp
// Wait for Game singleton
void* game = nullptr;
while (!game) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    game = Unity::GetStaticFieldValue<void*, "singleton">(StaticClass<Game>());
}
```

### Finding Children

```cpp
// Iterate children
for (auto& child : gameInstance->Children()->ToVector()) {
    spdlog::info("Child: {}", child->Name()->ToString());
}
```

### Error Handling

```cpp
UnityMethod* method = Unity::GetMethod<"SomeMethod">(klass);
nasec::Assert(method != nullptr, "Failed to get method");
```

## Best Practices

1. **Use StaticClass<T>()** - Type-safe class resolution
2. **Cache results** - Methods are internally cached
3. **Always attach threads** - Before any Unity API call
4. **Null check** - Objects can be null
5. **Use nasec::Assert** - For critical validation

## File Reference

| File | Purpose |
|------|---------|
| [`unity/unity.h`](unity/unity.h) | Main Unity API wrappers |
| [`unity/object.h`](unity/object.h) | Object CRTP system |
| [`unity/unity_internal.h`](unity/unity_internal.h) | UnityResolve internals |
| [`unity/il2cpp.h`](unity/il2cpp.h) | Low-level IL2CPP bindings |
