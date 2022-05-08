add_library(gl_utils OBJECT gl.cpp shader.cpp shader_program.cpp)
target_link_libraries(gl_utils PUBLIC glad)