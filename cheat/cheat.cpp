#include <ptoria/instance.h>
#include <nasec/assert.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <core/core.h>
#include <ptoria/game.h>
#include <ptoria/scriptinstance.h>
#include <ptoria/scriptservice.h>
#include <mirror/hooks.h>
#include <ptoria/networkevent.h>
#include <ui/ui.h>
#include <cheat/pipe.h>

int main_thread()
{
    OpenConsole();
    while (!GetModuleHandleW(L"GameAssembly.dll")) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    //NetworkEvent::InstallHooks();
    Unity::Init();
    Unity::ThreadAttach();

    UnityAssembly* assembly = Unity::GetAssembly<Unity::AssemblyCSharp>();


    // We gotta wait for game to be started !
    void* game = Unity::GetStaticFieldValue<void*, "singleton">(StaticClass<Game>());
    while (!game) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        game = Unity::GetStaticFieldValue<void*, "singleton">(StaticClass<Game>());
    }

    UI::state = UI::UiState::Ready;
    
    spdlog::info("Assembly Name: {}", assembly->name);
    UnityClass* instanceClass = StaticClass<Instance>();
    nasec::Assert(instanceClass != nullptr, "Failed to get Instance class");
    

    UnityClass* gameClass = StaticClass<Game>();
    nasec::Assert(gameClass != nullptr, "Failed to get Game class");

    spdlog::info("Instance Class Name: {}", instanceClass->name);
    spdlog::info("Game Class Name: {}", gameClass->name);

    Game* gameInstance = Game::GetSingleton();
    nasec::Assert(gameInstance != nullptr, "Failed to get Game singleton instance");

    spdlog::info("Game Instance Address: 0x{:016X}", reinterpret_cast<uintptr_t>(gameInstance));

    spdlog::info("Game Instance Name: {}", gameInstance->Name()->ToString());
    spdlog::info("Game Instance Full Name: {}", gameInstance->FullName()->ToString());
    for (auto& child: gameInstance->Children()->ToVector()) {
        spdlog::info("Child Instance Name: {}", child->Name()->ToString());
    }

    spdlog::info("ScriptInstance Class Name: {}", StaticClass<ScriptInstance>()->name);
    ScriptService::InstallHooks();
    ScriptService* scriptService = ScriptService::GetInstance();
    nasec::Assert(scriptService != nullptr, "Failed to get ScriptService instance");
    //ScriptService::RunScript<ScriptInstance>(R"(InvokeServerHook(game["Hidden"]["DraggerPlace"], function(msg) print(msg) end))");
    
    mirror::InstallHooks();
    
    // Start the named pipe server for external script execution
    StartPipeServer();

    UI::Setup();
    return 0;
}