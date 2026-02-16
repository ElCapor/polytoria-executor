#include <ptoria/tool.h>
#include <nasec/assert.h>

void Tool::CmdActivate()
{
    Unity::GetMethod<"CmdActivate">(StaticClass<Tool>())->Invoke<void>(this);
}

void Tool::CmdUnequip()
{
    Unity::GetMethod<"CmdUnequip">(StaticClass<Tool>())->Invoke<void>(this);
}

void Tool::CmdEquip()
{
    Unity::GetMethod<"CmdEquip">(StaticClass<Tool>())->Invoke<void>(this);
}