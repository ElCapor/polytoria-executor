#include <moonsharp/scriptexecutioncontext.h>

Script* ScriptExecutionContext::OwnerScript() {
    return Unity::GetMethod<"get_OwnerScript">(StaticClass<ScriptExecutionContext>())->Invoke<Script*>(this);
}