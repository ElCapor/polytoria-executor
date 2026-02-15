#include <ptoria/player.h>

void Player::EquipTool(Tool* tool)
{
    Unity::GetMethod<"EquipTool">(StaticClass<Player>())->Invoke<void>(this, tool);
}
