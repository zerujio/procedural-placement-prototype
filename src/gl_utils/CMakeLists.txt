add_library(gl_utils OBJECT gl.cpp shader.cpp shader_program.cpp buffer.cpp vertex_array.cpp vertex_array.hpp)
target_link_libraries(gl_utils PUBLIC glad)