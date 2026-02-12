#ifndef POLYTORIA
#define POLYTORIA

#include <unity/u.hpp>
#include <HookManager.hpp>

namespace polytoria
{
    struct Instance : public UC
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("Instance");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: Instance" << std::endl;
            }
            return klass;
        }

        auto GetName() -> US *
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("get_Name");
                if (!method)
                    std::cout << "[ERROR] Failed to find method: Instance::get_Name" << std::endl;
            }
            if (method)
                return method->Invoke<US *>(this);
            return {};
        }

        auto GetChildren() -> UArray<Instance *> *
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("GetChildren");
                if (!method)
                    std::cout << "[ERROR] Failed to find method: Instance::GetChildren" << std::endl;
            }
            if (method)
                return method->Invoke<UArray<Instance *> *>(this);
            return {};
        }

        auto GetFullName() -> US *
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("get_FullName");
                if (!method)
                    std::cout << "[ERROR] Failed to find method: Instance::get_FullName" << std::endl;
            }
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
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("Game");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: Game" << std::endl;
            }
            return klass;
        }

        static auto GetInstance() -> Game *
        {
            static Game *instance;
            if (!instance)
            {
                auto klass = GetClass();
                if (klass)
                {
                    auto field = klass->Get<UF>("singleton");
                    if (field)
                        field->GetStaticValue(&instance);
                }
            }
            return instance;
        }

        static auto GetGameName() -> US *
        {
            static US *gameName = nullptr;
            if (!gameName)
            {
                auto klass = GetClass();
                if (klass)
                {
                    auto field = klass->Get<UF>("GameName");
                    if (field)
                        field->GetStaticValue(&gameName);
                }
            }
            return gameName;
        }

        static auto GetGameID() -> int
        {
            static int gameID = 0;
            if (gameID == 0)
            {
                auto klass = GetClass();
                if (klass)
                {
                    auto field = klass->Get<UF>("gameID");
                    if (field)
                        field->GetStaticValue(&gameID);
                }
            }
            return gameID;
        }
    };

    struct GameIO : public UC
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("GameIO");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: GameIO" << std::endl;
            }
            return klass;
        }

        static auto GetInstance() -> GameIO *
        {
            static GameIO *instance;
            if (!instance)
            {
                auto klass = GetClass();
                if (klass)
                {
                    auto field = klass->Get<UF>("singleton");
                    if (field)
                        field->GetStaticValue(&instance);
                }
            }
            return instance;
        }

        static auto SaveInstance(const char *path)
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("SaveToFile");
                if (!method)
                    std::cout << "[ERROR] Failed to find method: GameIO::SaveToFile" << std::endl;
            }

            auto instance = GetInstance();
            if (method && instance)
                method->Invoke<void, void *, US *>(instance, US::New(path));
            else if (!method)
                std::cout << "[ERROR] Failed to save instance: method is null!" << std::endl;
            else if (!instance)
                std::cout << "[ERROR] Failed to save instance: instance is null!" << std::endl;
        }
    };

    struct BaseScript : public Instance
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("BaseScript");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: BaseScript" << std::endl;
            }
            return klass;
        }

        auto SetRunning(bool running) -> void
        {
            GetClass()->SetValue<bool>(this, "running", running);
        }

        auto IsRunning() -> bool
        {
            return GetClass()->GetValue<bool>(this, "running");
        }

        auto SetSource(US *source) -> void
        {
            GetClass()->SetValue<US *>(this, "source", source);
        }

        auto GetSource() -> US *
        {
            return GetClass()->GetValue<US *>(this, "source");
        }

        auto GetRequestedRun() -> bool
        {
            return GetClass()->GetValue<bool>(this, "requestedRun");
        }

        auto SetRequestedRun(bool requestedRun) -> void
        {
            GetClass()->SetValue<bool>(this, "requestedRun", requestedRun);
        }
    };

    struct ScriptInstance : public Instance
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("ScriptInstance");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: ScriptInstance" << std::endl;
            }
            return klass;
        }
    };

    struct ScriptService : public Instance
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("ScriptService");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: ScriptService" << std::endl;
            }
            return klass;
        }

        static void InstallHooks();

        static auto GetInstance() -> ScriptService *
        {
            static ScriptService *instance;
            if (!instance)
            {
                auto klass = GetClass();
                if (klass)
                {
                    auto method = klass->Get<UM>("get_Instance");
                    if (method)
                        instance = method->Invoke<ScriptService *>();
                    else
                        std::cout << "[ERROR] Failed to find method: ScriptService::get_Instance" << std::endl;
                }
            }
            return instance;
        }

        static auto RunScript(std::string script) -> void
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("RunScript");
                if (!method)
                    std::cout << "[ERROR] Failed to find method: ScriptService::RunScript" << std::endl;
            }

            Game *gameInstance = Game::GetInstance();
            if (!gameInstance)
            {
                std::cout << "[ERROR] Failed to run script: Game instance is null!" << std::endl;
                return;
            }

            BaseScript *scriptInstance = reinterpret_cast<BaseScript *>(gameInstance->GetGameObject()->AddComponent<ScriptInstance *>(ScriptInstance::GetClass()));
            scriptInstance->SetSource(US::New(script));
            scriptInstance->SetRunning(false);

            auto instance = GetInstance();
            if (method && instance)
                method->Invoke<void, void *, BaseScript *>(instance, scriptInstance);
            else if (!method)
                std::cout << "[ERROR] Failed to run script: method is null!" << std::endl;
            else if (!instance)
                std::cout << "[ERROR] Failed to run script: instance is null!" << std::endl;
        }

        static auto RunScript(BaseScript *scriptInstance) -> void
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("RunScript");
                if (!method)
                    std::cout << "[ERROR] Failed to find method: ScriptService::RunScript" << std::endl;
            }

            auto instance = GetInstance();
            if (method && instance)
                method->Invoke<void, void *, BaseScript *>(instance, scriptInstance);
            else if (!method)
                std::cout << "[ERROR] Failed to run script: method is null!" << std::endl;
            else if (!instance)
                std::cout << "[ERROR] Failed to run script: instance is null!" << std::endl;
        }

        static auto DecompileScript(BaseScript *scriptInstance) -> std::string
        {
            return scriptInstance->GetSource()->ToString();
        }
    };

    struct NetworkConnection
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get("Mirror.dll")->Get("NetworkConnection");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: NetworkConnection" << std::endl;
            }
            return klass;
        }
    };

    struct NetworkConnectionToClient : public NetworkConnection
    {

        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get("Mirror.dll")->Get("NetworkConnectionToClient");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: NetworkConnectionToClient" << std::endl;
            }
            return klass;
        }
    };

    struct NetMessage
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("NetMessage");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: NetMessage" << std::endl;
            }
            return klass;
        }
    };

    struct NetworkEvent : public Instance
    {

        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP)->Get("NetworkEvent");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: NetworkEvent" << std::endl;
            }
            return klass;
        }
    };
    struct DynValue;
    struct Table
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("Table", "MoonSharp.Interpreter");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: Table" << std::endl;
            }
            return klass;
        }

        // could be any object
        auto SetItem(UO *key, UO *value) -> void
        {
            static UM *setItemMethod;
            if (!setItemMethod)
            {
                setItemMethod = GetClass()->Get<UM>("set_Item", {"System.Object", "System.Object"});
                if (!setItemMethod)
                    std::cout << "[ERROR] Failed to find method: Table::set_Item" << std::endl;
            }
            if (setItemMethod)
                setItemMethod->Invoke<void>(this, key, value);
        }

        auto GetItem(UO *key) -> DynValue *
        {
            static UM *method;
            if (!method)
            {
                method = GetClass()->Get<UM>("get_Item", {"System.Object"});
                if (!method)
                    std::cout << "[ERROR] Failed to find method: Table::get_Item" << std::endl;
            }
            if (method)
                return method->Invoke<DynValue *>(this, key);
            return {};
        }
    };

    struct Script : public UO
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("Script", "MoonSharp.Interpreter");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: Script" << std::endl;
            }
            return klass;
        }

        Table *GetRegistry()
        {
            return GetClass()->GetValue<Table *>(this, "<Registry>k__BackingField");
        }
    };

    struct ScriptExecutionContext : public UO
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("ScriptExecutionContext", "MoonSharp.Interpreter");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: ScriptExecutionContext" << std::endl;
            }
            return klass;
        }

        auto GetOwnerScript() -> Script *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("get_OwnerScript");
            return method->Invoke<Script *>(this);
        }
    };

    struct Closure;

    struct DynValue : public UO
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("DynValue", "MoonSharp.Interpreter");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: DynValue" << std::endl;
            }
            return klass;
        }

        static auto NewString(US *str) -> DynValue *
        {
            static UM *newStringMethod;
            if (!newStringMethod)
            {
                newStringMethod = GetClass()->Get<UM>("NewString");
                if (!newStringMethod)
                    std::cout << "[ERROR] Failed to find method: DynValue::NewString" << std::endl;
            }
            if (newStringMethod)
                return newStringMethod->Invoke<DynValue *, US *>(str);
            return {};
        }

        static auto NewCallback(UO *callback, US *name) -> DynValue *
        {
            static UM *newCallbackMethod;
            if (!newCallbackMethod)
            {
                newCallbackMethod = GetClass()->Get<UM>("NewCallback");
                if (!newCallbackMethod)
                    std::cout << "[ERROR] Failed to find method: DynValue::NewCallback" << std::endl;
            }
            if (newCallbackMethod)
                return newCallbackMethod->Invoke<DynValue *, UO *, US *>(callback, name);
            return {};
        }

        static auto NewNil() -> DynValue *
        {
            static UM *newNilMethod;
            if (!newNilMethod)
            {
                newNilMethod = GetClass()->Get<UM>("NewNil");
                if (!newNilMethod)
                    std::cout << "[ERROR] Failed to find method: DynValue::NewNil" << std::endl;
            }
            if (newNilMethod)
                return newNilMethod->Invoke<DynValue *>();
            return {};
        }

        enum DataType : int
        {
            Nil,
            Void,
            Boolean,
            Number,
            String,
            Function,
            Table,
            Tuple,
            UserData,
            Thread,
            ClrFunction,
            TailCalRequest,
            YieldRequest,
        };

        auto GetDataType() -> DataType
        {
            return (DataType)(GetClass()->GetValue<int>(this, "m_Type"));
        }

        // TODO: replace with a templated function , that takes a constrained T with a getclass function, i.e std::is_same_as
        auto ToObject(void *csType) -> UO *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("ToObject", {"System.Type"});
            return method->Invoke<UO *>(this, csType);
        }

        auto GetFunction() -> Closure *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("get_Function");
            return method->Invoke<Closure *>(this);
        }
    };

    struct Closure : public UO
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("Closure", "MoonSharp.Interpreter");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: Closure" << std::endl;
            }
            return klass;
        }

        auto Call() -> DynValue *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("Call", {});
            return method->Invoke<DynValue *>(this);
        }

        auto Call(std::vector<UO *> objs) -> DynValue *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("Call", {"System.Object[]"});
            UArray<UO *> *arr = UArray<UO *>::New(UO::GetClass(), objs.size());
            for (int i = 0; i < objs.size(); i++)
            {
                arr->operator[](i) = objs[i];
            }
            return method->Invoke<DynValue *>(this, arr);
        }

        auto Call(std::vector<DynValue *> objs) -> DynValue *
        {
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("Call", {"MoonSharp.Interpreter.DynValue[]"});
            UArray<DynValue *> *arr = UArray<DynValue *>::New(DynValue::GetClass(), objs.size());
            for (int i = 0; i < objs.size(); i++)
            {
                arr->operator[](i) = objs[i];
            }
            return method->Invoke<DynValue *>(this, arr);
        }
    };

    struct CallbackArguments : public UO
    {
        static auto GetClass() -> UK *
        {
            static UK *klass;
            if (!klass)
            {
                klass = U::Get(ASSEMBLY_CSHARP_FIRSTPASS)->Get("CallbackArguments", "MoonSharp.Interpreter");
                if (!klass)
                    std::cout << "[ERROR] Failed to find class: CallbackArguments" << std::endl;
            }
            return klass;
        }

        auto GetArgs() -> U::UnityType::List<DynValue *> *
        {
            return GetClass()->GetValue<U::UnityType::List<DynValue *> *>(this, "m_Args");
        }

        auto GetCount() -> int
        {
            return GetClass()->GetValue<int>(this, "m_Count");
        }

        auto RawGet(int at, bool translatevoids) -> DynValue *
        {
            // if (at < GetCount() || GetArgs() == nullptr) return nullptr;
            static UM *method;
            if (!method)
                method = GetClass()->Get<UM>("RawGet");
            return method->Invoke<DynValue *, CallbackArguments *, int, bool>(this, at, translatevoids);
        }
    };

}

#endif /* POLYTORIA */
