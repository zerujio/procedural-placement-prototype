#ifndef PROCEDURALPLACEMENT_SHADER_LOAD_HPP
#define PROCEDURALPLACEMENT_SHADER_LOAD_HPP

#include <glad/glad.h>
#include "../gl_utils/gl.hpp"

GL::Shader loadShader(const std::string &path, GL::ShaderType shader_type);
GL::ShaderProgram loadProgram(const std::string & vs_path, const std::string & fs_path);
GL::ShaderProgram loadComputeProgram(const std::string &cs_path);

#endif //PROCEDURALPLACEMENT_SHADER_LOAD_HPP
