add_subdirectory(glfw_utils)
add_subdirectory(gl_utils)
add_subdirectory(utils)

add_executable(demo demo.cpp)
target_link_libraries(demo glfw_utils gl_utils glm utils ImGui)

target_compile_features(demo PUBLIC cxx_std_20)
