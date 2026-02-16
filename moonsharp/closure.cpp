#include <moonsharp/closure.h>
#include <moonsharp/dynvalue.h>

DynValue* Closure::Call() {
    return Unity::GetMethod<"Call">(StaticClass<Closure>())->Invoke<DynValue*>(this);
}

DynValue* Closure::Call(std::vector<UnityObject*> args) {
    UnityArray<UnityObject*>* argArray = UnityArray<UnityObject*>::New(UnityObject::GetClass(), args.size());
    for (size_t i = 0; i < args.size(); ++i) argArray->At(i) = args[i];
    return Unity::GetMethod<"Call", "System.Object[]">(StaticClass<Closure>())->Invoke<DynValue*>(this, argArray);
}

DynValue* Closure::Call(std::vector<DynValue*> args) {
    UnityArray<DynValue*>* argArray = UnityArray<DynValue*>::New(StaticClass<DynValue>(), args.size());
    for (size_t i = 0; i < args.size(); ++i) argArray->At(i) = args[i];
    return Unity::GetMethod<"Call", "MoonSharp.Interpreter.DynValue[]">(StaticClass<Closure>())->Invoke<DynValue*>(this, argArray);
}