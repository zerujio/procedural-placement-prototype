add_library(glfw_utils OBJECT init.cpp window.cpp callbacks.cpp)
target_link_libraries(glfw_utils PUBLIC glfw)