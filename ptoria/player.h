#ifndef PLAYER_H
#define PLAYER_H

#include <unity/unity.h>
#include <unity/object.h>
#include <ptoria/tool.h>

struct Player : public Object<Player, "Player", Unity::AssemblyCSharp>
{
    void EquipTool(Tool* tool);
};

#endif /* PLAYER_H */
