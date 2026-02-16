#include <unity/unity.h>
#include <Windows.h>


void Unity::Init() {
    HMODULE module = GetModuleHandleA("GameAssembly.dll");
    nasec::Assert(module != NULL, "Failed to get GameAssembly.dll module handle");

    UnityResolve::Init(module, UnityResolve::Mode::Il2Cpp);
}

void Unity::ThreadAttach() {
    UnityResolve::ThreadAttach();
}

void Unity::ThreadDetach() {
    UnityResolve::ThreadDetach();
}