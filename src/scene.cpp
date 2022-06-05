#include "scene.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

Scene::Scene() {
    world_data_texture->setWrapMode(GL::Texture::WrapAxis::S, GL::Texture::WrapMode::ClampToEdge);
    world_data_texture->setWrapMode(GL::Texture::WrapAxis::T, GL::Texture::WrapMode::ClampToEdge);

    constexpr unsigned int tex_unit = 0;
    glBindTextureUnit(tex_unit, world_data_texture->id());
    terrain.setWorldDataTexUnit(tex_unit);
    entities.setWorldDataTexUnit(tex_unit);

    terrain.generateMesh();
    entities.generateEntities();

    auto terrain_transform = glm::scale(glm::mat4(1.0f), {10, 0.52, 7.62});
    terrain_transform = glm::translate(terrain_transform, {-.5, 0, -.5});
    terrain.setParentTransform(terrain_transform);
    entities.setParentTransform(glm::translate(terrain_transform, {0.0f, 0.1f, 0.0f}));
}


void Scene::scrollCallback(GLFWwindow* w, double delta, glm::dvec2 offset) {
    if (offset.y != 0.0)
        camera.zoom(static_cast<float>(-offset.y * delta));
    m_view_changed = true;
}

void Scene::frameBufferSizeCallback(GLFWwindow* w, double, glm::ivec2 size) {
    camera.screen_size = size;
    m_proj_changed = true;
}

void Scene::update(GLFWwindow *w, double delta) {
    ImGui::Begin("Controles");

    ImGui::Text("WASD: desplazamiento");
    ImGui::Text("Click derecho: rotar cámara");
    ImGui::Text("Rueda del ratón: zoom");

    glm::vec2 key_input = {
            glfwGetKey(w, GLFW_KEY_D) - glfwGetKey(w, GLFW_KEY_A),
            glfwGetKey(w, GLFW_KEY_W) - glfwGetKey(w, GLFW_KEY_S)
    };

    if (key_input != glm::vec2(0, 0)) {
        camera.move(-glm::normalize(key_input) * static_cast<float>(delta));
        m_view_changed = true;
    }

    glm::dvec2 curr_cursor_pos;
    glfwGetCursorPos(w, &curr_cursor_pos.x, &curr_cursor_pos.y);
    auto cursor_delta = curr_cursor_pos - m_prev_cursor_pos;
    m_prev_cursor_pos = curr_cursor_pos;

    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) && cursor_delta != glm::dvec2()) {
        camera.rotate(glm::dvec2(1, -1) * cursor_delta * delta);
        m_view_changed = true;
    }

    if (m_view_changed) {
        auto view_matrix = camera.viewMatrix();

        terrain.setViewMatrix(view_matrix);
        terrain.setCameraPosition(camera.eye());
        axes.setViewMatrix(view_matrix);
        entities.setViewMatrix(view_matrix);

        m_view_changed = false;
    }

    if (m_proj_changed) {
        auto proj_matrix = camera.projMatrix();

        terrain.setProjMatrix(proj_matrix);
        axes.setProjMatrix(proj_matrix);
        entities.setProjMatrix(proj_matrix);

        m_proj_changed = false;
    }

    axes.draw();

    ImGui::Separator();
    terrain.update(w, camera, delta);

    ImGui::Separator();
    entities.update();

    ImGui::End();
}