add_rules("mode.debug", "mode.release")

add_requires("nativefiledialog-extended","microsoft-detours", "imgui", "boost", {configs = {dx11 = true, win32 =true, regex=true}})

target("skipsped")
    set_kind("shared")
    set_languages("c++23")
    add_includedirs("include")
    add_files("src/**.cpp")

    add_packages("microsoft-detours", "imgui", "boost", "nativefiledialog-extended")

    add_links("d3d11", "user32", "shell32", "comdlg32")

    after_build(function(target)
        os.cp(target:targetfile(), "C:\\Users\\delly\\AppData\\Roaming\\Polytoria\\Client\\1.4.152")
    end)

target("version")
    set_kind("shared")
    set_languages("c++20")
    add_includedirs("include")
    add_files("proxy/version.cpp")
    add_files("proxy/spoofer.cpp")
    set_filename("version.dll")
    add_packages("microsoft-detours")

    after_build(function(target)
        os.cp(target:targetfile(), "C:\\Users\\delly\\AppData\\Roaming\\Polytoria\\Client\\1.4.152")
    end)
