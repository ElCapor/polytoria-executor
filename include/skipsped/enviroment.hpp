#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <HookManager.hpp>
#include <polytoria/polytoria.hpp>

#include <unity/il2cpp.hpp>

#include <unity/u.hpp>

using CF = UO*(*)(UO*, UO*);

UO* CF_Poop(UO* ctx, UO* args)
{
	std::printf("Yas\n");

	const auto asmFirstPass = U::Get("Assembly-CSharp-firstpass.dll");
	auto dynvalueKlass = asmFirstPass->Get("DynValue", "MoonSharp.Interpreter");

	auto val = dynvalueKlass->Get<U::Method>("NewString")->Cast<UO*, US*>()(US::New("Nooo"));
	return val;
}

void RegisterCFunction(const std::string& name, CF func, UO* Table)
{
	// TODO: uhh maybe add some "type caching" feature to avoid doing lookups every time
	const static auto firstpassAsm = U::Get(ASSEMBLY_CSHARP_FIRSTPASS);
	auto dynValueKlass = firstpassAsm->Get("DynValue", "MoonSharp.Interpreter");
	auto scriptExecutionContext = firstpassAsm->Get("ScriptExecutionContext", "MoonSharp.Interpreter");
	auto callbackArguments = firstpassAsm->Get("CallbackArguments", "MoonSharp.Interpreter");
			
	auto mi = il2cpp::CreateCustomMethodInfo("huge anus",
		{
			(il2cpp::Il2CppType*)scriptExecutionContext->GetType(),
			(il2cpp::Il2CppType*)callbackArguments->GetType()
		},
		(il2cpp::Il2CppType*)dynValueKlass->GetType(),
		(il2cpp::Il2CppMethodPointer)func
	);


	const auto mscorelibAsm = U::Get("mscorlib.dll");
	const auto type = mscorelibAsm->Get("Func`3")->GetType();

	auto delegates = U::Invoke<UO*, uintptr_t>("il2cpp_object_new", *(uintptr_t*)type);
	il2cpp::BindDelegate((il2cpp::Il2CppDelegate*)delegates, mi);

	auto finalCallback = dynValueKlass->Get<U::Method>("NewCallback")->Cast<UO*, UO*, US*>()((UO*)delegates, 0);

	U::Get(ASSEMBLY_CSHARP_FIRSTPASS)
	->Get("Table")
	->Get<U::Method>("set_Item", {"System.Object", "System.Object"})
	->Invoke<void>(Table, US::New("Poop"), finalCallback);
}

void InstallEnviromentFunctions(UO* Table)
{
	RegisterCFunction("Poop", CF_Poop, Table);
}

#endif /* ENVIRONMENT_HPP */
