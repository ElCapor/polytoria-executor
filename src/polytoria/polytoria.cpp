#include <polytoria/polytoria.hpp>
#include <skipsped/enviroment.hpp> 

using namespace polytoria;

typedef UO* (*ExecuteScriptInstance_t)(ScriptService* self, UO* script, BaseScript* scriptInstance);

static auto dtExecuteScriptInstance(ScriptService* self, UO* script, BaseScript* scriptInstance) -> UO*
{
    static UK* scriptKlass;
    if (!scriptKlass) scriptKlass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("Script", "MoonSharp.Interpreter");
    
    if(!scriptKlass) {
        std::cout << "Failed to get Script class" << std::endl;
        return nullptr;
    }

    const auto table = scriptKlass->GetValue<UO*>(script, "m_GlobalTable");
    
    InstallEnviromentFunctions(table);

    return HookManager::Call(dtExecuteScriptInstance, self, script, scriptInstance);
}

void ScriptService::InstallHooks()
{
    static auto method = GetClass()->Get<UM>("ExecuteScriptInstance");
    
    std::printf("Shit %p poop %p\n", method->function, dtExecuteScriptInstance);

    HookManager::Install((ExecuteScriptInstance_t)method->function, dtExecuteScriptInstance);
}
