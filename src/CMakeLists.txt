add_subdirectory(glfw_utils)
add_subdirectory(gl_utils)
add_subdirectory(utils)

add_executable(demo demo.cpp boilerplate.cpp demo.hpp)
target_link_libraries(demo glfw_utils gl_utils glm utils ImGui)

target_compile_features(demo PUBLIC cxx_std_20)

file(CREATE_LINK ${CMAKE_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets SYMBOLIC)
file(CREATE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders SYMBOLIC)