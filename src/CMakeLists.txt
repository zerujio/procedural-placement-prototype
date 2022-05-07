add_library(glfw_utils OBJECT glfw_utils/init.cpp glfw_utils/window.cpp)
target_link_libraries(glfw_utils PUBLIC glfw)

add_executable(demo demo.cpp)
target_link_libraries(demo glfw_utils glad glm)
