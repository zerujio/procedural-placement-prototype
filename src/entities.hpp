#ifndef PROCEDURALPLACEMENT_ENTITIES_HPP
#define PROCEDURALPLACEMENT_ENTITIES_HPP

#include "gl_utils/gl.hpp"

#include "utils/shader_load.hpp"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

class Entities {
public:
    Entities();

    void update();

    void setParentTransform(const glm::mat4& matrix) const;

    void setViewMatrix(const glm::mat4& matrix) const;

    void setProjMatrix(const glm::mat4& matrix) const;

    void setWorldDataTexUnit(GLint unit) const;

    void generateEntities();

private:

    GL::ObjectManager<GL::ShaderProgram> shader_program {loadProgram("shaders/point.vert", "shaders/point.frag")};
    GL::ObjectManager<GL::ShaderProgram> compute_program {loadComputeProgram("shaders/placement.comp")};
    GL::ObjectManager<GL::Buffer> buffer {};
    GL::ObjectManager<GL::VertexArray> vao {};

    GLint a_position_loc = shader_program->getAttribLocation("a_position");
    GLint u_model_loc = shader_program->getUniformLocation("u_model"),
        u_view_loc = shader_program->getUniformLocation("u_view"),
        u_proj_loc = shader_program->getUniformLocation("u_proj"),
        u_point_size_loc = shader_program->getUniformLocation("u_point_size"),
        u_color_loc = shader_program->getUniformLocation("u_color");
    GLint u_world_data_loc = compute_program->getUniformLocation("u_world_data"),
        u_tex_coord_offset_loc = compute_program->getUniformLocation("u_tex_coord_offset"),
        u_tex_coord_scale_loc = compute_program->getUniformLocation("u_tex_coord_scale");

    glm::vec2 pos0 {.3f, .25f};
    glm::vec2 pos1 {.5f, .6f};
    glm::ivec2 num_work_groups {8, 8};

    GLsizei point_count {0};
    glm::vec4 point_color;
    float point_size;
};


#endif //PROCEDURALPLACEMENT_ENTITIES_HPP