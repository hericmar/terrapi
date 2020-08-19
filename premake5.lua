workspace "TerraPi"
        architecture "ARM"
        startproject "TerraPi"
        configurations
        {
            "Debug",
            "Release"
        }

        flags
        {
            "MultiProcessorCompile"
        }

        outputdir = "%{wks.location}/bin"
        targetdir (outputdir)
        objdir = "%{wks.location}/out/%{cfg.system}"

project "TerraPi"
        location "src"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        targetname "TerraPi"
        targetextension ("")
        linkoptions {"-lwiringPi" }

        pchheader "src/pch.h"
        pchsource "src/pch.cpp"

        files
        {
            "src/**.h",
            "src/**.hpp",
            "src/**.cpp"
        }
