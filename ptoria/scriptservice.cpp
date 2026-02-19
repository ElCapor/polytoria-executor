#include <ptoria/scriptservice.h>
#include <ptoria/scriptinstance.h>
#include <hooking/hookmanager.h>
#include <moonsharp/cinterface.h>
#include <ptoria/networkevent.h>
#include <ptoria/player.h>
#include <ptoria/tool.h>
#include <ptoria/chatservice.h>
#include <ptoria/mousefunctions.h>
#include <ptoria/drawinglib.h>
#include <ptoria/http.h>

std::vector<BaseScript *> ScriptService::whitelisted;

ScriptService *ScriptService::GetInstance()
{
    static UnityMethod *method = nullptr;
    if (!method)
        method = Unity::GetMethod<"get_Instance">(StaticClass<ScriptService>());
    nasec::Assert(method != nullptr, "Failed to get ScriptService::get_Instance method");
    return method->Invoke<ScriptService *>();
}

void ScriptService::RunScript(BaseScript *script)
{
    whitelisted.push_back(script);
    Unity::GetMethod<"RunScript">(StaticClass<ScriptService>())->Invoke<void, void *, BaseScript *>(GetInstance(), script);
}

std::string ScriptService::ScriptSource(BaseScript *script)
{
    nasec::Assert(script != nullptr, "Script was nullptr");
    return script->Source()->ToString();
}

void InstallEnvironnement(Script *script);
DynValue *ScriptService::ExecuteScriptInstanceHook(ScriptService *self, Script *script, BaseScript *instance)
{
    nasec::Assert(self != nullptr, "ScriptService instance was nullptr");
    nasec::Assert(script != nullptr, "Script was nullptr");
    nasec::Assert(instance != nullptr, "BaseScript instance was nullptr");

    if (std::find(whitelisted.begin(), whitelisted.end(), instance) == whitelisted.end())
    {
        return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
    }

    /*
    Inject our custom lua environnement
    */
    InstallEnvironnement(script);

    return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
}

DynValue *poop(void *, ScriptExecutionContext *context, CallbackArguments *args)
{
    return DynValue::FromString("Hello from C++!");
}

DynValue *activatetool(void *, ScriptExecutionContext *, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to activatetool");
    }

    DynValue *tool = args->RawGet(0, false);
    if (tool == nullptr || tool->Type() != DynValue::DataType::UserData)
    {
        return DynValue::FromString("Invalid argument, expected Tool");
    }

    Tool *toolobj = (Tool *)(tool->Cast(StaticClass<Tool>()->GetType()));
    if (toolobj == nullptr)
    {
        return DynValue::FromString("Failed to cast Tool");
    }

    toolobj->CmdActivate();
    return DynValue::FromNil();
}

DynValue *unequip(void *, ScriptExecutionContext *, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to unequiptool");
    }

    DynValue *tool = args->RawGet(0, false);
    if (tool == nullptr || tool->Type() != DynValue::DataType::UserData)
    {
        return DynValue::FromString("Invalid argument, expected Tool");
    }

    Tool *toolobj = (Tool *)(tool->Cast(StaticClass<Tool>()->GetType()));
    if (toolobj == nullptr)
    {
        return DynValue::FromString("Failed to cast Tool");
    }

    toolobj->CmdUnequip();
    return DynValue::FromNil();
}

DynValue *equip(void *, ScriptExecutionContext *, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to equiptool");
    }

    DynValue *tool = args->RawGet(0, false);
    if (tool == nullptr || tool->Type() != DynValue::DataType::UserData)
    {
        return DynValue::FromString("Invalid argument, expected Tool");
    }

    Tool *toolobj = (Tool *)(tool->Cast(StaticClass<Tool>()->GetType()));
    if (toolobj == nullptr)
    {
        return DynValue::FromString("Failed to cast Tool");
    }

    toolobj->CmdEquip();
    return DynValue::FromNil();
}

DynValue *loadstring(void *, ScriptExecutionContext *ctx, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to LoadString");
    }

    DynValue *source = args->RawGet(0, false);
    if (source == nullptr || source->Type() != DynValue::DataType::String)
    {
        return DynValue::FromString("Invalid argument, expected string");
    }

    UnityClass *stringClass = Unity::GetClass<"String", "mscorlib.dll", "System">();
    UnityString *code = (UnityString *)source->Cast(stringClass->GetType());
    if (code == nullptr)
    {
        return DynValue::FromString("Failed to cast string");
    }

    Script *script = ctx->OwnerScript();
    if (script == nullptr)
    {
        return DynValue::FromString("Owner script not available");
    }

    return script->loadstring(code, script->Globals(), UnityString::New("LoadString"));
}

DynValue *serverequiptool(void *, ScriptExecutionContext *, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 2)
    {
        return DynValue::FromString("Not enough arguments passed to serverequiptool");
    }

    DynValue *player = args->RawGet(0, false);
    if (player == nullptr || player->Type() != DynValue::DataType::UserData)
    {
        return DynValue::FromString("Invalid argument, expected Player");
    }

    DynValue *tool = args->RawGet(1, false);
    if (tool == nullptr || tool->Type() != DynValue::DataType::UserData)
    {
        return DynValue::FromString("Invalid argument, expected Tool");
    }

    Player *playerobj = (Player *)(player->Cast(StaticClass<Player>()->GetType()));
    if (playerobj == nullptr)
    {
        return DynValue::FromString("Failed to cast Player");
    }

    Tool *toolobj = (Tool *)(tool->Cast(StaticClass<Tool>()->GetType()));
    if (toolobj == nullptr)
    {
        return DynValue::FromString("Failed to cast Tool");
    }

    playerobj->EquipTool(toolobj);
    return DynValue::FromNil();
}

DynValue *sendchat(void *, ScriptExecutionContext *ctx, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to SendChat");
    }

    DynValue *message = args->RawGet(0, false);
    if (message == nullptr || message->Type() != DynValue::DataType::String)
    {
        return DynValue::FromString("Invalid argument, expected string");
    }

    UnityClass *stringClass = Unity::GetClass<"String", "mscorlib.dll", "System">();
    UnityString *msg = (UnityString *)message->Cast(stringClass->GetType());
    if (msg == nullptr)
    {
        return DynValue::FromString("Failed to cast string");
    }

    UnityObject *ChatServiceInstance = Unity::GetStaticFieldValue<UnityString *, "Instance">(StaticClass<ChatService>());

    nasec::Assert(ChatServiceInstance != nullptr, "Failed to get ChatService instance");

    UnityClass *chatClass = StaticClass<ChatService>();
    Unity::GetMethod<"SendChat", "System.String">(chatClass)->Invoke<void, void *, UnityString *>(ChatServiceInstance, msg);

    return DynValue::FromNil();
}

DynValue *fireclickdetector(void *, ScriptExecutionContext *ctx, CallbackArguments *args)
{
    int count = args->Count();
    if (count < 1)
    {
        return DynValue::FromString("Not enough arguments passed to FireClickDetector");
    }

    DynValue *instance = args->RawGet(0, false);
    if (instance == nullptr || instance->Type() != DynValue::DataType::UserData)
    {
        return DynValue::FromString("Invalid argument, expected Instance");
    }

    Instance *instanceobj = (Instance *)(instance->Cast(StaticClass<Instance>()->GetType()));
    if (instanceobj == nullptr)
    {
        return DynValue::FromString("Failed to cast Instance");
    }

    instanceobj->CmdClicked();

    return DynValue::FromNil();
}

DynValue *identifyexecutor(void *, ScriptExecutionContext *ctx, CallbackArguments *args)
{
    // elcapor compatible executor
    return DynValue::FromString("elcapor");
}

void InstallEnvironnement(Script *script)
{
    RegisterCallback(script->Globals(), "poop", poop);
    RegisterCallback(script->Globals(), "activatetool", activatetool);
    RegisterCallback(script->Globals(), "unequiptool", unequip);
    RegisterCallback(script->Globals(), "equiptool", equip);
    RegisterCallback(script->Globals(), "loadstring", loadstring);
    RegisterCallback(script->Globals(), "serverequiptool", serverequiptool);
    RegisterCallback(script->Globals(), "sendchat", sendchat);
    RegisterCallback(script->Globals(), "fireclickdetector", fireclickdetector);
    RegisterCallback(script->Globals(), "identifyexecutor", identifyexecutor);

    // Mouse control functions
    RegisterCallback(script->Globals(), "mouse_move", mouse_move);
    RegisterCallback(script->Globals(), "mouse_get_position", mouse_get_position);
    RegisterCallback(script->Globals(), "mouse_click", mouse_click);
    RegisterCallback(script->Globals(), "mouse_down", mouse_down);
    RegisterCallback(script->Globals(), "mouse_up", mouse_up);
    RegisterCallback(script->Globals(), "mouse_scroll", mouse_scroll);
    RegisterCallback(script->Globals(), "mouse_lock", mouse_lock);
    RegisterCallback(script->Globals(), "mouse_visible", mouse_visible);

    // Drawing library functions
    RegisterCallback(script->Globals(), "draw_line", draw_line);
    RegisterCallback(script->Globals(), "draw_rect", draw_rect);
    RegisterCallback(script->Globals(), "draw_circle", draw_circle);
    RegisterCallback(script->Globals(), "draw_text", draw_text_func);
    RegisterCallback(script->Globals(), "draw_clear", draw_clear);

    RegisterCallback(script->Globals(), "httpget", httpget);
}

void ScriptService::InstallHooks()
{
    HookManager::Install(Unity::GetMethod<"ExecuteScriptInstance">(StaticClass<ScriptService>())->Cast<DynValue *, ScriptService *, Script *, BaseScript *>(), ExecuteScriptInstanceHook);
}