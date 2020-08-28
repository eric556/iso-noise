function includeFastNoise()
    includedirs "libraries/fast-noise"
    files "libraries/fast-noise/*.cpp"
end

function includeEntt()
    includedirs "libraries/entt/"
end

function includeIMGUI()
    includedirs "libraries/IMGUI/imgui"
    includedirs "libraries/IMGUI/imgui-sfml"
end

function linkIMGUI()
    files "libraries/IMGUI/imgui/imgui.cpp"
    files "libraries/IMGUI/imgui/imgui_draw.cpp"
    files "libraries/IMGUI/imgui/imgui_widgets.cpp"
    files "libraries/IMGUI/imgui-sfml/imgui-SFML.cpp"
    files "libraries/IMGUI/imgui/imgui_demo.cpp"
end

function includeSFML()
    includedirs "libraries/SFML/include"
end

function linkSFML()
    libdirs "libraries/SFML/lib"
    defines {"SFML_STATIC"}

    configuration "Debug"
        links {
            "sfml-window-s-d",
            "sfml-system-s-d",
            "sfml-audio-s-d",
            "sfml-network-s-d",
            "sfml-graphics-s-d",
            "opengl32",
            "freetype",
            "winmm",
            "gdi32",
            "openal32",
            "flac",
            "vorbisenc",
            "vorbisfile",
            "vorbis",
            "ogg",
            "ws2_32"
        }

    configuration "Release"
        links {
            "sfml-window-s",
            "sfml-system-s",
            "sfml-audio-s",
            "sfml-network-s",
            "sfml-graphics-s",
            "opengl32",
            "freetype",
            "winmm",
            "gdi32",
            "openal32",
            "flac",
            "vorbisenc",
            "vorbisfile",
            "vorbis",
            "ogg",
            "ws2_32"
        }
    
    filter{}
end

function includeRaylib()
    includedirs "libraries/ray-lib/include"
end

function linkRaylib()
    libdirs "libraries/ray-lib/lib"
    defines {
        "GRAPHICS_API_OPENGL_33",
        "PLATFORM_DESKTOP"
    }

    links {
        "raylib",
    }
    filter{}
end

workspace "playground-workspace"
    location "projects"
    language "C++"
    architecture "x86_64"

    configurations {"Debug", "Release"}

    filter{"configurations:Debug"}
        symbols "On"

    filter{"configurations:Release"}
        optimize "On"

    filter{}

    targetdir("build/bin/%{prj.name}/%{cfg.longname}")
    objdir("build/obj/%{prj.name}/%{cfg.longname}")

project "iso"
    location "projects/iso"
    kind "ConsoleApp"
    cppdialect "C++17"
    files "projects/iso/src/**.cpp"
    files "projects/iso/src/**.h"
    files "projects/iso/src/**.hpp"
    includeFastNoise()
    includeSFML()
    linkSFML()
    includeIMGUI()
    linkIMGUI()

project "sand"
    location "projects/sand"
    kind "ConsoleApp"
    cppdialect "C++17"
    files "projects/sand/src/**.cpp"
    files "projects/sand/src/**.h"
    files "projects/sand/src/**.hpp"
    includeFastNoise()
    includeSFML()
    linkSFML()
    includeIMGUI()
    linkIMGUI()

project "dino"
    location "projects/dino"
    kind "ConsoleApp"
    cppdialect "C++17"
    files "projects/dino/src/**.cpp"
    files "projects/dino/src/**.h"
    files "projects/dino/src/**.hpp"
    includeRaylib()
    linkRaylib()
    includeEntt()
