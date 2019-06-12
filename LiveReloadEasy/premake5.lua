workspace "LiveCodeReload"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
project "Core"
    location "Core"
    kind "SharedLib"
    language "C++"

    targetdir("bin/" .. outputdir)
    objdir("bin-int/" .. outputdir)

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "Dependencies"
    }

    links
    {
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "10.0.17134.0"

        defines
        {
            "LIVE_CODE_EXPORT"
        }
    
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"

    filter "configurations:Dist"
        defines "NDEBUG"
        optimize "On"

project "LiveReload"
    location "LiveReload"
    kind "ConsoleApp"
    language "C++"

    targetdir("bin/" .. outputdir)
    objdir("bin-int/" .. outputdir)

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "Dependencies",
        "Core/src"
    }

    links
    {
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "10.0.17134.0"

        defines
        {
        }
    
    filter "configurations:Debug"
        defines "DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "On"

    filter "configurations:Dist"
        defines "NDEBUG"
        optimize "On"
