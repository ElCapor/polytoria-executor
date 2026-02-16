#include <moonsharp/callbackarguments.h>

int CallbackArguments::Count() {
    return Unity::GetFieldValue<int, "m_Count">(StaticClass<CallbackArguments>(), this);
}

UnityList<DynValue*>* CallbackArguments::Arguments() {
    return Unity::GetFieldValue<UnityList<DynValue*>*, "m_Args">(StaticClass<CallbackArguments>(), this);
}

DynValue* CallbackArguments::RawGet(int idx, bool translateVoids) {
    return Unity::GetMethod<"RawGet">(StaticClass<CallbackArguments>())->Invoke<DynValue*>(this, idx, translateVoids);
}