#include <ptoria/scriptservice.h>
#include <ptoria/scriptinstance.h>
#include <hooking/hookmanager.h>
#include <moonsharp/cinterface.h>
#include <ptoria/networkevent.h>

DynValue* ScriptService::callback = nullptr;
Script *ScriptService::exec = nullptr;

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

    /*
    Inject our custom lua environnement
    */
    InstallEnvironnement(script);

    return HookManager::Call(ScriptService::ExecuteScriptInstanceHook, self, script, instance);
}


DynValue* poop(void*, ScriptExecutionContext* context, CallbackArguments* args) {
    return DynValue::FromString("Hello from C++!");
}

DynValue* hookinvokeserver(void*, ScriptExecutionContext* ctx, CallbackArguments* args)
{
    std::printf("InvokeServer called!\n");

    // Get count - this should work
    int count = args->Count();
    std::cout << "count = " << count << std::endl;
	
	if (count < 2) {
		std::cout << "Not enough arguments passed to InvokeServer hook" << std::endl;
		return DynValue::FromString("Not enough arguments");
	}

	DynValue* net_event = args->RawGet(0, false);
	if (net_event == nullptr || net_event->Type() != DynValue::DataType::UserData)
	{
		std::cout << "fAAAAAAA" << std::endl;
		return DynValue::FromString("Invalid argument...");
	}
	
	NetworkEvent* obj = (NetworkEvent*)(net_event->Cast(StaticClass<NetworkEvent>()->GetType()));
	std::cout << obj->Name()->ToString() << std::endl;

	DynValue* callback = args->RawGet(1, false);
	if (callback == nullptr || callback->Type() != DynValue::DataType::Function)
	{
		std::cout << "sorry not a function wowie" << std::endl;
		return DynValue::FromString("Invalid arg, expected function...");
	}
	// WTFF IS WRONG WITH TS ????
	// InvokeServerHook(game["Hidden"]["DraggerPlace"], function(msg) print(msg) end)
	ScriptService::callback = callback;
	ctx->OwnerScript()->Registry()->Set((UnityObject*)UnityString::New("InvokeServerCallback"), (UnityObject*)callback);
	ScriptService::exec = ctx->OwnerScript();
	

	// print(InvokeServerHook(game["Hidden"]["CookTool"], function() print('hi') end))
	
    // // Get the args list - cast from IList to List if needed
    // auto argList = args->GetArgs();
    
    // // Try accessing size directly (it's a field in List<Type>)
    // // The IList interface might have Count property instead
    // int listSize = argList->size;  // If cast to List* worked
    // std::cout << "listSize = " << listSize << std::endl;
    
    // // // Iterate through arguments
    // // for (int i = 0; i < count; i++) {
    // //     DynValue* arg = (*argList)[i];  // Use operator[]
    // //     std::cout << "Arg " << i << std::endl;
    // // }
    
    return DynValue::FromString("Hooked InvokeServer successfully!");
}


void InstallEnvironnement(Script *script)
{
    RegisterCallback(script->Globals(), "poop", poop);
    RegisterCallback(script->Globals(), "InvokeServerHook", hookinvokeserver);
    //
}

void ScriptService::InstallHooks()
{
    HookManager::Install(Unity::GetMethod<"ExecuteScriptInstance">(StaticClass<ScriptService>())->Cast<DynValue*, ScriptService*, Script*, BaseScript*>(), ExecuteScriptInstanceHook);
}