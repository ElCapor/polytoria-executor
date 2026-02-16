#include <moonsharp/table.h>

void Table::Set(UnityObject* key, UnityObject* value) {
    Unity::GetMethod<"set_Item", "System.Object", "System.Object">(StaticClass<Table>())->Invoke<void>(this, key, value);
}

UnityObject* Table::Get(UnityObject* key) {
    return Unity::GetMethod<"get_Item", "System.Object">(StaticClass<Table>())->Invoke<UnityObject*>(this, key);
}