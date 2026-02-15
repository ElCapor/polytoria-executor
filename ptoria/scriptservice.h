#ifndef SCRIPTSERVICE_H
#define SCRIPTSERVICE_H

#include <ptoria/basescript.h>
#include <moonsharp/script.h>
#include <moonsharp/dynvalue.h>

struct ScriptService : public InstanceBase,
                       public Object<ScriptService, "ScriptService", Unity::AssemblyCSharp>
{
    static ScriptService *GetInstance();

    template <typename T>
    static void RunScript(std::string source)
    {
        BaseScript *script = BaseScript::New<T>(source);
        whitelisted.push_back(script);
        RunScript(script);
    }

    static void RunScript(BaseScript *script);
    static std::string ScriptSource(BaseScript *script);

    static DynValue *ExecuteScriptInstanceHook(ScriptService *self, Script *script, BaseScript *instance);

    static void InstallHooks();

    static std::vector<BaseScript*> whitelisted;
};

#endif /* SCRIPTSERVICE_H */
