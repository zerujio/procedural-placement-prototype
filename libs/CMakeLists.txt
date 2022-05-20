# glad
add_subdirectory(glad)

#glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW docs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW tests")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW examples")
set(GLFW_INSTALL OFF CACHE BOOL "Install GLFW")
add_subdirectory(glfw)

#glm
add_subdirectory(glm)

#stb_image
add_subdirectory(stb)

#imgui
add_library(ImGui STATIC)
target_sources(ImGui PRIVATE
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp)
target_include_directories(ImGui PUBLIC imgui)
target_link_libraries(ImGui PUBLIC glfw)
