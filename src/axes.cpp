#include "axes.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

Axes::Axes() {
    const glm::vec3 vertices[] {
            {0, 0, 0}, {1, 0, 0},
            {1, 0, 0}, {1, 0, 0},
            {0, 0, 0}, {0, 1, 0},
            {0, 1, 0}, {0, 1, 0},
            {0, 0, 0}, {0, 0, 1},
            {0, 0, 1}, {0, 0, 1}
    };

    buffer->initialize(sizeof(vertices), vertices, GL::Buffer::Usage::StaticDraw);

    GL::VertexArray vao = vertex_array.handle();

    const GLsizei stride = 2 * sizeof(glm::vec3);
    const GLuint buffer_binding_index = 0;
    vao.bindVertexBuffer(0, buffer, buffer_binding_index, stride);

    using AttribLocation = GLint;
    AttribLocation a_pos = program->getAttribLocation("a_position");
    AttribLocation a_color = program->getAttribLocation("a_color");

    vao.attribBinding(a_pos, buffer_binding_index);
    vao.attribFormat(a_pos, 3, GL_FLOAT, false, 0);
    vao.enableAttrib(a_pos);

    vao.attribBinding(a_color, buffer_binding_index);
    vao.attribFormat(a_color, 3, GL_FLOAT, false, stride / 2);
    vao.enableAttrib(a_color);

    mode = GL_LINES;
    first_index = 0;
    index_count = 6;
}

void Axes::setViewMatrix(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(program->id(), loc_view, 1, false, glm::value_ptr(matrix));
}

void Axes::setProjMatrix(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(program->id(), loc_proj, 1, false, glm::value_ptr(matrix));
}

void Axes::draw() const {
    vertex_array->bind();
    program->useProgram();
    glDrawArrays(mode, first_index, index_count);
}