set_allowedarchs("x64")
add_rules("mode.debug", "mode.release")
set_policy("package.cmake_generator.ninja", true)

add_requires("nativefiledialog-extended","microsoft-detours", "imgui", "boost", "libcurl", {configs = {dx11 = true, win32 =true, regex=true}})
add_requires("spdlog", {configs = {header_only=true}})

target("wowiezz")
    set_kind("shared")
    set_languages("c++23")
    add_includedirs(".")
    add_files("**.cpp|injector/*.cpp|hwidspoofer/proxy.cpp|hwidspoofer/hwidspoofer.cpp|hwidspoofer/multiclient.cpp")
    add_packages("spdlog", "microsoft-detours", "d3d11", "imgui", "boost", "nativefiledialog-extended", "libcurl")
    add_links("user32", "dbghelp", "d3d11")

    after_build(function(target)
        os.cp(target:targetfile(), ".download/wowiezz.dll")

        if os.exists("WpfUI/bin/Release/net8.0-windows") then 
            os.cp("WpfUI/bin/Release/net8.0-windows/*", ".download/")
        end
    end)


target("injector")
    set_kind("binary")
    set_languages("c++20")
    add_files("injector/main.cpp")

    after_build(function(target)
        os.cp(target:targetfile(), ".download/injector.exe")
    end)

target("version")
    set_kind("shared")
    set_languages("c++23")
    add_files("hwidspoofer/proxy.cpp", "hwidspoofer/hwidspoofer.cpp", "hwidspoofer/multiclient.cpp", "unity/unity.cpp", "core/core.cpp")
    add_includedirs(".")
    add_packages("spdlog", "microsoft-detours")
    add_links("user32", "dbghelp")

    after_build(function(target)
        os.cp(target:targetfile(), ".download/version.dll")
    end)