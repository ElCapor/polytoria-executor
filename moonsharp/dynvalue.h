#ifndef DYNVALUE_H
#define DYNVALUE_H

#include <unity/unity.h>
#include <unity/object.h>
#include <moonsharp/closure.h>

struct DynValue : public Object<DynValue, "DynValue", Unity::AssemblyFirstPass, "MoonSharp.Interpreter">
{
    enum DataType : int
    {
        Nil,
        Void,
        Boolean,
        Number,
        String,
        Function,
        Table,
        Tuple,
        UserData,
        Thread,
        ClrFunction,
        TailCalRequest,
        YieldRequest,
    };

    static DynValue *FromString(const std::string &str);
    static DynValue *FromNumber(double n);
    static DynValue *FromCallback(UnityObject *callback, UnityString *name);
    static DynValue *FromNil();
    
    DataType Type();


    UnityObject* Cast(void* csType);
    double AsNumber();
    Closure* AsFunction();
};
#endif /* DYNVALUE_H */
