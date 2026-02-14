#include <moonsharp/script.h>

Table* Script::Globals() {
    return Unity::GetFieldValue<Table*, "m_GlobalTable">(StaticClass<Script>(), this);
}

Table* Script::Registry() {
    return Unity::GetFieldValue<Table*, "<Registry>k__BackingField">(StaticClass<Script>(), this);
}