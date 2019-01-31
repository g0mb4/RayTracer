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
		"src/**.cpp",
		"lib_3rd/imgui-1.67/src/**.cpp"
	}
	
	includedirs
	{
		"lib_3rd/glew-2.1.0/include",
		"lib_3rd/glfw-3.2.1/include",
		"lib_3rd/imgui-1.67/include"
	}
	
	defines {
		"GLEW_STATIC"
	}	
 
	
	links
	{
		"opengl32.lib",
		"glu32.lib",
		"lib_3rd/glew-2.1.0/lib/Release/x64/glew32s.lib",
		"lib_3rd/glfw-3.2.1/lib/glfw3.lib"
	}
	
	postbuildcommands
	{
		--"{COPY} " .. path.getabsolute(".") .. "/lib_3rd/freeglut-3.0.0/bin/x64/freeglut.dll %{cfg.targetdir}"
	}
	
	filter "system:windows"
		location "msvc"
		cppdialect "C++17"
		systemversion "latest"
		defines { "_WIN" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"