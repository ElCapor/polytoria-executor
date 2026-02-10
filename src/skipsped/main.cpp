/**
 * @file main.cpp
 * @author ElCapor
 * @brief Main thread...
 * @version 0.1
 * @date 2026-02-10
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <skipsped/sped.hpp>

void sped::main_thread()
{
    sped::Sped& sped = sped::Sped::GetInstance();
    sped.Init();
}