#ifndef SCRIPT_H
#define SCRIPT_H

#include <unity/unity.h>
#include <unity/object.h>
#include <moonsharp/table.h>

struct Script : public Object<Script, "Script", Unity::AssemblyFirstPass, "MoonSharp.Interpreter">
{

    Table *Globals();
    Table *Registry();
};

#endif /* SCRIPT_H */
