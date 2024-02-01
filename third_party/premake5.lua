-- Third party builds

includedirs( "stb/include" );
includedirs( "glad/include" );
includedirs( "glfw/include" );
includedirs( "rapidobj/include" );
includedirs( "catch2/include" );
includedirs( "fontstash/include" );

filter "system:macosx"
	-- Additional dependencies required by GLFW on MacOS.
	links {
		"Cocoa.framework", 
		"OpenGL.framework", 
		"IOKit.framework", 
		"CoreVideo.framework" 
	};
filter "*"


project( "x-stb" )
	kind "StaticLib"

	location "."

	filter "toolset:msc-*"
		-- This is a third party project. We don't fix "upstreams" warnings, as
		-- the upstreams project may have different standards w.r.t. warning
		-- levels. Instead, we disable them specifically for this subproject.
		buildoptions {
			"/wd4204", -- nonstandard extension: non-constant aggregate
		}
	filter "*"

	files( "stb/src/*.c" )

project( "x-glad" )
	kind "StaticLib"

	location "."

	files( "glad/src/*.c" )

project( "x-glfw" )
	kind "StaticLib"

	location "."

	filter "system:linux"
		defines { "_GLFW_X11=1" }

	filter "system:windows"
		defines { "_GLFW_WIN32=1" }

	filter "system:macosx"
		defines { "_GLFW_COCOA=1" }

	filter "*"

	filter "toolset:msc-*"
		-- See comment on warnings above.
		buildoptions {
			"/wd4100", -- unreferenced formal parameter
			"/wd4201", -- nonstandard extension: nameless struct/union
			"/wd4204", -- nonstandard extension: non-constant aggregate
			"/wd4244", -- conversion from X to Y, possible loss of data
			"/wd4706", -- assignment within condition expression
		}
	filter "*"

	files {
		"glfw/src/context.c",
		"glfw/src/egl_context.c",
		"glfw/src/init.c",
		"glfw/src/input.c",
		"glfw/src/internal.h",
		"glfw/src/mappings.h",
		"glfw/src/monitor.c",
		"glfw/src/null_init.c",
		"glfw/src/null_joystick.c",
		"glfw/src/null_joystick.h",
		"glfw/src/null_monitor.c",
		"glfw/src/null_platform.h",
		"glfw/src/null_window.c",
		"glfw/src/platform.c",
		"glfw/src/platform.h",
		"glfw/src/vulkan.c",
		"glfw/src/window.c",
		"glfw/src/osmesa_context.c"
	};

	filter "system:linux"
		files {
			"glfw/src/posix_*",
			"glfw/src/x11_*", 
			"glfw/src/xkb_*",
			"glfw/src/glx_*",
			"glfw/src/linux_*",
		};
	filter "system:windows"
		files {
			"glfw/src/win32_*",
			"glfw/src/wgl_*", 
		};
	filter "system:macosx"
		-- WARNING: this is completely untested!
		files {
			"glfw/src/cocoa_*",
			"glfw/src/nsgl_context.m",
			"glfw/src/posix_thread.*",
			"glfw/src/posix_module.*"

		};

	filter "*"

project( "x-rapidobj" )
	kind "Utility"

	location "."

	files( "rapidobj/include/**.h*" )

project( "x-catch2" )
	kind "StaticLib"

	location "."

	filter "system:windows"
		-- Catch2 defaults to the non-standard wmain() entry function
		-- on Windows. Don't do that.
		defines { "DO_NOT_USE_WMAIN=1" }
	filter "*"

	files( "catch2/include/**.hpp" )
	files( "catch2/src/*.cpp" )

project( "x-fontstash" )
	kind "StaticLib"

	location "."

	files( "fontstash/src/*.cpp" )


--EOF
