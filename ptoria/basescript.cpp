#include <ptoria/basescript.h>

void BaseScriptBase::SetRunning(bool value) {
    Unity::SetFieldValue<bool, "running">(StaticClass<BaseScript>(), this, value);
}

bool BaseScriptBase::Running() {
    return Unity::GetFieldValue<bool, "running">(StaticClass<BaseScript>(), this);
}

void BaseScriptBase::SetSource(UnityString* value) {
    Unity::SetFieldValue<UnityString*, "source">(StaticClass<BaseScript>(), this, value);
}

UnityString* BaseScriptBase::Source() {
    return Unity::GetFieldValue<UnityString*, "source">(StaticClass<BaseScript>(), this);
}

bool BaseScriptBase::RequestedRun() {
    return Unity::GetFieldValue<bool, "requestedRun">(StaticClass<BaseScript>(), this);
}

void BaseScriptBase::SetRequestedRun(bool value) {
    Unity::SetFieldValue<bool, "requestedRun">(StaticClass<BaseScript>(), this, value);
}