#ifndef CALLBACKARGUMENTS_H
#define CALLBACKARGUMENTS_H

#include <unity/unity.h>
#include <unity/object.h>
#include <moonsharp/dynvalue.h>

struct CallbackArguments : public Object<CallbackArguments, "CallbackArguments", Unity::AssemblyFirstPass, "MoonSharp.Interpreter">
{
    int Count();
    UnityList<DynValue*>* Arguments();
    DynValue* RawGet(int idx, bool translateVoids);
};

#endif /* CALLBACKARGUMENTS_H */
