#ifndef PROCEDURALPLACEMENT_DEMO_HPP
#define PROCEDURALPLACEMENT_DEMO_HPP

#include "gl_utils/gl.hpp"
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "utils/camera.hpp"
#include "utils/shader_load.hpp"
#include "utils/texture_load.hpp"

struct ArrayShape {
    GLenum mode;
    GLint first_index;
    GLsizei index_count;
    GL::ObjectManager<GL::VertexArray> vertex_array;
};

struct ElementShape {
    GLenum mode;
    GLsizei index_count;
    GLenum index_type;
    GLsizeiptr index_offset;
    GL::ObjectManager<GL::VertexArray> vertex_array;
};

class Scene {
public:

    Scene();

    void update(GLFWwindow* window, double delta);

    void scrollCallback(GLFWwindow* w, double delta, glm::dvec2 offset);
    void frameBufferSizeCallback(GLFWwindow* w, double delta, glm::ivec2 size);


private:

    glm::dvec2 m_prev_cursor_pos;

    Camera camera;
    bool m_view_changed {true};
    bool m_proj_chaged {true};

    struct Terrain {
        Terrain();

        void generateMesh(glm::uvec2 num_work_groups);

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
        const GLint loc_blendTexture = blend_program->getUniformLocation("u_blendTexture");
        const GLint loc_color0 = blend_program->getUniformLocation("u_color0");
        const GLint loc_color1 = blend_program->getUniformLocation("u_color1");
        glm::vec4 color0 {.9, .9, .9, 1.};
        glm::vec4 color1 {.25, .3, .12, 1.};

        GL::ObjectManager<GL::ShaderProgram> compute_program {loadComputeProgram("shaders/heightmap.comp")};
        const GLint loc_computeWorldData = compute_program->getUniformLocation("u_worldData");

        GL::ObjectManager<GL::Texture> world_data_texture {loadTexture("assets/world_data.png")};
        GL::ObjectManager<GL::Texture> heightmap_texture {loadTexture("assets/heightmap.png")};
        GL::ObjectManager<GL::Buffer> vertex_buffer;
        GL::ObjectManager<GL::Buffer> element_buffer;

        ElementShape shape;
        GLsizei vertex_count {0};

        bool show_world_data {false};
        bool show_vertices_only {false};
    } terrain;

    struct Axes {
        Axes();

        GL::ObjectManager<GL::ShaderProgram> program {loadProgram("shaders/axes.vert", "shaders/axes.frag")};
        GL::ObjectManager<GL::Buffer> buffer {};

        const GLint loc_view = program->getUniformLocation("u_view");
        const GLint loc_proj = program->getUniformLocation("u_proj");

        ArrayShape shape;
    } axes;
};

#endif //PROCEDURALPLACEMENT_DEMO_HPP
