-- premake5.lua
workspace "RayTracer"
	architecture "x64"
	configurations { "Debug", "Release" }
	
	filter "system:windows"
		location "msvc"

project "RayTracer"
	kind "ConsoleApp"
	language "C++"
	
	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"
	
	files 
	{ 
		"src/**.h", 
		"src/**.cpp"
	}
	
	includedirs
	{
		"lib_3rd/freeglut-3.0.0/include",
		"lib_3rd/glui-2.36/include"
	}
	
	links
	{
		"lib_3rd/freeglut-3.0.0/lib/x64/freeglut.lib"
	}
	
	postbuildcommands
	{
		"{COPY} " .. path.getabsolute(".") .. "/lib_3rd/freeglut-3.0.0/bin/x64/freeglut.dll %{cfg.targetdir}"
	}
	
	filter "system:windows"
		location "msvc"
		cppdialect "C++17"
		systemversion "latest"
		defines { "_WIN" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
		links
		{
			"lib_3rd/glui-2.36/msvc/lib/glui32d.lib",
			"lib_3rd/glui-2.36/msvc/lib/glui32.lib"	-- ???
		}

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		
		links
		{
			"lib_3rd/glui-2.36/msvc/lib/glui32.lib"
		}