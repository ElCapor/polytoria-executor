#include <moonsharp/script.h>

Table* Script::Globals() {
    return Unity::GetFieldValue<Table*, "m_GlobalTable">(StaticClass<Script>(), this);
}

Table* Script::Registry() {
    return Unity::GetFieldValue<Table*, "<Registry>k__BackingField">(StaticClass<Script>(), this);
}

DynValue* Script::loadstring(UnityString* code, Table* globalContext, UnityString* friendlyName) {
    return Unity::GetMethod<"LoadString", "System.String", "MoonSharp.Interpreter.Table", "System.String">(StaticClass<Script>())
        ->Invoke<DynValue*>(this, code, globalContext, friendlyName);
}