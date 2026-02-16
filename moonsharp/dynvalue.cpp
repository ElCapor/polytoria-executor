#include <moonsharp/dynvalue.h>

DynValue* DynValue::FromString(const std::string& str) {
    return Unity::GetMethod<"NewString">(StaticClass<DynValue>())->Invoke<DynValue*>(UnityString::New(str));
}

DynValue* DynValue::FromCallback(UnityObject* callback, UnityString* name) {
    return Unity::GetMethod<"NewCallback">(StaticClass<DynValue>())->Invoke<DynValue*>(callback, name);
}

DynValue* DynValue::FromNil() {
    return Unity::GetMethod<"NewNil">(StaticClass<DynValue>())->Invoke<DynValue*>();
}

DynValue::DataType DynValue::Type() {
    return Unity::GetFieldValue<DataType, "m_Type">(StaticClass<DynValue>(), this);
}

UnityObject* DynValue::Cast(void* csType) {
    return Unity::GetMethod<"ToObject", "System.Type">(StaticClass<DynValue>())->Invoke<UnityObject*>(this, csType);
}

Closure* DynValue::AsFunction() {
    return Unity::GetMethod<"get_Function">(StaticClass<DynValue>())->Invoke<Closure*>(this);
}