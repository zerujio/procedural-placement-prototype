add_library(utils OBJECT image.cpp)
target_link_libraries(utils PUBLIC stb_image glm)