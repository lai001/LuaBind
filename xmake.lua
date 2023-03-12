add_requires("spdlog")

local RegisterIncludePath = "LuaGenerator/build/Register"

task("project_setup")
    on_run(function ()
        import("devel.git")
        if os.exists("./ThirdParty") == false then
            os.mkdir("./ThirdParty")
        end
        if os.exists("./lua/ThirdParty") == false then
            os.mkdir("./lua/ThirdParty")
        end        
        if os.exists("./ThirdParty/lua") == false then
            git.clone("https://github.com/lua/lua.git", {depth = 1, branch = "master", outputdir = "./ThirdParty/lua"})
        end
        if os.exists("./lua/ThirdParty/inspect.lua") == false then
            git.clone("https://github.com/kikito/inspect.lua.git", {depth = 1, branch = "master", outputdir = "./lua/ThirdParty/inspect.lua"})
        end
    end)

    set_menu {
        usage = "xmake project_setup",
        description = "Setup project.",
        options = {
        }
    }

target("Dependency")
    set_kind("phony")
    before_build(function (target)
        import("core.project.task")
        task.run("project_setup")
    end)

target("Lua")
    set_kind("static")
    set_languages("c11")
    add_rules("mode.debug", "mode.release")
    add_includedirs("ThirdParty/lua", {interface = true})
    add_headerfiles("ThirdParty/lua/*.h")
    add_files("ThirdParty/lua/*.c")
    remove_files("ThirdParty/lua/onelua.c", "thirdparty/lua/lua.c")

target("LuaBind")
    set_kind("binary")
    set_languages("c++17")
    add_rules("mode.debug", "mode.release")

    add_includedirs("Source", "Source/Include")
    add_headerfiles("Source/**.hpp")
    add_headerfiles("Source/**.h")
    add_files("Source/**.cpp")

    add_deps("Dependency")
    add_deps("Lua")
    
    add_includedirs(RegisterIncludePath)
    add_headerfiles(RegisterIncludePath .. "/**.hpp")
    add_headerfiles(RegisterIncludePath .. "/**.h")
    add_files(RegisterIncludePath .. "/**.cpp")

    add_packages("spdlog")

    after_build(function(target)
        os.cp("./lua", target:targetdir())
    end)
