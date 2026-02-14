#include <ptoria/instance.h>

UnityString* InstanceBase::Name() {
    return Unity::GetMethod<"get_Name">(StaticClass<Instance>())->Invoke<UnityString*>(this);
}

UnityArray<InstanceBase*>* InstanceBase::Children() {
    return Unity::GetMethod<"GetChildren">(StaticClass<Instance>())->Invoke<UnityArray<InstanceBase*>*>(this);
}

UnityString* InstanceBase::FullName() {
    return Unity::GetMethod<"get_FullName">(StaticClass<Instance>())->Invoke<UnityString*>(this);
}