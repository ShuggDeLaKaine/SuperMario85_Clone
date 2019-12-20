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
			defines "SFML_STATIC"
		
		libdirs 
		{
			"../../vendor/Box2D/x64/Debug",
			"../../vendor/SFML-2.4.2/lib"
		}
		
		links
		{
			"Box2D",
			"sfml-main",
			"sfml-graphics-s-d",
			"sfml-system-s-d",
			"sfml-window-s-d",
			"sfml-audio-s-d",
			"opengl32.lib",
			"winmm.lib",
			"gdi32.lib",
			"openal32.lib",
			"flac.lib",
			"vorbisenc.lib",
			"vorbisfile.lib",
			"vorbis.lib",
			"ogg.lib",
			"jpeg.lib"
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
		
		links
		{
			"Box2D",
			"sfml-main",
			"sfml-graphics-s",
			"sfml-system-s",
			"sfml-window-s",
			"sfml-audio-s",
			"opengl32.lib",
			"winmm.lib",
			"gdi32.lib",
			"openal32.lib",
			"flac.lib",
			"vorbisenc.lib",
			"vorbisfile.lib",
			"vorbis.lib",
			"ogg.lib",
			"jpeg.lib"
		}