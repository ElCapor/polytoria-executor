#ifndef ENVIROMENT
#define ENVIROMENT

#include <HookManager.hpp>
#include <polytoria/polytoria.hpp>
using namespace polytoria;
#include <unity/il2cpp.hpp>
#include <unity/u.hpp>
#include <format>
#include <skipsped/net_tracer.hpp>

using CF = DynValue*(*)(void*, ScriptExecutionContext*, CallbackArguments*);

DynValue* CF_Poop(void*, ScriptExecutionContext* ctx, CallbackArguments* args)
{
	std::printf("Yas\n");
;
	return DynValue::NewString(US::New("Poop"));
}

DynValue* hookinvokeserver(void*, ScriptExecutionContext* ctx, CallbackArguments* args)
{
    std::printf("InvokeServer called!\n");
	std::cout << "callback type" << args->GetType()->GetFullNameOrDefault()->ToString() << std::endl;


    // Get count - this should work
    int count = args->GetCount();
    std::cout << "count = " << count << std::endl;
	
	if (count < 2) {
		std::cout << "Not enough arguments passed to InvokeServer hook" << std::endl;
		return DynValue::NewString(US::New("Not enough arguments"));
	}

	DynValue* net_event = args->RawGet(0, false);
	if (net_event == nullptr && net_event->GetDataType() != 8)
	{
		std::cout << "fAAAAAAA" << std::endl;
		return DynValue::NewString(US::New("Invalid argument..."));
	}
	NetworkEvent* obj = (NetworkEvent*)(net_event->ToObject(NetworkEvent::GetClass()->GetType()));
	std::cout << obj->GetName()->ToString() << std::endl;

	DynValue* callback = args->RawGet(1, false);
	if (callback == nullptr && callback->GetDataType() != DynValue::DataType::Function)
	{
		std::cout << "sorry not a function wowie" << std::endl;
		return DynValue::NewString(US::New("Invalid arg, expected function..."));
	}

	NetTracer::callback = callback;
	NetTracer::exec = ctx->GetOwnerScript();

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
    
    return DynValue::NewString(US::New("check urcmd nigga"));
}


void RegisterCFunction(const std::string& name, CF func, Table* table)
{
	// ~TODO: uhh maybe add some "type caching" feature to avoid doing lookups every time~ / Solved by ElCapor :wink:
	auto dynValueKlass = DynValue::GetClass();
	auto scriptExecutionContext = ScriptExecutionContext::GetClass();
	auto callbackArguments = CallbackArguments::GetClass();
	
	if (!dynValueKlass || !scriptExecutionContext || !callbackArguments) return;
			
	auto mi = il2cpp::CreateCustomMethodInfo(std::format("{}_methodinfo", name).c_str(),
		{
			(il2cpp::Il2CppType*)scriptExecutionContext->GetType(),
			(il2cpp::Il2CppType*)callbackArguments->GetType()
		},
		(il2cpp::Il2CppType*)dynValueKlass->GetType(),
		(il2cpp::Il2CppMethodPointer)func
	);


	const auto mscorelibAsm = U::Get("mscorlib.dll");
	if (!mscorelibAsm) {
		std::cout << "[ERROR] Failed to find assembly: mscorlib.dll" << std::endl;
		return;
	}

	const auto typeKlass = mscorelibAsm->Get("Func`3", "System");
	if (!typeKlass) {
		std::cout << "[ERROR] Failed to find class: Func`3" << std::endl;
		return;
	}

	const auto type = typeKlass->GetType();

	auto delegates = U::Invoke<UO*, uintptr_t>("il2cpp_object_new", *(uintptr_t*)type);
	il2cpp::BindDelegate((il2cpp::Il2CppDelegate*)delegates, mi);

	auto finalCallbackMethod = dynValueKlass->Get<UM>("NewCallback");
	if (!finalCallbackMethod) {
		std::cout << "[ERROR] Failed to find method: DynValue::NewCallback" << std::endl;
		return;
	}
	auto finalCallback = finalCallbackMethod->Cast<UO*, UO*, US*>()((UO*)delegates, 0);

	table->SetItem(US::New(name.c_str()), (UO*)finalCallback);
}

void InstallEnviromentFunctions(Table* table)
{
	RegisterCFunction("Poop", CF_Poop, table);
	RegisterCFunction("InvokeServerHook", hookinvokeserver, table);
}

#endif /* ENVIROMENT */
