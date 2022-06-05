#ifndef PROCEDURALPLACEMENT_SCENE_HPP
#define PROCEDURALPLACEMENT_SCENE_HPP

#include "gl_utils/gl.hpp"
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

#include "utils/texture_load.hpp"
#include "utils/shader_load.hpp"

#include "utils/camera.hpp"
#include "terrain.hpp"
#include "axes.hpp"
#include "entities.hpp"

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
    bool m_proj_changed {true};

    Terrain terrain;
    Axes axes;
    Entities entities;

    GL::ObjectManager<GL::Texture> world_data_texture {loadTexture("textures/world_data.png")};
};


#endif //PROCEDURALPLACEMENT_SCENE_HPP
