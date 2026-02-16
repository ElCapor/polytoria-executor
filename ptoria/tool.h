#ifndef TOOL_H
#define TOOL_H

#include <unity/unity.h>
#include <unity/object.h>

struct Tool : public Object<Tool, "Tool", Unity::AssemblyCSharp>
{
    void CmdActivate();
    void CmdUnequip();
    void CmdEquip();
};

#endif /* TOOL_H */
