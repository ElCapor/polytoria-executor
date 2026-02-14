// basescript.h
#ifndef BASESCRIPT_H
#define BASESCRIPT_H

#include <ptoria/instance.h>

struct BaseScriptBase : public InstanceBase {
    void SetRunning(bool value);
    bool Running();
    void SetSource(UnityString* value);
    UnityString* Source();
    bool RequestedRun();
    void SetRequestedRun(bool value);
};

struct BaseScript : public BaseScriptBase,
                    public Object<BaseScript, "BaseScript", Unity::AssemblyCSharp> {};

#endif