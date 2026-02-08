{
    files = {
        [[build\.objs\poly\windows\x64\release\src\main.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\renderer\gui\gui.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\renderer\renderer.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui_demo.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui_draw.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui_impl_dx11.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui_impl_win32.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui_tables.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\imgui_widgets.cpp.obj]],
        [[build\.objs\poly\windows\x64\release\src\imgui\TextEditor\TextEditor.cpp.obj]]
    },
    values = {
        [[C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-machine:x64",
            [[-libpath:C:\Users\Administrator\AppData\Local\.xmake\packages\m\minhook\v1.3.4\42db4578518a4e7dab516663d1f46c6b\lib]],
            "/opt:ref",
            "/opt:icf",
            "minhook.lib",
            "winhttp.lib",
            "user32.lib",
            "d3d11.lib",
            "dxgi.lib"
        }
    }
}