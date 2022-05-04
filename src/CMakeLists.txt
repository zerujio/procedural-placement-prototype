add_executable(demo demo.cpp)
target_sources(demo PRIVATE glfw_window.cpp glfw_init.cpp)
target_link_libraries(demo PRIVATE glfw glad)
