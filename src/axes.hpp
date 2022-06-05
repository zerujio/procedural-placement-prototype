#ifndef PROCEDURALPLACEMENT_AXES_HPP
#define PROCEDURALPLACEMENT_AXES_HPP

#include "gl_utils/gl.hpp"
#include <GLFW/glfw3.h>

#include "utils/shader_load.hpp"

#include <glm/mat4x4.hpp>

class Axes {
public:
    Axes();

    void draw() const;

    void setViewMatrix(const glm::mat4& matrix) const;
    void setProjMatrix(const glm::mat4& matrix) const;

private:
    GL::ObjectManager<GL::ShaderProgram> program {loadProgram("shaders/axes.vert", "shaders/axes.frag")};
    GL::ObjectManager<GL::Buffer> buffer {};

    const GLint loc_view = program->getUniformLocation("u_view");
    const GLint loc_proj = program->getUniformLocation("u_proj");

    GLenum mode;
    GLint first_index;
    GLsizei index_count;
    GL::ObjectManager<GL::VertexArray> vertex_array;
};


#endif //PROCEDURALPLACEMENT_AXES_HPP
