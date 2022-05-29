#include "shader_load.hpp"

#include <fstream>
#include <sstream>

GL::Shader loadShader(const std::string &path, GL::ShaderType shader_type) {
    std::fstream file {path};
    std::stringstream sstream;
    sstream << file.rdbuf();

    auto shader = GL::Shader::create(shader_type);
    glObjectLabel(GL_SHADER, shader.id(), -1, path.c_str());
    shader.setSource( sstream.str() );
    shader.compileShader();

    return shader;
}

GL::ShaderProgram loadProgram(const std::string & vs_path, const std::string & fs_path) {
    auto program = GL::ShaderProgram::create();
    GL::ObjectManager vert {loadShader(vs_path, GL::ShaderType::Vertex)};
    GL::ObjectManager frag {loadShader(fs_path, GL::ShaderType::Fragment)};

    program.attachShader(vert.handle());
    program.attachShader(frag.handle());

    program.linkProgram();

    program.detachShader(vert.handle());
    program.detachShader(frag.handle());

    return program;
}

GL::ShaderProgram loadComputeProgram(const std::string &cs_path) {
    auto program = GL::ShaderProgram::create();
    GL::ObjectManager comp {loadShader(cs_path, GL::ShaderType::Compute)};

    program.attachShader(comp.handle());

    program.linkProgram();

    program.detachShader(comp.handle());

    return program;
}