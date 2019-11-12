workspace "CWStarter"
	architecture "x64"
	startproject "CWStarter"

	configurations
	{
		"Debug",
		"Release"
	}

project "CWStarter"
	location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/")
	objdir ("build/")

	
	files
	{
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/include/",
		"../../vendor/Box2D/",
		"../../vendor/SFML-2.4.2/include"
	}

	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		
		libdirs 
		{
			"../../vendor/Box2D/x64/Debug",
			"../../vendor/SFML-2.4.2/lib"
		}
		
		
		debugenvs { "PATH=%PATH%;../../../vendor/SFML-2.4.2/bin;"}
		
		links
		{
			"Box2D",
			"sfml-main",
			"sfml-graphics-d",
			"sfml-system-d",
			"sfml-window-d",
			"sfml-audio-d"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines "SFML_STATIC"
		
		libdirs 
		{
			"../../vendor/Box2D/x64/Release",
			"../../vendor/SFML-2.4.2/lib"
		}