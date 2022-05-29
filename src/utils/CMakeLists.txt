add_library(utils OBJECT image.cpp camera.cpp shader_load.cpp texture_load.cpp)
target_link_libraries(utils PUBLIC stb_image glm glad)