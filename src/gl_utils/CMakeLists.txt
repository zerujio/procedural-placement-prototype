add_library(gl_utils OBJECT gl.cpp shader.cpp shader_program.cpp buffer.cpp vertex_array.cpp texture.cpp)
target_link_libraries(gl_utils PUBLIC glad)