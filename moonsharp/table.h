#ifndef TABLE_H
#define TABLE_H

#include <unity/object.h>
#include <unity/unity.h>

struct Table : public Object<Table, "Table", Unity::AssemblyFirstPass, "MoonSharp.Interpreter"> {
    void Set(UnityObject* key, UnityObject* value);
    UnityObject* Get(UnityObject* key);
};

#endif /* TABLE_H */
