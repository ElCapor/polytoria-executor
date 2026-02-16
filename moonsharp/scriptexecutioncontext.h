#ifndef SCRIPTEXECUTIONCONTEXT_H
#define SCRIPTEXECUTIONCONTEXT_H

#include <unity/unity.h>
#include <unity/object.h>
#include <moonsharp/script.h>

struct ScriptExecutionContext : public Object<ScriptExecutionContext, "ScriptExecutionContext", Unity::AssemblyFirstPass, "MoonSharp.Interpreter">
{
    Script* OwnerScript();
};


#endif /* SCRIPTEXECUTIONCONTEXT_H */
