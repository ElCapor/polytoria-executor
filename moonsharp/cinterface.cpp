#include <moonsharp/cinterface.h>
#include <unity/il2cpp.h>
#include <format>

void RegisterCallback(Table* globals, const std::string& name, CallbackFunction func) {
    // Create an il2cpp method info
    auto mi = il2cpp::CreateCustomMethodInfo(
        std::format("{}_methodinfo", name).c_str(),
        {
            (il2cpp::Il2CppType*)StaticClass<ScriptExecutionContext>()->GetType(),
            (il2cpp::Il2CppType*)StaticClass<CallbackArguments>()->GetType()
        },
        (il2cpp::Il2CppType*)StaticClass<DynValue>()->GetType(),
        (il2cpp::Il2CppMethodPointer)func
    );

    auto delegate_class = Unity::GetClass<"Func`3", "mscorlib.dll", "System">()->GetType();
    auto delegate = UnityResolve::Invoke<UnityObject*, uintptr_t>("il2cpp_object_new", *(uintptr_t*)delegate_class);

    il2cpp::BindDelegate((il2cpp::Il2CppDelegate*)delegate, mi);

    auto callback = DynValue::FromCallback((UnityObject*)delegate, 0);
    
    globals->Set(UnityString::New(name), (UnityObject*)callback);
}