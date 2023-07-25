cmake_minimum_required(VERSION 3.21)
project(ASHART)

include_directories(include)
add_executable(ashart src/main.cpp src/asha.cpp src/GUI.cpp)
set_property(TARGET ashart PROPERTY CXX_STANDARD 20)
set_property(TARGET ashart PROPERTY CXX_STANDARD_REQUIRED On)
link_libraries(GL)
# add_library(asha src/asha.cpp)

# SimpleBLE
include_directories(external/SimpleBLE/simpleble/include/)
include_directories(external/SimpleBLE/build_simpleble/export)
add_subdirectory(external/SimpleBLE/simpleble ./external/SimpleBLE/build_simpleble)
target_link_libraries(ashart simpleble::simpleble)
set_property(TARGET simpleble PROPERTY CXX_STANDARD 20)
set_property(TARGET simpleble PROPERTY CXX_STANDARD_REQUIRED On)

# GLFW
include_directories(external/glfw/include)
add_subdirectory(external/glfw)
target_link_libraries(ashart glfw)
set_property(TARGET glfw PROPERTY CXX_STANDARD 20)
set_property(TARGET glfw PROPERTY CXX_STANDARD_REQUIRED On)

# DearImGUI
include_directories(external/imgui external/imgui/backends)
add_library(imgui 
    external/imgui/imgui.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_draw.cpp
    # external/imgui/imgui_demo.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(ashart imgui)
set_property(TARGET imgui PROPERTY CXX_STANDARD 20)
set_property(TARGET imgui PROPERTY CXX_STANDARD_REQUIRED On)