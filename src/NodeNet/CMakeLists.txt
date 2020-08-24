cmake_minimum_required(VERSION 3.16)
project(NodeNet)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

include_directories(NodeNet ${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-F/Library/Frameworks -framework SDL2 -framework CoreFoundation -stdlib=libc++")

#find_package(implot)


include_directories(.)
include_directories(bin)
include_directories(build)
include_directories(dependencies)
include_directories(dependencies/gl3w)
include_directories(dependencies/gl3w/include)
include_directories(dependencies/gl3w/include/GL)
include_directories(dependencies/gl3w/src)
include_directories(dependencies/imgui-1.76)
include_directories(fonts)
include_directories(lib)
include_directories(src)
include_directories(include/)

file(GLOB SOURCES
        include/*.h
        include/*.cpp
        )

add_executable(NodeNet
        dependencies/gl3w/include/GL/gl3w.h
        dependencies/gl3w/include/GL/glcorearb.h
        dependencies/gl3w/src/gl3w.c
        dependencies/imgui-1.76/imconfig.h
        dependencies/imgui-1.76/imgui.cpp
        dependencies/imgui-1.76/imgui.h
        dependencies/imgui-1.76/imgui_demo.cpp
        dependencies/imgui-1.76/imgui_draw.cpp
        dependencies/imgui-1.76/imgui_impl_opengl3.cpp
        dependencies/imgui-1.76/imgui_impl_opengl3.h
        dependencies/imgui-1.76/imgui_impl_sdl.cpp
        dependencies/imgui-1.76/imgui_impl_sdl.h
        dependencies/imgui-1.76/imgui_internal.h
        dependencies/imgui-1.76/imgui_widgets.cpp
        dependencies/imgui-1.76/imstb_rectpack.h
        dependencies/imgui-1.76/imstb_textedit.h
        dependencies/imgui-1.76/imstb_truetype.h
        lib/Debug/libgl3w.a
        lib/Debug/libimgui.a
        lib/Debug/libimnodes.a
        src/main.cpp
        src/editor.h
        src/editor.cpp
        src/tinyfiledialogs.c
        src/tinyfiledialogs.h
        imnodes.cpp
        imnodes.h
        Makefile src/data_types.cpp
        ${SOURCES}
        src/generator.cpp
        src/settings.h
        src/utils.cpp
        src/executor.h
        src/templates.h
        src/generator.h
        src/utils.h
        src/data_types.h
        src/executor.cpp
        src/trainer.cpp
        src/trainer.h
        src/dataset_manager.cpp
        src/dataset_manager.h
        src/code_manager.h
        src/logger.h
        src/exceptions.h
        src/serializerTest.h src/fonts.h)

target_link_libraries(NodeNet ${SDL2_LIBRARIES})