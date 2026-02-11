add_rules("mode.debug", "mode.release")

add_requires("nativefiledialog-extended","microsoft-detours", "imgui", "boost", {configs = {dx11 = true, win32 =true, regex=true}})

target("skipsped")
    set_kind("shared")
    set_languages("c++23")
    add_includedirs("include")
    add_files("src/**.cpp")

    add_packages("microsoft-detours", "imgui", "boost", "nativefiledialog-extended")

    add_links("d3d11", "user32", "shell32", "comdlg32")

target("version")
    set_kind("shared")
    set_languages("c++23")
    add_includedirs("include")
    add_files("proxy/version.cpp", "proxy/spoofer.cpp")
    add_packages("microsoft-detours")
    set_filename("version.dll")
