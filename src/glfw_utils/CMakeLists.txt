add_library(glfw_utils OBJECT init.cpp window.cpp)
target_link_libraries(glfw_utils PUBLIC glfw)