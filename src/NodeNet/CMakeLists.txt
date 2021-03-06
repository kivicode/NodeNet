cmake_minimum_required(VERSION 3.16)
project(NodeNet)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

include_directories(NodeNet ${SDL2_INCLUDE_DIRS})

#set(CMAKE_CXX_FLAGS "-F/Library/Frameworks -framework SDL2 -framework CoreFoundation -stdlib=libc++ -L/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/config-3.6m-darwin -lpython3.6m -ldl -framework CoreFoundation -I/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m -I/Library/Frameworks/Python.framework/Versions/3.6/include/python3.6m -fno-strict-aliasing -Wsign-compare -fno-common -dynamic -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes -g")


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
        include/tinyfiledialogs.c
        include/tinyfiledialogs.h
        imnodes.cpp
        imnodes.h
        Makefile src/Core/data_types.cpp
        ${SOURCES}
        src/Core/generator.cpp
        src/settings.h
        src/Core/utils.cpp
        src/Generation/executor.h
        src/Debug/templates.h
        src/Core/generator.h
        src/Core/utils.h
        src/Core/data_types.h
        src/Generation/executor.cpp
        src/Net/trainer.cpp
        src/Net/trainer.h
        src/Net/dataset_manager.cpp
        src/Net/dataset_manager.h
        src/Net/code_manager.h
        src/Log/logger.h
        src/Core/exceptions.h
        src/fonts.h src/Core/ThreadsManager.h src/Log/plotter.h)

target_link_libraries(NodeNet ${SDL2_LIBRARIES})