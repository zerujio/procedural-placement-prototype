#ifndef PROCEDURALPLACEMENT_TERRAIN_HPP
#define PROCEDURALPLACEMENT_TERRAIN_HPP

#include "gl_utils/gl.hpp"
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "utils/camera.hpp"
#include "utils/shader_load.hpp"
#include "utils/texture_load.hpp"

class Terrain {
public:
    Terrain();

    void update(GLFWwindow *window, const Camera& camera, double delta);
    void setCameraPosition(const glm::vec3& pos) const;
    void setViewMatrix(const glm::mat4& matrix) const;
    void setProjMatrix(const glm::mat4& matrix) const;
    void setParentTransform(const glm::mat4& matrix) const;
    void setWorldDataTexUnit(GLint u) const;
    void generateMesh();

private:

    enum UniformLocation {
        loc_model,
        loc_view,
        loc_proj,
        loc_lightColor,
        loc_lightDirection,
        loc_ambientStrength,
        loc_specularStrength,
        loc_viewPosition,
    };

    GL::ObjectManager<GL::ShaderProgram> texture_program {loadProgram("shaders/textured.vert", "shaders/textured.frag")};
    const GLint loc_texture = texture_program->getUniformLocation("u_texture");

    GL::ObjectManager<GL::ShaderProgram> blend_program {loadProgram("shaders/textured.vert", "shaders/blend_textured.frag")};
    const GLint loc_blendTexture = blend_program->getUniformLocation("u_worldData");
    const GLint loc_color0 = blend_program->getUniformLocation("u_color0");
    const GLint loc_color1 = blend_program->getUniformLocation("u_color1");
    glm::vec4 color0 {.9, .9, .9, 1.};
    glm::vec4 color1 {.25, .3, .12, 1.};

    GL::ObjectManager<GL::ShaderProgram> compute_program {loadComputeProgram("shaders/heightmap.comp")};
    const GLint loc_computeWorldData = compute_program->getUniformLocation("u_worldData");
    glm::ivec2 num_work_groups {8, 8};

    GL::ObjectManager<GL::Buffer> vertex_buffer;
    GL::ObjectManager<GL::Buffer> element_buffer;

    GLenum mode {GL_TRIANGLES};
    GLenum index_type {GL_UNSIGNED_INT};
    GLsizeiptr index_offset {0};
    GLsizei index_count {0};
    GLsizei vertex_count {0};
    GL::ObjectManager<GL::VertexArray> vertex_array;

    bool show_world_data {false};
    bool show_vertices_only {false};
};

#endif //PROCEDURALPLACEMENT_TERRAIN_HPP
