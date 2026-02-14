#ifndef GAME_H
#define GAME_H

#include <ptoria/instance.h>

#include <ptoria/singleton.h>

struct Game : public InstanceBase,
              public Object<Game, "Game", Unity::AssemblyCSharp>,
              public Singleton<Game> {
    UnityString* GameName();
    int GameID();
};

struct GameIO : public InstanceBase,
                public Object<GameIO, "GameIO", Unity::AssemblyCSharp>,
                public Singleton<GameIO> {
    void SaveToFile(const char* path);
};

#endif