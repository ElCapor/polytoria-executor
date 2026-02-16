// basescript.h
#ifndef BASESCRIPT_H
#define BASESCRIPT_H

#include <ptoria/instance.h>

struct BaseScriptBase : public InstanceBase
{
    void SetRunning(bool value);
    bool Running();
    void SetSource(UnityString *value);
    UnityString *Source();
    bool RequestedRun();
    void SetRequestedRun(bool value);
};

struct BaseScript : public BaseScriptBase,
                    public Object<BaseScript, "BaseScript", Unity::AssemblyCSharp>
{

    template <typename T>
    static BaseScript *New(const std::string &source)
    {
        UnityGameObject *gameObject = UnityGameObject::Create("BaseScript");
        nasec::Assert(gameObject != nullptr, "Failed to create GameObject for BaseScript");
        BaseScript *script = gameObject->AddComponent<BaseScript *>(StaticClass<T>());
        nasec::Assert(script != nullptr, "Failed to add BaseScript component");
        script->SetSource(UnityString::New(source));
        script->SetRunning(false);
        return script;
    }
};

#endif /* BASESCRIPT_H */
