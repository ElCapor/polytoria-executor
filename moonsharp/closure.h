#ifndef CLOSURE_H
#define CLOSURE_H

#include <unity/unity.h>
#include <unity/object.h>

struct DynValue;
struct Closure : public Object<Closure, "Closure", Unity::AssemblyFirstPass, "MoonSharp.Interpreter"> {
    DynValue* Call();
    DynValue* Call(std::vector<UnityObject*> args);
    DynValue* Call(std::vector<DynValue*> args);
};

#endif /* CLOSURE_H */
