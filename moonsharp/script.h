#ifndef SCRIPT_H
#define SCRIPT_H

#include <unity/unity.h>
#include <unity/object.h>
#include <moonsharp/table.h>
#include <moonsharp/dynvalue.h>

struct Script : public Object<Script, "Script", Unity::AssemblyFirstPass, "MoonSharp.Interpreter">
{

    Table *Globals();
    Table *Registry();
    DynValue *loadstring(UnityString *code, Table *globalContext, UnityString *friendlyName);
};

#endif /* SCRIPT_H */
