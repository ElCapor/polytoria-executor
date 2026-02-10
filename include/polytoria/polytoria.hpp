#ifndef TYPES
#define TYPES

#include <unity/u.hpp>

namespace polytoria
{
    struct Instance : public UC
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
                klass = U::Get(ASSEMBLY_CSHARP)->Get("Instance");
            return klass;
        }

        auto GetName() -> US *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("get_Name");
            if (method)
                return method->Invoke<US *>(this);
            return {};
        }

        auto GetChildren() -> UArray<Instance *> *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("GetChildren");
            if (method)
                return method->Invoke<UArray<Instance *> *>(this);
            return {};
        }

        auto GetFullName() -> US *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("get_FullName");
            if (method)
                return method->Invoke<US *>(this);
            return {};
        }


    };

    struct Game : public Instance
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
                klass = U::Get(ASSEMBLY_CSHARP)->Get("Game");
            return klass;
        }

        static auto GetInstance() -> Game *
        {
            static Game *instance;
            if (!instance)
                GetClass()->Get<UF>("singleton")->GetStaticValue(&instance);
            return instance;
        }

        static auto GetGameName() -> US *
        {
            static US* gameName = nullptr;
            if (!gameName)
                GetClass()->Get<UF>("GameName")->GetStaticValue(&gameName);
            return gameName;
        }

        static auto GetGameID() -> int
        {
            static int gameID = 0;
            if (gameID == 0)
                GetClass()->Get<UF>("gameID")->GetStaticValue(&gameID);
            return gameID;
        }
    };

    struct GameIO : public UnityResolve::UnityType::MonoBehaviour
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
                klass = U::Get(ASSEMBLY_CSHARP)->Get("GameIO");
            return klass;
        }

        static auto GetInstance() -> GameIO *
        {
            static GameIO *instance;
            if (!instance)
                GetClass()->Get<UF>("singleton")->GetStaticValue(&instance);
            return instance;
        }

        static auto SaveInstance(const char *path)
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("SaveToFile");

            auto instance = GetInstance();
            if (method && instance)
                method->Invoke<void, void *, US *>(instance, US::New(path));
            else
                std::cout << "[ERROR] Failed to save instance: method or instance is null!" << std::endl;
        }
    };

    struct BaseScript : public Instance
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
                klass = U::Get(ASSEMBLY_CSHARP)->Get("BaseScript");
            return klass;
        }

        auto SetRunning(bool running) -> void
        {
            GetClass()->SetValue<bool>(this, "running", running);
        }

        auto SetSource(US* source) -> void
        {
            GetClass()->SetValue<US*>(this, "source", source);
        }

        auto GetSource() -> US*
        {
            return GetClass()->GetValue<US*>(this, "source");
        }
    };

    struct ScriptInstance : public Instance {
            static auto GetClass() -> UK *
            {
                static UK *klass;
                if (!klass)
                    klass = U::Get(ASSEMBLY_CSHARP)->Get("ScriptInstance");
                return klass;
            }
    };

    struct ScriptService : public Instance
    {
        static auto GetClass() -> UK*
        {
            static UK* klass;
            if (!klass)
                klass = U::Get(ASSEMBLY_CSHARP)->Get("ScriptService");
            return klass;
        }

        static auto GetInstance() -> ScriptService*
        {
            static ScriptService* instance;
            if (!instance)
                instance = GetClass()->Get<UM>("get_Instance")->Invoke<ScriptService*>();
            return instance;
        }

        static auto RunScript(std::string script) -> void
        {
            static UM* method;
            if (!method)
                method = GetClass()->Get<UM>("RunScript");

            Game* gameInstance = Game::GetInstance();
            if (!gameInstance)
            {
                std::cout << "[ERROR] Failed to run script: Game instance is null!" << std::endl;
                return;
            }

            BaseScript* scriptInstance = reinterpret_cast<BaseScript*>(gameInstance->GetGameObject()->AddComponent<ScriptInstance*>(ScriptInstance::GetClass()));
            scriptInstance->SetSource(US::New(script));
            scriptInstance->SetRunning(false);
            auto instance = GetInstance();
            if (method && instance)
                method->Invoke<void, void *, BaseScript*>(instance, scriptInstance);
            else
                std::cout << "[ERROR] Failed to run script: method or instance is null!" << std::endl;
        }

        static auto DecompileScript(BaseScript* scriptInstance) -> std::string
        {
            return scriptInstance->GetSource()->ToString();
        }
    };
}

#endif /* TYPES */
 