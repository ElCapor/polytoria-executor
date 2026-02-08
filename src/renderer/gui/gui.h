#pragma once
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#include "imgui/TextEditor/TextEditor.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class c_gui {
public:
	using execute_cb_t = void(*)(const char*);

	static void set_execute_callback(execute_cb_t cb);
	static TextEditor editor;
	static void draw();

private:
	static execute_cb_t execute_cb;
};
