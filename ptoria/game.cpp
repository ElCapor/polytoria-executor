#include <ptoria/game.h>

UnityString* Game::GameName() {
    return Unity::GetStaticFieldValue<UnityString*, "GameName">(StaticClass<Game>());
}

int Game::GameID() {
    return Unity::GetStaticFieldValue<int, "gameID">(StaticClass<Game>());
}

void GameIO::SaveToFile(const char* path) {
    Unity::GetMethod<"SaveToFile">(StaticClass<GameIO>())->Invoke<void, void*, UnityString*>(this, UnityString::New(path));
}