#include "gui.h"

TextEditor c_gui::editor;
c_gui::execute_cb_t c_gui::execute_cb = nullptr;

void c_gui::set_execute_callback(execute_cb_t cb) {
	execute_cb = cb;
}

void c_gui::draw() {
	using namespace ImGui;

	SetNextWindowSize(ImVec2(600, 380), ImGuiCond_FirstUseEver);
	if (Begin("Miisploit", nullptr, ImGuiWindowFlags_NoCollapse)) {
		Text("Miisploit ImGui overlay");
		Separator();
		editor.Render("LuaEditor", ImVec2(0, GetContentRegionAvail().y - 32.0f), true);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
		if (Button("Execute", ImVec2(120, 28))) {
			if (execute_cb) {
				const std::string script = editor.GetText();
				execute_cb(script.c_str());
			}
		}
		SameLine();
		if (Button("Clear", ImVec2(120, 28))) editor.SetText("");
		SameLine();
		Text("Injected");
	}
	End();
}
